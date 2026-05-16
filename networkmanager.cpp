#include "networkmanager.h"
#include "qtimer.h"

NetworkManager::NetworkManager(std::function<void(QByteArray)> fun){
    SetCallback(fun);

    _udpSocket = new QUdpSocket(this);
    _broadcastTimer = new QTimer(this);
    _broadcastTimer->setInterval(1000);

    connect(_broadcastTimer, &QTimer::timeout, this, [this]() {
        sendBroadcast(true);
    });

    connect(_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::readIncomingBroadcasts);
}
NetworkManager::~NetworkManager() = default;

void NetworkManager::SetMode(const Mode& newMode)
{
    Cleanup();

    _mode = newMode;

    switch (_mode)
    {
    case PID:
        _server = std::make_unique<TcpServer>();
        _server->SetCallback(_callback);
        connect(_server.get(), &Tcp::statusChanged, this, &NetworkManager::statusChanged);
        break;

    case ARX:
        _client = std::make_unique<TcpClient>();
        _client->SetCallback(_callback);
        connect(_client.get(), &Tcp::statusChanged, this, &NetworkManager::statusChanged);
        break;

    case Local:
    default:
        break;
    }
    qDebug() << _mode;
}

void NetworkManager::SetCallback(Tcp::Callback cb)
{
    _callback = cb;

    if (_server) _server->SetCallback(cb);
    else if (_client) _client->SetCallback(cb);
}

// SEND
void NetworkManager::SendMsg(const QByteArray& msg)
{
    qDebug() << "Wysyłanie wiadomości (network manager)";
    if (_mode == Local)
        throw std::runtime_error("Local mode - no network");

    if (_server){
        qDebug() << "Wysyłanie wiadomości (network manager - serwer)";
        _server->SendMsg(msg);
    }
    else if (_client){
        qDebug() << "Wysyłanie wiadomości (network manager - serwer)";
        _client->SendMsg(msg);
    }
    else
        throw std::runtime_error("No active connection");
}

// SERVER
void NetworkManager::StartListening(int port)
{
    if (_mode != PID){
        qDebug() << "Not pid mode";
        throw std::runtime_error("Not in server mode");
    }

    if (!_server){
        qDebug() << "server not initialized";
        throw std::runtime_error("Server not initialized");
    }
    qDebug() << "Network manager nasluchiwanie";
    _server->StartListening(port);
    qDebug() << "Network manager nasluchiwanie po";
}

void NetworkManager::StopListening()
{
    if (_mode != PID)
        throw std::runtime_error("Not in server mode");

    if (_server)
        _server->StopListening();
}

void NetworkManager::DisconnectClient()
{
    if (_mode != PID)
        throw std::runtime_error("Not in server mode");

    if (!_server)
        throw std::runtime_error("Server not initialized");

    // rozłącza aktualnego klienta (serwer dalej nasłuchuje)
    _server->DisconnectClient();
}

// CLIENT
void NetworkManager::Connect(const QString& ip, int port)
{
    if (_mode != ARX)
        throw std::runtime_error("Not in client mode");

    if (!_client)
        throw std::runtime_error("Client not initialized");

    _client->Connect(ip, port);
}

void NetworkManager::Disconnect()
{
    if (_mode != ARX)
        throw std::runtime_error("Not in client mode");

    if (_client)
        _client->Disconnect();
}

NetworkManager::Mode NetworkManager::GetMode() const { return _mode; }

void NetworkManager::Cleanup()
{
    if (_server)
    {
        _server->StopListening();
        _server.reset();
    }

    if (_client)
    {
        _client->Disconnect();
        _client.reset();
    }
}

bool NetworkManager::IsListening() const
{
    if(_server == nullptr) return false;
    return _server->IsListening();
}

bool NetworkManager::isConnected() const
{
    if (_mode == Mode::Local) { return false; }
    if (_mode == Mode::ARX) { return _client && _client->isConnected(); }
    if (_mode == Mode::PID) { return _server && _server->hasConnectedClients(); }

    return false;
}

// BROADCAST

void NetworkManager::setPublicServer(bool publiczny, int tcpPort)
{
    _activeTcpPort = tcpPort;
    if (publiczny && _mode == Mode::PID) {
        _broadcastTimer->start();
        sendBroadcast(true);
    } else {
        if (_broadcastTimer->isActive()) {
            _broadcastTimer->stop();
            sendBroadcast(false);
        }
    }
}

void NetworkManager::startListeningForServers()
{

    _udpSocket->bind(45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

void NetworkManager::stopListeningForServers()
{
    _udpSocket->close();
}

void NetworkManager::sendBroadcast(bool alive)
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << QString("UAR_DISCOVER") << (bool)alive << (quint16)_activeTcpPort;

    // Wysyłamy na port 45454 do wszystkich w sieci lokalnej
    _udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void NetworkManager::readIncomingBroadcasts()
{
    while (_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(_udpSocket->pendingDatagramSize());
        QHostAddress senderIp;

        _udpSocket->readDatagram(datagram.data(), datagram.size(), &senderIp);

        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_6_0);

        QString header;
        bool alive;
        quint16 tcpPort;
        in >> header >> alive >> tcpPort;

        if (header == "UAR_DISCOVER") {
            QString ipStr = QHostAddress(senderIp.toIPv4Address()).toString();
            emit serverDiscovered(ipStr, (int)tcpPort, alive);
        }
    }
}
