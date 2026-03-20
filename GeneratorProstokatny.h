#ifndef GENERATORPROSTOKATNY_H
#define GENERATORPROSTOKATNY_H

#include "Generator.h"

class GeneratorProstokatny : public Generator
{
    double duty_cycle;

public:
    GeneratorProstokatny();
    double tick() override;
    void setDutyCycle(double duty_cycle);
    double getDutyCycle() const;
};

#endif // GENERATORPROSTOKATNY_H
