#include "Test.h"
#include "ListWithWxtremes.hpp"
#include "State.h"
#include <filesystem>
#include <random>

void Test::wykonajTesty()
{
    std::array testy = {
        TEST_ListWithExtremes_poprawne_dzialanie,
        TEST_State_czy_jest_signletonem,
        TEST_State_zapis_i_odczyt_z_pliku,
        TEST_Generator_Prostokatny_zmiana_okresu,
        TEST_Generator_Prostokatny_zmiana_amplitudy,
        TEST_Generator_Prostokatny_bias,
        TEST_Generator_Prostokatny_100_procent,
        TEST_Generator_Prostokatny_50_procent,
        TEST_Generator_Prostokatny_10_procent,
        TEST_Generator_Sinusoidalny,
        TEST_Generator_Sinusoidalny_bias,
        TEST_Generator_Sinusoidalny_zmiana_amplitudy,
        TEST_Generator_Sinusoidalny_zmiana_okresu,
        TEST_PID_czlon_P_regulacja_odwrotna,
        TEST_PID_czlon_D_przyspiesza_regulacje,
        TEST_ARX_zaklocenia,
        TEST_ARX_nietypowe_AB,
        TEST_ARX_ustawienie_K,
    };
    std::shuffle(testy.begin(), testy.end(), std::random_device() );


    std::map<const char*, bool> wyniki_testow;
    for(auto& test : testy)
    {
        auto wynik = test();
        wyniki_testow[wynik.second] = wynik.first;
    };

    for (auto& wynik : wyniki_testow)
    {
        if(!wynik.second)
        {
            qDebug() << wynik.first << " [ FAIL ]\n";
        }
    }
    qDebug() << "TESTY POMYŚLNE: [ "
             << std::count_if(wyniki_testow.begin(), wyniki_testow.end(),
                    [](std::pair<const char*, bool> wynik){return wynik.second;})
             << " / " << wyniki_testow.size() << " ]";

}

std::pair<bool, const char*> Test::TEST_ListWithExtremes_poprawne_dzialanie()
{
    try
    {
        ListWithExtremes lista;
        bool test_passed = true;

        lista.appendLastValue(QPointF(1.0, 4.0));
        if(lista.max() != 4.0) test_passed = false;
        if(lista.min() != 4.0) test_passed = false;

        lista.appendLastValue(QPointF(1.1, 4.0));
        if(lista.max() != 4.0) test_passed = false;
        if(lista.min() != 4.0) test_passed = false;

        lista.appendLastValue(QPointF(1.8, 7.0));
        if(lista.max() != 7.0) test_passed = false;
        if(lista.min() != 4.0) test_passed = false;


        lista.appendLastValue(QPointF(1.8, 9.0));
        if(lista.max() != 9.0) test_passed = false;
        if(lista.min() != 4.0) test_passed = false;

        lista.deleteFirstValue();
        lista.deleteFirstValue();
        if(lista.max() != 9.0) test_passed = false;
        if(lista.min() != 7.0) test_passed = false;


        lista.appendLastValue(QPointF(1.8, -1.0));
        if(lista.max() != 9.0)  test_passed = false;
        if(lista.min() != -1.0) test_passed = false;
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_State_czy_jest_signletonem()
{
    try
    {

        class State* adres_State1 = &State::getInstance();
        class State* adres_State2 = &State::getInstance();

        return std::make_pair(adres_State1 == adres_State2, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_State_zapis_i_odczyt_z_pliku()
{
    try
    {
        bool test_passed = true;
        State::getInstance().setGeneneratorAmplitude(1.0);
        State::getInstance().setGeneneratorPeriodMS(100);
        State::getInstance().setGeneneratorDutyCycle(1.0);
        State::getInstance().setGenerator(State::TypGeneratora::Prostokatny);
        State::getInstance().setARXCoefficients({1.0, 2.0, 3.0, 4.0}, {1.0, 2.0, 3.0, 4.0});
        State::getInstance().setARXTransportDelay(1);
        State::getInstance().setARXNoiseStandardDeviation(1.0);
        //TODO RESZTA PARAMATROW DO ZAPISU

        State::getInstance().saveToFile(std::string("C:\\testowy.json"));
        State::getInstance().setARXTransportDelay(2);
        State::getInstance().readFromFile(std::string("C:\\testowy.json"));

        if(State::getInstance().getARXTransportDelay() != 1)
            test_passed = false;


        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_zmiana_amplitudy()
{

    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(2);
        gen.setDutyCycle(1.0);

        gen.setAmplitude(10.0);
        if(gen.tick() != 10.0) test_passed = false;

        gen.setAmplitude(5.4);
        if(gen.tick() != 5.4) test_passed = false;

        gen.setAmplitude(0.0);
        if(gen.tick() != 0.0) test_passed = false;

        gen.setAmplitude(515.0);
        if(gen.tick() != 515.0) test_passed = false;
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_zmiana_okresu()
{

    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setDutyCycle(0.5);
        gen.setAmplitude(1.0);

        gen.setSamplesPerCycle(2);
        gen.resetClock();
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;

        gen.setSamplesPerCycle(6);
        gen.resetClock();
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_bias()
{

    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(2);
        gen.setDutyCycle(1.0);
        gen.setAmplitude(1.0);

        gen.setBias(0.0);
        if(gen.tick() != 1.0) test_passed = false;

        gen.setBias(1.0);
        if(gen.tick() != 2.0) test_passed = false;

        gen.setBias(-1.0);
        if(gen.tick() != 0.0) test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_100_procent()
{

    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(2);
        gen.setDutyCycle(1.0);
        gen.setAmplitude(1.0);

        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_50_procent()
{

    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(6);
        gen.setDutyCycle(0.5);
        gen.setAmplitude(1.0);

        gen.resetClock();
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Prostokatny_10_procent()
{
    try
    {
        bool test_passed = true;
        GeneratorProstokatny gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(10);
        gen.setDutyCycle(0.1);
        gen.setAmplitude(1.0);

        gen.resetClock();
        if(gen.tick() != 1.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 0.0) test_passed = false;
        if(gen.tick() != 1.0) test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Sinusoidalny()
{
    try
    {
        bool test_passed = true;
        GeneratorSinusoida gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(18);
        gen.setAmplitude(1.0);

        double oczekiwany_sygnal[] = {
            0.0,
            0.34202014,
            0.64278761,
            0.8660254 ,
            0.98480775,
            0.98480775,
            0.8660254 ,
            0.64278761,
            0.34202014,
            0.0,
            -0.34202014,
            -0.64278761,
            -0.8660254 ,
            -0.98480775,
            -0.98480775,
            -0.8660254 ,
            -0.64278761,
            -0.34202014,
        };
        for (double& oczekiwanie : oczekiwany_sygnal)
        {
            if(!porownajZTolerancja(gen.tick(), oczekiwanie))
                test_passed = false;
        }
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Sinusoidalny_bias()
{   try
    {
        bool test_passed = true;
        GeneratorSinusoida gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(18);
        gen.setAmplitude(1.0);

        double oczekiwany_sygnal[] = {
            0.0,
            0.34202014,
            0.64278761,
            0.8660254 ,
            0.98480775,
            0.98480775,
            0.8660254 ,
            0.64278761,
            0.34202014,
            0.0,
            -0.34202014,
            -0.64278761,
            -0.8660254 ,
            -0.98480775,
            -0.98480775,
            -0.8660254 ,
            -0.64278761,
            -0.34202014,
        };
        gen.setBias(4.0);
        for (double& oczekiwanie : oczekiwany_sygnal)
        {
            if(!porownajZTolerancja(gen.tick(), oczekiwanie + 4.0))
                test_passed = false;
        }
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Sinusoidalny_zmiana_amplitudy()
{   try
    {
        bool test_passed = true;
        GeneratorSinusoida gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(18);
        gen.setAmplitude(1.0);

        double oczekiwany_sygnal[] = {
            0.0,
            0.34202014,
            0.64278761,
            0.8660254 ,
            0.98480775,
            0.98480775,
            0.8660254 ,
            0.64278761,
            0.34202014,
            0.0,
            -0.34202014,
            -0.64278761,
            -0.8660254 ,
            -0.98480775,
            -0.98480775,
            -0.8660254 ,
            -0.64278761,
            -0.34202014,
        };
        gen.setAmplitude(5.34);
        for (double& oczekiwanie : oczekiwany_sygnal)
        {
            if(!porownajZTolerancja(gen.tick(), oczekiwanie * 5.34))
                test_passed = false;
        }
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}
std::pair<bool, const char*> Test::TEST_Generator_Sinusoidalny_zmiana_okresu()
{   try
    {
        bool test_passed = true;
        GeneratorSinusoida gen;
        gen.setBias(0.0);
        gen.setSamplesPerCycle(60);
        gen.setAmplitude(1.0);

        double oczekiwany_sygnal_okres_60[] = {
            0.0,
            0.10452846,
            0.20791169,
            0.30901699,
            0.40673664,
            0.5,
        };
        double oczekiwany_sygnal_okres_7[] = {
            0.0,
            0.78183148,
            0.97492791,
            0.43388374,
            -0.43388374,
            -0.97492791,
            -0.78183148,
            0.0,
        };
        for (double& oczekiwanie : oczekiwany_sygnal_okres_60)
        {
            if(!porownajZTolerancja(gen.tick(), oczekiwanie))
                test_passed = false;
        }
        gen.setSamplesPerCycle(7);
        gen.resetClock();
        for (double& oczekiwanie : oczekiwany_sygnal_okres_7)
        {
            if(!porownajZTolerancja(gen.tick(), oczekiwanie))
                test_passed = false;
        }
        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}

std::pair<bool, const char*> Test::TEST_PID_czlon_P_regulacja_odwrotna()
{
    try
    {
        bool test_passed = true;
        RegulatorPID pid(-1.0);
        double sygnal_wejsciowy[] = {
            -346.0,
            -333.0,
            -536.0,
            -12.0,
            6.0,
        };
        double oczekiwany_sygnal_wyjsciowy[] = {
            346.0,
            333.0,
            536.0,
            12.0,
            -6.0,
        };
        constexpr const size_t ROZMIAR_DANYCH_TESTOWYCH = sizeof(sygnal_wejsciowy) / sizeof(double);
        for (size_t i = 0; i < ROZMIAR_DANYCH_TESTOWYCH; i++)
        {
            if(!porownajZTolerancja(pid.tick(sygnal_wejsciowy[i]), oczekiwany_sygnal_wyjsciowy[i]))
                test_passed = false;
        }

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}

std::pair<bool, const char*> Test::TEST_PID_czlon_D_przyspiesza_regulacje()
{
    try
    {
        bool test_passed = true;
        RegulatorPID pid_bez_d(0.4, 0.0, 0.0);
        RegulatorPID pid_d    (0.4, 0.0, 1.0);

        pid_bez_d.tick(0.0);
        pid_d.tick(0.0);

        double sygnal_wejsciowy = 10.0;
        double delta_pid_d = std::abs(sygnal_wejsciowy - pid_d.tick(sygnal_wejsciowy));
        double delta_pid_bez_d = std::abs(sygnal_wejsciowy - pid_bez_d.tick(sygnal_wejsciowy));
        if( delta_pid_d > delta_pid_bez_d )
            test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}

std::pair<bool, const char*> Test::TEST_ARX_zaklocenia()
{
    try
    {
        bool test_passed = true;
        constexpr const size_t ILE_PROBEK = 10000000;
        constexpr const double ODCHYLENIE = 5.0;
        ARX arx({1.0}, {0.0}, 1, ODCHYLENIE);


        std::vector<double> zaklocenia;

        zaklocenia.reserve(ILE_PROBEK);
        for (int iteracja = 0; iteracja < ILE_PROBEK; ++iteracja)
        {
            zaklocenia.push_back(arx.getGaussianDistribValue());
        }

        // srednia
        double suma = 0.0;
        for (auto zaklocenie : zaklocenia)
            suma += zaklocenie;
        double srednia = suma / ILE_PROBEK;

        double wariancja = 0.0;
        for (auto zaklocenie : zaklocenia)
            wariancja += (zaklocenie - srednia) * (zaklocenie - srednia);
        wariancja /= ILE_PROBEK;

        double odchylenie_standartowe_wyliczone = std::sqrt(wariancja);
        if(!porownajZTolerancja(odchylenie_standartowe_wyliczone, ODCHYLENIE, 1e-1))
            test_passed = false;

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}

std::pair<bool, const char*> Test::TEST_ARX_nietypowe_AB()
{
    try
    {
        bool test_passed = true;
        ARX arx({0.0, 0.0}, {1.0, 1.0}, 1, 0.0);
        double sygnal_wejsciowy[] = {
            1.0,
            2.0,
            3.0,
            4.0,
            5.0,
        };
        double oczekiwany_sygnal_wyjsciowy[] = {
            0.0,
            1.0,
            3.0,
            5.0,
            7.0,
        };

        constexpr const size_t ROZMIAR_DANYCH_TESTOWYCH = sizeof(sygnal_wejsciowy) / sizeof(double);
        for (size_t i = 0; i < ROZMIAR_DANYCH_TESTOWYCH; i++)
        {
            if(!porownajZTolerancja(arx.tick(sygnal_wejsciowy[i]), oczekiwany_sygnal_wyjsciowy[i]))
                test_passed = false;
        }

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}

std::pair<bool, const char*> Test::TEST_ARX_ustawienie_K()
{
    try
    {
        bool test_passed = true;
        ARX arx({0.0, 0.0}, {1.0, 0.0}, 1, 0.0);

        arx.setK(2);

        double sygnal_wejsciowy[] = {
            1.0,
            2.0,
            3.0,
            4.0,
            5.0,
        };
        double oczekiwany_sygnal_wyjsciowy[] = {
            0.0,
            0.0,
            1.0,
            2.0,
            3.0,
        };

        constexpr const size_t ROZMIAR_DANYCH_TESTOWYCH = sizeof(sygnal_wejsciowy) / sizeof(double);
        for (size_t i = 0; i < ROZMIAR_DANYCH_TESTOWYCH; i++)
        {
            if(!porownajZTolerancja(arx.tick(sygnal_wejsciowy[i]), oczekiwany_sygnal_wyjsciowy[i]))
                test_passed = false;
        }

        return std::make_pair(test_passed, __FUNCTION__);
    }
    catch(...)
    {
        return std::make_pair(false, __FUNCTION__);
    }
}


bool Test::porownajZTolerancja(const double a, const double b, const double EPS)
{
    return b - EPS < a && a < b + EPS;
}
