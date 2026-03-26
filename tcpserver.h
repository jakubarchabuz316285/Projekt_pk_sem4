#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "QTcpServer"
#include "QTcpSocket"
#include "QObject"

class TcpServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer _server;
    QTcpSocket* _client;
    int _port;
    bool _isListening;
    std::function<void(QByteArray)> _callback;
public:
    TcpServer(std::function<void(QByteArray)>, QObject *parent = nullptr);

    bool startListening(int port);

    void stopListening();

    void sendMsg(QString msg, int numCli);

    void slot_new_client();

    void slot_client_disconnetcted();

    void slot_newMsg();

    void onReadyRead();
};

#endif // TCPSERVER_H
