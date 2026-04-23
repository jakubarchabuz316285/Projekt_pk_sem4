#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcp.h"
#include <QTcpSocket>

class TcpClient : public Tcp
{
public:
    TcpClient();

    ~TcpClient();

    void Connect(const QString& ip, int p);

    void Disconnect();

    void SendMsg(const QByteArray& msg) override;

    void ReadMsg() override;

    void Connected() override;

    void Disconnected() override;

private:
    QTcpSocket* _socket;
    QString _address;
};

#endif // TCPCLIENT_H
