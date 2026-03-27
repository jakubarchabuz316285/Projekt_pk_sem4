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


    void on_RdioNet_toggled(bool checked);
    void UpdateNetworkUI();
    void on_rdio_reg_toggled(bool checked);

    void on_rdio_arx_toggled(bool checked);

private:
    static bool LocalSimulation;

    QString composeIPAddres()
    {
        QString address = ui->SpnFirst->text() + "." +
                          ui->SpnSecond->text() + "." +
                          ui->SpnThird->text() + "." +
                          ui->SpnFourth->text();
        return address;
    }
    bool validateConnectionData(QString adr, int port)
    {
        QHostAddress ipAdr(adr);
        if(ipAdr.protocol() != QAbstractSocket::IPv4Protocol)
        {
            //ui->statusbar->showMessage("Invalid IPv4 Address!");
            return false;
        }
        if(port < 0 || 65535 < port)
        {
            //ui->statusbar->showMessage("Invalid TCP port number!");
            return false;
        }
        return true;
    }
    Ui::WorkDialog *ui;
    NetworkManager* manager;
};

#endif // WORKDIALOG_H
