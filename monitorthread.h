#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#include <QThread>
#include <QDebug>
#include <atomic>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <time.h>
#include "syschange.h"

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    MonitorThread(QObject* parent = nullptr);
    ~MonitorThread() override;

    enum State
    {
        Stoped,
        Running,
        Paused
    };
    State state() const;

private:
    time_t GetCurrentTimeStampMS();
    std::atomic_bool pauseFlag;
    std::atomic_bool stopFlag;
    QMutex mutex;
    QWaitCondition condition;

    QTimer* _timer = nullptr;
public slots:
    void timerRun();//用于反复跑的内容
    void start(Priority pri = InheritPriority);
    void stop_th();
    void pause();
    void resume();
protected:
    virtual void run() override final;

signals:
    void sendCtrlSignal();//和主线程交互触发的signal
};

#endif // MONITORTHREAD_H
