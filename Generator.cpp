#include "Generator.h"
#include "qdebug.h"
#include "qglobal.h"
#include <cassert>

Generator::Generator()
    : amplitude(1.0)
    , samples_per_cycle(2)
    , bias(0.0)
    , internal_clock(0)
{}

double Generator::advanceClockAndReturn(double value)
{
    internal_clock++;
    if (internal_clock == samples_per_cycle)
        internal_clock = 0;
    last_output = value + bias;
    return last_output;
}
void Generator::setAmplitude(double amplitude)
{
    assert(amplitude >= 0.0);
    this->amplitude = amplitude;
}
void Generator::setSamplesPerCycle(uint16_t samples_per_cycle)
{
    assert(samples_per_cycle >= 1);

    // zmiana długości cyklu nie zmienia momentu w którym aktualnie jest generator w cyklu
    //this->internal_clock = ((double) this->internal_clock / this->samples_per_cycle) * samples_per_cycle;
    this->internal_clock = this->internal_clock % samples_per_cycle;
    this->samples_per_cycle = samples_per_cycle;
}
void Generator::setBias(double bias)
{
    this->bias = bias;
}
double Generator::getAmplitude() const
{
    return this->amplitude;
}
uint16_t Generator::getSamplesPerCycle() const
{
    return this->samples_per_cycle;
}
double Generator::getBias() const
{
    return this->bias;
}
void Generator::resetClock()
{
    internal_clock = 0;
}
double Generator::getLastOutput(){
    return last_output;
}
