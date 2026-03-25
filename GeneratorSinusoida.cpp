#include "GeneratorSinusoida.h"
#define _USE_MATH_DEFINES
#include <cmath>

GeneratorSinusoida::GeneratorSinusoida()
    : Generator()
{}
/*
double GeneratorSinusoida::tick()
{
    constexpr const double DEGEES_TO_RADIAN = 2.0 * M_PI;
    double cycle_moment = ((double) (internal_clock % samples_per_cycle) / samples_per_cycle) * DEGEES_TO_RADIAN;
    cycle_moment = ((double)internal_clock / samples_per_cycle) * 2.0 * M_PI;
    return Generator::advanceClockAndReturn(amplitude * std::sin(cycle_moment));
}
*/
double GeneratorSinusoida::tick() {
    double cycle_pos = static_cast<double>(internal_clock) / samples_per_cycle; // dokładne pozycje w cyklu
    double output = amplitude * std::sin(cycle_pos * 2.0 * M_PI);
    return Generator::advanceClockAndReturn(output); // zwiększa internal_clock
}

