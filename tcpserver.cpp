#include "tcpserver.h"

TcpServer::TcpServer(std::function<void(QByteArray)> fun, QObject *parent)
    : QObject{parent},
    _server(this)
{
    _callback = fun;
    connect(&_server,SIGNAL(newConnection()),this,SLOT(slot_new_client()));
}

bool TcpServer::startListening(int port)
{
    _port = port;
    _isListening = _server.listen(QHostAddress::Any, port);
    return _isListening;
}

void TcpServer::stopListening()
{
    _server.close();
    _isListening = false;
}

void TcpServer::sendMsg(QString msg, int numCli)
{
    _client->write(msg.toUtf8());
}

void TcpServer::slot_new_client()
{
    QTcpSocket* newClient = _server.nextPendingConnection();

    if (_client && _client->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Server busy - rejecting client";

        newClient->write("Server is busy");
        newClient->disconnectFromHost();
        newClient->deleteLater();
        return;
    }

    _client = newClient;

    qDebug() << "Client connected:" << _client->peerAddress().toString();

    _client->write("Hello client");

    connect(_client, &QTcpSocket::readyRead,
            this, &TcpServer::slot_newMsg);

    connect(_client, &QTcpSocket::disconnected,
            this, &TcpServer::slot_client_disconnetcted);
}

void TcpServer::slot_client_disconnetcted()
{
    _isListening = false;
}

void TcpServer::slot_newMsg()
{
    _callback(_client->readAll());
}
