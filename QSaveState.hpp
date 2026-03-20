#ifndef QSAVESTATE_HPP
#define QSAVESTATE_HPP
#include "State.h"

class QSaveState : public SaveStateInterface
{
    QJsonArray vectorToJsonArray(const std::vector<double>& vec);
    std::vector<double> qJsonArrayToVector(const QJsonArray& jsonArray);
public:
    QSaveState();
    void saveToFile(std::string& path, UAR* uar, bool simmulation_running, State::TypGeneratora typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin) override;
    void readFromFile(std::string& path, UAR* uar, bool* simmulation_running, State::TypGeneratora* typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin) override;
};

#endif // QSAVESTATE_HPP
