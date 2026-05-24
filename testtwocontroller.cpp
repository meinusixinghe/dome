#include "testtwocontroller.h"
#include "EfortSdk.h"
#include <QDebug>
#include <QThread>
#include <iostream>
#include <time.h>
#include <chrono>

using namespace RobotAPI;
using namespace std;
TestTwoController::TestTwoController()
{
    monThread = new MonitorThread();
}

TestTwoController::~TestTwoController()
{

}

void TestTwoController::GetRobotType(string robotName, unsigned dev)
{
    int res = GetCurrentRobotType(robotName, dev);
    if(res != ERROR_OK)
    {
        cout << "GetRobotType failed" << endl;
    }
    else
    {
        cout << "Robot Name is " << robotName << endl;
    }
}

void GetServoStatus(bool svoValue, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentServoStatus(svoValue, dev);
        if(res != ERROR_OK)
        {
            cout << "GetCurrentServo failed" << endl;
        }
        else
        {
            cout << "Servo Status is " << svoValue << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void ClrAlm(unsigned dev)
{
    while(true)
    {
        int res = ClearAlarm(dev);
        if(res != ERROR_OK)
        {
            cout << "ClrAlm failed" << endl;
        }
        else
        {
            cout << "ClearAlarm ok" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetMvState(bool moveState, unsigned dev)
{
    while(true)
    {
        int res = GetMoveState(moveState, dev);
        if(res != ERROR_OK)
        {
            cout << "GetMvState failed" << endl;
        }
        else
        {
            cout << "Move Status is " << moveState << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetAlmList(vector<unsigned int> list, unsigned dev)
{
    while(true)
    {
        int res = GetAlarmList(list, dev);
        if(res != ERROR_OK)
        {
            cout << "GetAlmList failed" << endl;
        }
        else
        {
            cout << "Alarm number is " << list.size() << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetKeyMode(RoboxKeyMode modeValue, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentKeyMode(modeValue, dev);
        if(res != ERROR_OK)
        {
            cout << "GetKeyMode failed" << endl;
        }
        else
        {
            cout << "Current Keymode is " << modeValue << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetSpeedRatio(unsigned int speed, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentSpeedRatio(speed, dev);
        if(res != ERROR_OK)
        {
            cout << "GetSpeedRatio failed" << endl;
        }
        else
        {
            cout << "Current speed is " << speed << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetToolName(string toolName, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentToolName(toolName, dev);
        if(res != ERROR_OK)
        {
            cout << "GetToolName failed" << endl;
        }
        else
        {
            cout << "Current Toolname is " << toolName << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetUframeName(string userName, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentUframeName(userName, dev);
        if(res != ERROR_OK)
        {
            cout << "GetUframeName failed" << endl;
        }
        else
        {
            cout << "Current Uframe name is " << userName << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetToolCoor(string toolName, RobotTool toolValue, unsigned dev)
{
    while(true)
    {
        int res = GetToolCoordinate(toolName, toolValue, dev);
        if(res != ERROR_OK)
        {
            cout << "GetToolCoor failed" << endl;
        }
        else
        {
            cout << "tool.x = " << toolValue.x << ", y = " << toolValue.y << ", z = " << toolValue.z << ", a = " << toolValue.a
                << ", b = " << toolValue.b << ", c = " << toolValue.c << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetUserCoor(string userName, RobotWorkpiece userValue, unsigned dev)
{
    while(true)
    {
        int res = GetUserCoordinate(userName, userValue, dev);
        if(res != ERROR_OK)
        {
            cout << "GetUserCoor failed" << endl;
        }
        else
        {
            cout << "user.x = " << userValue.x << ", y = " << userValue.y << ", z = " << userValue.z << ", a = " << userValue.a
                << ", b = " << userValue.b << ", c = " << userValue.c << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void IsApiCtrl(unsigned dev)
{
    while(true)
    {
        bool res = IsApiControl(dev);
        cout << "ApiCtrl status is " << res << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetJoint(RobotJoint value, unsigned dev)
{
    while(true)
    {
        int res = GetJointPos(value, dev);
        if(res != ERROR_OK)
        {
            cout << "GetJoint failed" << endl;
        }
        else
        {
            cout << "value.j[0] = " << value.j[0] << ", value.j[1] = " << value.j[1] << ", value.j[2] = " << value.j[2] << ", value.j[3] = " << value.j[3]
                << ", value.j[4] = " << value.j[4] << ", value.j[5] = " << value.j[5] << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void GetJoint2(RobotJoint value, unsigned dev)
{
    while(true)
    {
        int res = GetJointPos(value, dev);
        if(res != ERROR_OK)
        {
            cout << "GetJoint failed" << endl;
        }
        else
        {
            cout << "value2.j[0] = " << value.j[0] << ", value2.j[1] = " << value.j[1] << ", value2.j[2] = " << value.j[2] << ", value2.j[3] = " << value.j[3]
                << ", value2.j[4] = " << value.j[4] << ", value2.j[5] = " << value.j[5] << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void ConRobot(unsigned dev1)
{
    while(true)
    {
        int res = ConnectRobot("192.168.1.12", dev1);
        if(res != ERROR_OK)
        {
            cout << "ConRobot failed, res = " << res << endl;
        }
        else
        {
            cout << "ConRobot ok" << endl;
        }
    }
}

void DisCon(unsigned dev)
{
    while(true)
    {
        DisconnectRobot(dev);
        cout << "DisConnect Success" << endl;
    }
}

void enApiCtrl(unsigned dev)
{
    while(true)
    {
        int res = EnableApiControl(true, dev);
        if(res != ERROR_OK)
        {
            cout << "Enable ApiCtrl failed" << endl;
        }
        else
        {
            cout << "Enable Api ok" << endl;
        }
    }
}
void enApiCtrl2(unsigned dev)
{
    while(true)
    {
        int res = EnableApiControl(true, dev);
        if(res != ERROR_OK)
        {
            cout << "Enable ApiCtrl2 failed" << endl;
        }
        else
        {
            cout << "Enable Api ok 2" << endl;
        }
    }
}

void PwrOn(unsigned dev)
{
    while(true)
    {
        int res = PowerOn(dev);
        if(res != ERROR_OK)
        {
            cout << "Power On failed" << endl;
        }
        else
        {
            cout << "Power On success" << endl;
        }
    }
}

void LdFile(unsigned dev)
{
    vector<string> list;
    while(true)
    {
        int enumFod = EnumerateFolder("/rpl/", list, dev);
        if(enumFod != ERROR_OK)
        {
            cout << "EnumerateFolder Failed" << endl;
        }
        else
        {
            cout << "EnumerateFolder ok" << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int enumFile = EnumerateFile("/rpl/", list, dev);
        if(enumFile != ERROR_OK)
        {
            cout << "EnumerateFolder Failed" << endl;
        }
        else
        {
            cout << "EnumerateFolder ok" << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int lFres = LoadFile("/rpl/TEST.XPL", "D:/RPL/MYTEST.XPL", dev);
        if(lFres != ERROR_OK)
        {
            cout << "LoadFile Failed" << endl;
        }
        else
        {
            cout << "LoadFile ok" << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int lFFres = LoadFromFile("TEST.XPL", dev);
        if(lFFres != ERROR_OK)
        {
            cout << "LoadFromFile Failed" << endl;
        }
        {
            cout << "LoadFromFile ok" << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int sFres = SaveFile("D:/override2.cfg", "/fa/override.cfg", dev);
        if(sFres != ERROR_OK)
        {
            cout << "SaveFile Failed" << endl;
        }
        else
        {
            cout << "SaveFile ok" << endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int dFres = DeleteControllerFile("/rpl/TEST.XPL", dev);
        if(dFres != ERROR_OK)
        {
            cout << "DeleteFile Failed" << endl;
        }
        else
        {
            cout << "DeleteFile ok" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetBaseCoor(RobotPos value, unsigned dev)
{
    while(true)
    {
        int res = GetBaseCoordinatePos(value, dev);
        if(res != ERROR_OK)
        {
            cout << "GetBaseCoordinate failed" << endl;
        }
        else
        {
            cout << "value.x = " << value.x << ", value.y = " << value.y << ", value.z = " << value.z << ", value.a = " << value.a
                 << ", value.b = " << value.b << ", value.c = " << value.c << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GetProStatus(RoboxProgramState proStatus, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentProStatus(proStatus, dev);
        if(res != ERROR_OK)
        {
            cout << "GetProStatus failed" << endl;
        }
        else
        {
            cout << "PeoStatus is " << proStatus << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void GetProStatus2(RoboxProgramState proStatus, unsigned dev)
{
    while(true)
    {
        int res = GetCurrentProStatus(proStatus, dev);
        if(res != ERROR_OK)
        {
            cout << "GetProStatus failed" << endl;
        }
        else
        {
            cout << "PeoStatus2 is " << proStatus << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void ReadIn(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDIn(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDIn failed" << endl;
        }
        else
        {
            cout << "DIn value[" << index << "]" << " = " << value << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void ReadIn2(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDIn(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDIn failed" << endl;
        }
        else
        {
            cout << "DIn2 value[" << index << "]" << " = " << value << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void ReadIn3(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDIn(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDIn failed" << endl;
        }
        else
        {
            cout << "DIn3 value[" << index << "]" << " = " << value << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void ReadOut(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDOut(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDout failed" << endl;
        }
        else
        {
            cout << "DOut value[" << value << "]" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void ReadOut2(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDOut(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDout failed" << endl;
        }
        else
        {
            cout << "DOut2 value[" << value << "]" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void ReadOut3(unsigned int index, bool value, unsigned int dev)
{
    while(true)
    {
        int res = ReadDOut(index, value, dev);
        if(res != ERROR_OK)
        {
            cout << "ReadDout failed" << endl;
        }
        else
        {
            cout << "DOut3 value[" << value << "]" << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void IsCon(unsigned dev)
{
    while(true)
    {
        int res = IsConnected(dev);
        cout << "robox isconnected: " << res << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void IsCon2(unsigned dev)
{
    while(true)
    {
        int res = IsConnected(dev);
        cout << "robox isconnected2: " << res << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void TestTwoController::IsMove(double pos[], int dev)
{
    RobotPos joint;
    bool moveFlag = true;
    while (true)
    {
        RobotAPI::GetMoveState(moveFlag, dev);
        if (!moveFlag)
        {
            time_t now = GetCurrentTimeStampMS();
            cout << "MOVE END time = " << now << endl;
            RobotAPI::GetBaseCoordinatePos(joint, dev);
            // cout << "joint.x = " << joint.x << "pos[0]" << pos[0] << endl;
            if(joint.x == pos[0])
            {
                time_t now = GetCurrentTimeStampMS();
                cout << "机器人就位时间" << now << endl;
                return;
            }
            // cout << "MOVE END" << endl;
        }
        QThread::sleep(1);
    }
}

void TestTwoController::main()
{
    unsigned dev1;
    // unsigned dev2;
    int res1;

    res1 = ConnectRobot("192.168.1.12", dev1);
    if (res1 != ERROR_OK)
    {
        cout<<"Connect Robot1 error:" << res1 << endl;
        return;
    }
    else
    {
        cout << "Connect Robot1 success" << endl;
    }

    int apires = EnableApiControl(true, dev1);
    cout << "apires = " << apires << endl;

    QThread::sleep(2);
}

time_t TestTwoController::GetCurrentTimeStampMS()
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
