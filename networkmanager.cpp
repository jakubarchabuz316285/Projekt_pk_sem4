#include "networkmanager.h"
#include "qtimer.h"
#include <QDataStream>

NetworkManager::NetworkManager(std::function<void(QByteArray)> fun){
    SetCallback(fun);
    initUdp();
}

void NetworkManager::initUdp() {
    if (!_udpSocket) {
        _udpSocket = new QUdpSocket(this);
    }
    if (!_broadcastTimer) {
        _broadcastTimer = new QTimer(this);
        _broadcastTimer->setInterval(1000);
        connect(_broadcastTimer, &QTimer::timeout, this, [this]() {
            sendBroadcast(true);
        });
    }
    connect(_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::readIncomingBroadcasts);
}

NetworkManager::~NetworkManager() = default;

void NetworkManager::SetMode(const Mode& newMode)
{
    if (_mode == Mode::PID && _broadcastTimer && _broadcastTimer->isActive()) {
        setPublicServer(false, 0);
    }

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
    qDebug() << "NetworkManager mode changed to:" << _mode;
}

void NetworkManager::SetCallback(Tcp::Callback cb)
{
    _callback = cb;
    if (_server) _server->SetCallback(cb);
    else if (_client) _client->SetCallback(cb);
}

void NetworkManager::SendMsg(const QByteArray& msg)
{
    if (_mode == Local)
        throw std::runtime_error("Local mode - no network");

    if (_server){
        _server->SendMsg(msg);
    }
    else if (_client){
        _client->SendMsg(msg);
    }
    else
        throw std::runtime_error("No active connection");
}

void NetworkManager::StartListening(int port)
{
    if (_mode != PID) throw std::runtime_error("Not in server mode");
    if (!_server) throw std::runtime_error("Server not initialized");

    _server->StartListening(port);
}

void NetworkManager::StopListening()
{
    if (_mode != PID) throw std::runtime_error("Not in server mode");

    setPublicServer(false, 0);

    if (_server)
        _server->StopListening();
}

void NetworkManager::DisconnectClient()
{
    if (_mode != PID) throw std::runtime_error("Not in server mode");
    if (!_server) throw std::runtime_error("Server not initialized");

    _server->DisconnectClient();
}

void NetworkManager::Connect(const QString& ip, int port)
{
    if (_mode != ARX) throw std::runtime_error("Not in client mode");
    if (!_client) throw std::runtime_error("Client not initialized");

    _client->Connect(ip, port);
}

void NetworkManager::Disconnect()
{
    if (_mode != ARX) throw std::runtime_error("Not in client mode");
    if (_client) _client->Disconnect();
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

void NetworkManager::setPublicServer(bool publiczny, int tcpPort)
{
    _activeTcpPort = tcpPort;
    if (!_broadcastTimer) return;

    if (publiczny && _mode == Mode::PID) {
        if (!_broadcastTimer->isActive()) {
            _broadcastTimer->start();
            sendBroadcast(true);
        }
    } else {
        if (_broadcastTimer->isActive()) {
            _broadcastTimer->stop();
            sendBroadcast(false);
        }
    }
}

void NetworkManager::startListeningForServers()
{
    if (!_udpSocket) return;

    _udpSocket->close();

    bool success = _udpSocket->bind(45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    qDebug() << "[UDP] Rozpoczęto nasłuchiwanie serwerów na porcie 45454. Status:" << success;
}

void NetworkManager::stopListeningForServers()
{
    if (_udpSocket) {
        _udpSocket->close();
        qDebug() << "[UDP] Zatrzymano nasłuchiwanie serwerów.";
    }
}

void NetworkManager::sendBroadcast(bool alive)
{
    if (!_udpSocket) return;

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << QString("UAR_DISCOVER") << (bool)alive << (quint16)_activeTcpPort;

    _udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
    qDebug() << "[UDP BROADCAST SENT] Alive:" << alive << "TCP Port:" << _activeTcpPort;
}

void NetworkManager::readIncomingBroadcasts()
{
    if (!_udpSocket) return;

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
            quint32 ipv4 = senderIp.toIPv4Address();
            QString ipStr = QHostAddress(ipv4).toString();

            qDebug() << "[UDP DISCOVER RECEIVED] Od:" << ipStr << "Port TCP:" << tcpPort << "Żyje:" << alive;
            emit serverDiscovered(ipStr, (int)tcpPort, alive);
        }
    }
}
