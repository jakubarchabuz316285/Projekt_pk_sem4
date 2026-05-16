#include "workdialog.h"
#include "qbuttongroup.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include "ui_workdialog.h"
bool WorkDialog::LocalSimulation = true;

WorkDialog::WorkDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkDialog)
{
    ui->setupUi(this);
    ui->rdio_arx->setChecked(true);
    updateLedStatus(false);
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
    ui->SpnFirst->setRange(0, 255);
    ui->SpnSecond->setRange(0, 255);
    ui->SpnThird->setRange(0, 255);
    ui->SpnFourth->setRange(0, 255);
    UpdateNetworkUI();
    connect(&State::getInstance(), &State::statusChanged, this, [this](bool connected){
        if(connected) {
            ui->LblStatusLed->setStyleSheet("background-color: #2ecc71; border-radius: 8px; border: 1px solid #27ae60;");
            ui->LblStatusText->setText("Połączono");
        } else {
            ui->LblStatusLed->setStyleSheet("background-color: #e74c3c; border-radius: 8px; border: 1px solid #c0392b;");
            ui->LblStatusText->setText("Rozłączono");
        }
    });

    ui->LblStatusLed->setStyleSheet("background-color: #95a5a6; border-radius: 8px;");
    ui->LblStatusText->setText("Brak połączenia");

    // BROADCAST

    connect(&State::getInstance(), &State::serverDiscovered, this, &::WorkDialog::serverDiscovered);
}

WorkDialog::~WorkDialog()
{
    delete ui;
}
void WorkDialog::updateLedStatus(bool connected) {
    QString color = connected ? "#2ecc71" : "#e74c3c";
    QString style = QString(
                        "border-radius: 7px;"
                        "background-color: %1;"
                        "border: 1px solid #555;"
                        ).arg(color);

    ui->LblStatusLed->setStyleSheet(style);
}

void WorkDialog::on_RdioLocal_toggled(bool checked)
{
    LocalSimulation = checked;
    ui->GBoxNetwork->setEnabled(!checked);
    emitCurrentSettings();
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
    ui->listWidgetSerwery->setVisible(isArx);

    ui->BtnListen->setVisible(isPid);
    ui->chckboxPublicznySerwer->setVisible(isPid);

    ui->LblPort->setVisible(true);
    ui->spnBoxPort->setVisible(true);
    ui->BtnDisconnect->setVisible(true);

    if (!isPid) {
        ui->chckboxPublicznySerwer->setChecked(false);
    }

    this->adjustSize();
}


void WorkDialog::on_rdio_reg_toggled(bool checked)
{
    UpdateNetworkUI();
    try
    {
        if(checked){
           if(checked) emitCurrentSettings();
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
void WorkDialog::emitCurrentSettings() {
    NetworkManager::Mode mode = ui->rdio_arx->isChecked() ? NetworkManager::Mode::ARX : NetworkManager::Mode::PID;
    State::getInstance().setMode(mode);
    bool isLocal = ui->RdioLocal->isChecked();
    emit settingsChanged(mode, isLocal);
}

void WorkDialog::on_rdio_arx_toggled(bool checked)
{
    UpdateNetworkUI();
    try
    {
        if(checked) {
            emitCurrentSettings();
            if(State::getInstance().getMode() == NetworkManager::Mode::ARX) {
                State::getInstance().startServerDiscovery();
            }
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
        if(State::getInstance().isConnected())
        {
            QMessageBox::warning(this, "Ostrzeżenie", "Jesteś już połączony lub trwa próba nawiązywania połączenia!");
            return;
        }
        QString ip_address = composeIPAddres();
        int port = ui->spnBoxPort->value();
        State::getInstance().connect(ip_address, port);
    }
    catch (std::runtime_error e)
    {
        QMessageBox::critical(this, "Błąd połączenia", e.what());
    }
    catch (...)
    {
        QMessageBox::critical(this, "Błąd", "Wystąpił nieznany błąd aplikacji.");
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
            if(checked) {
                emitCurrentSettings();
                if(State::getInstance().getMode() == NetworkManager::Mode::ARX) {
                    State::getInstance().startServerDiscovery();
                }
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
}

// BROADCAST

void WorkDialog::on_chckboxPublicznySerwer_toggled(bool checked)
{
    int port = ui->spnBoxPort->value();
    State::getInstance().setPublicServer(checked, port);
}

void WorkDialog::onServerDiscovered(const QString& ip, int port, bool alive)
{
    QString serverText = QString("%1:%2").arg(ip).arg(port);
    auto items = ui->listWidgetSerwery->findItems(serverText, Qt::MatchExactly);

    if (alive) {
        if (items.isEmpty()) {
            ui->listWidgetSerwery->addItem(serverText);
        }
    } else {
        for (auto* item : items) {
            delete ui->listWidgetSerwery->takeItem(ui->listWidgetSerwery->row(item));
        }
    }
}
