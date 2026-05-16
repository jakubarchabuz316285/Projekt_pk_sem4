#include "tcpserver.h"

TcpServer::TcpServer()
{
    _server = new QTcpServer();
    _socket = nullptr;

    QObject::connect(_server, &QTcpServer::newConnection, this, &TcpServer::Connected);
}

TcpServer::~TcpServer()
{
    StopListening();
    delete _server;
}

void TcpServer::StartListening(int p)
{
    qDebug() << "nasluchiwanie server";
    _server->listen(QHostAddress::Any, p);
    qDebug() << "nasluchiwanie server po";
    // if (!_server->listen(QHostAddress::Any, p)){
    //     throw std::runtime_error("Cannot start server");
    // }

    _port = p;
}

void TcpServer::StopListening()
{
    if (_socket)
    {
        _socket->disconnectFromHost();
        _socket = nullptr;
    }

    _server->close();
    _port = -1;
}

void TcpServer::DisconnectClient()
{
    if (!_socket) return;
    _socket->disconnectFromHost();
}

void TcpServer::SendMsg(const QByteArray& msg)
{
    if (!_socket)
        throw std::runtime_error("No client connected");

    qDebug() << "Wysyłanie wiadomości (serwer)";
    _socket->write(msg);
}

void TcpServer::ReadMsg()
{
    const QByteArray& msg = _socket->readAll();
    if (_callback)
        _callback(msg);
}

bool TcpServer::IsListening()
{
    return _server->isListening();
}

void TcpServer::Connected()
{
    _socket = _server->nextPendingConnection();

    connect(_socket, &QTcpSocket::readyRead, this, &TcpServer::ReadMsg);
    connect(_socket, &QTcpSocket::disconnected, this, &TcpServer::Disconnected);

    _socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    qDebug() << "Klient polaczony z serwerem";
    emit statusChanged(true);
}

void TcpServer::Disconnected()
{
    qDebug() << "Klient rozlaczony od serwera";
    if (_socket) {
        _socket->deleteLater();
        _socket = nullptr;
    }
    emit statusChanged(false);
}

bool TcpServer::hasConnectedClients() const
{
    return _socket && (_socket->state() == QAbstractSocket::ConnectedState);
}
