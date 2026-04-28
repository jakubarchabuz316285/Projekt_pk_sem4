#include "networkmanager.h"

NetworkManager::NetworkManager(std::function<void(QByteArray)> fun){
    SetCallback(fun);
}
NetworkManager::~NetworkManager() = default;

void NetworkManager::SetMode(const Mode& newMode)
{
    // cleanup starego trybu
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

bool NetworkManager::IsListening()
{
    if(_server == nullptr) return false;
    return _server->IsListening();
}
