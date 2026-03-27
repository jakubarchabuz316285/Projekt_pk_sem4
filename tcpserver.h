#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp.h"
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public Tcp
{
public:
    TcpServer()
    {
        _server = new QTcpServer();
        _socket = nullptr;

        QObject::connect(_server, &QTcpServer::newConnection, [this]() {
            if (_socket != nullptr)
            {
                // już ktoś jest podłączony → odrzucamy
                QTcpSocket* extra = _server->nextPendingConnection();
                extra->disconnectFromHost();
                extra->deleteLater();
                return;
            }

            _socket = _server->nextPendingConnection();

            QObject::connect(_socket, &QTcpSocket::readyRead, [this]() {
                QByteArray data = _socket->readAll();
                ReadMsg(data);
            });

            QObject::connect(_socket, &QTcpSocket::disconnected, [this]() {
                _socket->deleteLater();
                _socket = nullptr;
            });
        });
    }

    ~TcpServer()
    {
        StopListening();
        delete _server;
    }

    void StartListening(int p)
    {
        if (!_server->listen(QHostAddress::Any, p))
            throw std::runtime_error("Cannot start server");

        _port = p;
    }

    void StopListening()
    {
        if (_socket)
        {
            _socket->disconnectFromHost();
            _socket = nullptr;
        }

        _server->close();
        _port = -1;
    }

    void DisconnectClient()
    {
        if (!_socket) return;
        _socket->disconnectFromHost();
    }

    void SendMsg(const QByteArray& msg) override
    {
        if (!_socket)
            throw std::runtime_error("No client connected");

        _socket->write(msg);
    }

    void ReadMsg(const QByteArray& msg) override
    {
        if (_callback)
            _callback(msg);
    }

private:
    QTcpServer* _server;
    QTcpSocket* _socket;
};
#endif // TCPSERVER_H
