#ifndef ROBOXCOMWIDGET_H
#define ROBOXCOMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>
#include <thread>
#include "EfortSdk.h"
using namespace RobotAPI;

namespace Ui {
class RoboxComWidget;
}

class RoboxComWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoboxComWidget(QWidget *parent = 0);
    ~RoboxComWidget();

private slots:
    void on_conBtn_clicked();
    void on_disconBtn_clicked();
    void onTimer();
    void OnEnableAPI(bool);
    void onPwronBtn();
    void on_pwrOnBtn_2_clicked();
    void on_startButton_clicked();
    void on_holdButton_clicked();
    void on_terminateButton_clicked();
    void on_restartButton_clicked();
    void buttonMovePressedSlot();
    void buttonMoveReleasedSlot();
    void on_saveFileFromController_clicked();
    void on_loadFileToController_clicked();
    void createFolderSlot();
    void lineEditCreateFolderSlot(const QString &);
    void on_MJointButton_clicked();
    void on_MLINButton_clicked();
    void on_EMLINButton_clicked();
    void on_MJOINT_clicked();
    void on_EMJOINT_clicked();
    void on_EMLIN_clicked();
    void on_jogButton_clicked();
    void on_loadFromFile_clicked();
    void on_btn_mm_add_clicked();
    void on_btn_mm_del_clicked();
    void on_btn_mm_clear_clicked();
    void on_btn_mm_start_clicked();
    void on_btn_mm_hold_clicked();
    void on_btn_mm_resume_clicked();
    void on_btn_mm_reset_clicked();
    void on_pushButtonDeleteFolder_clicked();
    void on_bmm_posAdd_btn_clicked();
    void on_bmm_postype_currentIndexChanged(int index);
    void on_bmm_posDel_btn_clicked();
    void on_bmm_posClr_btn_clicked();   
    void on_bmm_Start_btn_clicked();   
    void on_bmm_Stop_btn_clicked();
    void on_bmm_movetype_currentIndexChanged(int index);
    void on_IkSolver_btn_clicked();
    void on_fkSolver_btn_clicked();
    void on_mm2_postype_currentIndexChanged(int index);
    void on_mm2_movetype_currentIndexChanged(int index);
    void on_mm2_add_btn_clicked();
    void on_mm2_del_btn_clicked();
    void on_mm2_clear_btn_clicked();
    void on_mm2_start_btn_clicked();
    void on_mm2_hold_btn_clicked();
    void on_mm2_resume_btn_clicked();
    void on_mm2_reset_btn_clicked();  
    void on_auxjCount_currentIndexChanged(int index);
    void on_AuxjMove1_btn_clicked();
    void on_AuxjMove2_btn_clicked();
    void on_SetAuxj_btn_clicked();
    void on_setVoltage_btn_clicked();
    void on_setCurrent_btn_clicked();
    void on_setSpeed_btn_clicked();
    void on_curTool_combox_currentTextChanged(const QString &arg1);
    void on_curWobj_combox_currentTextChanged(const QString &arg1);
    // void on_interpCount_currentIndexChanged(int index);
    void on_getbool_btn_clicked();
    void on_getint_btn_clicked();
    void on_getdouble_btn_clicked();
    void on_getjoint_btn_clicked();
    void on_getpos_btn_clicked();
    void on_setBool_btn_clicked();
    void on_setInt_btn_clicked();
    void on_setDouble_btn_clicked();
    void on_setJoint_btn_clicked();
    void on_setPos_btn_clicked();
    void on_getpointj_btn_clicked();
    void on_getpointc_btn_clicked();
    void on_setPointJ_btn_clicked();
    void on_setPointC_btn_clicked();
    void on_readEPointJ_btn_clicked();
    void on_readEPointC_btn_clicked();
    void on_writeEpointj_btn_clicked();
    void on_writeEpointc_btn_clicked();
    void on_readDIn_btn_clicked();
    void on_readDOut_btn_clicked();
    void on_writeDOut_btn_clicked();
    void on_forceDIn_btn_clicked();
    void on_cancelFDIn_btn_clicked();
    void on_forceDOut_btn_clicked();
    void on_cancelFDOut_btn_clicked();
    void on_getTool_btn_clicked();
    void on_getWobj_btn_clicked();
    void on_ClrAlmBtn_clicked();

    void on_setTool_btn_clicked();

    void on_setWobj_btn_clicked();

    void on_gantry_currentIndexChanged(int index);

    void on_RMJointButton_clicked();

    void on_RMLINButton_clicked();

    void on_loadFromFile_2_clicked();

    void on_pushjointque_clicked();

    void on_pushcartque_clicked();

    void on_getPermButton_clicked();

    void on_freePermButton_clicked();

    void on_swModeButton_clicked();

    void on_holdMove_btn_clicked();

    void on_powerOnManual_clicked();

    void on_powerOffManual_clicked();

    void on_sendShell_btn_clicked();

private:
    Ui::RoboxComWidget *ui;
    void initUI();
    QTimer *testTimer;
    bool m_alarmStatus;
    bool m_conStatus;
    bool m_servoStatus;
    RoboxProgramState m_proStatus;
    RoboxStartMode m_stepStatus;
    RoboxKeyMode m_keyMode;
    RoboxJogMode m_jogMode;
    int m_speedRatio;
    string m_currentToolName;
    string m_currentWobjName;
    bool m_currentMoveState;
    int m_currentMultiMoveState;

    string m_folderPath;
    RobotJoint m_jointPos;
    RobotPos m_baseCPos;
    RobotPos m_refCPos;
    QList<QLabel*> qj;
    QList<QPushButton*> moveButtonList;
    unsigned int m_deviceId;
    
    QList<QLineEdit*> bmmPos1List;
    RobotJoint bmm_ap[4];
    RobotPos bmm_cp[4];

    QList<QLineEdit*> mm2Pos1List;
    RobotJoint mm2_ap[4];
    RobotPos mm2_cp[4];
    int m_currentMultiMove2State;

    double m_voltageValue;
    double m_currentValue;

    std::thread *bmmStop_thread;

    void monitorRobotType();
    void setAlarmStatus(bool value);
    void setAlarmList(std::vector<unsigned int>& list);
    void setConnectStatus(bool value);
    void setServoStatus(bool value);
    void setProStatus(RoboxProgramState value);
    void setStepStatus(RoboxStartMode value);
    void setKeyMode(RoboxKeyMode value);
    void setJogMode(RoboxJogMode value);
    void setSpeedRatio(int value);
    void setTool(string value);
    void setWobj(string value);
    void setMoveState(bool value);
    void setMultiMoveState(int value);
    void initPos();
    void updatePosJoint(RobotJoint value);
    void updatePosBaseC(RobotPos value);
    void updatePosRefC(RobotPos value);
    void setMultiMove2State(int value);
    void setWeldingVoltage(double value);
    void setWeldingCurrent(double value);
    void setAuxjMoveState(bool value);

    RobotPos value;
};

#endif // ROBOXCOMWIDGET_H
