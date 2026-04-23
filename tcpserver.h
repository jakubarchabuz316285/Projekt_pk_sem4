#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcp.h"
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public Tcp
{
public:
    TcpServer();

    ~TcpServer();

    void StartListening(int p);

    void StopListening();

    void DisconnectClient();

    void SendMsg(const QByteArray& msg) override;

    void ReadMsg() override;

    bool IsListening();

    void Connected() override;

    void Disconnected() override;
private:
    QTcpServer* _server;
    QTcpSocket* _socket;
};
#endif // TCPSERVER_H
