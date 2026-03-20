#ifndef DIALOGDEBUG_H
#define DIALOGDEBUG_H

#include <QDialog>

namespace Ui {
class DialogDebug;
}

class DialogDebug : public QDialog
{
    Q_OBJECT

public:
    void write(QString message);
    explicit DialogDebug(QWidget *parent = nullptr);
    ~DialogDebug();

private:
    Ui::DialogDebug *ui;
};

#endif // DIALOGDEBUG_H
