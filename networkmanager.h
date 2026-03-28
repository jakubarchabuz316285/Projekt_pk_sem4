#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#pragma once
#include "tcpclient.h"
#include "tcpserver.h"
#include <stdexcept>
#include <memory>

class NetworkManager
{
public:
    enum Mode { Local, PID, ARX };

    NetworkManager() = default;
    NetworkManager(std::function<void(QByteArray)> fun){
        SetCallback(fun);
    }
    ~NetworkManager() = default;

    void SetMode(Mode newMode)
    {
        // cleanup starego trybu
        Cleanup();

        _mode = newMode;

        switch (_mode)
        {
        case PID:
            _server = std::make_unique<TcpServer>();
            _server->SetCallback(_callback);
            break;

        case ARX:
            _client = std::make_unique<TcpClient>();
            _client->SetCallback(_callback);
            break;

        case Local:
        default:
            break;
        }
    }

    void SetCallback(Tcp::Callback cb)
    {
        _callback = cb;

        if (_server) _server->SetCallback(cb);
        if (_client) _client->SetCallback(cb);
    }

    // 🔹 SEND
    void SendMsg(const QByteArray& msg)
    {
        if (_mode == Local)
            throw std::runtime_error("Local mode - no network");

        if (_server)
            _server->SendMsg(msg);
        else if (_client)
            _client->SendMsg(msg);
        else
            throw std::runtime_error("No active connection");
    }

    // 🔹 SERVER
    void StartListening(int port)
    {
        if (_mode != PID)
            throw std::runtime_error("Not in server mode");

        if (!_server)
            throw std::runtime_error("Server not initialized");

        _server->StartListening(port);
    }

    void StopListening()
    {
        if (_mode != PID)
            throw std::runtime_error("Not in server mode");

        if (_server)
            _server->StopListening();
    }

    void DisconnectClient()
    {
        if (_mode != PID)
            throw std::runtime_error("Not in server mode");

        if (!_server)
            throw std::runtime_error("Server not initialized");

        // rozłącza aktualnego klienta (serwer dalej nasłuchuje)
        _server->DisconnectClient();
    }

    // 🔹 CLIENT
    void Connect(const std::string& ip, int port)
    {
        if (_mode != ARX)
            throw std::runtime_error("Not in client mode");

        if (!_client)
            throw std::runtime_error("Client not initialized");

        _client->Connect(ip, port);
    }

    void Disconnect()
    {
        if (_mode != ARX)
            throw std::runtime_error("Not in client mode");

        if (_client)
            _client->Disconnect();
    }

    Mode GetMode() const { return _mode; }

    void Cleanup()
    {
        if (_server)
        {
            _server->StopListening();
            _server.reset();
        }

        if (_client)
        {
            _client->Disconnect();
            _client.reset();
        }
    }

private:
    Mode _mode = Local;

    std::unique_ptr<TcpServer> _server;
    std::unique_ptr<TcpClient> _client;

    Tcp::Callback _callback;
};
#endif // NETWORKMANAGER_H
