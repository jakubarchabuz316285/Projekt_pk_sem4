#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "qobject.h"
class NetworkManager
{
public:
    enum Role {Server, Client};
    NetworkManager(Role role, QObject *parent = nullptr);

    bool isConnected();

private:

};

#endif // NETWORKMANAGER_H
