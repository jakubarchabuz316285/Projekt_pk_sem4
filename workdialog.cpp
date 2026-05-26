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

    if (this->layout()) {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }

    // --- 1. ODCZYT BIEŻĄCEGO STANU APLIKACJI ---
    NetworkManager::Mode currentMode = State::getInstance().getMode();
    bool isConnected = State::getInstance().isConnected();

    // --- 2. BLOKOWANIE SYGNAŁÓW (Zapobiega resetowaniu stanu przez toggled!) ---
    ui->RdioLocal->blockSignals(true);
    ui->RdioNet->blockSignals(true);
    ui->rdio_arx->blockSignals(true);
    ui->rdio_reg->blockSignals(true);

    // --- 3. USTAWIENIE ODPOWIEDNICH PRZYCISKÓW ---
    if (currentMode == NetworkManager::Mode::Local) {
        ui->RdioLocal->setChecked(true);
    } else {
        ui->RdioNet->setChecked(true);
        if (currentMode == NetworkManager::Mode::ARX) {
            ui->rdio_arx->setChecked(true);
        } else if (currentMode == NetworkManager::Mode::PID) {
            ui->rdio_reg->setChecked(true);
        }
    }

    // --- 4. ODBLOKOWANIE SYGNAŁÓW ---
    ui->RdioLocal->blockSignals(false);
    ui->RdioNet->blockSignals(false);
    ui->rdio_arx->blockSignals(false);
    ui->rdio_reg->blockSignals(false);

    ui->spnBoxPort->setMinimumWidth(50);
    ui->spnBoxPort->setMinimum(0);
    ui->spnBoxPort->setMaximum(65535);
    ui->SpnFirst->setRange(0, 255);
    ui->SpnSecond->setRange(0, 255);
    ui->SpnThird->setRange(0, 255);
    ui->SpnFourth->setRange(0, 255);

    // --- 5. ODCZYT DIODY I STATUSU POŁĄCZENIA ---
    updateLedStatus(isConnected);
    if(isConnected) {
        ui->LblStatusText->setText("Połączono");
    } else {
        ui->LblStatusText->setText("Brak połączenia");
    }

    UpdateNetworkUI();

    // Połączenia zdarzeń
    connect(&State::getInstance(), &State::statusChanged, this, [this](bool connected){
        if(connected) {
            ui->LblStatusLed->setStyleSheet("background-color: #2ecc71; border-radius: 8px; border: 1px solid #27ae60;");
            ui->LblStatusText->setText("Połączono");
        } else {
            ui->LblStatusLed->setStyleSheet("background-color: #e74c3c; border-radius: 8px; border: 1px solid #c0392b;");
            ui->LblStatusText->setText("Rozłączono");
        }
    });

    connect(&State::getInstance(), &State::serverDiscovered, this, &::WorkDialog::onServerDiscovered);
    connect(ui->listWidgetSerwery, &QListWidget::itemDoubleClicked, this, &WorkDialog::onServerDoubleClicked);
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
    if(checked) {
        emitCurrentSettings();
        UpdateNetworkUI();
        ui->spnBoxPort->setValue(123);
    }
}
void WorkDialog::emitCurrentSettings() {
    bool isLocal = ui->RdioLocal->isChecked();
    NetworkManager::Mode mode;

    // Decyzja o głównym trybie pracy na podstawie kontrolek
    if (isLocal) {
        mode = NetworkManager::Mode::Local;
    } else {
        mode = ui->rdio_arx->isChecked() ? NetworkManager::Mode::ARX : NetworkManager::Mode::PID;
    }

    // Bezpieczne ustawienie trybu w klasie State
    State::getInstance().setMode(mode);

    // Zawsze wysyłamy do GUI tryb ARX lub PID (nawet jak jest Local),
    // by MainWindow wiedziało, które panele odblokować po przejściu w Sieć.
    NetworkManager::Mode guiMode = ui->rdio_arx->isChecked() ? NetworkManager::Mode::ARX : NetworkManager::Mode::PID;
    emit settingsChanged(guiMode, isLocal);
}

void WorkDialog::on_rdio_arx_toggled(bool checked)
{
    if(checked) {
        emitCurrentSettings();
        UpdateNetworkUI();
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
    if(checked) {
        emitCurrentSettings();
        UpdateNetworkUI();
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

    if (alive) {
        // Serwer żyje - szukamy dokładnego dopasowania IP:Port
        auto items = ui->listWidgetSerwery->findItems(serverText, Qt::MatchExactly);
        if (items.isEmpty()) {
            ui->listWidgetSerwery->addItem(serverText);
        }
    } else {
        // Serwer umiera (port przyszedł jako 0, więc szukamy po samym IP)
        // MatchStartsWith dopasuje "26.124.116.142:123" na podstawie samego "26.124.116.142"
        auto items = ui->listWidgetSerwery->findItems(ip, Qt::MatchStartsWith);

        for (auto* item : items) {
            delete ui->listWidgetSerwery->takeItem(ui->listWidgetSerwery->row(item));
        }
        qDebug() << "[GUI] Serwer zamknął się czysto. Usunięto z listy IP:" << ip;
    }
}

void WorkDialog::closeEvent(QCloseEvent *event)
{
    // if (State::getInstance().getMode() == NetworkManager::Mode::PID)
    // {
    //     // Bezpieczne gaszenie serwera
    //     //State::getInstance().setPublicServer(false, 0);
    //     //State::getInstance().stopListening();
    //     State::getInstance().stopServerDiscovery();
    // }
    // else if (State::getInstance().getMode() == NetworkManager::Mode::ARX)
    // {
    //     // Klient MUSI przestać słuchać UDP zanim okno zostanie usunięte z pamięci!
    //     State::getInstance().stopServerDiscovery();
    //     State::getInstance().disconnect(); // Czyste zerwanie TCP, jeśli było połączone
    // }
    State::getInstance().stopServerDiscovery();
    event->accept();
}

void WorkDialog::onServerDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString rawText = item->text(); // Pobieramy tekst np. "26.124.116.142:123"
    QStringList parts = rawText.split(':');

    if (parts.size() == 2) {
        QString ipAddress = parts.at(0); // "26.124.116.142"
        QString portStr = parts.at(1);   // "123"

        // 1. UZUPEŁNIENIE PORTU
        ui->spnBoxPort->setValue(portStr.toInt());

        // 2. ROZBICIE IP NA OSOBNE OKTETY (SEGMENTY)
        QStringList octets = ipAddress.split('.');

        if (octets.size() == 4) {
            // Zamieniamy każdy segment tekstu na liczbę i ładujemy do spinboxów
            ui->SpnFirst->setValue(octets.at(0).toInt());
            ui->SpnSecond->setValue(octets.at(1).toInt());
            ui->SpnThird->setValue(octets.at(2).toInt());
            ui->SpnFourth->setValue(octets.at(3).toInt());

            qDebug() << "[GUI] Rozbito IP na segmenty ->"
                     << octets.at(0) << octets.at(1) << octets.at(2) << octets.at(3)
                     << "Port:" << portStr;
        } else {
            qWarning() << "[GUI BŁĄD] Niepoprawny format adresu IP w obiekcie:" << ipAddress;
        }
    }
}
