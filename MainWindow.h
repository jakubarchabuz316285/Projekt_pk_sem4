#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BUILD_DEFINE.h"

#ifdef DEBUG
    #include "DialogDebug.hpp"
#endif

#include <QMainWindow>
#include <QtCharts>
#include "UAR.h"
#include "ListWithWxtremes.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void addToPlots(TickData tick_data);
private slots:
    void updateCharts();
    void zapiszDoPliku();
    void wczytajZPliku();
    void przelaczOknoDebugowania();
    void on_pushButton_symulacja_star_stop_released();
    void on_spinBox_symulacja_interwal_valueChanged(int arg1);
    void on_horizontalSlider_symulacja_interwal_valueChanged(int value);
    void on_spinBox_symulacja_interwal_editingFinished();
    void on_horizontalSlider_symulacja_interwal_sliderReleased();
    void on_pushButton_dostosuj_parametry_clicked();
    void on_verticalSlider_generator_amplituda_sliderMoved(int position);
    void on_doubleSpinBox_generator_amplituda_valueChanged(double arg1);
    void on_doubleSpinBox_generator_amplituda_editingFinished();
    void on_verticalSlider_generator_amplituda_sliderReleased();
    void on_horizontalSlider_generator_okres_valueChanged(int value);
    void on_doubleSpinBox_generator_okres_editingFinished();
    void on_horizontalSlider_generator_okres_sliderReleased();
    void on_comboBox_generator_typ_currentTextChanged(const QString &arg1);
    void on_spinBox_generator_wypelnienie_editingFinished();
    void on_horizontalSlider_generator_wypelnienie_sliderReleased();
    void on_horizontalSlider_generator_wypelnienie_valueChanged(int value);
    void on_spinBox_generator_wypelnienie_valueChanged(int arg1);
    void on_pushButton_symulacja_reset_clicked();
    void on_horizontalSlider_symulacja_okno_obserwacji_valueChanged(int value);
    void on_spinBox_symulacja_okno_obserwacji_valueChanged(int arg1);
    void on_pushButton_reset_pam_calk_clicked();
    void on_radioButton_stala_calkowania_przed_clicked();
    void on_radioButton_stala_calkowania_pod_clicked();
    void on_horizontalSlider_pid_k_valueChanged(int value);
    void on_doubleSpinBox_pid_k_editingFinished();
    void on_horizontalSlider_pid_k_sliderReleased();
    void on_doubleSpinBox_pid_k_valueChanged(double arg1);
    void on_horizontalSlider_pid_Ti_valueChanged(int value);
    void on_horizontalSlider_pid_Ti_sliderReleased();
    void on_doubleSpinBox_pid_Ti_valueChanged(double arg1);
    void on_doubleSpinBox_pid_Ti_editingFinished();
    void on_horizontalSlider_pid_Td_valueChanged(int value);
    void on_doubleSpinBox_pid_Td_valueChanged(double arg1);
    void on_doubleSpinBox_pid_Td_editingFinished();
    void on_horizontalSlider_pid_Td_sliderReleased();
    void on_pushButton_reset_pam_roz_clicked();
    void on_checkBoxOgraniczenia_clicked();
    void on_doubleSpinBox_generator_bias_valueChanged(double arg1);
    void on_doubleSpinBox_generator_bias_editingFinished();
    void on_horizontalSlider_generator_bias_sliderReleased();
    void on_horizontalSlider_generator_bias_valueChanged(int value);
    void on_doubleSpinBox_generator_czas_skoku_editingFinished();

    void on_doubleSpinBox_generator_okres_valueChanged(double arg1);

private:
    static constexpr const double SLIDER_TO_SPINBOX_RATIO = 100.0;
    void updateUiFromState();
    void recalculate_generator_period();

    uint32_t samples_count;

    Ui::MainWindow *ui;

    QChart* chart_wartosc_zadana_i_regulowana;
    QChart* chart_uchyb;
    QChart* chart_sterowanie;
    QChart* chart_skladowe_sterowania;

    QTimer* update_charts_timer;

    ListWithExtremes* lista_wartosc_zadana;
    ListWithExtremes* lista_wartosc_regulowana;
    ListWithExtremes* lista_uchyb;
    ListWithExtremes* lista_sterowanie;
    ListWithExtremes* lista_sterowanie_P;
    ListWithExtremes* lista_sterowanie_I;
    ListWithExtremes* lista_sterowanie_D;

    qint64 miliseconds_of_simulation;

#ifdef DEBUG
    QElapsedTimer debug_timer;
    int debug_last_time;

    DialogDebug* debug_dialog;
#endif

};

#endif // MAINWINDOW_H
