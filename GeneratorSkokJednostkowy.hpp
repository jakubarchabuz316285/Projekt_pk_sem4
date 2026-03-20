#ifndef GENERATORSKOKJEDNOSTKOWY_H
#define GENERATORSKOKJEDNOSTKOWY_H

#include "Generator.h"
#include <cstdint>
class GeneratorSkokJednostkowy : public Generator
{
    uint32_t activation_time_int_tikcs;
public:
    GeneratorSkokJednostkowy();
    void setActivationTime(uint32_t activation_time_ticks);
    uint32_t getActivationTime() const;
    double tick() override;
    void resetClock();
};

#endif // GENERATORSKOKJEDNOSTKOWY_H
