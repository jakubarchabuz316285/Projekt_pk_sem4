#include "workdialog.h"
#include "qbuttongroup.h"
#include "qsettings.h"
#include "ui_workdialog.h"
bool WorkDialog::LocalSimulation = true;

WorkDialog::WorkDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkDialog)
{
    ui->setupUi(this);
    if(LocalSimulation) {
        ui->RdioLocal->setChecked(true);
    } else {
        ui->RdioNet->setChecked(true);
    }
    ui->rdio_arx->setChecked(true);
    UpdateNetworkUI();
}

WorkDialog::~WorkDialog()
{
    delete ui;
}

void WorkDialog::on_RdioLocal_toggled(bool checked)
{
    LocalSimulation = checked;
    ui->GBoxNetwork->setEnabled(!checked);
}
void WorkDialog::on_ChkLocalSimulation_checkStateChanged(const Qt::CheckState &arg1){
    return;
}

void WorkDialog::on_RdioLocal_clicked(){
    return;
}

void WorkDialog::UpdateNetworkUI()
{
    bool isArx = ui->rdio_arx->isChecked();
    bool isPid = ui->rdio_reg->isChecked();

    ui->LblIP->setVisible(isArx);
    ui->SpnFirst->setVisible(isArx);
    ui->SpnSecond->setVisible(isArx);
    ui->SpnThird->setVisible(isArx);
    ui->SpnFourth->setVisible(isArx);

    ui->BtnConnect->setVisible(isArx);

    ui->BtnListen->setVisible(isPid);

    ui->LblPort->setVisible(true);
    ui->spinBox->setVisible(true);
    ui->BtnDisconnect->setVisible(true);
}


void WorkDialog::on_rdio_reg_toggled(bool checked)
{
    UpdateNetworkUI();
}


void WorkDialog::on_rdio_arx_toggled(bool checked)
{
    UpdateNetworkUI();
}

