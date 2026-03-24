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
{
    choosen_generator = &gen_sin;
    save = new QSaveState();
    timer = new QTimerState();
    timer->setIntervalMS(200);
    timer->setTimeout(std::bind(&State::tick, this));
    timer->setRunning(simmulation_running);
    // simmulation_timer = new QTimer();
    // simmulation_timer->setSingleShot(false);
    // simmulation_timer->setInterval(200);
    // simmulation_timer->connect(simmulation_timer, &QTimer::timeout, this, &State::tick);
    // if(simmulation_running)
    //     simmulation_timer->start();
    // else
    //     simmulation_timer->stop();

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
}
void State::setGeneneratorDutyCycle(const double& duty_cycle)
{
    this->gen_pros.setDutyCycle(duty_cycle);
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
    sample -= (sample & 1);
    this->gen_pros.setSamplesPerCycle(sample);
    this->gen_sin.setSamplesPerCycle(sample);
}
uint8_t State::getGeneneratorPeriodJumpMS()
{
    return getSimmulationIntervalMS();
}
void State::setGeneratorUnitJumpTimeMS(uint32_t time)
{
    this->gen_skok.setActivationTime(time / getSimmulationIntervalMS());
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
}
void State::setPIDIntegration(double T_i)
{
    this->uar.getRegulatorPID().setT_i(T_i);
}
void State::setPIDDerrivative(double T_d)
{
    this->uar.getRegulatorPID().setT_d(T_d);
}
void State::setPIDIntegrationType(IntegType integration_type)
{
    this->uar.getRegulatorPID().setIntegrationType(integration_type);
}
void State::resetPIDIntegration()
{
    this->uar.getRegulatorPID().resetIntegrationPart();
}
void State::resetPIDDerrivative()
{
    this->uar.getRegulatorPID().resetDerrivativePart();
}


void State::setARXCoefficients(std::vector<double> a, std::vector<double> b)
{
    this->uar.getARX().setAB(a, b);
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
}
uint16_t State::getARXTransportDelay()
{
    return this->uar.getARX().getK();
}
void State::setARXInputLimits(double low, double high)
{
    this->uar.getARX().setInputLimits(low, high);
}
const std::pair<double, double>& State::getARXInputLimits()
{
    return this->uar.getARX().getInputLimits();
}
void State::setARXOutputLimits(double low, double high)
{
    this->uar.getARX().setOutputLimits(low, high);
}
const std::pair<double, double>& State::getARXOutputLimits()
{
    return this->uar.getARX().getOutputLimits();
}
void State::setARXNoiseStandardDeviation(double standard_deviation)
{
    this->uar.getARX().setStandardDeviation(standard_deviation);
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

QByteArray State::serializeState(State& state)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << (quint8)TypPakietu::FullConfig;
    out << state.getSimmulationRunning();
    out << (quint32)state.getSimmulationIntervalMS();
    out << (qint32)state.getGenerator();

    std::vector<double> a = state.getARXCoefficientsA();
    std::vector<double> b = state.getARXCoefficientsB();
    out << QList<double>(a.begin(), a.end());
    out << QList<double>(b.begin(), b.end());

    out << (quint16)state.getARXTransportDelay();
    out << state.getARXNoiseStandardDeviation();

    out << state.getARXInputLimits().first << state.getARXInputLimits().second;
    out << state.getARXOutputLimits().first << state.getARXOutputLimits().second;

    out << uar.getRegulatorPID().getK();
    out << uar.getRegulatorPID().getT_d();
    out << uar.getRegulatorPID().getT_i();
    out << uar.getRegulatorPID().getIntegrationType();

    out << choosen_generator->getAmplitude();
    out << choosen_generator->getBias();
    out << choosen_generator->getSamplesPerCycle();
    return data;
}


StateGlobalAccess State;

