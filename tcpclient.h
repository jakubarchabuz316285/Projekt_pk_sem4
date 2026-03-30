#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcp.h"
#include <QTcpSocket>

class TcpClient : public Tcp
{
public:
    TcpClient()
    {
        _socket = new QTcpSocket();

        QObject::connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::ReadMsg);
        QObject::connect(_socket, &QTcpSocket::connected, this, &TcpClient::Connected);
        QObject::connect(_socket, &QTcpSocket::disconnected, this, &TcpClient::Disconnected);
    }

    ~TcpClient()
    {
        Disconnect();
        delete _socket;
    }

    void Connect(const QString& ip, int p)
    {
        _socket->connectToHost(ip, p);

        if (!_socket->waitForConnected())
            throw std::runtime_error("Connection failed");

        _port = p;
        _address = ip;
    }

    void Disconnect()
    {
        _socket->disconnectFromHost();
        _port = -1;
    }

    void SendMsg(const QByteArray& msg) override
    {
        if (_socket->state() != QAbstractSocket::ConnectedState)
            throw std::runtime_error("Not connected");

        _socket->write(msg);
    }

    void ReadMsg() override
    {
        QByteArray msg = _socket->readAll();
        if (_callback)
            _callback(msg);
    }

    void Connected() override {
        qDebug() << "Polaczono (client)";
        emit statusChanged(true); // Emitujemy prawdę
    }

    void Disconnected() override {
        qDebug() << "Rozlaczono (client)";
        emit statusChanged(false); // Emitujemy fałsz
    }

private:
    QTcpSocket* _socket;
    QString _address;
};

#endif // TCPCLIENT_H
