#include "ARX.h"
#include <cassert>
#include <random>

double ARX::getGaussianDistribValue()
{
    static std::random_device random_device{};
    static std::mt19937 generator{random_device()};
    if (standard_deviation == 0.0)
        return 0.0;

    std::normal_distribution<double> distribution(0.0, standard_deviation);
    return distribution(generator);
}
double ARX::applyLimits(std::pair<double, double> &limits, double value)
{
    if (limits_active)
        return std::max(std::min(value, limits.second), limits.first);
    else
        return value;
}
ARX::ARX(std::vector<double> &&a, std::vector<double> &&b, uint16_t k, double standard_deviation)
    : limits_active(true),
    input_limits(std::make_pair(-10.0, 10.0)),
    output_limits(std::make_pair(-10.0, 10.0))
{
    assert(a.size() == b.size()); // Rozmiary wektorów wspołaczynników a i b są różne
    assert(k >= 1); //K nie moze byc mniejsze od 1
    this->a = a;
    this->b = b;
    this->k = k;
    this->standard_deviation = standard_deviation;
    u_i = std::deque<double>(a.size() + k, 0.0);
    y_i = std::deque<double>(a.size(), 0.0);
}
double ARX::tick(double u)
{
    assert(a.size() == b.size());

    double y = 0.0;

    u = applyLimits(input_limits, u);
    u_i.push_front(u);

    auto u_i_iter = std::next(u_i.begin(), k);
    auto y_i_iter = y_i.begin();
    for (size_t i = 0; i < a.size(); i++) {
        y += b[i] * (*u_i_iter);
        y -= a[i] * (*y_i_iter);
        u_i_iter = std::next(u_i_iter);
        y_i_iter = std::next(y_i_iter);
    }
    // gorsza optymalizacja
    // for(int i = 0; i < a.size(); i++)
    // {
    //     y += b[i] * u_i[i + k];
    //     y -= a[i] * y_i[i];
    // }

    y += getGaussianDistribValue();

    y = applyLimits(output_limits, y);

    y_i.push_front(y);
    u_i.pop_back();
    y_i.pop_back();

    return y;
}
void ARX::setK(uint16_t k)
{
    assert(k >= 1); // K nie moze byc mniejsze od 1
    this->k = k;
    while (this->u_i.size() != a.size() + k)
    {
        if (this->u_i.size() < a.size() + k)
        {
            u_i.push_back(0.0);
        } else
        {
            u_i.pop_back();
        }
    }
}
double ARX::getK() const
{
    return k;
}
const std::vector<double> ARX::getA() const
{
    return a;
}
const std::vector<double> ARX::getB() const
{
    return b;
}
void ARX::setAB(std::vector<double> a, std::vector<double> b)
{
    assert(a.size() == b.size());
    //Rozmiary wektorów wspołaczynników a i b są różne
    this->a = a;
    this->b = b;

    while (this->u_i.size() != a.size() + k)
    {
        if (this->u_i.size() < a.size() + k)
        {
            u_i.push_back(0.0);
            y_i.push_back(0.0);
        }
        else
        {
            y_i.pop_back();
            u_i.pop_back();
        }
    }
}
void ARX::reset()
{
    u_i = std::deque<double>(a.size(), 0.0);
    y_i = std::deque<double>(a.size(), 0.0);
}
void ARX::disableLimits()
{
    this->limits_active = false;
}
void ARX::enableLimits()
{
    this->limits_active = true;
}
bool ARX::getLimitsActive() const
{
    return this->limits_active;
}

void ARX::setInputLimits(double low, double high)
{
    assert(low < high);
    //Dolne ograniczenie nie może być wyższe niż górne
    this->input_limits = std::make_pair(low, high);
}
void ARX::setOutputLimits(double low, double high)
{
    assert(low < high);
    //Dolne ograniczenie nie może być wyższe niż górne
    this->output_limits = std::make_pair(low, high);
}
const std::pair<double, double>& ARX::getInputLimits() const
{
    return this->input_limits;
}
const std::pair<double, double>& ARX::getOutputLimits() const
{
    return this->output_limits;
}
void ARX::setStandardDeviation(double standard_deviation)
{
    this->standard_deviation = standard_deviation;
}
double ARX::getStandardDeviation() const
{
    return standard_deviation;
}
