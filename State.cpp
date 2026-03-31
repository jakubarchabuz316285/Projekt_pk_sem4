#include "State.h"
#include "QSaveState.hpp"
#include "QTimerState.hpp"
#include "qimage.h"
#include <cassert>
#include <stdexcept>
#include <QIODevice>


State::State()
    : uar(UAR(ARX({-0.4}, {0.6}), RegulatorPID(0.5, 5.0, 0.2)))
    , gen_sin{}
    , gen_pros{}
    , gen_skok{}
    , simmulation_running(false)
    , _networkManager(std::bind(&State::receivePacket, this, std::placeholders::_1))
{
    choosen_generator = &gen_sin;
    save = new QSaveState();
    timer = new QTimerState();
    timer->setIntervalMS(200);
    timer->setTimeout(std::bind(&State::tick, this));
    timer->setRunning(simmulation_running);
     QObject::connect(&_networkManager, &NetworkManager::statusChanged, this, &State::statusChanged);
    // simmulation_timer = new QTimer();
    // simmulation_timer->setSingleShot(false);
    // simmulation_timer->setInterval(200);
    // simmulation_timer->connect(simmulation_timer, &QTimer::timeout, this, &State::tick);
    // if(simmulation_running)
    //     simmulation_timer->start();
    // else
    //     simmulation_timer->stop();

    // ONLINE
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
    // delete simmulation_timer;

    delete save;
    delete timer;
}

class State &State::getInstance()
{
    static State instance = State();
    return instance;
}


void State::setSimmulationRunning(bool simmulation_running)
{
    this->simmulation_running = simmulation_running;
    timer->setRunning(simmulation_running);
    // if (simmulation_running)
    //     simmulation_timer->start();
    // else
    //     simmulation_timer->stop();
}
void State::setSimmulationIntervalMS(uint32_t interwal)
{
    timer->setIntervalMS(interwal);
    // this->simmulation_timer->setInterval(interwal);
}
bool State::getSimmulationRunning()
{
    return simmulation_running;
}
uint32_t State::getSimmulationIntervalMS()
{
    return timer->getIntervalMS();
    // return simmulation_timer->interval();
}
void State::setOutputCallback(const std::function<void(TickData)> callback)
{
    assert(callback != nullptr); // Przeslany callback musi być poprawny
    this->tick_callback = callback;
}
void State::resetSimmulation()
{
    resetGenerator();
    this->uar.resetAll();
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
}
State::TypGeneratora State::getGenerator()
{
    if(choosen_generator == &gen_pros)
        return TypGeneratora::Prostokatny;
    if(choosen_generator == &gen_sin)
        return TypGeneratora::Sinusoidalny;
    return TypGeneratora::SkokJednostkowy;
}
void State::setGeneneratorAmplitude(const double& amplitude)
{
    this->gen_pros.setAmplitude(amplitude);
    this->gen_sin.setAmplitude(amplitude);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}
void State::setGeneneratorDutyCycle(const double& duty_cycle) // ustawia kiedy sygnal jest w gorze
{
    this->gen_pros.setDutyCycle(duty_cycle);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
}
void State::setGeneratorSkladowaStala(double skladowa_stala)
{
    this->gen_pros.setBias(skladowa_stala);
    this->gen_sin.setBias(skladowa_stala);
    this->gen_skok.setBias(skladowa_stala);
}
void State::setGeneneratorPeriodMS(uint32_t period)
{
    uint32_t sample = period / getSimmulationIntervalMS();
    //sample -= (sample & 1);
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
    console_print_state();
}
void State::setPIDIntegration(double T_i)
{
    this->uar.getRegulatorPID().setT_i(T_i);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
    console_print_state();
}
void State::setPIDDerrivative(double T_d)
{
    this->uar.getRegulatorPID().setT_d(T_d);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
    console_print_state();
}
void State::setPIDIntegrationType(IntegType integration_type)
{
    this->uar.getRegulatorPID().setIntegrationType(integration_type);
    if(_networkManager.GetMode() == NetworkManager::Mode::PID) _networkManager.SendMsg(serializePIDState(getPIDConfig()));
    console_print_state();
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

    // PID

    package.k = uar.getRegulatorPID().getK();
    package.T_i = uar.getRegulatorPID().getT_i();
    package.T_d = uar.getRegulatorPID().getT_d();
    package.integType = (uint8_t)uar.getRegulatorPID().getIntegrationType();

    // GEN

    package.amplitude = choosen_generator->getAmplitude();
    package.bias = choosen_generator->getBias();
    package.samples_per_cycle = choosen_generator->getSamplesPerCycle();
    package.genType = (uint8_t)getGenerator();

    // SIM

    package.interval = (uint8_t)timer->getIntervalMS();

    return package;
}

ArxInstancePackage State::getArxConfig(){
    ArxInstancePackage package;

    //ARX
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
    if(_networkManager.GetMode() == NetworkManager::Mode::ARX) _networkManager.SendMsg(serializeARXState(getArxConfig()));
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
    if(enabled)
        this->uar.getARX().enableLimits();
    else
        this->uar.getARX().disableLimits();
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
    if(save == nullptr)
        throw std::runtime_error("Brak obiektu do zapisywania w State");
    this->save->saveToFile(path, &uar, simmulation_running, getGenerator(), &gen_pros, &gen_sin);
}
void State::readFromFile(std::string path)
{
    if(save == nullptr)
        throw std::runtime_error("Brak obiektu do zapisywania w State");
    TypGeneratora typ_generatora;
    this->save->readFromFile(path, &uar, &simmulation_running, &typ_generatora, &gen_pros, &gen_sin);
    setGenerator(typ_generatora);
}

void State::tick()
{
    this->tick_callback(uar.tickMoreInfo(choosen_generator->tick()));
}

const std::tuple<const ARX*, const RegulatorPID*, const State::TypGeneratora, const GeneratorSinusoida*, const GeneratorProstokatny*> State::getAppState()
{
    return std::make_tuple(&uar.getARX(), &uar.getRegulatorPID(), getGenerator(), &gen_sin, &gen_pros);
}

class State& StateGlobalAccess::operator()()
{
    return State::getInstance();
}

QByteArray State::serializeU(double u)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<(quint8)TypPakietu::USample << u;
    return data;
}

QByteArray State::serializeY(double y)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<(quint8)TypPakietu::YSample << y;
    return data;
}

QByteArray State::serializePIDState(const RegulatorInstancePackage& data)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);

    stream.setVersion(QDataStream::Qt_6_0); // opcjonalnie (dopasuj do wersji Qt)
    qDebug() << "samples z serializacji:" << data.samples_per_cycle;
    stream << (quint8)TypPakietu::PidConfig
           << data.k
           << data.T_i
           << data.T_d
           << data.integType
           << data.amplitude
           << data.samples_per_cycle
           << data.bias
           << data.genType
           << data.interval;

    return byteArray;
}

QByteArray State::serializeARXState(const ArxInstancePackage& data){
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);

    stream.setVersion(QDataStream::Qt_6_0); // opcjonalnie (dopasuj do wersji Qt)

    QByteArray wsp_aData;
    QDataStream wsp_aStream(&wsp_aData, QIODevice::WriteOnly);

    for(double vecData : data.wsp_a){
        wsp_aStream << vecData;
    }

    QByteArray wsp_bData;
    QDataStream wsp_bStream(&wsp_bData, QIODevice::WriteOnly);

    for(double vecData : data.wsp_b){
        wsp_bStream << vecData;
    }

    stream << (quint8)TypPakietu::ArxConfig
           << data.vec_size
           << wsp_aData
           << wsp_bData
           << data.transport_delay
           << data.noise
           << data.is_limited
           << data.input_max
           << data.input_min
           << data.output_max
           << data.output_min;

    return byteArray;
}

QByteArray State::serializePIDOutput(){
    double output = this->uar.getRegulatorPID().getLastOutput();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<(quint8)TypPakietu::PIDSample << output;
    return data;
}

void State::deserializeAndApply(const QByteArray& byteArray)
{
    QDataStream stream(byteArray);
    stream.setVersion(QDataStream::Qt_6_0);

    quint8 typRaw;
    stream >> typRaw;

    TypPakietu typ = static_cast<TypPakietu>(typRaw);

    switch (typ)
    {
    case TypPakietu::PidConfig:
    {
        double k, T_i, T_d;
        int integType;
        double amplitude;
        int samples_per_cycle;
        double bias;
        int genType;
        int interval;

        stream >> k
            >> T_i
            >> T_d
            >> integType
            >> amplitude
            >> samples_per_cycle
            >> bias
            >> genType
            >> interval;

        // 🔧 SETTERY
        setPIDProportional(k);
        setPIDIntegration(T_i);
        setPIDDerrivative(T_d);
        setPIDIntegrationType(IntegType(integType));
        setGenerator(TypGeneratora(genType));
        qDebug() << "samples per cycle: " << samples_per_cycle;
        this->choosen_generator->setSamplesPerCycle(samples_per_cycle);
        this->choosen_generator->setBias(bias);
        this->choosen_generator->setAmplitude(amplitude);
        timer->setIntervalMS(interval);

        break;
    }

    case TypPakietu::ArxConfig:
    {
        QByteArray wsp_aData;
        QByteArray wsp_bData;
        int vec_size;
        double transport_delay;
        double noise;
        bool is_limited;
        double input_max, input_min;
        double output_max, output_min;

        stream >>vec_size
            >> wsp_aData
            >> wsp_bData
            >> transport_delay
            >> noise
            >> is_limited
            >> input_max
            >> input_min
            >> output_max
            >> output_min;

        // 🔧 DESERIALIZACJA wsp_a
        std::vector<double> wsp_a;
        QDataStream wsp_aStream(wsp_aData);
        wsp_aStream.setVersion(QDataStream::Qt_6_0);

        double val;
        for(int i = 0; i < vec_size/2; i++)
        {
            wsp_aStream >> val;
            wsp_a.push_back(val);
        }

        // 🔧 DESERIALIZACJA wsp_b
        std::vector<double> wsp_b;
        QDataStream wsp_bStream(wsp_bData);
        wsp_bStream.setVersion(QDataStream::Qt_6_0);

        for(int i = 0; i < vec_size/2; i++)
        {
            wsp_bStream >> val;
            wsp_b.push_back(val);
        }

        // 🔧 SETTERY
        setARXCoefficients(wsp_a, wsp_b);
        setARXTransportDelay(transport_delay);
        setARXNoiseStandardDeviation(noise);
        setARXLimitsEnabled(is_limited);
        setARXInputLimits(input_min, input_max);
        setARXOutputLimits(output_min, output_max);

        break;
    }

    case TypPakietu::USample:
    {
        double u;
        stream >> u;
        // TODO Logika
        break;
    }

    case TypPakietu::YSample:
    {
        double y;
        stream >> y;
        // TODO Logika
        break;
    }

    case TypPakietu::PIDSample:
    {
        double val;
        stream >> val;
        // TODO Logika
        break;
    }

    case TypPakietu::GENSample:
    {
        double val;
        stream >> val;
        // TODO Logika
        break;
    }

    case TypPakietu::ResetGen:
    {
        resetGenerator();
        break;
    }

    case TypPakietu::ResetPidIntegration:
    {
        resetPIDIntegration();
        break;
    }
    case TypPakietu::ResetPidDerrivative:
    {
        resetPIDDerrivative();
        break;
    }
    case TypPakietu::SimStart:
    {
        timer->setRunning(true);
    }
    case TypPakietu::SimStop:
    {
        timer->setRunning(true);
    }
    case TypPakietu::SimReset:
    {
        resetSimmulation();
    }

    default:
        qWarning() << "Nieznany typ pakietu:" << typRaw;
        break;
    }
    console_print_state();
}

void State::receivePacket(const QByteArray& packet){
    deserializeAndApply(packet);
}


StateGlobalAccess State;

