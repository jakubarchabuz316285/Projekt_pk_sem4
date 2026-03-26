#include "tcpclient.h"

TcpClient::TcpClient(std::function<void(QByteArray)> fun, QObject *parent)
    : QObject{parent}, _client(this)
{
    _callback = fun;
    connect(&_client,SIGNAL(connected()),this,SLOT(slot_connected()));
    connect(&_client,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    connect(&_client,SIGNAL(readyRead()),this,SLOT(slot_readyRead()));
}

void TcpClient::connectTo(QString address, int port)
{
    _ipAddress = address;
    _port = port;
    _client.connectToHost(_ipAddress,port);
}

void TcpClient::disconnectFrom()
{
    _client.close();
}

void TcpClient::sendMsg(QString msg)
{
    QByteArray data = msg.toUtf8();
    _client.write(data);
}

void TcpClient::slot_connected()
{

}

void TcpClient::slot_readyRead()
{
    _callback(_client.readAll());
}
