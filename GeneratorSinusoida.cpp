#include "GeneratorSinusoida.h"
#define _USE_MATH_DEFINES
#include <cmath>

GeneratorSinusoida::GeneratorSinusoida()
    : Generator()
{}
double GeneratorSinusoida::tick()
{
    constexpr const double DEGEES_TO_RADIAN = 2.0 * M_PI;
    double cycle_moment = ((double) (internal_clock % samples_per_cycle) / samples_per_cycle) * DEGEES_TO_RADIAN;
    return Generator::advanceClockAndReturn(amplitude * std::sin(cycle_moment));
}
