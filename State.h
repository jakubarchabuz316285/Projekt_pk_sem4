#ifndef STATE_H
#define STATE_H
#include "GeneratorProstokatny.h"
#include "GeneratorSinusoida.h"
#include "UAR.h"
#include "GeneratorSkokJednostkowy.hpp"
#include "networkmanager.h"

class SaveStateInterface;
class TimerStateInterface;



/**
 * @brief Klasa warstwy usług
 *
 */
class State
{
public:

    enum class TypGeneratora { Sinusoidalny, Prostokatny, SkokJednostkowy };
    enum class TypPakietu : quint8 { USample = 1, YSample = 2, FullConfig = 3, PIDSample = 4, GENSample = 5};
    struct Packet
    {
        TypPakietu type;
        double value;
        bool valid;
    };
    static State &getInstance();
    void setSimmulationRunning(bool simmulation_running);
    bool getSimmulationRunning();
    void setSimmulationIntervalMS(uint32_t interval);
    uint32_t getSimmulationIntervalMS();
    void setOutputCallback(const std::function<void(TickData)> callback);
    void resetSimmulation();

    void setGenerator(TypGeneratora type);
    TypGeneratora getGenerator();
    void setGeneneratorAmplitude(const double& amplitude);
    void setGeneneratorDutyCycle(const double& duty_cycle);
    void setGeneneratorPeriodMS(uint32_t period);
    void setGeneratorSkladowaStala(double skladowa_stala);
    uint8_t getGeneneratorPeriodJumpMS();
    void setGeneratorUnitJumpTimeMS(uint32_t time);
    void resetGenerator();

    void setPIDProportional(double k);
    void setPIDIntegration(double T_i);
    void setPIDDerrivative(double T_d);
    void setPIDIntegrationType(IntegType integration_type);
    void resetPIDIntegration();
    void resetPIDDerrivative();

    void setARXCoefficients(std::vector<double> a, std::vector<double> b);
    const std::vector<double> getARXCoefficientsA();
    const std::vector<double> getARXCoefficientsB();
    void setARXTransportDelay(uint16_t k);
    uint16_t getARXTransportDelay();
    void setARXInputLimits(double low, double high);
    const std::pair<double, double>& getARXInputLimits();
    void setARXOutputLimits(double low, double high);
    const std::pair<double, double>& getARXOutputLimits();
    void setARXNoiseStandardDeviation(double standard_deviation);
    double getARXNoiseStandardDeviation();
    void setARXLimitsEnabled(bool enabled);
    void resetARX();

    void setSaveStateObject(SaveStateInterface* object);
    void saveToFile(std::string path);
    void readFromFile(std::string path);

    const std::tuple<const ARX*, const RegulatorPID*, const TypGeneratora,  const GeneratorSinusoida*, const GeneratorProstokatny*> getAppState();
    void tick();

    QByteArray serializeU(double u);
    QByteArray serializeY(double y);
    QByteArray serializeState(State& state);

    //Pid i Gen serializacja

    QByteArray serializePIDOutput();
    Packet deserialize(const QByteArray& data);

    void receivePacket(QByteArray packet);
    void sendPacket();

    // ONLINE

    void setMode(NetworkManager::Mode mode){
        _networkManager.SetMode(mode);
    }

    void connect(const QString& ip, int port){
        _networkManager.Connect(ip, port);
    }

    void disconnect(){
        _networkManager.Disconnect();
    }

    void startListening(int port){
        _networkManager.StartListening(port);
    }

    void stopListening(){
        _networkManager.StopListening();
    }

    bool isListening(){
        return _networkManager.IsListening();
    }

private:
    UAR uar;
    GeneratorSinusoida gen_sin;
    GeneratorProstokatny gen_pros;
    GeneratorSkokJednostkowy gen_skok;
    Generator* choosen_generator;
    bool simmulation_running;
    std::function<void(TickData)> tick_callback;
    SaveStateInterface* save;
    TimerStateInterface* timer;

    // Online

    NetworkManager _networkManager;

    State(const State &) = delete;
    State &operator=(const State &) = delete;
    State();
    ~State();


};

class SaveStateInterface
{
public:
    virtual ~SaveStateInterface() = default;
    virtual void saveToFile(std::string& path, UAR* uar, bool simmulation, State::TypGeneratora typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin) = 0;
    virtual void readFromFile(std::string& path, UAR* uar, bool* simmulation, State::TypGeneratora* typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin) = 0;
};

class TimerStateInterface
{
public:
    virtual ~TimerStateInterface() = default;
    virtual void setTimeout(std::function<void()>) = 0;
    virtual void setIntervalMS(unsigned int) = 0;
    virtual unsigned int getIntervalMS() = 0;
    virtual void setRunning(bool running) = 0;
};

class StateGlobalAccess
{
public:
    State& operator()();
};
extern StateGlobalAccess State;
#endif // STATE_H
