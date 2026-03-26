#ifndef WORKDIALOG_H
#define WORKDIALOG_H

#include "ui_workdialog.h"
#include <QWidget>
#include "networkmanager.h"

namespace Ui {
class WorkDialog;
}

class WorkDialog : public QWidget
{
    Q_OBJECT

public:
    explicit WorkDialog(QWidget *parent = nullptr);
    ~WorkDialog();

private slots:
    void on_ChkLocalSimulation_checkStateChanged(const Qt::CheckState &arg1);

    void on_RdioLocal_clicked();

    void on_RdioLocal_toggled(bool checked);


private:
    QString composeIPAddres()
    {
        QString address = ui->SpnFirst->text() + "." +
                          ui->SpnSecond->text() + "." +
                          ui->SpnThird->text() + "." +
                          ui->SpnFourth->text();
        return address;
    }
    Ui::WorkDialog *ui;
    NetworkManager* manager;
};

#endif // WORKDIALOG_H
