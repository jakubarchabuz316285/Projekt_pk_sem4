#include "ArxCoefficientItem.hpp"
#include "ui_ARXCoefficientItem.h"

ArxCoefficientItem::ArxCoefficientItem(qint32 index, double A, double B, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ArxCoefficientItem)
{
    ui->setupUi(this);
    this->setLayout(ui->horizontalLayout);
    ui->label_a->setText("A" + QString::number(index));
    ui->label_b->setText("B" + QString::number(index));

    ui->doubleSpinBox_a->setValue(A);
    ui->doubleSpinBox_b->setValue(B);
}

ArxCoefficientItem::~ArxCoefficientItem()
{
    delete ui;
}
double ArxCoefficientItem::getA()
{
    return ui->doubleSpinBox_a->value();
}
double ArxCoefficientItem::getB()
{
    return ui->doubleSpinBox_b->value();
}

void ArxCoefficientItem::on_doubleSpinBox_a_valueChanged(double arg1)
{
    ui->horizontalSlider_a->blockSignals(true);
    ui->horizontalSlider_a->setValue(arg1 * 100);
    ui->horizontalSlider_a->blockSignals(false);
}

void ArxCoefficientItem::on_doubleSpinBox_b_valueChanged(double arg1)
{
    ui->horizontalSlider_b->blockSignals(true);
    ui->horizontalSlider_b->setValue(arg1 * 100);
    ui->horizontalSlider_b->blockSignals(false);
}

void ArxCoefficientItem::on_horizontalSlider_a_valueChanged(int value)
{
    ui->doubleSpinBox_a->setValue((double) value / 100.0);
}

void ArxCoefficientItem::on_horizontalSlider_b_valueChanged(int value)
{
    ui->doubleSpinBox_b->setValue((double) value / 100.0);
}
