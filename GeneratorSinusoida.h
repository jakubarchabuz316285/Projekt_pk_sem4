#ifndef GENERATORSINUSOIDA_H
#define GENERATORSINUSOIDA_H

#include "Generator.h"

class GeneratorSinusoida : public Generator
{
public:
    GeneratorSinusoida();
    double tick() override;
};

#endif // GENERATORSINUSOIDA_H
