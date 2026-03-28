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

        QObject::connect(_socket, &QTcpSocket::readyRead, [this]() {
            QByteArray data = _socket->readAll();
            ReadMsg(data);
        });
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

    void ReadMsg(const QByteArray& msg) override
    {
        if (_callback)
            _callback(msg);
    }

    void Connected() override
    {
        // TODO LOGIKA
    }

    void Disconnected() override
    {
        // TODO LOGIKA
    }

private:
    QTcpSocket* _socket;
    QString _address;
};

#endif // TCPCLIENT_H
