#include "tcpclient.h"

TcpClient::TcpClient()
{
    _socket = new QTcpSocket();

    QObject::connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::ReadMsg);
    QObject::connect(_socket, &QTcpSocket::connected, this, &TcpClient::Connected);
    QObject::connect(_socket, &QTcpSocket::disconnected, this, &TcpClient::Disconnected);
}

TcpClient::~TcpClient()
{
    Disconnect();
    delete _socket;
}

void TcpClient::Connect(const QString& ip, int p)
{
    _socket->connectToHost(ip, p);

    if (!_socket->waitForConnected(2000))
        throw std::runtime_error("Connection failed");

    _port = p;
    _address = ip;
}

void TcpClient::Disconnect()
{
    _socket->disconnectFromHost();
    _port = -1;
}

void TcpClient::SendMsg(const QByteArray& msg)
{
    if (_socket->state() != QAbstractSocket::ConnectedState)
        throw std::runtime_error("Not connected");

    qDebug() << "Wysyłanie wiadomości (klient)";
    _socket->write(msg);
}

void TcpClient::ReadMsg()
{
    QByteArray msg = _socket->readAll();
    if (_callback)
        _callback(msg);
}

void TcpClient::Connected()  {
    qDebug() << "Polaczono (client)";
    _socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    emit statusChanged(true); // Emitujemy prawdę
}

void TcpClient::Disconnected()  {
    qDebug() << "Rozlaczono (client)";
    emit statusChanged(false); // Emitujemy fałsz
}
