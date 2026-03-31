#include "MainWindow.h"
#include "State.h"
#include "ui_MainWindow.h"
#include "DialogARX.h"
#include "BUILD_DEFINE.h"
#include "workdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , samples_count(40)
    , ui(new Ui::MainWindow)
    , lista_wartosc_zadana(new ListWithExtremes())
    , lista_wartosc_regulowana(new ListWithExtremes())
    , lista_uchyb(new ListWithExtremes())
    , lista_sterowanie(new ListWithExtremes())
    , lista_sterowanie_P(new ListWithExtremes())
    , lista_sterowanie_I(new ListWithExtremes())
    , lista_sterowanie_D(new ListWithExtremes())
    , miliseconds_of_simulation(0)

{
    ui->setupUi(this);
    this->ui->centralwidget->setLayout(ui->horizontalLayout_13);
    this->setWindowTitle("SymulatorUAR");


    chart_sterowanie = new QChart();
    chart_uchyb = new QChart();
    chart_wartosc_zadana_i_regulowana = new QChart();
    chart_skladowe_sterowania = new QChart();



    {
        auto seria = new QLineSeries();
        seria->setName("Sterowanie");
        seria->setColor(QColor(30, 30, 220));
        chart_sterowanie->addSeries(seria);
    }
    {
        auto seria = new QLineSeries();
        seria->setName("Uchyb");
        seria->setColor(QColor(235, 0, 80));
        chart_uchyb->addSeries(seria);
    }
    {
        auto seria = new QLineSeries();
        seria->setName("Wartość zadana");
        seria->setColor(QColor(0, 200, 55));
        chart_wartosc_zadana_i_regulowana->addSeries(seria);

        seria = new QLineSeries();
        seria->setName("Wartość regulowana");
        seria->setColor(QColor(130, 20, 150));
        chart_wartosc_zadana_i_regulowana->addSeries(seria);
    }
    {
        auto seria = new QLineSeries();
        seria->setName("Część proporcjonalna");
        seria->setColor(QColor(100, 200, 50));
        chart_skladowe_sterowania->addSeries(seria);

        seria = new QLineSeries();
        seria->setName("Część całkująca");
        seria->setColor(QColor(220, 220, 10));
        chart_skladowe_sterowania->addSeries(seria);

        seria = new QLineSeries();
        seria->setName("Część różniczkująca");
        seria->setColor(QColor(20, 10, 20));
        chart_skladowe_sterowania->addSeries(seria);

    }
    const qreal AXES_X_WIDTH = ui->spinBox_symulacja_interwal->value() / 1000.0 * 5.0;
    const qreal AXES_Y_HALFWIDTH = 1.0;

    chart_sterowanie->addAxis(new QValueAxis(), Qt::AlignBottom);
    chart_sterowanie->axes(Qt::Horizontal).at(0)->setRange(0.0, AXES_X_WIDTH);
    chart_sterowanie->addAxis(new QValueAxis(), Qt::AlignLeft);
    chart_sterowanie->axes(Qt::Vertical).at(0)->setRange(-AXES_Y_HALFWIDTH, AXES_Y_HALFWIDTH);

    chart_uchyb->addAxis(new QValueAxis(), Qt::AlignBottom);
    chart_uchyb->axes(Qt::Horizontal).at(0)->setRange(0.0, AXES_X_WIDTH);
    chart_uchyb->addAxis(new QValueAxis(), Qt::AlignLeft);
    chart_uchyb->axes(Qt::Vertical).at(0)->setRange(-AXES_Y_HALFWIDTH, AXES_Y_HALFWIDTH);

    chart_wartosc_zadana_i_regulowana->addAxis(new QValueAxis(), Qt::AlignBottom);
    chart_wartosc_zadana_i_regulowana->axes(Qt::Horizontal).at(0)->setRange(0.0, AXES_X_WIDTH);
    chart_wartosc_zadana_i_regulowana->addAxis(new QValueAxis(), Qt::AlignLeft);
    chart_wartosc_zadana_i_regulowana->axes(Qt::Vertical).at(0)->setRange(-AXES_Y_HALFWIDTH, AXES_Y_HALFWIDTH);
    chart_wartosc_zadana_i_regulowana->axes(Qt::Horizontal).at(0)->setTitleText("Czas [s]");

    chart_skladowe_sterowania->addAxis(new QValueAxis(), Qt::AlignBottom);
    chart_skladowe_sterowania->axes(Qt::Horizontal).at(0)->setRange(0.0, AXES_X_WIDTH);
    chart_skladowe_sterowania->addAxis(new QValueAxis(), Qt::AlignLeft);
    chart_skladowe_sterowania->axes(Qt::Vertical).at(0)->setRange(-AXES_Y_HALFWIDTH, AXES_Y_HALFWIDTH);

    chart_sterowanie->series().at(0)->attachAxis(chart_sterowanie->axes(Qt::Horizontal).at(0));
    chart_sterowanie->series().at(0)->attachAxis(chart_sterowanie->axes(Qt::Vertical).at(0));

    chart_uchyb->series().at(0)->attachAxis(chart_uchyb->axes(Qt::Horizontal).at(0));
    chart_uchyb->series().at(0)->attachAxis(chart_uchyb->axes(Qt::Vertical).at(0));

    chart_wartosc_zadana_i_regulowana->series().at(0)->attachAxis(
        chart_wartosc_zadana_i_regulowana->axes(Qt::Horizontal).at(0));
    chart_wartosc_zadana_i_regulowana->series().at(0)->attachAxis(
        chart_wartosc_zadana_i_regulowana->axes(Qt::Vertical).at(0));
    chart_wartosc_zadana_i_regulowana->series().at(1)->attachAxis(
        chart_wartosc_zadana_i_regulowana->axes(Qt::Horizontal).at(0));
    chart_wartosc_zadana_i_regulowana->series().at(1)->attachAxis(
        chart_wartosc_zadana_i_regulowana->axes(Qt::Vertical).at(0));

    chart_skladowe_sterowania->series().at(0)->attachAxis(chart_skladowe_sterowania->axes(Qt::Horizontal).at(0));
    chart_skladowe_sterowania->series().at(0)->attachAxis(chart_skladowe_sterowania->axes(Qt::Vertical).at(0));
    chart_skladowe_sterowania->series().at(1)->attachAxis(chart_skladowe_sterowania->axes(Qt::Horizontal).at(0));
    chart_skladowe_sterowania->series().at(1)->attachAxis(chart_skladowe_sterowania->axes(Qt::Vertical).at(0));
    chart_skladowe_sterowania->series().at(2)->attachAxis(chart_skladowe_sterowania->axes(Qt::Horizontal).at(0));
    chart_skladowe_sterowania->series().at(2)->attachAxis(chart_skladowe_sterowania->axes(Qt::Vertical).at(0));

    // chart_sterowanie->series().at(0)->setUseOpenGL(true);
    // chart_uchyb->series().at(0)->setUseOpenGL(true);
    // chart_wartosc_zadana_i_regulowana->series().at(0)->setUseOpenGL(true);
    // chart_wartosc_zadana_i_regulowana->series().at(1)->setUseOpenGL(true);
    // chart_skladowe_sterowania->series().at(0)->setUseOpenGL(true);
    // chart_skladowe_sterowania->series().at(1)->setUseOpenGL(true);
    // chart_skladowe_sterowania->series().at(2)->setUseOpenGL(true);


    auto q_chart_view = new QChartView(chart_sterowanie);
    chart_sterowanie->setMargins(QMargins(0,0,0,0));
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontAdjustForAntialiasing);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::IndirectPainting);
    ui->verticalLayout_wykresy->addWidget(q_chart_view, 2);


    q_chart_view = new QChartView(chart_uchyb);
    chart_uchyb->setMargins(QMargins(0,0,0,0));
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontAdjustForAntialiasing);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::IndirectPainting);
    ui->verticalLayout_wykresy->addWidget(q_chart_view, 2);


    q_chart_view = new QChartView(chart_skladowe_sterowania);
    chart_skladowe_sterowania->setMargins(QMargins(0,0,0,0));
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontAdjustForAntialiasing);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::IndirectPainting);
    ui->verticalLayout_wykresy->addWidget(q_chart_view, 2);


    q_chart_view = new QChartView(chart_wartosc_zadana_i_regulowana);
    chart_wartosc_zadana_i_regulowana->setMargins(QMargins(0,0,0,0));
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontAdjustForAntialiasing);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
    q_chart_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::IndirectPainting);
    ui->verticalLayout_wykresy->addWidget(q_chart_view, 4);

    State().setOutputCallback(std::bind(&MainWindow::addToPlots, this, std::placeholders::_1));


    QObject::connect(ui->actionzapisz_ustawienia, &QAction::triggered, this, &MainWindow::zapiszDoPliku);
    QObject::connect(ui->actionwczytaj_ustawienia, &QAction::triggered, this, &MainWindow::wczytajZPliku);
#ifdef DEBUG
    debug_timer.start();
    debug_last_time = 0;
    debug_dialog = new DialogDebug(this);
    QObject::connect(ui->actionokno_debugowania, &QAction::triggered, this, &MainWindow::przelaczOknoDebugowania);
#else
    ui->menuplik->removeAction(ui->actionokno_debugowania);
#endif


    update_charts_timer = new QTimer(this);
    update_charts_timer->setInterval(250);
    update_charts_timer->start();
    QObject::connect(update_charts_timer, &QTimer::timeout, this, &MainWindow::updateCharts);

    updateUiFromState();
    on_doubleSpinBox_generator_okres_editingFinished();

}
void MainWindow::updateCharts()
{
    dynamic_cast<QLineSeries *>(chart_sterowanie->series().at(0))->replace(*lista_sterowanie->getList());

    dynamic_cast<QLineSeries *>(chart_uchyb->series().at(0))->replace(*lista_uchyb->getList());

    dynamic_cast<QLineSeries *>(chart_wartosc_zadana_i_regulowana->series().at(0))->replace(*lista_wartosc_zadana->getList());
    dynamic_cast<QLineSeries *>(chart_wartosc_zadana_i_regulowana->series().at(1))->replace(*lista_wartosc_regulowana->getList());

    dynamic_cast<QLineSeries *>(chart_skladowe_sterowania->series().at(0))->replace(*lista_sterowanie_P->getList());
    dynamic_cast<QLineSeries *>(chart_skladowe_sterowania->series().at(1))->replace(*lista_sterowanie_I->getList());
    dynamic_cast<QLineSeries *>(chart_skladowe_sterowania->series().at(2))->replace(*lista_sterowanie_D->getList());


    constexpr const qreal ROZSZEZRENIE_SKALI_WYKRESOW_W_PIONIE = 0.1;

    // Osie pionowe - skalowanie
    qreal range_width_uchyb = (lista_uchyb->max() - lista_uchyb->min()) * ROZSZEZRENIE_SKALI_WYKRESOW_W_PIONIE;
    chart_uchyb->axes(Qt::Vertical).at(0)->setRange(lista_uchyb->min() - range_width_uchyb, lista_uchyb->max() + range_width_uchyb);


    qreal range_width_sterowanie = (lista_sterowanie->max() - lista_sterowanie->min()) * ROZSZEZRENIE_SKALI_WYKRESOW_W_PIONIE;
    chart_sterowanie->axes(Qt::Vertical).at(0)->setRange(lista_sterowanie->min() - range_width_sterowanie, lista_sterowanie->max() + range_width_sterowanie);

    qreal range_width_wartosc_zadana_i_regulowana = (std::max(lista_wartosc_regulowana->max(), lista_wartosc_zadana->max())
                                                     - std::min(lista_wartosc_regulowana->min(), lista_wartosc_zadana->min()))
                                                    * ROZSZEZRENIE_SKALI_WYKRESOW_W_PIONIE;

    chart_wartosc_zadana_i_regulowana->axes(Qt::Vertical)
        .at(0)
        ->setRange(std::min(lista_wartosc_regulowana->min(), lista_wartosc_zadana->min())
                       - range_width_wartosc_zadana_i_regulowana,
                   std::max(lista_wartosc_regulowana->max(), lista_wartosc_zadana->max())
                       + range_width_wartosc_zadana_i_regulowana);

    qreal max_skladowych_sterowania = std::max(std::max(lista_sterowanie_P->max(), lista_sterowanie_I->max()),
                                               lista_sterowanie_D->max());
    qreal min_skladowych_sterowania = std::min(std::min(lista_sterowanie_P->min(), lista_sterowanie_I->min()),
                                               lista_sterowanie_D->min());
    qreal range_width_skladowych_sterowania = (max_skladowych_sterowania - min_skladowych_sterowania) * ROZSZEZRENIE_SKALI_WYKRESOW_W_PIONIE;

    chart_skladowe_sterowania->axes(Qt::Vertical)
        .at(0)
        ->setRange(min_skladowych_sterowania - range_width_skladowych_sterowania, max_skladowych_sterowania + range_width_skladowych_sterowania);
#ifdef DEBUG
    qDebug() << "P: " << lista_sterowanie_P->max()
        << "I: " << lista_sterowanie_I->max()
        << "D: " << lista_sterowanie_D->max();
#endif
}

void MainWindow::addToPlots(TickData tick_data)
{
#ifdef DEBUG
    int debug_current_time = debug_timer.elapsed();
    debug_dialog->write(QString::number(debug_current_time - debug_last_time));
    debug_last_time = debug_current_time;
#endif
    const qint64 interwal_symulacji = State::getInstance().getSimmulationIntervalMS();
    const qint64 liczba_probek = (double) ui->spinBox_symulacja_okno_obserwacji->value()
                           / ((double) interwal_symulacji / 1000.0);

    const qreal seconds_of_simulation = (double) (miliseconds_of_simulation += interwal_symulacji) / 1000.0;

    lista_wartosc_zadana->appendLastValue(QPointF(seconds_of_simulation, tick_data.wartosc_zadana));
    lista_wartosc_regulowana->appendLastValue(QPointF(seconds_of_simulation, tick_data.wartosc_regulowana));
    lista_uchyb->appendLastValue(QPointF(seconds_of_simulation, tick_data.uchyb));
    lista_sterowanie->appendLastValue(QPointF(seconds_of_simulation, static_cast<double>(tick_data.sterowanie)));
    lista_sterowanie_P->appendLastValue(QPointF(seconds_of_simulation, tick_data.sterowanie.Proportional));
    lista_sterowanie_I->appendLastValue(QPointF(seconds_of_simulation, tick_data.sterowanie.Integral));
    lista_sterowanie_D->appendLastValue(QPointF(seconds_of_simulation, tick_data.sterowanie.Derrivative));

    // Osie poziome - skalowanie
    constexpr const qreal LICZBA_DODATKOWYCH_PROBEK_PO_PRAWEJ = 4.0;

    double xs = State::getInstance().getARXNoiseStandardDeviation();
    qreal range_start = lista_uchyb->getList()->front().x();
    qreal range_end = lista_uchyb->getList()->back().x() + LICZBA_DODATKOWYCH_PROBEK_PO_PRAWEJ / interwal_symulacji;

    chart_sterowanie->axes(Qt::Horizontal).at(0)->setRange(range_start, range_end);
    chart_uchyb->axes(Qt::Horizontal).at(0)->setRange(range_start, range_end);
    chart_wartosc_zadana_i_regulowana->axes(Qt::Horizontal).at(0)->setRange(range_start, range_end);
    chart_skladowe_sterowania->axes(Qt::Horizontal).at(0)->setRange(range_start, range_end);


    if (lista_uchyb->getList()->count() > liczba_probek) {
        lista_wartosc_zadana->deleteFirstValue();
        lista_wartosc_regulowana->deleteFirstValue();
        lista_uchyb->deleteFirstValue();
        lista_sterowanie->deleteFirstValue();
        lista_sterowanie_P->deleteFirstValue();
        lista_sterowanie_I->deleteFirstValue();
        lista_sterowanie_D->deleteFirstValue();

        if (lista_uchyb->getList()->count() > liczba_probek) {
            lista_wartosc_zadana->deleteFirstValue();
            lista_wartosc_regulowana->deleteFirstValue();
            lista_uchyb->deleteFirstValue();
            lista_sterowanie->deleteFirstValue();
            lista_sterowanie_P->deleteFirstValue();
            lista_sterowanie_I->deleteFirstValue();
            lista_sterowanie_D->deleteFirstValue();
        }

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_symulacja_star_stop_released()
{
    if (State().getSimmulationRunning())
    {
        State().setSimmulationRunning(false);
        ui->pushButton_symulacja_star_stop->setText(tr("START"));
    }
    else
    {
        State().setSimmulationRunning(true);
        ui->pushButton_symulacja_star_stop->setText(tr("STOP"));
    }
}

// Interwał symulacji
void MainWindow::on_spinBox_symulacja_interwal_valueChanged(int arg1)
{
    ui->horizontalSlider_symulacja_interwal->setValue(arg1);
}
void MainWindow::on_horizontalSlider_symulacja_interwal_valueChanged(int value)
{
    ui->spinBox_symulacja_interwal->setValue(value);
}

void MainWindow::recalculate_generator_period()
{
    constexpr double MIN_MULTIPLE = 8.0;
    constexpr double MAX_MULTIPLE = 130.0;

    uint32_t interwal_symulacji_w_milisekundach = State::getInstance().getSimmulationIntervalMS();
    double interwal_symulacji_w_sekundach = (double)interwal_symulacji_w_milisekundach / 1000.0;
    double nowy_okres_generatora_w_sekundach = ((ui->doubleSpinBox_generator_okres->value() * 1000.0) - (uint32_t)std::round(ui->doubleSpinBox_generator_okres->value() * 1000.0) % interwal_symulacji_w_milisekundach) / 1000.0;
    if(((uint32_t)std::round(ui->doubleSpinBox_generator_okres->value() * 1000.0) % interwal_symulacji_w_milisekundach) > interwal_symulacji_w_milisekundach / 2)
        nowy_okres_generatora_w_sekundach += interwal_symulacji_w_sekundach;


    ui->doubleSpinBox_generator_okres->setValue(nowy_okres_generatora_w_sekundach);

    ui->doubleSpinBox_generator_okres->setMinimum(interwal_symulacji_w_sekundach * MIN_MULTIPLE);
    ui->doubleSpinBox_generator_okres->setMaximum(interwal_symulacji_w_sekundach * MAX_MULTIPLE);
    ui->doubleSpinBox_generator_okres->setSingleStep(interwal_symulacji_w_sekundach);

    ui->horizontalSlider_generator_okres->setMinimum(interwal_symulacji_w_sekundach * MIN_MULTIPLE * SLIDER_TO_SPINBOX_RATIO);
    ui->horizontalSlider_generator_okres->setMaximum(interwal_symulacji_w_sekundach * MAX_MULTIPLE * SLIDER_TO_SPINBOX_RATIO);
    ui->horizontalSlider_generator_okres->setSingleStep(interwal_symulacji_w_sekundach * SLIDER_TO_SPINBOX_RATIO);
    ui->horizontalSlider_generator_okres->setPageStep(interwal_symulacji_w_sekundach * SLIDER_TO_SPINBOX_RATIO);


    // uint32_t interwal_symulacji = State::getInstance().getSimmulationIntervalMS();
    // int new_generator_okres_value = (int)(ui->doubleSpinBox_generator_okres->value() * 1000.0) / interwal_symulacji * interwal_symulacji;
    // if((int)((ui->doubleSpinBox_generator_okres->value() * 1000.0)) % interwal_symulacji > interwal_symulacji / 2)
    //     new_generator_okres_value += interwal_symulacji;

    // ui->doubleSpinBox_generator_okres->setValue(new_generator_okres_value / 1000.0);

    // ui->doubleSpinBox_generator_okres->setMinimum(interwal_symulacji_w_sekundach * 8.0);
    // ui->doubleSpinBox_generator_okres->setMaximum(interwal_symulacji_w_sekundach * 100.0);
    // ui->doubleSpinBox_generator_okres->setSingleStep(interwal_symulacji_w_sekundach);

    // ui->horizontalSlider_generator_okres->setMinimum(interwal_symulacji_w_sekundach * 8 * SLIDER_TO_SPINBOX_RATIO);
    // ui->horizontalSlider_generator_okres->setMaximum(interwal_symulacji_w_sekundach * 100 * SLIDER_TO_SPINBOX_RATIO);
    // ui->horizontalSlider_generator_okres->setSingleStep(interwal_symulacji * SLIDER_TO_SPINBOX_RATIO);
    // ui->horizontalSlider_generator_okres->setPageStep(interwal_symulacji_w_sekundach * SLIDER_TO_SPINBOX_RATIO);

#ifdef DEBUG
    qDebug() << "Recalc" << nowy_okres_generatora_w_sekundach;
#endif
}

void MainWindow::on_spinBox_symulacja_interwal_editingFinished()
{
    State().setSimmulationIntervalMS(ui->spinBox_symulacja_interwal->value());


    recalculate_generator_period();
    emit ui->horizontalSlider_generator_okres->sliderReleased();
}
void MainWindow::on_horizontalSlider_symulacja_interwal_sliderReleased()
{
    on_spinBox_symulacja_interwal_editingFinished();
}

//Włączanie okna ustawień obiektu ARX
void MainWindow::on_pushButton_dostosuj_parametry_clicked()
{
    DialogArx arxDialog(this);
    arxDialog.exec();
}

// Amplituda generatora
void MainWindow::on_verticalSlider_generator_amplituda_sliderMoved(int position)
{
    ui->doubleSpinBox_generator_amplituda->setValue((double) position / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_generator_amplituda_valueChanged(double arg1)
{
    ui->verticalSlider_generator_amplituda->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_generator_amplituda_editingFinished()
{
    State().setGeneneratorAmplitude(ui->doubleSpinBox_generator_amplituda->value());
}
void MainWindow::on_verticalSlider_generator_amplituda_sliderReleased()
{
    on_doubleSpinBox_generator_amplituda_editingFinished();
}

// Okres generatora (T)
void MainWindow::on_horizontalSlider_generator_okres_valueChanged(int value)
{
    ui->doubleSpinBox_generator_okres->setValue(value / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_generator_okres_valueChanged(double arg1)
{
    ui->horizontalSlider_generator_okres->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_generator_okres_editingFinished()
{
    recalculate_generator_period();
    State().setGeneneratorPeriodMS(ui->doubleSpinBox_generator_okres->value() * 1000.0);
}
void MainWindow::on_horizontalSlider_generator_okres_sliderReleased()
{
    on_doubleSpinBox_generator_okres_editingFinished();
}

//Zmiana używanego generatora
void MainWindow::on_comboBox_generator_typ_currentTextChanged(const QString &arg1)
{
    // Dla generatorow okresowych
    ui->horizontalSlider_generator_okres->setEnabled(true);
    ui->doubleSpinBox_generator_czas_skoku->setEnabled(false);
    ui->verticalSlider_generator_amplituda->setEnabled(true);
    ui->doubleSpinBox_generator_amplituda->setEnabled(true);
    ui->doubleSpinBox_generator_okres->setEnabled(true);
    if (arg1 == "Prostokątny")
    {
        ui->horizontalSlider_generator_wypelnienie->setEnabled(true);
        ui->spinBox_generator_wypelnienie->setEnabled(true);
        State().setGenerator(State::TypGeneratora::Prostokatny);
    }
    if (arg1 == "Sinusoida")
    {
        ui->horizontalSlider_generator_wypelnienie->setEnabled(false);
        State().setGenerator(State::TypGeneratora::Sinusoidalny);
    }
    if (arg1 == "Skok Jednostkowy")
    {
        State().setGenerator(State::TypGeneratora::SkokJednostkowy);
        ui->horizontalSlider_generator_wypelnienie->setEnabled(false);
        ui->horizontalSlider_generator_okres->setEnabled(false);
        ui->verticalSlider_generator_amplituda->setEnabled(false);
        ui->doubleSpinBox_generator_amplituda->setEnabled(false);
        ui->doubleSpinBox_generator_okres->setEnabled(false);
        ui->doubleSpinBox_generator_czas_skoku->setEnabled(true);
    }

}

// Wypelnienie generatora (P)
void MainWindow::on_spinBox_generator_wypelnienie_editingFinished()
{
    State().setGeneneratorDutyCycle((double) ui->spinBox_generator_wypelnienie->value() / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_horizontalSlider_generator_wypelnienie_sliderReleased()
{
    on_spinBox_generator_wypelnienie_editingFinished();
}

#pragma warning(disable : 4100) // nieużywsany parametr wyłącza
void MainWindow::on_horizontalSlider_generator_wypelnienie_valueChanged(int value)
{
    ui->spinBox_generator_wypelnienie->setValue(ui->horizontalSlider_generator_wypelnienie->value());
}
void MainWindow::on_spinBox_generator_wypelnienie_valueChanged(int arg1)
{
    ui->horizontalSlider_generator_wypelnienie->setValue(ui->spinBox_generator_wypelnienie->value());
}

// reset symulacji
void MainWindow::on_pushButton_symulacja_reset_clicked()
{
    State().resetSimmulation();

    this->lista_sterowanie->clear();
    this->lista_uchyb->clear();
    this->lista_wartosc_regulowana->clear();
    this->lista_wartosc_zadana->clear();
    this->lista_sterowanie->clear();
    this->lista_sterowanie_P->clear();
    this->lista_sterowanie_I->clear();
    this->lista_sterowanie_D->clear();

    miliseconds_of_simulation = 0;
}

// okno obserwacji - zmiany w wykresach
void MainWindow::on_horizontalSlider_symulacja_okno_obserwacji_valueChanged(int value)
{
    ui->spinBox_symulacja_okno_obserwacji->setValue(value);
}
void MainWindow::on_spinBox_symulacja_okno_obserwacji_valueChanged(int arg1)
{
    ui->horizontalSlider_symulacja_okno_obserwacji->setValue(arg1);
}


// PID - kontrolki całkowania i rozniczkowania
void MainWindow::on_radioButton_stala_calkowania_przed_clicked()
{
    State().setPIDIntegrationType(IntegType::outside);
}
void MainWindow::on_radioButton_stala_calkowania_pod_clicked()
{
    State().setPIDIntegrationType(IntegType::insde);
}
void MainWindow::on_pushButton_reset_pam_calk_clicked()
{
    State().resetPIDIntegration();
}
void MainWindow::on_pushButton_reset_pam_roz_clicked()
{
    State().resetPIDDerrivative();
}

// PID - część proporcjonalna (K)
void MainWindow::on_horizontalSlider_pid_k_valueChanged(int value)
{
    ui->doubleSpinBox_pid_k->setValue((double) value / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_pid_k_editingFinished()
{
    State().setPIDProportional(ui->doubleSpinBox_pid_k->value());
}
void MainWindow::on_doubleSpinBox_pid_k_valueChanged(double arg1)
{
    ui->horizontalSlider_pid_k->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_horizontalSlider_pid_k_sliderReleased()
{
    on_doubleSpinBox_pid_k_editingFinished();
}

// PID - część całkująca (Ti)
void MainWindow::on_horizontalSlider_pid_Ti_valueChanged(int value)
{
    ui->doubleSpinBox_pid_Ti->setValue((double) value / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_pid_Ti_valueChanged(double arg1)
{
    ui->horizontalSlider_pid_Ti->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_pid_Ti_editingFinished()
{
    State().setPIDIntegration(ui->doubleSpinBox_pid_Ti->value());
}
void MainWindow::on_horizontalSlider_pid_Ti_sliderReleased()
{
    on_doubleSpinBox_pid_Ti_editingFinished();
}

// PID - część różniczkująca (Td)
void MainWindow::on_horizontalSlider_pid_Td_valueChanged(int value)
{
    ui->doubleSpinBox_pid_Td->setValue((double) value / SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_pid_Td_valueChanged(double arg1)
{
    ui->horizontalSlider_pid_Td->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}
void MainWindow::on_doubleSpinBox_pid_Td_editingFinished()
{
    State().setPIDDerrivative(ui->doubleSpinBox_pid_Td->value());
}
void MainWindow::on_horizontalSlider_pid_Td_sliderReleased()
{
    on_doubleSpinBox_pid_Td_editingFinished();
}

void MainWindow::zapiszDoPliku()
{
    State().saveToFile(QFileDialog::getSaveFileName(this, tr("Zapisz konfigurację"), QDir::homePath(), tr("JSON (*.json)")).toStdString());
}
void MainWindow::wczytajZPliku()
{
    State().readFromFile(QFileDialog::getOpenFileName(this, tr("Wczytaj konfigurację"), QDir::homePath(), tr("JSON (*.json)")).toStdString());
    updateUiFromState();
}

void MainWindow::updateUiFromState()
{
    auto [arx, pid, typ_generatora, gen_sinusoida, gen_prostokatny] = State().getAppState();
    this->ui->checkBoxOgraniczenia->setCheckState(arx->getLimitsActive() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);


    this->ui->doubleSpinBox_pid_k->setValue(pid->getK());
    this->ui->doubleSpinBox_pid_Ti->setValue(pid->getT_i());
    this->ui->doubleSpinBox_pid_Td->setValue(pid->getT_d());

    this->ui->radioButton_stala_calkowania_pod->setChecked(pid->getIntegrationType() == IntegType::insde);
    this->ui->radioButton_stala_calkowania_przed->setChecked(pid->getIntegrationType() == IntegType::outside);

    const Generator* gen;
    switch(typ_generatora)
    {
    case State::TypGeneratora::Sinusoidalny:
        gen = gen_sinusoida;
        this->ui->comboBox_generator_typ->setCurrentIndex(0);
        break;
    case State::TypGeneratora::Prostokatny:
        gen = gen_prostokatny;
        this->ui->comboBox_generator_typ->setCurrentIndex(2);
        break;
    case State::TypGeneratora::SkokJednostkowy:
        this->ui->comboBox_generator_typ->setCurrentIndex(1);
        this->ui->spinBox_symulacja_interwal->setValue(State().getSimmulationIntervalMS());
        return;
        break;
    }
    this->ui->doubleSpinBox_generator_amplituda->setValue(gen->getAmplitude());
    this->ui->doubleSpinBox_generator_bias->setValue(gen->getBias());
    this->ui->doubleSpinBox_generator_okres->setValue(gen->getSamplesPerCycle() * State().getSimmulationIntervalMS() / 1000.0);
    this->ui->spinBox_generator_wypelnienie->setValue(gen_prostokatny->getDutyCycle() * 100.0);

    this->ui->spinBox_symulacja_interwal->setValue(State().getSimmulationIntervalMS());
}

void MainWindow::przelaczOknoDebugowania()
{
#ifdef DEBUG
    if(debug_dialog->isHidden())
        debug_dialog->show();
    else
        debug_dialog->hide();
#endif
}


void MainWindow::on_checkBoxOgraniczenia_clicked()
{
    State().setARXLimitsEnabled(ui->checkBoxOgraniczenia->isChecked());
}


void MainWindow::on_doubleSpinBox_generator_bias_valueChanged(double arg1)
{
    ui->horizontalSlider_generator_bias->setValue(arg1 * SLIDER_TO_SPINBOX_RATIO);
}


void MainWindow::on_doubleSpinBox_generator_bias_editingFinished()
{
    State().setGeneratorSkladowaStala(ui->doubleSpinBox_generator_bias->value());
}


void MainWindow::on_horizontalSlider_generator_bias_sliderReleased()
{
    on_doubleSpinBox_generator_bias_editingFinished();
}


void MainWindow::on_horizontalSlider_generator_bias_valueChanged(int value)
{
    ui->doubleSpinBox_generator_bias->setValue(value / SLIDER_TO_SPINBOX_RATIO);
}


void MainWindow::on_doubleSpinBox_generator_czas_skoku_editingFinished()
{
    State().setGeneratorUnitJumpTimeMS(ui->doubleSpinBox_generator_czas_skoku->value() * 1000);
    ui->doubleSpinBox_generator_czas_skoku->setValue(0.0);
}

void MainWindow::on_BtnTrybPracy_clicked()
{
    WorkDialog *dialog = new WorkDialog();
    dialog->show();
}

