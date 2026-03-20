#ifndef ARX_H
#define ARX_H
#include "Test.h"
#include <cstdint>
#include <deque>
#include <vector>

/**
 * @brief
 * Klasa implementująca obiek ARX
 */
class ARX
{
    friend class Test;

private:
    std::vector<double> a; /**< tablica współczyników A */
    std::vector<double> b; /**< tablica współczynników B */
    uint16_t k; /**< Opóźnienie transportowe */
    double standard_deviation; /**< Odchylenie Standardowe */
    std::deque<double> y_i; /**< poprzednie wartości wyjścia */
    std::deque<double> u_i; /**< poprzednie wartości wejścia */
    bool limits_active; /**< czy limity na wejściu i wyjściu obiektu są aktywne */
    std::pair<double, double> output_limits; /**< wartości limitów na wyjąsiu obiektu */
    std::pair<double, double> input_limits; /**< wartości limitów na wejściu obiektu */
    /**
     * @brief Zapewnia, że wartości mięsci się w limitach (nasycenie)
     *
     * @param std::pair<double, double> limits limity jakich ma się trzymać funkcja
     * @param double value wartości do nałożenie limitów
     * @return double wartości po nałożeniu limitów
     */
inline double applyLimits(std::pair<double, double> &limits, double value);
    /**
     * @brief losuje wartość z dystrybucji Gausjańskiej
     *
     * @return double wylosowana wartość
     */
inline double getGaussianDistribValue();

public:
    /**
     * @brief Konstruktowr obiektu ARX
     *
     * @param a
     * @param b
     * @param k
     * @param standard_deviation
     */
    ARX(std::vector<double> &&a,
        std::vector<double> &&b,
        uint16_t k = 1,
        double standard_deviation = 0.0);
    /**
     * @brief Wynikuje jeden krok symualcji dla obiektu
     *
     * @param u wartość na wejściu
     * @return double wartośc na wyjściu
     */
double tick(double u);
    /**
     * @brief ustawia opóżnienie transportowe
     *
     * @param k
     */
void setK(uint16_t k);
    /**
     * @brief opóżnienie transportowe
     *
     * @return double
     */
double getK() const;
    /**
     * @brief tablica współczynników A
     *
     * @return const std::vector<double>
     */
const std::vector<double> getA() const;
    /**
     * @brief tablica współczynników B
     *
     * @return const std::vector<double>
     */
const std::vector<double> getB() const;
    /**
     * @brief ustawia tablice współczynników A i B. Zwaradca wyjątek przy niepowodzeniu
     *
     * @param a
     * @param b
     */
void setAB(std::vector<double> a, std::vector<double> b);
    /**
     * @brief resetuje wewnętrzene bufory obiektu. Przydantne podczas resetowania symualcji UAR
     *
     */
void reset();
    /**
     * @brief wyłącza limity na wejściu i wyjściu
     *
     */
void disableLimits();
    /**
     * @brief włącza limity na wejściu i wyjściu
     *
     */
void enableLimits();
    /**
     * @brief informuje czy aktywne są limity na wejściu i wyjściu
     *
     * @return bool
     */
bool getLimitsActive() const;
    /**
     * @brief ustawia limity na wejściu
     *
     * @param low
     * @param high
     */
void setInputLimits(double low, double high);
    /**
     * @brief ustawia limity na wyjściu
     *
     * @param low
     * @param high
     */
void setOutputLimits(double low, double high);
    /**
     * @brief zwraca limity na wejściu
     *
     * @return const std::pair<double, double>
     */
const std::pair<double, double>& getInputLimits() const;
    /**
     * @brief ustawia limity na wyjściu
     *
     * @return const std::pair<double, double>
     */
const std::pair<double, double>& getOutputLimits() const;
    /**
     * @brief ustawia wartość odchylenia dla zakłócen obiektu. Wartość 0.0 wyłącza zakłówcenia
     *
     * @param standard_deviation
     */
void setStandardDeviation(double standard_deviation);
    /**
     * @brief zwraca wartość odchylenia dla zakłócen obiektu
     *
     * @return double
     */
double getStandardDeviation() const;
};

#endif // ARX_H
