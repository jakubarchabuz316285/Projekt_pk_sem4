#ifndef ArxCoefficientItem_H
#define ArxCoefficientItem_H

#include <QWidget>

namespace Ui {
class ArxCoefficientItem;
}

class ArxCoefficientItem : public QWidget
{
    Q_OBJECT

public:
    explicit ArxCoefficientItem(qint32 index, double A, double B, QWidget *parent = nullptr);
    ~ArxCoefficientItem();
    double getA();
    double getB();

private slots:
    void on_doubleSpinBox_a_valueChanged(double arg1);

    void on_doubleSpinBox_b_valueChanged(double arg1);

    void on_horizontalSlider_b_valueChanged(int value);

    void on_horizontalSlider_a_valueChanged(int value);

private:
    Ui::ArxCoefficientItem *ui;
};

#endif // ArxCoefficientItem_H
