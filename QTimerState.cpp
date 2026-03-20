#include "QTimerState.hpp"

QTimerState::QTimerState()
    :timer{QTimer(nullptr)}
{
    timer.setSingleShot(false);
    timer.setTimerType(Qt::TimerType::PreciseTimer);
    QObject::connect(&timer, &QTimer::timeout, this, &QTimerState::runCallback);
}

void QTimerState::setTimeout(std::function<void()> func)
{
    callback = func;
}
void QTimerState::setIntervalMS(unsigned int interval)
{
    timer.setInterval(interval);
}
unsigned int QTimerState::getIntervalMS()
{
    return timer.interval();
}
void QTimerState::setRunning(bool running)
{
    if(running)
        timer.start();
    else
        timer.stop();
}

void QTimerState::runCallback()
{
    callback();
}
