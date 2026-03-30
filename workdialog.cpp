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
    ui->rdio_arx->setChecked(true);

    if (this->layout()) {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }

    if(LocalSimulation) {
        ui->RdioLocal->setChecked(true);
    } else {
        ui->RdioNet->setChecked(true);
    }
    ui->spnBoxPort->setMinimumWidth(50);
    ui->spnBoxPort->setMinimum(0);
    ui->spnBoxPort->setMaximum(65535);
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
    try
    {
        State::getInstance().setMode(NetworkManager::Mode::Local);
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
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

    ui->IPContainter->setVisible(isArx);

    ui->BtnConnect->setVisible(isArx);

    ui->BtnListen->setVisible(isPid);

    ui->LblPort->setVisible(true);
    ui->spnBoxPort->setVisible(true);
    ui->BtnDisconnect->setVisible(true);
    this->adjustSize();
}


void WorkDialog::on_rdio_reg_toggled(bool checked)
{
    UpdateNetworkUI();
    try
    {
        if(checked){
            qDebug() << "rdio reg toggled";
            State::getInstance().setMode(NetworkManager::Mode::PID);
        }
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
}


void WorkDialog::on_rdio_arx_toggled(bool checked)
{
    UpdateNetworkUI();
    try
    {
        if(checked){
            qDebug() << "rdio arx toggled";
            State::getInstance().setMode(NetworkManager::Mode::ARX);
        }
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
}

void WorkDialog::on_BtnListen_clicked()
{
    try
    {
        if(!State::getInstance().isListening()){
            qDebug() << "nasluchiwanie (workdialog)";
            State::getInstance().startListening(ui->spnBoxPort->value());
            qDebug() << "nasluchiwanie (workdialog po funkcji)";
            ui->BtnListen->setText("Przestań nasłuchiwać");
        } else {
            State::getInstance().stopListening();
            ui->BtnListen->setText("Nasłuchuj");
        }
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
}


void WorkDialog::on_BtnConnect_clicked()
{
    try
    {
        QString ip_address = composeIPAddres();
        int port = ui->spnBoxPort->value();
        State::getInstance().connect(ip_address, port);
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
}


void WorkDialog::on_BtnDisconnect_clicked()
{
    try
    {
        State::getInstance().disconnect();
    }
    catch (std::runtime_error e)
    {
        //TODO Okiekno errora // błąd e
    }
    catch (...)
    {
        //TODO Okienko errora // inny błąd
    }
}


void WorkDialog::on_RdioNet_toggled(bool checked)
{
    qDebug() << "Rdionet toggled";
    if(checked){
        try
        {
            qDebug() << "Rdionet toggled checked";
            NetworkManager::Mode mode;
            if(ui->rdio_arx->isChecked()){
                qDebug() << "Rdionet toggled mode arx";
                mode = NetworkManager::Mode::ARX;
            }
            else if(ui->rdio_reg->isChecked()){
                qDebug() << "Rdionet toggled mode pid";
                mode = NetworkManager::Mode::PID;
            }
            State::getInstance().setMode(mode);
        }
        catch (std::runtime_error e)
        {
            //TODO Okiekno errora // błąd e
        }
        catch (...)
        {
            //TODO Okienko errora // inny błąd
        }
    }
}

