#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QTcpSocket>
class TcpClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket _client;
    QString _ipAddress = "127.0.0.1";
    int _port = 12345;
    std::function<void(QByteArray)> _callback;
public:
    TcpClient(std::function<void(QByteArray)>, QObject *parent = nullptr);

    void connectTo(QString address, int port);

    void disconnectFrom();

    void sendMsg(QString msg);

    void slot_connected();

    void slot_readyRead();
};

#endif // TCPCLIENT_H
