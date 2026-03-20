#ifndef QTIMERSTATE_HPP
#define QTIMERSTATE_HPP

#include "State.h"
#include <QObject>
#include <qtimer.h>

class QTimerState : public QObject, public TimerStateInterface
{
    Q_OBJECT

    QTimer timer;
    std::function<void()> callback;
public slots:
    void runCallback();
public:
    QTimerState();

    void setTimeout(std::function<void()>) override;
    void setIntervalMS(unsigned int) override;
    unsigned int getIntervalMS() override;
    void setRunning(bool running) override;

};

#endif // QTIMERSTATE_HPP
