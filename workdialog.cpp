#include "workdialog.h"
#include "qbuttongroup.h"
#include "qdatetime.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include "qtimer.h"
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

    _udpCleanupTimer = new QTimer(this);
    connect(_udpCleanupTimer, &QTimer::timeout, this, [this]() {
        QDateTime now = QDateTime::currentDateTime();
        auto it = _discoveredServersTime.begin();
        while (it != _discoveredServersTime.end()) {
            // Jeśli serwer nie dał znaku życia przez ponad 3 sekundy -> usuń go
            if (it.value().secsTo(now) > 3) {
                QString serverText = it.key();
                auto items = ui->listWidgetSerwery->findItems(serverText, Qt::MatchExactly);
                for (auto* item : items) {
                    delete ui->listWidgetSerwery->takeItem(ui->listWidgetSerwery->row(item));
                }
                it = _discoveredServersTime.erase(it);
                qDebug() << "[UDP TIMEOUT] Usunięto martwy serwer z listy:" << serverText;
            } else {
                ++it;
            }
        }
    });
    _udpCleanupTimer->start(1000); // Sprawdzaj co sekundę

    connect(&State::getInstance(), &State::serverDiscovered, this, &::WorkDialog::onServerDiscovered);
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
    // Sprawdzamy, czy w ogóle jesteśmy w trybie sieciowym
    bool isNet = ui->RdioNet->isChecked();
    bool isArx = ui->rdio_arx->isChecked() && isNet;
    bool isPid = ui->rdio_reg->isChecked() && isNet;

    ui->IPContainter->setVisible(isArx);
    ui->BtnConnect->setVisible(isArx);
    ui->listWidgetSerwery->setVisible(isArx);

    ui->BtnListen->setVisible(isPid);
    ui->chckboxPublicznySerwer->setVisible(isPid);

    ui->LblPort->setVisible(isNet);
    ui->spnBoxPort->setVisible(isNet);
    ui->BtnDisconnect->setVisible(isNet);

    if (isArx) {
        State::getInstance().startServerDiscovery();
    } else {
        State::getInstance().stopServerDiscovery();
        ui->listWidgetSerwery->clear();
    }

    if (!isPid) {
        if (ui->chckboxPublicznySerwer->isChecked()) {
            ui->chckboxPublicznySerwer->setChecked(false);
        }
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
           UpdateNetworkUI();
           ui->spnBoxPort->setValue(123);
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
            UpdateNetworkUI();
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
            int port = ui->spnBoxPort->value();
            State::getInstance().startListening(port);
            ui->BtnListen->setText("Przestań nasłuchiwać");

            if (ui->chckboxPublicznySerwer->isChecked()) {
                State::getInstance().setPublicServer(true, port);
            }
        } else {
            State::getInstance().setPublicServer(false, 0);

            State::getInstance().stopListening();
            ui->BtnListen->setText("Nasłuchuj");
        }
    }
    catch (...)
    {

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
                UpdateNetworkUI();
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
    if(State::getInstance().isListening())
    {
        State::getInstance().setPublicServer(checked, port);
    }
}

void WorkDialog::onServerDiscovered(const QString& ip, int port, bool alive)
{
    QString serverText = QString("%1:%2").arg(ip).arg(port);
    auto items = ui->listWidgetSerwery->findItems(serverText, Qt::MatchExactly);

    if (alive) {
        _discoveredServersTime[serverText] = QDateTime::currentDateTime();

        if (items.isEmpty()) {
            ui->listWidgetSerwery->addItem(serverText);
        }
    } else {
        _discoveredServersTime.remove(serverText);
        for (auto* item : items) {
            delete ui->listWidgetSerwery->takeItem(ui->listWidgetSerwery->row(item));
        }
    }
}

void WorkDialog::closeEvent(QCloseEvent *event)
{
    State::getInstance().setPublicServer(false, 0);

    State::getInstance().stopListening();
    State::getInstance().stopServerDiscovery();

    event->accept();
}
