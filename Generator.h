#ifndef GENERATOR_H
#define GENERATOR_H
#include <cstdint>

/**
 * @brief
 *
 */
class Generator
{
protected:
    double amplitude; /**< amplituda generowanego sygnału */
    uint16_t samples_per_cycle; /**< ile impulsów zegarywch an jeden okres */
    double bias; /**< offset sygnału */

    // ERROR - kamil

    // uint16_t internal_clock; /**< wartośc wewnętrzna. nie ruszać */
    char internal_clock; /**< wartośc wewnętrzna. nie ruszać */

    /**
     * @brief funckja zwiększa zegar wewnętrzny i zwraca wartość podaną jako arguemnt + bias
     *
     * @param double
     * @return double
     */
double advanceClockAndReturn(double);
public:
    /**
     * @brief
     *
     */
    Generator();
    /**
     * @brief
     *
     * @return double
     */
virtual double tick() = 0;
    /**
     * @brief
     *
     * @param amplitude
     */
void setAmplitude(double amplitude);
    /**
     * @brief
     *
     * @param samples_per_cycle
     */
void setSamplesPerCycle(uint16_t samples_per_cycle);
    /**
     * @brief
     *
     * @param bias
     */
void setBias(double bias);
    /**
     * @brief
     *
     * @return double
     */
double getAmplitude() const;
    /**
     * @brief
     *
     * @return uint16_t
     */
uint16_t getSamplesPerCycle() const;
    /**
     * @brief
     *
     * @return double
     */
double getBias() const;
    /**
     * @brief
     *
     */
void resetClock();
};

#endif // GENERATOR_H
