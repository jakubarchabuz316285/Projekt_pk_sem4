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

        QObject::connect(_server, &QTcpServer::newConnection, this, &TcpServer::Connected);
        QObject::connect(_socket, &QTcpSocket::readyRead, this, &TcpServer::ReadMsg);
        QObject::connect(_socket, &QTcpSocket::disconnected, this, &TcpServer::Disconnected);

    }

    ~TcpServer()
    {
        StopListening();
        delete _server;
    }

    void StartListening(int p)
    {
        qDebug() << "nasluchiwanie server";
        _server->listen(QHostAddress::Any, p);
        qDebug() << "nasluchiwanie server po";
        // if (!_server->listen(QHostAddress::Any, p)){
        //     throw std::runtime_error("Cannot start server");
        // }

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

    void ReadMsg() override
    {
        const QByteArray& msg = _socket->readAll();
        if (_callback)
            _callback(msg);
    }

    bool IsListening(){
        return _server->isListening();
    }

    void Connected() override
    {
        qDebug() << "Polaczono (server)";
        // TODO tu masz sloty do sygnałów connected i disconnected i przyda ci się to do logiki lampek w gui to samow tcp client
    }

    void Disconnected() override
    {
        qDebug() << "Polaczono (server)";
        // TODO
    }
private:
    QTcpServer* _server;
    QTcpSocket* _socket;
};
#endif // TCPSERVER_H
