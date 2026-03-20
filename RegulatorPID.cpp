#include "RegulatorPID.h"

PIDTickData RegulatorPID::tickMoreData(double e_i)
{
    double u_i_P = k * e_i;
    double u_i_I;

    if (T_i == 0.0)
    {
        u_i_I = 0;
    }
    else
    {
        sum_of_e_outside_integ += e_i;
        sum_of_e_inside_integ += e_i / T_i;
        if (integration_type == IntegType::outside)
        {
            u_i_I = sum_of_e_outside_integ / T_i;
        }
        else // IntegType::inside
        {
            u_i_I = sum_of_e_inside_integ;
        }
    }

    double u_i_D = T_d * (e_i - previous_e);
    previous_e = e_i;

    return PIDTickData{u_i_P, u_i_I, u_i_D};
}

double RegulatorPID::tick(double e_i)
{
    return static_cast<double>(tickMoreData(e_i));
}

RegulatorPID::RegulatorPID(double k, double T_i, double T_d, IntegType integration_type)
    : k(k)
    , T_i(T_i)
    , T_d(T_d)
    , previous_e(0.0)
    , integration_type(integration_type)
{
    resetIntegrationPart();
}
void RegulatorPID::setK(double k)
{
    this->k = k;
}
void RegulatorPID::setT_i(double T_i)
{
    this->T_i = T_i;
}
void RegulatorPID::setT_d(double T_d)
{
    this->T_d = T_d;
}
void RegulatorPID::setIntegrationType(IntegType integ_type)
{
    if (this->integration_type != integ_type)
    {
        this->integration_type = integ_type;

        if (integ_type == IntegType::outside)
            sum_of_e_outside_integ = sum_of_e_inside_integ * T_i;
        else
            sum_of_e_inside_integ = sum_of_e_outside_integ / T_i;
    }
}
IntegType RegulatorPID::getIntegrationType() const
{
    return integration_type;
}
double RegulatorPID::getK() const
{
    return k;
}
double RegulatorPID::getT_i() const
{
    return T_i;
}
double RegulatorPID::getT_d() const
{
    return T_d;
}
void RegulatorPID::resetIntegrationPart()
{
    sum_of_e_inside_integ = 0.0;
    sum_of_e_outside_integ = 0.0;
}
void RegulatorPID::resetDerrivativePart()
{
    previous_e = 0.0;
}
void RegulatorPID::reset()
{
    resetDerrivativePart();
    resetIntegrationPart();
}
