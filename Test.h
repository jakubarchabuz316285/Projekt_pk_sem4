#ifndef TEST_H
#define TEST_H

#include <qdebug.h>

class Test
{

    static std::pair<bool, const char*> TEST_ARX_ustawienie_K();
    static std::pair<bool, const char*> TEST_ARX_nietypowe_AB();
    static std::pair<bool, const char*> TEST_ARX_zaklocenia();
    static std::pair<bool, const char*> TEST_PID_czlon_D_przyspiesza_regulacje();
    static std::pair<bool, const char*> TEST_PID_czlon_P_regulacja_odwrotna();

    static std::pair<bool, const char*> TEST_Generator_Sinusoidalny();
    static std::pair<bool, const char*> TEST_Generator_Sinusoidalny_bias();
    static std::pair<bool, const char*> TEST_Generator_Sinusoidalny_zmiana_amplitudy();
    static std::pair<bool, const char*> TEST_Generator_Sinusoidalny_zmiana_okresu();


    static std::pair<bool, const char*> TEST_Generator_Prostokatny_zmiana_amplitudy();
    static std::pair<bool, const char*> TEST_Generator_Prostokatny_zmiana_okresu();
    static std::pair<bool, const char*> TEST_Generator_Prostokatny_bias();
    static std::pair<bool, const char*> TEST_Generator_Prostokatny_100_procent();
    static std::pair<bool, const char*> TEST_Generator_Prostokatny_50_procent();
    static std::pair<bool, const char*> TEST_Generator_Prostokatny_10_procent();


    static std::pair<bool, const char*> TEST_State_czy_jest_signletonem();
    static std::pair<bool, const char*> TEST_State_zapis_i_odczyt_z_pliku();
    static std::pair<bool, const char*> TEST_ListWithExtremes_poprawne_dzialanie();

    static bool porownajZTolerancja(const double a, const double b, const double EPS = 1e-4);
public:
    Test() = delete;
    static void wykonajTesty();
};

#endif // TEST_H
