#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "qobject.h"
#include "tcpserver.h"
#include "tcpclient.h"

class NetworkManager
{
public:
    enum class Role {Local, Server, Client};
    NetworkManager(Role role, QObject *parent = nullptr);
    TcpServer *_server = nullptr;
    QTcpSocket *_client = nullptr;
    bool isConnected();
    void deleteOldConnections(){
        if(_server != nullptr) {
            _server->stopListening();
            delete _server;
        }
        if(_client != nullptr) {
            _client->disconnect();
            delete _server;
        }
    }

    void setRole(Role role){
        deleteOldConnections();
        if(role == Role::Local){

        }
        if (role == Role::Server){
            _server = new TcpServer(this);
        }
        if (role == Role::Client){
            _client = new TcpClient(this);
        }
    }

private:

};

#endif // NETWORKMANAGER_H
