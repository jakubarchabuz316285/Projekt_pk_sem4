#ifndef DIALOGARXNOISE_H
#define DIALOGARXNOISE_H

#include <QDialog>

namespace Ui {
class DialogARXnoise;
}

class DialogARXnoise : public QDialog
{
    Q_OBJECT

public:
    explicit DialogARXnoise(QWidget *parent = nullptr);
    ~DialogARXnoise();

private:
    Ui::DialogARXnoise *ui;
};

#endif // DIALOGARXNOISE_H
