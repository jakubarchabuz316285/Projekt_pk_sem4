#include "GeneratorProstokatny.h"
#include <stdexcept>

GeneratorProstokatny::GeneratorProstokatny()
    : Generator()
    , duty_cycle(0.5)
{}
double GeneratorProstokatny::tick()
{
    if (internal_clock % samples_per_cycle < duty_cycle * samples_per_cycle)
        return Generator::advanceClockAndReturn(amplitude);
    else
        return Generator::advanceClockAndReturn(0);
}
void GeneratorProstokatny::setDutyCycle(double duty_cycle)
{
    if (0.0 > duty_cycle || duty_cycle > 1.0)
        throw std::invalid_argument("duty_cycle must be between 0.0 and 1.0");
    this->duty_cycle = duty_cycle;
}
double GeneratorProstokatny::getDutyCycle() const
{
    return this->duty_cycle;
}
