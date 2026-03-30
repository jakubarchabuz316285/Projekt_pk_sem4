#ifndef TCP_H
#define TCP_H

#include "qobject.h"
#pragma once
#include <functional>
#include <QByteArray>

class Tcp:  public QObject
{
    Q_OBJECT
public:
    using Callback = std::function<void(const QByteArray&)>;

    virtual ~Tcp() = default;

    virtual void SendMsg(const QByteArray& msg) = 0;
    virtual void ReadMsg() = 0;

    virtual void Connected() = 0;
    virtual void Disconnected() = 0;

    void SetCallback(Callback cb)
    {
        _callback = cb;
    }

protected:
    int _port = -1;
    Callback _callback;
};

#endif // TCP_H
