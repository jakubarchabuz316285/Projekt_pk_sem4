#ifndef UAR_H
#define UAR_H
#include "ARX.h"
#include "RegulatorPID.h"

/**
 * @brief Prechowuje inforamcje o jednym tiku symulacji
 *
 */
struct TickData
{
    double wartosc_zadana;
    double wartosc_regulowana;
    double uchyb;
    PIDTickData sterowanie;
};

class UAR
{
    ARX arx;
    RegulatorPID pid;
    double previous_y_i;

public:
    UAR(ARX &&arx, RegulatorPID &&pid);
    UAR(ARX &arx, RegulatorPID &pid);
    double tick(double input);
    TickData tickMoreInfo(double input);
    void resetAll();
    ARX &getARX();
    RegulatorPID &getRegulatorPID();
};

#endif // UAR_H
