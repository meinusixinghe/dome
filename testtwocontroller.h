#ifndef TESTTWOCONTROLLER_H
#define TESTTWOCONTROLLER_H

#include "syschange.h"
#include "monitorthread.h"
#include <time.h>
#include <thread>

class TestTwoController
{
public:
    explicit TestTwoController();
    ~TestTwoController();
    void main();

    QThread *monThread;

    std::thread *getCurType_thread;
    std::thread *getCurSvoStu_thread;
    std::thread *clearAlarm_thread;
    std::thread *getMoveStu_thread;
    std::thread *getAlmList_thread;
    std::thread *getCurKeyMode_thread;
    std::thread *getCurSpe_thread;
    std::thread *getCurTool_thread;
    std::thread *getCurUser_thread;
    std::thread *getTool_thread;
    std::thread *getUser_thread;
    std::thread *apiCtrl_thread;
    std::thread *getJoint_thread;
    std::thread *getJoint2_thread;
    std::thread *con_thread;
    std::thread *apiEnable_thread;
    std::thread *apiEnable2_thread;
    std::thread *pwrOn_thread;
    std::thread *loadFile_thread;
    std::thread *getBaseCor_thread;
    std::thread *getProStus_thread;
    std::thread *getProStus2_thread;
    std::thread *readIn_thread;
    std::thread *readIn2_thread;
    std::thread *readIn3_thread;
    std::thread *readOut_thread;
    std::thread *readOut2_thread;
    std::thread *readOut3_thread;
    std::thread *isCon_thread;
    std::thread *isCon2_thread;
    std::thread *disCon_thread;
    std::thread *LdFile_thread;

public slots:
    void GetRobotType(std::string robotName, unsigned dev);
    void IsMove(double pos[], int dev);
    void CurPos(double pos[], int dev);
private:
    time_t GetCurrentTimeStampMS();
};

#endif // TESTTWOCONTROLLER_H
