#include "GeneratorSkokJednostkowy.hpp"

GeneratorSkokJednostkowy::GeneratorSkokJednostkowy()
    :activation_time_int_tikcs{0}
{}

void GeneratorSkokJednostkowy::setActivationTime(uint32_t activation_time_ticks)
{
    this->activation_time_int_tikcs = activation_time_ticks;
}
uint32_t GeneratorSkokJednostkowy::getActivationTime() const
{
    return activation_time_int_tikcs;
}
void GeneratorSkokJednostkowy::resetClock()
{
    activation_time_int_tikcs = 0;
}
double GeneratorSkokJednostkowy::tick()
{
    if(activation_time_int_tikcs > 0)
        activation_time_int_tikcs--;
    if(activation_time_int_tikcs == 0)
        return 1.0 * amplitude + bias;
    else
        return 0.0 + bias;
}
