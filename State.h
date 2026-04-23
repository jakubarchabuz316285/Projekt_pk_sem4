#ifndef STATE_H
#define STATE_H
#include "GeneratorProstokatny.h"
#include "GeneratorSinusoida.h"
#include "UAR.h"
#include "GeneratorSkokJednostkowy.hpp"
#include "networkmanager.h"

class SaveStateInterface;
class TimerStateInterface;

struct RegulatorInstancePackage{
    // PID
    double k;
    double T_i;
    double T_d;
    int integType;
    // GEN
    double amplitude;
    uint16_t samples_per_cycle;
    double bias;
    int genType; // 0 - sin, 1 - pros, 2 - skok
    // SIM
    uint8_t interval;
};

struct ArxInstancePackage{
    int vec_size;
    std::vector<double> wsp_a;
    std::vector<double> wsp_b;
    uint16_t transport_delay;
    double noise;
    double input_min;
    double input_max;
    double output_min;
    double output_max;
    bool is_limited;
};

/**
 * @brief Klasa warstwy usług
 *
 */
class State : public QObject
{
    Q_OBJECT
public:

    enum class TypGeneratora { Sinusoidalny = 0, Prostokatny = 1, SkokJednostkowy = 2 };
    enum class TypPakietu : quint8 { PidConfig = 1, ArxConfig = 2, PIDSample = 3,
                                     ARXSample = 4, ResetGen = 5, ResetPidIntegration = 6, ResetPidDerrivative = 7,
                                     SimStart = 8, SimStop = 9, SimReset = 10};
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
    RegulatorInstancePackage getPIDConfig();
    ArxInstancePackage getArxConfig();

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

    QByteArray serializeSample(double value, TypPakietu typ);

    //Pid i Gen serializacja

    QByteArray serializePIDState(const RegulatorInstancePackage& data);
    QByteArray serializeARXState(const ArxInstancePackage& data);

    void deserializeAndApply(const QByteArray& data);

    void receivePacket(const QByteArray& packet);
    void sendPacket();
    void console_print_state();
    // ONLINE

    bool isReadyForNextTick() const
    {
        return readyForNextTick;
    }

    void setMode(const NetworkManager::Mode& mode)
    {
        _networkManager.SetMode(mode);
    }
    NetworkManager::Mode getMode() const
    {
        return _networkManager.GetMode();
    }

    void connect(const QString& ip, int port){
        _networkManager.Connect(ip, port);
    }

    void disconnect(){
        _networkManager.Disconnect();
    }

    void startListening(int port){
        qDebug() << "Starting to listen : (state)";
        _networkManager.StartListening(port);
        qDebug() << "Starting to listen : (state po funkcji)";
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
    TickData current_tick_data;
    bool readyForNextTick;

    // Online

    NetworkManager _networkManager;

    State(const State &) = delete;
    State &operator=(const State &) = delete;
    State();
    ~State();

signals:
    void statusChanged(bool connected);
    void requestUiUpdate();


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
