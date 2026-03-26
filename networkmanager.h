#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "qobject.h"
#include "tcpserver.h"
#include "QTcpSocket"

class NetworkManager
{
public:
    enum Role {Local, Server, Client};
    NetworkManager(Role role, QObject *parent = nullptr);
    TcpServer *_pid = nullptr;
    QTcpSocket *_arx = nullptr;
    bool isConnected();

private:

};

#endif // NETWORKMANAGER_H
