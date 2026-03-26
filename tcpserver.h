#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "QTcpServer"
#include "QTcpSocket"
class TcpServer
{
private:
    QTcpServer _server;
    QTcpSocket* _client;
    int _port;
    bool _isListening;
public:
    TcpServer();
    bool startListening(int port)
    {
        _port = port;
        _isListening = _server.listen(QHostAddress::Any, port);
        return _isListening;
    }

    void stopListening()
    {
        _server.close();
        _isListening = false;
    }
    void sendPacket(QString msg)
    {
        _client->write(msg.toUtf8()); // TODO Zmienić argument na QByteArray i message
    }
};

#endif // TCPSERVER_H
