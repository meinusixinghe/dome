#include "monitorthread.h"
#include "EfortSdk.h"

using namespace RobotAPI;
MonitorThread::MonitorThread(QObject* parent)
    : QThread(parent),
    pauseFlag(false),
    stopFlag(false)
{
}

MonitorThread::~MonitorThread()
{
    stop_th();
}

MonitorThread::State MonitorThread::state() const
{
    State s = Stoped;
    if (!MonitorThread::isRunning())
    {
        s = Stoped;
    }
    else if (MonitorThread::isRunning() && pauseFlag)
    {
        s = Paused;
    }
    else if (MonitorThread::isRunning() && (!pauseFlag))
    {
        s = Running;
    }
    return s;
}

void MonitorThread::start(Priority pri)
{
    QThread::start(pri);
}

void MonitorThread::stop_th()
{
    if (MonitorThread::isRunning())
    {
        stopFlag = true;
        condition.wakeAll();
        MonitorThread::quit();
        MonitorThread::wait();
    }
}

void MonitorThread::pause()
{
    if (MonitorThread::isRunning())
    {
        pauseFlag = true;
    }
}

void MonitorThread::resume()
{
    if (MonitorThread::isRunning())
    {
        pauseFlag = false;
        condition.wakeAll();
    }
}

void MonitorThread::run()
{
    // qDebug() << QString("开始执行计时线程") << MonitorThread::currentThreadId();
    _timer = new QTimer();
    _timer->setInterval(100);
    connect(_timer, &QTimer::timeout, [=] {
        if (!pauseFlag)
        {
            timerRun();
        }
        if (stopFlag)
        {
            _timer->stop();
        }
        });
    _timer->start();
    this->exec();

    pauseFlag = false;
    stopFlag = false;
    // qDebug() << QString("结束执行计时线程") << MonitorThread::currentThreadId();
}

void MonitorThread::timerRun()
{
    int res = 0;

    IsConnected();
    // qDebug() << QString("is connected: ") << isCon << QThread::currentThreadId()<<endl;

    time_t startTime = GetCurrentTimeStampMS();
    // qDebug() << "startTime:" << startTime << endl;
    {
        bool emgStatus = false;
        res = GetCurrentEmgStatus(emgStatus);
        // qDebug() << "GetCurrentEmgStatus time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        //qDebug() << QString("Emg status: ") << emgStatus << endl;
    }

    {
        std::string robotName;
        startTime = GetCurrentTimeStampMS();
        GetCurrentRobotType(robotName);
        //qDebug() << "GetCurrentRobotType time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        //qDebug() << QString("GetCurrentRobotType: ")<< robotName.c_str()<< endl;
    }

    {
        bool servoStatus = false;
        startTime = GetCurrentTimeStampMS();
        res = GetCurrentServoStatus(servoStatus);
        //qDebug() << "GetCurrentServoStatus time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
    }

    {
        RobotJoint joint;
        startTime = GetCurrentTimeStampMS();
        res = GetJointPos(joint);
        // qDebug() << "GetJointPos time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        // qDebug() << QString("Joint Position: %1").arg(joint.j[0]) << endl;
    }

    {
        RobotPos pos;
        startTime = GetCurrentTimeStampMS();
        res = GetBaseCoordinatePos(pos);
        //qDebug() << "GetJointPos time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        //qDebug() << QString("Cartesian Position: %1 ") .arg(pos.x)<< endl;
    }

    {
        RoboxProgramState rplStatus;
        startTime = GetCurrentTimeStampMS();
        res = GetCurrentProStatus(rplStatus);
        //qDebug() << "GetCurrentProStatus time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        // qDebug() << QString("GetCurrentProStatus: %1 ") .arg(rplStatus)<< endl;
    }

    for (int i = 0; i < 8; i++)
    {
        bool dinVlaue = false;
        startTime = GetCurrentTimeStampMS();
        res = ReadDIn(i, dinVlaue);
        //qDebug() << "ReadDIn time: " << (GetCurrentTimeStampMS() - startTime) <<endl;
        //qDebug() << QString("ReadDIn:index<%1>, value<%2> ") .arg(i).arg(dinVlaue)<< endl;
    }

    emit sendCtrlSignal();
}


time_t MonitorThread::GetCurrentTimeStampMS()
{
#ifdef _WIN32
    SYSTEMTIME currentTime = {0};
    GetLocalTime(&currentTime);

    struct tm currTm = {
        currentTime.wSecond,
        currentTime.wMinute,
        currentTime.wHour,
        currentTime.wDay,
        currentTime.wMonth - 1,
        currentTime.wYear - 1900
    };

    time_t nNow = mktime(&currTm) * 1000 + currentTime.wMilliseconds;
    return nNow;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    time_t timestamp = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return timestamp;
#endif
}
