#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#pragma once
#include "tcpclient.h"
#include "tcpserver.h"
#include <stdexcept>
#include <memory>
/**
    * @brief class networkmanager
    * class containing and encapsulating logic of server and client class.
    * methods lets you set app mode and use methods to connect and sent packets through internet via tcp
    * class is secured with throws. Please use catch runtime error.
    */
class NetworkManager: public QObject
{
    Q_OBJECT
public:
    enum Mode { Local, PID, ARX };

    NetworkManager() = default;
    NetworkManager(std::function<void(QByteArray)> fun);
    ~NetworkManager();

    /**
    * @brief method setMode
    * Sets app mode
    *
    */
    void SetMode(const Mode& newMode);
    /**
    * @brief method setCallback
    * Used once in a constructor, i think it should be private but im too sleepy to check
    *
    */
    void SetCallback(Tcp::Callback cb);

    // SEND
    /**
    * @brief method sendMsg
    * Sends QByteArray through internet. Must be ARX or PID mode
    *
    */
    void SendMsg(const QByteArray& msg);

    // SERVER
    /**
    * @brief method startListening
    * Starts listening. Must be server
    *
    */
    void StartListening(int port);
    /**
    * @brief method stopListening
    * Stops listening. Must be server
    *
    */
    void StopListening();
    /**
    * @brief method disconnectClient
    * Disconnects current client. Client can be null. Must be server
    *
    */
    void DisconnectClient();

    // CLIENT
    /**
    * @brief method Connect
    * Connects to server. Must be client
    * @param ip @param port
    *
    */
    void Connect(const QString& ip, int port);

    /**
    * @brief method Disconnect
    * Disconnects from server. Must be client
    *
    *
    */
    void Disconnect();
    /**
    * @brief method GetMode
    * Returns current mode
    *
    */
    Mode GetMode() const;
    /**
    * @brief method Cleanup
    * Resets everything. Stops listening and disconnects client
    *
    */
    void Cleanup();
    /**
    * @brief method isListening
    * returns true if server is listeing
    * @return isListening
    *
    */
    bool IsListening();
signals:
    void statusChanged(bool connected);
private:
    Mode _mode = Local;

    std::unique_ptr<TcpServer> _server;
    std::unique_ptr<TcpClient> _client;

    Tcp::Callback _callback;
};
#endif // NETWORKMANAGER_H
