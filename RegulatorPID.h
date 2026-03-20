#ifndef REGULATOR_PID_H
#define REGULATOR_PID_H

/**
 * @brief typ całkowania
 *
 */
enum class IntegType {
    outside,
    insde,
};

/**
 * @brief przechowuje informacje o jednym ticku symulacji regulatora
 *
 */
struct PIDTickData
{
    double Proportional;
    double Integral;
    double Derrivative;
    explicit operator double()
    {
        return Proportional + Integral + Derrivative;
    }
};

/**
 * @brief klasa implementująca regulator PID
 *
 */
class RegulatorPID
{
    double k; /**< TODO: describe */
    double T_i; /**< TODO: describe */
    double T_d; /**< TODO: describe */
    double sum_of_e_inside_integ; /**< TODO: describe */
    double sum_of_e_outside_integ; /**< TODO: describe */
    double previous_e; /**< TODO: describe */
    IntegType integration_type; /**< TODO: describe */

public:

double tick(double e_i);
    /**
     * @brief funkcja dokonująca jednego kroku symulacji z większą ilością danych
     *
     * @param e_i
     * @return double
     */
PIDTickData tickMoreData(double e_i);

    RegulatorPID(double k,
                 double T_i = 0.0,
                 double T_d = 0.0,
                 IntegType integration_type = IntegType::outside);

void setK(double k);

void setT_i(double T_i);

void setT_d(double T_d);

void setIntegrationType(IntegType integ_type);

IntegType getIntegrationType() const;

double getK() const;

double getT_i() const;

double getT_d() const;

void resetIntegrationPart();

void resetDerrivativePart();

void reset();
};

#endif // REGULATOR_PID_H
