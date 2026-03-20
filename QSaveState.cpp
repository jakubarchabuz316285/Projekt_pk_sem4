#include "QSaveState.hpp"
#include <qdir.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

QSaveState::QSaveState() {}

QJsonArray QSaveState::vectorToJsonArray(const std::vector<double>& vec)
{
    QJsonArray jsonArray;
    for (const auto& value : vec)
    {
        jsonArray.append(QJsonValue(value));
    }
    return jsonArray;
}
std::vector<double> QSaveState::qJsonArrayToVector(const QJsonArray& jsonArray)
{
    std::vector<double> result;
    result.reserve(jsonArray.size());
    for (const QJsonValue& value : jsonArray)
    {
        if (value.isDouble())
            result.push_back(value.toDouble());
    }
    return result;
}

void QSaveState::saveToFile(std::string& path, UAR* uar, bool simmulation_running, State::TypGeneratora typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin)
{
    QJsonObject jsonObject;

    QJsonObject jsonAXR;
    QJsonObject jsonPID;
    QJsonObject jsonGenSin;
    QJsonObject jsonGenPros;

    jsonObject["simmulation_running"] = simmulation_running;
    jsonObject["wybrany_typ_generatora"] = (int)typ;
    jsonAXR["k"] = uar->getARX().getK();
    jsonAXR["standard_deviation"] = uar->getARX().getStandardDeviation();
    jsonAXR["input_limit_low"] = uar->getARX().getInputLimits().first;
    jsonAXR["input_limit_high"] = uar->getARX().getInputLimits().second;
    jsonAXR["output_limit_low"] = uar->getARX().getOutputLimits().first;
    jsonAXR["output_limit_high"] = uar->getARX().getOutputLimits().second;
    jsonAXR["limits_active"] = uar->getARX().getLimitsActive();
    jsonAXR["A"] = vectorToJsonArray(uar->getARX().getA());
    jsonAXR["B"] = vectorToJsonArray(uar->getARX().getB());
    jsonObject["arx"] = jsonAXR;

    jsonPID["itegration_type"] = (int)uar->getRegulatorPID().getIntegrationType();
    jsonPID["k"] = uar->getRegulatorPID().getK();
    jsonPID["T_i"] = uar->getRegulatorPID().getT_i();
    jsonPID["T_d"] = uar->getRegulatorPID().getT_d();
    jsonObject["pid"] = jsonPID;

    jsonGenSin["amplitude"] = gen_sin->getAmplitude();
    jsonGenSin["samples_per_cycle"] = gen_sin->getSamplesPerCycle();
    jsonGenSin["bias"] = gen_sin->getBias();
    jsonObject["generator_sinusoidalny"] = jsonGenSin;

    jsonGenPros["amplitude"] = gen_pros->getAmplitude();
    jsonGenPros["samples_per_cycle"] = gen_pros->getSamplesPerCycle();
    jsonGenPros["bias"] = gen_pros->getBias();
    jsonGenPros["duty_cycle"] = gen_pros->getDutyCycle();
    jsonObject["generator_prostokatny"] = jsonGenPros;

    QJsonDocument jsonDoc(jsonObject);

    QFile file(path.c_str());
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Could not open file for writing";
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
}
void QSaveState::readFromFile(std::string& path, UAR* uar, bool* simmulation_running, State::TypGeneratora* typ, GeneratorProstokatny* gen_pros, GeneratorSinusoida* gen_sin)
{
    QFile file(path.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open file for reading";
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qWarning() << "Invalid JSON format in file. U stupid" ;
        return;
    }
    QJsonObject jsonObject = jsonDoc.object();

    QJsonObject jsonARX = jsonObject["arx"].toObject();
    QJsonObject jsonPID = jsonObject["pid"].toObject();
    QJsonObject jsonGenSin = jsonObject["generator_sinusoidalny"].toObject();
    QJsonObject jsonGenPros = jsonObject["generator_prostokatny"].toObject();

    *simmulation_running = jsonObject["simmulation_running"].toBool();
    *typ = (State::TypGeneratora)jsonObject["wybrany_typ_generatora"].toInt();
    uar->getARX().setK(jsonARX["k"].toDouble());
    uar->getARX().setStandardDeviation(jsonARX["standard_deviation"].toDouble());
    uar->getARX().setInputLimits(jsonARX["input_limit_low"].toDouble(), jsonARX["input_limit_high"].toDouble());
    uar->getARX().setOutputLimits(jsonARX["output_limit_low"].toDouble(), jsonARX["output_limit_high"].toDouble());
    if(jsonARX["limits_active"].toBool())
        uar->getARX().enableLimits();
    else
        uar->getARX().disableLimits();
    uar->getARX().setAB(qJsonArrayToVector(jsonARX["A"].toArray()), qJsonArrayToVector(jsonARX["B"].toArray()));

    uar->getRegulatorPID().setIntegrationType((IntegType)jsonPID["itegration_type"].toInt());
    uar->getRegulatorPID().setK(jsonPID["k"].toDouble());
    uar->getRegulatorPID().setT_i(jsonPID["T_i"].toDouble());
    uar->getRegulatorPID().setT_d(jsonPID["T_d"].toDouble());

    gen_sin->setAmplitude(jsonGenSin["amplitude"].toDouble());
    gen_sin->setSamplesPerCycle(jsonGenSin["samples_per_cycle"].toInt());
    gen_sin->setBias(jsonGenSin["bias"].toDouble());

    gen_pros->setAmplitude(jsonGenPros["amplitude"].toDouble());
    gen_pros->setSamplesPerCycle(jsonGenPros["samples_per_cycle"].toInt());
    gen_pros->setBias(jsonGenPros["bias"].toDouble());
    gen_pros->setDutyCycle(jsonGenPros["duty_cycle"].toDouble());

}
