#include "UAR.h"

UAR::UAR(ARX &&arx, RegulatorPID &&pid)
    : arx(arx)
    , pid(pid)
    , previous_y_i(0.0)
{}
UAR::UAR(ARX &arx, RegulatorPID &pid)
    : arx(arx)
    , pid(pid)
    , previous_y_i(0.0)
{}
double UAR::tick(double input)
{
    previous_y_i = this->arx.tick(this->pid.tick(input - previous_y_i));
    return previous_y_i;
}
TickData UAR::tickMoreInfo(double input)
{
    TickData tick_data;

    tick_data.wartosc_zadana = input;
    tick_data.uchyb = tick_data.wartosc_zadana - previous_y_i;
    tick_data.sterowanie = this->pid.tickMoreData(tick_data.uchyb);
    tick_data.wartosc_regulowana = this->arx.tick(static_cast<double>(tick_data.sterowanie));
    previous_y_i = tick_data.wartosc_regulowana;
    return tick_data;
}
void UAR::resetAll()
{
    arx.reset();
    pid.reset();
    previous_y_i = 0.0;
}
ARX &UAR::getARX()
{
    return this->arx;
}
RegulatorPID &UAR::getRegulatorPID()
{
    return this->pid;
}
