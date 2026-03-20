#ifndef DIALOGARXCOOEFICIENTS_H
#define DIALOGARXCOOEFICIENTS_H

#include <QDialog>

namespace Ui {
class DialogARXCooeficients;
}

class DialogARXCooeficients : public QDialog
{
    Q_OBJECT

public:
    explicit DialogARXCooeficients(QWidget *parent = nullptr);
    ~DialogARXCooeficients();

private:
    Ui::DialogARXCooeficients *ui;
};

#endif // DIALOGARXCOOEFICIENTS_H
