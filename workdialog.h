#ifndef WORKDIALOG_H
#define WORKDIALOG_H

#include <QWidget>

namespace Ui {
class WorkDialog;
}

class WorkDialog : public QWidget
{
    Q_OBJECT

public:
    explicit WorkDialog(QWidget *parent = nullptr);
    ~WorkDialog();

private:
    Ui::WorkDialog *ui;
};

#endif // WORKDIALOG_H
