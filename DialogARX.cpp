#include "DialogARX.h"
#include <QLineSeries>
#include <QtCharts/QBarCategoryAxis>
#include "ARXCoefficientItem.hpp"
#include "ui_DialogARX.h"

DialogArx::DialogArx(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogArx)
{
    ui->setupUi(this);

    chart_zaklocenia = new QChart();
    rozklad_zaklocen = new QLineSeries();
    rozklad_zaklocen->setName("Rozkład normalny");

    chart_zaklocenia->addSeries(rozklad_zaklocen);
    ustaw_wykres();

    arx_coefficients_widget = new QWidget();
    arx_coefficients_layout = new QVBoxLayout(arx_coefficients_widget);
    arx_coefficients_widget->setLayout(arx_coefficients_layout);
    ui->scrollArea->setWidget(arx_coefficients_widget);

    zaczytaj_dane();
}

DialogArx::~DialogArx()
{
    delete ui;
}

void DialogArx::zaczytaj_dane()
{
    //Załaduj współczynniki
    std::vector<double> a = State().getARXCoefficientsA();
    std::vector<double> b = State().getARXCoefficientsB();

    for (int i = 0; i < a.size(); i++) {
        addARXCoefficientItem(a[i], b[i]);
    }

    while (arx_coefficients_items.size() < MIN_WSPOLCZYNNIKOW) {
        addARXCoefficientItem(0.0, 0.0);
        ui->usun_wspolczynnik->setEnabled(false);
    }

    if (a.size() >= MAX_WSPOLCZYNNIKOW)
        ui->dodaj_wspolczynnik->setEnabled(false);

    ui->opoznienie_wartosc->setValue(State().getARXTransportDelay());

    ui->zaklocenie_wartosc->setValue(State().getARXNoiseStandardDeviation());

    ui->ograniczenie_wej_max->setValue(State().getARXInputLimits().second);
    ui->ograniczenie_wej_min->setValue(State().getARXInputLimits().first);

    ui->ograniczenie_wyj_max->setValue(State().getARXOutputLimits().second);
    ui->ograniczenie_wyj_min->setValue(State().getARXOutputLimits().first);
}

void DialogArx::on_opoznienie_suwak_valueChanged(int value)
{
    ui->opoznienie_wartosc->setValue(value);
}

void DialogArx::on_opoznienie_wartosc_valueChanged(int arg1)
{
    ui->opoznienie_suwak->setValue(arg1);
}

void DialogArx::on_zaklocenie_suwak_valueChanged(int value)
{
    ui->zaklocenie_wartosc->setValue((double) value / 100.0);
    aktualizuj_widok((double) value / 100.0);
}

void DialogArx::on_zaklocenie_wartosc_valueChanged(double arg1)
{
    ui->zaklocenie_suwak->setValue(arg1 * 100);
    aktualizuj_widok(arg1);
}

void DialogArx::ustaw_wykres()
{
    chart_zaklocenia->removeAllSeries();
    chart_zaklocenia->removeAxis(axisX);
    chart_zaklocenia->removeAxis(axisY);

    rozklad_zaklocen = new QLineSeries(chart_zaklocenia);
    rozklad_zaklocen->setName("Gauss");
    chart_zaklocenia->addSeries(rozklad_zaklocen);

    axisX = new QValueAxis(chart_zaklocenia);
    chart_zaklocenia->addAxis(axisX, Qt::AlignBottom);
    rozklad_zaklocen->attachAxis(axisX);

    axisY = new QValueAxis(chart_zaklocenia);
    axisY->setRange(0.0, 1.0);
    chart_zaklocenia->addAxis(axisY, Qt::AlignLeft);

    rozklad_zaklocen->attachAxis(axisY);

    chart_zaklocenia->legend()->hide();

    auto *chartView = new QChartView(chart_zaklocenia, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->chart_zaklocenia_view->addWidget(chartView);
}

static double gauss(double x, double sigma)
{
    if (sigma <= 0.0)
        return 0.0;

    const double coef = 1.0 / (sigma * std::sqrt(2.0 * M_PI));
    return coef * std::exp(-(x * x) / (2.0 * sigma * sigma));
}

void DialogArx::aktualizuj_widok(double sigma)
{
    rozklad_zaklocen->clear();

    if (sigma <= 0.0) {
        axisX->setRange(-1.0, 1.0);
        axisY->setRange(0.0, 1.0);
        return;
    }

    const double minX = -3.0 * sigma;
    const double maxX = 3.0 * sigma;

    const double dx = (maxX - minX) / LICZBA_PUNKTOW;

    double maxY = 0.0;

    for (int i = 0; i <= LICZBA_PUNKTOW; ++i) {
        double x = minX + i * dx;
        double y = gauss(x, sigma);

        rozklad_zaklocen->append(x, y);

        if (y > maxY)
            maxY = y;
    }

    axisX->setRange(minX, maxX);
    axisY->setRange(0.0, maxY * 1.1);
}

void DialogArx::addARXCoefficientItem(const double A, const double B)
{
    ArxCoefficientItem *item = new ArxCoefficientItem(arx_coefficients_items.size() + 1, A, B);
    arx_coefficients_items.push_back(item);
    arx_coefficients_layout->addWidget(item);
    arx_coefficients_widget->adjustSize();
}

void DialogArx::on_dodaj_wspolczynnik_clicked()
{
    addARXCoefficientItem(0.0, 0.0);

    if (arx_coefficients_items.size() >= MAX_WSPOLCZYNNIKOW)
        ui->dodaj_wspolczynnik->setEnabled(false);

    if (arx_coefficients_items.size() > MIN_WSPOLCZYNNIKOW)
        ui->usun_wspolczynnik->setEnabled(true);

    return;
}

void DialogArx::on_usun_wspolczynnik_clicked()
{
    ArxCoefficientItem *item = arx_coefficients_items.back();
    arx_coefficients_layout->removeWidget(item);
    item->deleteLater();
    arx_coefficients_items.pop_back();

    if (arx_coefficients_items.size() < MAX_WSPOLCZYNNIKOW)
        ui->dodaj_wspolczynnik->setEnabled(true);

    if (arx_coefficients_items.size() <= MIN_WSPOLCZYNNIKOW)
        ui->usun_wspolczynnik->setEnabled(false);
    return;
}

void DialogArx::on_buttonBox_accepted()
{
    std::vector<double> a(arx_coefficients_items.size()), b(arx_coefficients_items.size());

    for (long index = 0; index < arx_coefficients_items.size(); index++) {
        a[index] = arx_coefficients_items[index]->getA();
        b[index] = arx_coefficients_items[index]->getB();
    }
    while (a.back() == 0.0 && b.back() == 0.0) {
        a.pop_back();
        b.pop_back();
    }

    State().setARXCoefficients(a, b);
    State().setARXTransportDelay(ui->opoznienie_wartosc->value());
    State().setARXInputLimits(ui->ograniczenie_wej_min->value(), ui->ograniczenie_wej_max->value());
    State().setARXOutputLimits(ui->ograniczenie_wyj_min->value(), ui->ograniczenie_wyj_max->value());
    State().setARXNoiseStandardDeviation(ui->zaklocenie_wartosc->value());

    State().console_print_state();
}
