#include "State.h"
#include "QSaveState.hpp"
#include "QTimerState.hpp"
#include "qelapsedtimer.h"
#include <cassert>
#include <stdexcept>
#include <QIODevice>
#include <QDataStream>
#include <QDebug>

State::State()
    : uar(UAR(ARX({-0.4}, {0.6}), RegulatorPID(0.5, 5.0, 0.2)))
    , gen_sin{}
    , gen_pros{}
    , gen_skok{}
    , readyForNextTick(true)
    , _networkManager(std::bind(&State::receivePacket, this, std::placeholders::_1))
    , simmulation_running(false)
{
    // Liczniki
    current_sample_id = 0;
    last_processed_pid_id = 0;
    last_processed_arx_id = 0;

    choosen_generator = &gen_sin;
    save = new QSaveState();
    timer = new QTimerState();
    timer->setIntervalMS(200);
    timer->setTimeout(std::bind(&State::tick, this));
    timer->setRunning(simmulation_running);

    QObject::connect(&_networkManager, &NetworkManager::statusChanged, this, [this](bool connected){
        if(connected) {
            if(getMode() == NetworkManager::Mode::PID) {
                _networkManager.SendMsg(serializePIDState(getPIDConfig()));
                readyForNextTick = true;
            } else {
                _networkManager.SendMsg(serializeARXState(getArxConfig()));
            }
        }
        else {
            // --- REAKCJA NA UTRATĘ POŁĄCZENIA ---
            if(getMode() == NetworkManager::Mode::ARX) {
                qDebug() << "[SYSTEM] Utracono połączenie z serwerem. Powrót asynchroniczny.";

                // ASYNCHRONICZNE ZMIENIANIE TRYBU - KLUCZOWE ABY UNIKNĄĆ CRASHA!
                // Wywołanie jest odkładane na moment, gdy socket skończy pracę ze swoimi sygnałami.
                QTimer::singleShot(0, this, [this]() {
                    _networkManager.SetMode(NetworkManager::Mode::Local);
                    readyForNextTick = true;
                    emit networkModeChanged(NetworkManager::Mode::Local, true);
                });
            }
        }
        emit statusChanged(connected);
    });

    QObject::connect(&_networkManager, &NetworkManager::serverDiscovered, this, &State::serverDiscovered);
}

void State::console_print_state()
{
    qDebug() << "ARX";
    qDebug() << "Vec a: " << this->getARXCoefficientsA();
    qDebug() << "Vec b: " <<this->getARXCoefficientsB();
    qDebug() << "Transport: " <<this->getARXTransportDelay();
    qDebug() << "Out max: " <<this->getARXOutputLimits().first;
    qDebug() << "Out min: " <<this->getARXOutputLimits().second;
    qDebug() << "In max: " <<this->getARXInputLimits().first;
    qDebug() << "In min: " <<this->getARXInputLimits().second;
    qDebug() << "Noise: " <<this->getARXNoiseStandardDeviation();

    qDebug() << "PID";
    qDebug() << "TD: " << this->getPIDConfig().T_d;
    qDebug() << "Ti: " << this->getPIDConfig().T_i;
    qDebug() << "k: " << this->getPIDConfig().k;
    qDebug() << "typ: " << this->getPIDConfig().integType;

    qDebug() << "GEN";
    qDebug() << "Amp" << this->choosen_generator->getAmplitude();
    qDebug() << "Bias" << this->choosen_generator->getBias();
    qDebug() << "Amp" << this->choosen_generator->getSamplesPerCycle();
    qDebug() << "Amp" << (int)this->getGenerator();

    qDebug() << "Sim";
    qDebug() << "Amp" << this->timer->getIntervalMS();
}

State::~State()
{
    _networkManager.setPublicServer(false, 0);
    _networkManager.stopListeningForServers();
    _networkManager.Cleanup();

    delete save;
    delete timer;
}

class State &State::getInstance()
{
    static State instance;
    return instance;
}

void State::setSimmulationRunning(bool simmulation_running)
{
    if (simmulation_running) {
        readyForNextTick = true; // Zabezpieczenie przed falstartem
    }
    this->simmulation_running = simmulation_running;
    timer->setRunning(simmulation_running);
    if (getMode() != NetworkManager::Mode::Local) {
        _networkManager.SendMsg(wrapPacket(simmulation_running ? TypPakietu::SimStart : TypPakietu::SimStop, QByteArray()));
    }
}

void State::setSimmulationIntervalMS(uint32_t interwal)
{
    timer->setIntervalMS(interwal);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

bool State::getSimmulationRunning()
{
    return simmulation_running;
}

uint32_t State::getSimmulationIntervalMS()
{
    return timer->getIntervalMS();
}

void State::setOutputCallback(const std::function<void(TickData)> callback)
{
    assert(callback != nullptr);
    this->tick_callback = callback;
}

void State::resetSimmulation(bool notifyNetwork)
{
    resetGenerator();
    uar.resetAll();
    readyForNextTick = true;

    // Reset liczników próbek przy restarcie symulacji
    current_sample_id = 0;
    last_processed_pid_id = 0;
    last_processed_arx_id = 0;

    if (notifyNetwork && getMode() != NetworkManager::Mode::Local) {
        _networkManager.SendMsg(wrapPacket(TypPakietu::SimReset, QByteArray()));
    }
}

void State::setGenerator(TypGeneratora type)
{
    switch(type)
    {
    case TypGeneratora::Prostokatny:
        choosen_generator = &gen_pros;
        break;
    case TypGeneratora::Sinusoidalny:
        choosen_generator = &gen_sin;
        break;
    case TypGeneratora::SkokJednostkowy:
        choosen_generator = &gen_skok;
        break;
    }
    qDebug() << "Wysyłam zmianę w generatorze: Gen mode";
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

State::TypGeneratora State::getGenerator()
{
    if(choosen_generator == &gen_pros) return TypGeneratora::Prostokatny;
    if(choosen_generator == &gen_sin) return TypGeneratora::Sinusoidalny;
    return TypGeneratora::SkokJednostkowy;
}

void State::setGeneneratorAmplitude(const double& amplitude)
{
    qDebug() << "Wysyłam zmianę w generatorze: Amplitude";
    this->gen_pros.setAmplitude(amplitude);
    this->gen_sin.setAmplitude(amplitude);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setGeneneratorDutyCycle(const double& duty_cycle)
{
    qDebug() << "Wysyłam zmianę w generatorze: dutycycle";
    this->gen_pros.setDutyCycle(duty_cycle);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setGeneratorSkladowaStala(double skladowa_stala)
{
    qDebug() << "Wysyłam zmianę w generatorze: bias";
    this->gen_pros.setBias(skladowa_stala);
    this->gen_sin.setBias(skladowa_stala);
    this->gen_skok.setBias(skladowa_stala);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setGeneneratorPeriodMS(uint32_t period)
{
    qDebug() << "Wysyłam zmianę w generatorze: GeneratorPeriodMS";
    uint32_t sample = period / getSimmulationIntervalMS();
    this->gen_pros.setSamplesPerCycle(sample);
    this->gen_sin.setSamplesPerCycle(sample);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

uint8_t State::getGeneneratorPeriodJumpMS()
{
    return getSimmulationIntervalMS();
}

void State::setGeneratorUnitJumpTimeMS(uint32_t time)
{
    qDebug() << "Wysyłam zmianę w generatorze: UnitTimeMS";
    this->gen_skok.setActivationTime(time / getSimmulationIntervalMS());
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::resetGenerator()
{
    this->gen_pros.resetClock();
    this->gen_sin.resetClock();
    this->gen_skok.resetClock();
}

void State::setPIDProportional(double k)
{
    this->uar.getRegulatorPID().setK(k);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setPIDIntegration(double T_i)
{
    this->uar.getRegulatorPID().setT_i(T_i);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setPIDDerrivative(double T_d)
{
    this->uar.getRegulatorPID().setT_d(T_d);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::setPIDIntegrationType(IntegType integration_type)
{
    this->uar.getRegulatorPID().setIntegrationType(integration_type);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}

void State::resetPIDIntegration()
{
    this->uar.getRegulatorPID().resetIntegrationPart();
}

void State::resetPIDDerrivative()
{
    this->uar.getRegulatorPID().resetDerrivativePart();
}

RegulatorInstancePackage State::getPIDConfig(){
    RegulatorInstancePackage package;
    package.k = uar.getRegulatorPID().getK();
    package.T_i = uar.getRegulatorPID().getT_i();
    package.T_d = uar.getRegulatorPID().getT_d();
    package.integType = (uint8_t)uar.getRegulatorPID().getIntegrationType();
    package.amplitude = choosen_generator->getAmplitude();
    package.bias = choosen_generator->getBias();
    package.samples_per_cycle = choosen_generator->getSamplesPerCycle();
    package.genType = (uint8_t)getGenerator();
    package.interval = timer->getIntervalMS();
    return package;
}

ArxInstancePackage State::getArxConfig(){
    ArxInstancePackage package;
    package.wsp_a = uar.getARX().getA();
    package.wsp_b = uar.getARX().getB();
    package.vec_size = uar.getARX().getA().size();
    package.transport_delay = uar.getARX().getK();
    package.noise = uar.getARX().getStandardDeviation();
    package.is_limited = uar.getARX().getLimitsActive();
    package.input_max = uar.getARX().getInputLimits().second;
    package.input_min = uar.getARX().getInputLimits().first;
    package.output_max = uar.getARX().getOutputLimits().second;
    package.output_min = uar.getARX().getOutputLimits().first;
    return package;
}

void State::setARXCoefficients(std::vector<double> a, std::vector<double> b)
{
    this->uar.getARX().setAB(a, b);
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

const std::vector<double> State::getARXCoefficientsA()
{
    return this->uar.getARX().getA();
}

const std::vector<double> State::getARXCoefficientsB()
{
    return this->uar.getARX().getB();
}

void State::setARXTransportDelay(uint16_t k)
{
    this->uar.getARX().setK(k);
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

uint16_t State::getARXTransportDelay()
{
    return this->uar.getARX().getK();
}

void State::setARXInputLimits(double low, double high)
{
    this->uar.getARX().setInputLimits(low, high);
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

const std::pair<double, double>& State::getARXInputLimits()
{
    return this->uar.getARX().getInputLimits();
}

void State::setARXOutputLimits(double low, double high)
{
    this->uar.getARX().setOutputLimits(low, high);
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

const std::pair<double, double>& State::getARXOutputLimits()
{
    return this->uar.getARX().getOutputLimits();
}

void State::setARXNoiseStandardDeviation(double standard_deviation)
{
    this->uar.getARX().setStandardDeviation(standard_deviation);
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

double State::getARXNoiseStandardDeviation()
{
    return this->uar.getARX().getStandardDeviation();
}

void State::setARXLimitsEnabled(bool enabled)
{
    if(enabled) this->uar.getARX().enableLimits();
    else this->uar.getARX().disableLimits();

    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
}

void State::resetARX()
{
    this->uar.getARX().reset();
}

void State::setSaveStateObject(SaveStateInterface* object)
{
    this->save = object;
}

void State::saveToFile(std::string path)
{
    if(save == nullptr) throw std::runtime_error("Brak obiektu do zapisywania w State");
    this->save->saveToFile(path, &uar, simmulation_running, getGenerator(), &gen_pros, &gen_sin);
}

void State::readFromFile(std::string path)
{
    if(save == nullptr) throw std::runtime_error("Brak obiektu do zapisywania w State");
    TypGeneratora typ_generatora;
    this->save->readFromFile(path, &uar, &simmulation_running, &typ_generatora, &gen_pros, &gen_sin);
    setGenerator(typ_generatora);
}

void State::tick()
{
    if (getMode() == NetworkManager::Mode::Local) {
        this->tick_callback(uar.tickMoreInfo(choosen_generator->tick()));
        emit lagStatusChanged(false);
    }
    else if (getMode() == NetworkManager::Mode::PID) {
        if (!isConnected()) {
            qDebug() << "[TICK BŁĄD] Próba kroku PID bez aktywnego połączenia! Zatrzymuję timer.";
            setSimmulationRunning(false);
            emit requestUiUpdate();
            return;
        }

        emit lagStatusChanged(!readyForNextTick);

        current_sample_id++;

        current_tick_data = uar.tickMoreInfo(choosen_generator->tick());

        if (tick_callback) {
            tick_callback(current_tick_data);
        }

        _networkManager.SendMsg(serializePidSample(
            current_sample_id,
            current_tick_data.wartosc_zadana,
            current_tick_data.sterowanie,
            current_tick_data.uchyb
            ));

        readyForNextTick = false;
    }
}
const std::tuple<const ARX*, const RegulatorPID*, const State::TypGeneratora, const GeneratorSinusoida*, const GeneratorProstokatny*> State::getAppState()
{
    return std::make_tuple(&uar.getARX(), &uar.getRegulatorPID(), getGenerator(), &gen_sin, &gen_pros);
}

class State& StateGlobalAccess::operator()()
{
    return State::getInstance();
}

QByteArray State::serializeArxSample(quint32 sampleId, double value)
{
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << sampleId << value; // Pakujemy ID zwrotne
    return wrapPacket(TypPakietu::ARXSample, payload);
}

QByteArray State::serializePidSample(quint32 sampleId, double gen, PIDTickData pid, double uchyb)
{
    QByteArray payload;
    QDataStream out(&payload, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << sampleId << gen << pid.Proportional << pid.Integral << pid.Derrivative << uchyb; // Pakujemy ID nadawcze
    return wrapPacket(TypPakietu::PIDSample, payload);
}

QByteArray State::serializePIDState(const RegulatorInstancePackage& data)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_0);

    double dutyCycle = 0.0;
    GeneratorProstokatny* prostokatny = dynamic_cast<GeneratorProstokatny*>(this->choosen_generator);
    if (prostokatny != nullptr) dutyCycle = prostokatny->getDutyCycle();

    stream << data.k << data.T_i << data.T_d << data.integType
           << data.amplitude << data.samples_per_cycle
           << data.bias << data.genType << data.interval << dutyCycle;

    return wrapPacket(TypPakietu::PidConfig, payload);
}

QByteArray State::serializeARXState(const ArxInstancePackage& data)
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_0);

    stream << (qint32)data.wsp_a.size();
    for(double v : data.wsp_a) stream << v;

    stream << (qint32)data.wsp_b.size();
    for(double v : data.wsp_b) stream << v;

    stream << (quint16)data.transport_delay
           << (double)data.noise
           << (bool)data.is_limited
           << (double)data.input_max
           << (double)data.input_min
           << (double)data.output_max
           << (double)data.output_min;

    return wrapPacket(TypPakietu::ArxConfig, payload);
}

QByteArray State::wrapPacket(TypPakietu typ, const QByteArray& payload)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << (quint8)typ;
    out << (quint32)payload.size();

    out.writeRawData(payload.constData(), payload.size());

    return data;
}

void State::deserializeAndApplyPayload(TypPakietu typ, const QByteArray& byteArray)
{
    QDataStream stream(byteArray);
    stream.setVersion(QDataStream::Qt_6_0);

    switch (typ)
    {
    case TypPakietu::ARXSample: {
        quint32 response_id;
        double val;
        stream >> response_id >> val;

        if (stream.status() != QDataStream::Ok) return;

        if (response_id <= last_processed_arx_id) return;
        last_processed_arx_id = response_id;

        uar.setPreviousYi(val);

        if (response_id == current_sample_id) {
            readyForNextTick = true;
        }
        break;
    }

    case TypPakietu::PIDSample: {
        TickData tick_data;
        quint32 received_id;

        stream >> received_id
            >> tick_data.wartosc_zadana
            >> tick_data.sterowanie.Proportional
            >> tick_data.sterowanie.Integral
            >> tick_data.sterowanie.Derrivative
            >> tick_data.uchyb;

        if (stream.status() != QDataStream::Ok) return;

        if (received_id <= last_processed_pid_id) {
            qDebug() << "[ARX DROP] Ignoruję przeterminowany pakiet PID. ID:" << received_id;
            return;
        }
        last_processed_pid_id = received_id;

        double u_k = tick_data.sterowanie.Proportional + tick_data.sterowanie.Integral + tick_data.sterowanie.Derrivative;
        tick_data.wartosc_regulowana = uar.getARX().tick(u_k);

        _networkManager.SendMsg(serializeArxSample(received_id, tick_data.wartosc_regulowana));

        if(tick_callback) tick_callback(tick_data);
        break;
    }

    case TypPakietu::PidConfig:
    {
        double k, T_i, T_d;
        int integType;
        double amplitude;
        uint16_t samples_per_cycle;
        double bias;
        int genType;
        uint32_t interval;
        double dutyCycle;

        stream >> k >> T_i >> T_d >> integType >> amplitude
            >> samples_per_cycle >> bias >> genType >> interval >> dutyCycle;

        setPIDProportional(k);
        setPIDIntegration(T_i);
        setPIDDerrivative(T_d);
        setPIDIntegrationType(IntegType(integType));
        setGenerator(TypGeneratora(genType));
        GeneratorProstokatny* prostokatny = dynamic_cast<GeneratorProstokatny*>(this->choosen_generator);
        this->choosen_generator->setSamplesPerCycle(samples_per_cycle);
        this->choosen_generator->setBias(bias);
        this->choosen_generator->setAmplitude(amplitude);
        if (prostokatny != nullptr) prostokatny->setDutyCycle(dutyCycle);
        timer->setIntervalMS(interval);

        emit requestUiUpdate();
        break;
    }

    case TypPakietu::ArxConfig:
    {
        qint32 sizeA, sizeB;
        uint16_t transport_delay;
        double noise;
        bool is_limited;
        double inMax, inMin, outMax, outMin;

        stream >> sizeA;
        std::vector<double> wsp_a(sizeA);
        for(int i = 0; i < sizeA; ++i) stream >> wsp_a[i];

        stream >> sizeB;
        std::vector<double> wsp_b(sizeB);
        for(int i = 0; i < sizeB; ++i) stream >> wsp_b[i];

        stream >> transport_delay >> noise >> is_limited
            >> inMax >> inMin >> outMax >> outMin;

        setARXCoefficients(wsp_a, wsp_b);
        setARXTransportDelay(transport_delay);
        setARXNoiseStandardDeviation(noise);
        setARXLimitsEnabled(is_limited);
        setARXInputLimits(inMin, inMax);
        setARXOutputLimits(outMin, outMax);

        emit requestUiUpdate();
        break;
    }

    case TypPakietu::ResetGen:
        resetGenerator();
        break;

    case TypPakietu::ResetPidIntegration:
        resetPIDIntegration();
        break;

    case TypPakietu::ResetPidDerrivative:
        resetPIDDerrivative();
        break;

    case TypPakietu::SimStart:
        timer->setRunning(true);
        break;

    case TypPakietu::SimStop:
        timer->setRunning(false);
        break;

    case TypPakietu::SimReset:
        resetSimmulation(false);
        emit requestChartsReset();
        break;

    default:
        qWarning() << "Nieznany typ pakietu:" << (int)typ;
        break;
    }
}

void State::receivePacket(const QByteArray& packet)
{
    incomingBuffer.append(packet);

    constexpr int HEADER_SIZE = sizeof(quint8) + sizeof(quint32);
    const quint32 MAX_PACKET_SIZE = 10 * 1024 * 1024;

    while (true)
    {
        if (incomingBuffer.size() < HEADER_SIZE)
            return;

        QDataStream stream(&incomingBuffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_0);

        quint8 typRaw;
        quint32 size;

        stream >> typRaw >> size;

        if (size > MAX_PACKET_SIZE)
        {
            qWarning() << "Za duzy pakiet, drop";
            incomingBuffer.clear();
            return;
        }

        int totalSize = HEADER_SIZE + size;

        if (incomingBuffer.size() < totalSize)
            return;

        QByteArray payload = incomingBuffer.mid(HEADER_SIZE, size);
        incomingBuffer.remove(0, totalSize);

        deserializeAndApplyPayload(static_cast<TypPakietu>(typRaw), payload);
    }
}

bool State::isReadyForNextTick() const
{
    return readyForNextTick;
}

void State::setMode(const NetworkManager::Mode& mode)
{
    if (_networkManager.GetMode() == mode) return;

    bool wasRunning = getSimmulationRunning();
    if (wasRunning) {
        setSimmulationRunning(false);
    }


    if (mode != NetworkManager::Mode::Local) {
        resetSimmulation();
        emit requestChartsReset();
    }

    _networkManager.SetMode(mode);
    emit requestUiUpdate();

    if (wasRunning) {
        setSimmulationRunning(true);
    }
}

NetworkManager::Mode State::getMode() const
{
    return _networkManager.GetMode();
}

void State::connect(const QString& ip, int port)
{
    _networkManager.Connect(ip, port);
}

void State::disconnect()
{
    _networkManager.Disconnect();
}

void State::startListening(int port)
{
    _networkManager.StartListening(port);
}

void State::stopListening()
{
    _networkManager.StopListening();
}

bool State::isListening() const
{
    return _networkManager.IsListening();
}

bool State::isConnected() const
{
    return _networkManager.isConnected();
}

// BROADCAST

void State::setPublicServer(bool publiczny, int tcpPort) {
    _networkManager.setPublicServer(publiczny, tcpPort);
}

void State::startServerDiscovery() {
    _networkManager.startListeningForServers();
}

void State::stopServerDiscovery() {
    _networkManager.stopListeningForServers();
}

StateGlobalAccess State;
