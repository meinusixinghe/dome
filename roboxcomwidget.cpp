#include "roboxcomwidget.h"
#include "ui_roboxcomwidget.h"
#include <QMessageBox>
#include <QDebug>
#include <iostream>

RoboxComWidget::RoboxComWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoboxComWidget)
{
    m_alarmStatus = false;
    m_conStatus = false;
    m_servoStatus = false;
    m_proStatus = RPL_INIT;
    m_stepStatus = ROBOX_MODE_CONTINUOUS;
    m_keyMode = ROBOX_MODE_MANUAL;
    m_jogMode = ROBOX_MODE_JOG;
    m_speedRatio = 0;
    m_currentToolName = "tool0";
    m_currentWobjName = "wobj0";
    m_currentMoveState = false;
    m_folderPath = "/test";
    bmmStop_thread = nullptr;
    ui->setupUi(this);
    initPos();
    ui->stackedWidget->setCurrentIndex(0);
    testTimer = new QTimer(this);
    connect(testTimer, SIGNAL(timeout()),this, SLOT(onTimer()));
    initUI();
    connect(ui->cbxEnableAPI, SIGNAL(toggled(bool)), this, SLOT(OnEnableAPI(bool)));
    connect(ui->pwrOnBtn, SIGNAL(clicked()), this, SLOT(onPwronBtn()));

    moveButtonList<<ui->jp1<<ui->jp2<<ui->jp3<<ui->jp4<<ui->jp5<<ui->jp6<<ui->jm1<<ui->jm2<<ui->jm3<<ui->jm4<<ui->jm5<<ui->jm6<<ui->speedPlus_btn<<ui->speedMinus_btn;
    for(int i = 0; i < moveButtonList.count(); i++)
    {
        connect(moveButtonList[i], SIGNAL(pressed()), this, SLOT(buttonMovePressedSlot()));
        connect(moveButtonList[i], SIGNAL(released()), this, SLOT(buttonMoveReleasedSlot()));
    }
    connect(ui->pushButtonCreateFolder, SIGNAL(clicked()), this, SLOT(createFolderSlot()));
    connect(ui->lineEditCreateFolder, SIGNAL(textChanged(QString)), this, SLOT(lineEditCreateFolderSlot(QString)));

    // BlockMultiMove ui Initialization
    ui->tabWidget_2->setTabEnabled(1,false);
    ui->tabWidget_2->setTabEnabled(2,false);
    ui->tabWidget_2->setTabEnabled(3,false);
    bmmPos1List<<ui->bmm_P1J1<<ui->bmm_P1J2<<ui->bmm_P1J3<<ui->bmm_P1J4<<ui->bmm_P1J5<<ui->bmm_P1J6
              <<ui->bmm_P1ej1<<ui->bmm_P1ej2<<ui->bmm_P1ej3<<ui->bmm_P1ej4<<ui->bmm_P1ej5<<ui->bmm_P1ej6
             <<ui->bmm_P1Cfgx<<ui->bmm_P1Cfg1<<ui->bmm_P1Cfg4<<ui->bmm_P1Cfg6;

    // MultiMove2 ui Initialization
    ui->tabWidget_3->setTabEnabled(1,false);
    ui->tabWidget_3->setTabEnabled(2,false);
    ui->tabWidget_3->setTabEnabled(3,false);
    mm2Pos1List<<ui->mm2_P1J1<<ui->mm2_P1J2<<ui->mm2_P1J3<<ui->mm2_P1J4<<ui->mm2_P1J5<<ui->mm2_P1J6
              <<ui->mm2_P1ej1<<ui->mm2_P1ej2<<ui->mm2_P1ej3<<ui->mm2_P1ej4<<ui->mm2_P1ej5<<ui->mm2_P1ej6
             <<ui->mm2_P1Cfgx<<ui->mm2_P1Cfg1<<ui->mm2_P1Cfg4<<ui->mm2_P1Cfg6;
}

RoboxComWidget::~RoboxComWidget()
{
    if (m_deviceId)
    {
        DisconnectRobot(m_deviceId);
    }
    delete ui;
}

void RoboxComWidget::initUI()
{
    ui->ipEdt->setText("192.168.1.12");

    ui->mm_type->addItem("joint");
    ui->mm_type->addItem("cart");
    ui->mm_type->addItem("circle");
}

void RoboxComWidget::monitorRobotType()
{
    std::string robotType;
    if (GetCurrentRobotType(robotType, m_deviceId) == ERROR_OK)
    {
        ui->robotTypeLabel->setText(QString::fromStdString(robotType));
    }
    else
    {
        ui->robotTypeLabel->setText("");
    }
}

void RoboxComWidget::setAuxjMoveState(bool value)
{
    if(value)
    {
        ui->auxjMoveState_label->setText("false");
    }
    else
    {
        ui->auxjMoveState_label->setText("true");
    }
}

void RoboxComWidget::setAlarmStatus(bool value)
{
    if(value == m_alarmStatus)
        return;
    m_alarmStatus = value;

    if(value)
    {
        ui->alarmStatusLabel->setText(QString::fromLocal8Bit("有报警"));
    }
    else
    {
        ui->alarmStatusLabel->setText(QString::fromLocal8Bit("无报警"));
    }
}

void RoboxComWidget::setAlarmList(std::vector<unsigned int>& list)
{
    ui->alarmListWidget->clear();
    for(int i=0;i<list.size();i++)
    {
        int val=(int)list[i];
        QString str=QString("%1").arg(val);
        ui->alarmListWidget->addItem(str);
    }
}

void RoboxComWidget::setConnectStatus(bool value)
{
    if(value == m_conStatus)
        return;
    m_conStatus = value;

    if(value)
    {
        ui->conLabel->setText(QString::fromLocal8Bit("已连接"));
    }
    else
    {
        ui->conLabel->setText(QString::fromLocal8Bit("未连接"));
    }
}

void RoboxComWidget::setServoStatus(bool value)
{
    if(value == m_servoStatus)
        return;
    m_servoStatus = value;

    if(value)
    {
        ui->pwrLabel->setText(QString::fromLocal8Bit("已上电"));
    }
    else
    {
        ui->pwrLabel->setText(QString::fromLocal8Bit("未上电"));
    }
}

void RoboxComWidget::setProStatus(RoboxProgramState value)
{
    if(value == m_proStatus)
        return;
    m_proStatus = value;
    switch (value)
    {
        case RPL_INIT:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("未加载"));
            break;
        case RPL_RUN:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("运行中"));
            break;
        case RPL_PAUSE:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("暂停中"));
            break;
        case RPL_STOPPED:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("已终止"));
            break;
        case RPL_ENDED:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("已结束"));
            break;
        case RPL_ERROR:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("程序报错"));
            break;
        default:
            ui->proStatusLabel->setText(QString::fromLocal8Bit("未加载"));
            break;
    }
}

void RoboxComWidget::setStepStatus(RoboxStartMode value)
{
    if(value == m_stepStatus)
        return;
    m_stepStatus = value;
    switch (value)
    {
    case ROBOX_MODE_CONTINUOUS:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("连续"));
        break;
    case ROBOX_MODE_STEPIN:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("单步进入"));
        break;
    case ROBOX_MODE_STEPOVER:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("单步跳过"));
        break;
    case ROBOX_MODE_M_STEPIN:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("运动进入"));
        break;
    case ROBOX_MODE_M_STEPOVER:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("运动跳过"));
        break;
    case ROBOX_MODE_ANTIDROMIC:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("回退"));
        break;
    default:
        ui->stepModeLabel->setText(QString::fromLocal8Bit("手动慢速"));
        break;
    }
}

void RoboxComWidget::setKeyMode(RoboxKeyMode value)
{
    if(value == m_keyMode)
        return;
    m_keyMode = value;
    switch (value)
    {
    case ROBOX_MODE_MANUAL:
        ui->keyModeLabel->setText(QString::fromLocal8Bit("手动低速"));
        break;
    case ROBOX_MODE_MANUFAST:
        ui->keyModeLabel->setText(QString::fromLocal8Bit("手动高速"));
        break;
    case ROBOX_MODE_AUTO:
        ui->keyModeLabel->setText(QString::fromLocal8Bit("自动模式"));
        break;
    default:
        ui->keyModeLabel->setText(QString::fromLocal8Bit("手动低速"));
        break;
    }
}

void RoboxComWidget::setJogMode(RoboxJogMode value)
{
    if(value == m_jogMode)
        return;
    m_jogMode = value;

    switch (value)
    {
    case ROBOX_MODE_JOG:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("关节坐标"));
        break;
    case ROBOX_MODE_ROBOT:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("机器人坐标"));
        break;
    case ROBOX_MODE_TOOL:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("工具坐标"));
        break;
    case ROBOX_MODE_UFRAME:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("用户坐标"));
        break;
    case ROBOX_MODE_AUX:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("附加轴"));
        break;
    default:
        ui->jogModeLabel->setText(QString::fromLocal8Bit("关节坐标"));
        break;
    }
}

void RoboxComWidget::setSpeedRatio(int value)
{
    if(value == m_speedRatio)
        return;
    m_speedRatio = value;

    ui->speedLabel->setText(QString::number(value)+"%");
}

void RoboxComWidget::setTool(string value)
{
    if(value == m_currentToolName)
        return;
    m_currentToolName = value;

    ui->currentToolLabel->setText(QString::fromStdString(value));
}

void RoboxComWidget::setWobj(string value)
{
    if(value == m_currentWobjName)
        return;
    m_currentWobjName = value;

    ui->currentWobjLabel->setText(QString::fromStdString(value));
}

void RoboxComWidget::setMoveState(bool value)
{
    if(value == m_currentMoveState)
        return;
    m_currentMoveState = value;

    QString str=value?QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否");
    ui->currentMoveState->setText(str);
}

void RoboxComWidget::setMultiMoveState(int value)
{
    if(value == m_currentMultiMoveState)
        return;
    m_currentMultiMoveState = value;

    QString str="";
    switch (value) {
    case MULTI_MOVE_INIT:
        str="MULTI_MOVE_INIT";
        break;
    case MULTI_MOVE_LANUCH:
        str="MULTI_MOVE_LANUCH";
        break;
    case MULTI_MOVE_EXEC:
        str="MULTI_MOVE_EXEC";
        break;
    case MULTI_MOVE_HOLD_DEC:
        str="MULTI_MOVE_HOLD_DEC";
        break;
    case MULTI_MOVE_HOLD:
        str="MULTI_MOVE_HOLD";
        break;
    case MULTI_MOVE_ERROR:
        str="MULTI_MOVE_ERROR";
        break;
    case MULTI_MOVE_ENDED:
        str="MULTI_MOVE_ENDED";
        break;
    default:
        break;
    }
    ui->mm_state->setText(str);
}

void RoboxComWidget::initPos()
{
    qj<< ui->j1label << ui->j2label << ui->j3label << ui->j4label << ui->j5label << ui->j6label
      << ui->ejlabel_1 << ui->ejlabel_2 << ui->ejlabel_3 << ui->ejlabel_4;

    for(int i = 0; i < 12; i++)
    {
        m_jointPos.j[i] = 0.00;
    }
    m_baseCPos.x = 0.00;
    m_baseCPos.y = 0.00;
    m_baseCPos.z = 0.00;
    m_baseCPos.a = 0.00;
    m_baseCPos.b = 0.00;
    m_baseCPos.c = 0.00;
    m_baseCPos.cfgx = 0;
    m_baseCPos.cfg1 = 0;
    m_baseCPos.cfg4 = 0;
    m_baseCPos.cfg6 = 0;
    m_baseCPos.ej1 = 0.00;
    m_baseCPos.ej2 = 0.00;
    m_baseCPos.ej3 = 0.00;
    m_baseCPos.ej4 = 0.00;
    m_baseCPos.ej5 = 0.00;
    m_baseCPos.ej6 = 0.00;

    m_refCPos.x = 0.00;
    m_refCPos.y = 0.00;
    m_refCPos.z = 0.00;
    m_refCPos.a = 0.00;
    m_refCPos.b = 0.00;
    m_refCPos.c = 0.00;
    m_baseCPos.cfgx = 0;
    m_baseCPos.cfg1 = 0;
    m_baseCPos.cfg4 = 0;
    m_baseCPos.cfg6 = 0;
    m_refCPos.ej1 = 0.00;
    m_refCPos.ej2 = 0.00;
    m_refCPos.ej3 = 0.00;
    m_refCPos.ej4 = 0.00;
    m_refCPos.ej5 = 0.00;
    m_refCPos.ej6 = 0.00;
}

void RoboxComWidget::updatePosJoint(RobotJoint value)
{
    for (int i = 0; i < 12; i++)
    {
        if (m_jointPos.j[i] != value.j[i])
        {
            m_jointPos.j[i] = value.j[i];
            qj[i]->setText(QString::number(m_jointPos.j[i], 'f', 2));
        }
    }
}

void RoboxComWidget::updatePosBaseC(RobotPos value)
{
    if(m_baseCPos.x != value.x)
    {
        m_baseCPos.x = value.x;
        ui->baseX->setText(QString::number(m_baseCPos.x, 'f', 2));
    }

    if(m_baseCPos.y != value.y)
    {
        m_baseCPos.y = value.y;
        ui->baseY->setText(QString::number(m_baseCPos.y, 'f', 2));
    }

    if(m_baseCPos.z != value.z)
    {
        m_baseCPos.z = value.z;
        ui->baseZ->setText(QString::number(m_baseCPos.z, 'f', 2));
    }

    if(m_baseCPos.a != value.a)
    {
        m_baseCPos.a = value.a;
        ui->baseA->setText(QString::number(m_baseCPos.a, 'f', 2));
    }

    if(m_baseCPos.b != value.b)
    {
        m_baseCPos.b = value.b;
        ui->baseB->setText(QString::number(m_baseCPos.b, 'f', 2));
    }

    if(m_baseCPos.c != value.c)
    {
        m_baseCPos.c = value.c;
        ui->baseC->setText(QString::number(m_baseCPos.c, 'f', 2));
    }
}

void RoboxComWidget::updatePosRefC(RobotPos value)
{
    if(m_refCPos.x != value.x)
    {
        m_refCPos.x = value.x;
        ui->userX->setText(QString::number(m_refCPos.x, 'f', 2));
    }

    if(m_refCPos.y != value.y)
    {
        m_refCPos.y = value.y;
        ui->userY->setText(QString::number(m_refCPos.y, 'f', 2));
    }

    if(m_refCPos.z != value.z)
    {
        m_refCPos.z = value.z;
        ui->userZ->setText(QString::number(m_refCPos.z, 'f', 2));
    }

    if(m_refCPos.a != value.a)
    {
        m_refCPos.a = value.a;
        ui->userA->setText(QString::number(m_refCPos.a, 'f', 2));
    }

    if(m_refCPos.b != value.b)
    {
        m_refCPos.b = value.b;
        ui->userB->setText(QString::number(m_refCPos.b, 'f', 2));
    }

    if(m_refCPos.c != value.c)
    {
        m_refCPos.c = value.c;
        ui->userC->setText(QString::number(m_refCPos.c, 'f', 2));
    }
}

void RoboxComWidget::setMultiMove2State(int value)
{
    if(value == m_currentMultiMove2State) return;
    m_currentMultiMove2State = value;

    QString str="";
    switch (value) {
    case MULTI_MOVE2_INIT:
        str="MULTI_MOVE2_INIT";
        break;
    case MULTI_MOVE2_LANUCH:
        str="MULTI_MOVE2_LANUCH";
        break;
    case MULTI_MOVE2_EXEC:
        str="MULTI_MOVE2_EXEC";
        break;
    case MULTI_MOVE2_HOLD:
        str="MULTI_MOVE2_HOLD";
        break;
    case MULTI_MOVE2_ERROR:
        str="MULTI_MOVE2_ERROR";
        break;
    case MULTI_MOVE2_ENDED:
        str="MULTI_MOVE2_ENDED";
        break;
    default:
        break;
    }
    ui->mm2_movestate_label->setText(str);
}

void RoboxComWidget::setWeldingVoltage(double value)
{
    if(value == m_voltageValue)
    {
        return;
    }
    m_voltageValue = value;
    ui->voltage_label->setText(QString::number(value));
    return;
}

void RoboxComWidget::setWeldingCurrent(double value)
{
    if(value == m_currentValue)
    {
        return;
    }
    m_voltageValue = value;
    ui->voltage_label->setText(QString::number(value));
    return;
}

void RoboxComWidget::on_conBtn_clicked()
{
    QString strIP = ui->ipEdt->text();
    if (strIP.isEmpty())
    {
        QMessageBox::information(this, tr("warning"), tr("IP address can not empty!"));
        return;
    }

    int res = ConnectRobot(strIP.toStdString().c_str(), m_deviceId, true);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
        return;
    }

    testTimer->start(200);
}

void RoboxComWidget::on_disconBtn_clicked()
{
    if (!IsConnected(m_deviceId))
    {
        QMessageBox::information(this, tr("warning"), tr("Operation failed!"));
        return;
    }
    DisconnectRobot(m_deviceId);
    testTimer->stop();
    setConnectStatus(IsConnected(m_deviceId));
}

void RoboxComWidget::onTimer()
{
    bool isConnected = IsConnected(m_deviceId);
    setConnectStatus(isConnected);
    if (!isConnected)
    {
        return;
    }

    monitorRobotType();
    bool alarmStatus = false;
    GetCurrentAlarmStatus(alarmStatus, m_deviceId);
    setAlarmStatus(alarmStatus);

    bool auxjState;
    int auxIndex = ui->auxMoveStop_combox->currentIndex()+1;
    AuxjStoped(auxIndex, auxjState, m_deviceId);
    setAuxjMoveState(auxjState);

    std::vector<unsigned int> alarmList;
    GetAlarmList(alarmList, m_deviceId);
    setAlarmList(alarmList);

    bool servoStatus = false;
    GetCurrentServoStatus(servoStatus, m_deviceId);
    setServoStatus(servoStatus);

    RoboxProgramState proStatus = RoboxProgramState::RPL_INIT;
    GetCurrentProStatus(proStatus, m_deviceId);
    setProStatus(proStatus);

    RoboxStartMode stepMode = RoboxStartMode::ROBOX_MODE_CONTINUOUS;
    GetCurrentStepMode(stepMode, m_deviceId);
    setStepStatus(stepMode);

    RoboxKeyMode keyMode = RoboxKeyMode::ROBOX_MODE_MANUAL;
    GetCurrentKeyMode(keyMode, m_deviceId);
    setKeyMode(keyMode);

    RoboxJogMode jogMode = RoboxJogMode::ROBOX_MODE_JOG;
    GetCurrentJogMode(jogMode, m_deviceId);
    setJogMode(jogMode);

    unsigned int speed;
    GetCurrentSpeedRatio(speed, m_deviceId);
    setSpeedRatio(speed);

    string toolName;
    GetCurrentToolName(toolName, m_deviceId);
    setTool(toolName);

    string wobjName;
    GetCurrentUframeName(wobjName, m_deviceId);
    setWobj(wobjName);


    bool moveState=false;
    if(GetMoveState(moveState, m_deviceId) == ERROR_OK)
    {
        setMoveState(moveState);
    }

    int moveState2=0;
    if(GetMultiMoveState(moveState2, m_deviceId) == ERROR_OK)
    {
        setMultiMoveState(moveState2);
    }

    RobotJoint jointPos;
    if (GetJointPos(jointPos, m_deviceId) == ERROR_OK)
    {
        updatePosJoint(jointPos);
    }

    RobotPos basePos;
    if (GetBaseCoordinatePos(basePos, m_deviceId) == ERROR_OK)
    {
        updatePosBaseC(basePos);
    }

    RobotPos refPos;
    if (GetUserCoordinatePos(refPos, m_deviceId) == ERROR_OK)
    {
        updatePosRefC(refPos);
    }

    int move2State2 = 0;
    if(GetMultiMove2State(move2State2, m_deviceId) == ERROR_OK)
    {
        setMultiMove2State(move2State2);
    }

    /*
    double voltageValue = 0;
    if(ArcWeldingRxVoltage(voltageValue, m_deviceId) == ERROR_OK)
    {
        setWeldingVoltage(voltageValue);
    }

    double currentValue = 0;
    if(ArcWeldingRxCurrent(currentValue, m_deviceId) == ERROR_OK)
    {
        setWeldingCurrent(currentValue);
    }
    */
}

void RoboxComWidget::OnEnableAPI(bool bEnable)
{
    int res = EnableApiControl(bEnable, m_deviceId);
    cout << "res = " << res << endl;
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::onPwronBtn()
{
    int res = PowerOn(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_startButton_clicked()
{
    int res = StartProgram(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_holdButton_clicked()
{
    int res = StopProgram(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_terminateButton_clicked()
{
    int res = TerminateProgram(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_restartButton_clicked()
{
    int res = RestartProgram(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::buttonMovePressedSlot()
{
    QPushButton* button = (QPushButton*)sender();
    if(button->objectName() == "jp1")
    {
        Jog1Plus(true, m_deviceId);
    }
    if(button->objectName() == "jp2")
    {
        Jog2Plus(true, m_deviceId);
    }
    if(button->objectName() == "jp3")
    {
        Jog3Plus(true, m_deviceId);
    }
    if(button->objectName() == "jp4")
    {
        Jog4Plus(true, m_deviceId);
    }
    if(button->objectName() == "jp5")
    {
        Jog5Plus(true, m_deviceId);
    }
    if(button->objectName() == "jp6")
    {
        Jog6Plus(true, m_deviceId);
    }
    if(button->objectName() == "jm1")
    {
        Jog1Minus(true, m_deviceId);
    }
    if(button->objectName() == "jm2")
    {
        Jog2Minus(true, m_deviceId);
    }
    if(button->objectName() == "jm3")
    {
        Jog3Minus(true, m_deviceId);
    }
    if(button->objectName() == "jm4")
    {
        Jog4Minus(true, m_deviceId);
    }
    if(button->objectName() == "jm5")
    {
        Jog5Minus(true, m_deviceId);
    }
    if(button->objectName() == "jm6")
    {
        Jog6Minus(true, m_deviceId);
    }
    if(button->objectName() == "speedPlus_btn")
    {
        IncreaseVelocity(true, m_deviceId);
    }
    if(button->objectName() == "speedMinus_btn")
    {
        DecreaseVelocity(true, m_deviceId);
    }
}

void RoboxComWidget::buttonMoveReleasedSlot()
{
    QPushButton* button = (QPushButton*)sender();
    if(button->objectName() == "jp1")
    {
        Jog1Plus(false, m_deviceId);
    }
    if(button->objectName() == "jp2")
    {
        Jog2Plus(false, m_deviceId);
    }
    if(button->objectName() == "jp3")
    {
        Jog3Plus(false, m_deviceId);
    }
    if(button->objectName() == "jp4")
    {
        Jog4Plus(false, m_deviceId);
    }
    if(button->objectName() == "jp5")
    {
        Jog5Plus(false, m_deviceId);
    }
    if(button->objectName() == "jp6")
    {
        Jog6Plus(false, m_deviceId);
    }
    if(button->objectName() == "jm1")
    {
        Jog1Minus(false, m_deviceId);
    }
    if(button->objectName() == "jm2")
    {
        Jog2Minus(false, m_deviceId);
    }
    if(button->objectName() == "jm3")
    {
        Jog3Minus(false, m_deviceId);
    }
    if(button->objectName() == "jm4")
    {
        Jog4Minus(false, m_deviceId);
    }
    if(button->objectName() == "jm5")
    {
        Jog5Minus(false, m_deviceId);
    }
    if(button->objectName() == "jm6")
    {
        Jog6Minus(false, m_deviceId);
    }
    if(button->objectName() == "speedPlus_btn")
    {
        IncreaseVelocity(false, m_deviceId);
    }
    if(button->objectName() == "speedMinus_btn")
    {
        DecreaseVelocity(false, m_deviceId);
    }
}

void RoboxComWidget::on_saveFileFromController_clicked()
{
    if(ui->saveSourceFile->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "saveSourceFile path empty!");
        return;
    }
    if(ui->saveTargetFile->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "saveTargetFile path empty!");
        return;
    }
    string source = ui->saveSourceFile->text().toStdString();
    string target = ui->saveTargetFile->text().toStdString();
    if(SaveFile(target, source, m_deviceId) != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_loadFileToController_clicked()
{
    if(ui->loadSourceFile->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "loadSourceFile path empty!");
        return;
    }
    if(ui->loadTargetFileName->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "loadTargetFileName empty!");
        return;
    }

    string source = ui->loadSourceFile->text().toStdString();
    string targetName = ui->loadTargetFileName->text().toStdString();
    if(LoadFile(targetName, source, m_deviceId) != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_pwrOnBtn_2_clicked()
{
    int res = PowerOff(m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_MJointButton_clicked()
{
    double pos[6];
    pos[0] = ui->mjLineEdit_1->text().toDouble();
    pos[1] = ui->mjLineEdit_2->text().toDouble();
    pos[2] = ui->mjLineEdit_3->text().toDouble();
    pos[3] = ui->mjLineEdit_4->text().toDouble();
    pos[4] = ui->mjLineEdit_5->text().toDouble();
    pos[5] = ui->mjLineEdit_6->text().toDouble();
    int speed = ui->mjspeed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    int res = MJOINT(pos, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_EMLINButton_clicked()
{
    double pos[6];
    pos[0] = ui->eml_x->text().toDouble();
    pos[1] = ui->eml_y->text().toDouble();
    pos[2] = ui->eml_z->text().toDouble();
    pos[3] = ui->eml_a->text().toDouble();
    pos[4] = ui->eml_b->text().toDouble();
    pos[5] = ui->eml_c->text().toDouble();

    double apos[4];
    apos[0] = ui->eml_aux1->text().toDouble();
    apos[1] = ui->eml_aux2->text().toDouble();
    apos[2] = ui->eml_aux3->text().toDouble();
    apos[3] = ui->eml_aux4->text().toDouble();


    int speed = ui->eml_speed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    int res = EMLIN(pos, apos, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_MLINButton_clicked()
{
    double pos[6];
    pos[0] = ui->ml_x->text().toDouble();
    pos[1] = ui->ml_y->text().toDouble();
    pos[2] = ui->ml_z->text().toDouble();
    pos[3] = ui->ml_a->text().toDouble();
    pos[4] = ui->ml_b->text().toDouble();
    pos[5] = ui->ml_c->text().toDouble();
    int speed = ui->mlspeed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    int res = MLIN(pos, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_MJOINT_clicked()
{
    double pos[6];
    pos[0] = ui->mjoint_j1->text().toDouble();
    pos[1] = ui->mjoint_j2->text().toDouble();
    pos[2] = ui->mjoint_j3->text().toDouble();
    pos[3] = ui->mjoint_j4->text().toDouble();
    pos[4] = ui->mjoint_j5->text().toDouble();
    pos[5] = ui->mjoint_j6->text().toDouble();
    int speed = ui->mjoint_speed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    string tool = ui->mjoint_tool->text().toStdString();
    string wobj = ui->mjoint_wobj->text().toStdString();
    int res = MJOINT(pos, tool, wobj, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_EMJOINT_clicked()
{
    double pos[6];
    pos[0] = ui->emjoint_j1->text().toDouble();
    pos[1] = ui->emjoint_j2->text().toDouble();
    pos[2] = ui->emjoint_j3->text().toDouble();
    pos[3] = ui->emjoint_j4->text().toDouble();
    pos[4] = ui->emjoint_j5->text().toDouble();
    pos[5] = ui->emjoint_j6->text().toDouble();

    double apos[4];
    apos[0] = ui->emjoint_aux1->text().toDouble();
    apos[1] = ui->emjoint_aux2->text().toDouble();
    apos[2] = ui->emjoint_aux3->text().toDouble();
    apos[3] = ui->emjoint_aux4->text().toDouble();

    int speed = ui->emjoint_speed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    string tool = ui->emjoint_tool->text().toStdString();
    string wobj = ui->emjoint_wobj->text().toStdString();
    int res = EMJOINT(pos, apos, tool, wobj, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_EMLIN_clicked()
{
    double pos[6];
    pos[0] = ui->emlin_x->text().toDouble();
    pos[1] = ui->emlin_y->text().toDouble();
    pos[2] = ui->emlin_z->text().toDouble();
    pos[3] = ui->emlin_a->text().toDouble();
    pos[4] = ui->emlin_b->text().toDouble();
    pos[5] = ui->emlin_c->text().toDouble();

    double apos[4];
    apos[0] = ui->emlin_aux1->text().toDouble();
    apos[1] = ui->emlin_aux2->text().toDouble();
    apos[2] = ui->emlin_aux3->text().toDouble();
    apos[3] = ui->emlin_aux4->text().toDouble();

    int speed = ui->emlin_speed->text().toInt();
    if(speed > 100)
    {
        QMessageBox::information(this, "warning", "speed failed!");
    }
    string tool = ui->emlin_tool->text().toStdString();
    string wobj = ui->emlin_wobj->text().toStdString();
    int res = EMLIN(pos, apos, tool, wobj, speed, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_jogButton_clicked()
{
    RoboxJogMode jogMode = RoboxJogMode::ROBOX_MODE_JOG;
    int res1 = GetCurrentJogMode(jogMode, m_deviceId);
    if (res1 != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
    RoboxJogMode tempJogMode;
    switch (jogMode) {
    case ROBOX_MODE_JOG:
        tempJogMode = ROBOX_MODE_ROBOT;
        break;
    case ROBOX_MODE_ROBOT:
        tempJogMode = ROBOX_MODE_TOOL;
        break;
    case ROBOX_MODE_TOOL:
        tempJogMode = ROBOX_MODE_UFRAME;
        break;
    case ROBOX_MODE_UFRAME:
        tempJogMode = ROBOX_MODE_AUX;
        break;
    case ROBOX_MODE_AUX:
        tempJogMode = ROBOX_MODE_JOG;
        break;
    default:
        tempJogMode = ROBOX_MODE_JOG;
        break;
    }

    int res = RobotAPI::SetJogMode(tempJogMode, m_deviceId);
    if (res != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_loadFromFile_clicked()
{
    if(ui->loadProgramFileName->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "fileName empty!");
        return;
    }

    string targetName = ui->loadProgramFileName->text().toStdString();
    if(LoadFromFile(targetName, m_deviceId) != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Operation failed!");
    }
}

void RoboxComWidget::on_btn_mm_add_clicked()
{
    if(ui->mm_list->count()>=MAX_MULTI_MOVE)
    {
        QMessageBox::information(this, "warning", "Operation failed! Max quantity is 20!");
        return;
    }

    QString type=ui->mm_type->currentText();
    double pos[12];
    pos[0]=ui->mm_j1->text().toDouble();
    pos[1]=ui->mm_j2->text().toDouble();
    pos[2]=ui->mm_j3->text().toDouble();
    pos[3]=ui->mm_j4->text().toDouble();
    pos[4]=ui->mm_j5->text().toDouble();
    pos[5]=ui->mm_j6->text().toDouble();
    pos[6]=ui->mm_ej1->text().toDouble();
    pos[7]=ui->mm_ej2->text().toDouble();
    pos[8]=ui->mm_ej3->text().toDouble();
    pos[9]=ui->mm_ej4->text().toDouble();
    pos[10]=ui->mm_ej5->text().toDouble();
    pos[11]=ui->mm_ej6->text().toDouble();
    int cfg=ui->mm_cfg->text().toInt();
    double speed=ui->mm_speed->text().toDouble();
    double overlapping=ui->mm_overlapping->text().toDouble();
    QString str=QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16").arg(type)
            .arg(pos[0]).arg(pos[1]).arg(pos[2]).arg(pos[3]).arg(pos[4]).arg(pos[5])
            .arg(pos[6]).arg(pos[7]).arg(pos[8]).arg(pos[9]).arg(pos[10]).arg(pos[11])
            .arg(cfg).arg(speed).arg(overlapping);
    ui->mm_list->addItem(str);
}
void RoboxComWidget::on_btn_mm_del_clicked()
{
    QListWidgetItem* cur=ui->mm_list->currentItem();
    if(cur==NULL)
    {
        QMessageBox::information(this, "warning", "Operation failed! Please select!");
        return;
    }
    delete cur;
}
void RoboxComWidget::on_btn_mm_clear_clicked()
{
    ui->mm_list->clear();
}
void RoboxComWidget::on_btn_mm_start_clicked()
{
    vector<MultiMovePos> list;
    int length=ui->mm_list->count();
    for(int i=0;i<length;i++)
    {
        QString text=ui->mm_list->item(i)->text();
        QStringList lst=text.split(",");
        if(lst.count()==16)
        {

            MultiMovePos pos;
            if(lst[0]==QString("joint"))
                pos.type=MULTI_MOVE_MJOINT;
            else if(lst[0]==QString("cart"))
                pos.type=MULTI_MOVE_MLIN;
            else if(lst[0]==QString("circle"))
                pos.type=MULTI_MOVE_MCIRC;
            else
                continue;
            pos.pos[0]=lst[1].toDouble();
            pos.pos[1]=lst[2].toDouble();
            pos.pos[2]=lst[3].toDouble();
            pos.pos[3]=lst[4].toDouble();
            pos.pos[4]=lst[5].toDouble();
            pos.pos[5]=lst[6].toDouble();
            pos.pos[6]=lst[7].toDouble();
            pos.pos[7]=lst[8].toDouble();
            pos.pos[8]=lst[9].toDouble();
            pos.pos[9]=lst[10].toDouble();
            pos.pos[10]=lst[11].toDouble();
            pos.pos[11]=lst[12].toDouble();
            pos.cfg=lst[13].toInt();
            pos.speed=lst[14].toDouble();
            pos.overlapping=lst[15].toDouble();
            list.push_back(pos);
        }
    }
    std::cout << "MultiMoveStart, list = " << list.size() << std::endl;
    int ret=MultiMoveStart(list, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "start Operation failed!");
    }
}
void RoboxComWidget::on_btn_mm_hold_clicked()
{
    int ret=MultiMoveHold(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "hold Operation failed!");
    }
}
void RoboxComWidget::on_btn_mm_resume_clicked()
{
    int ret=MultiMoveResume(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "resume Operation failed!");
    }
}
void RoboxComWidget::on_btn_mm_reset_clicked()
{
    int ret=MultiMoveReset(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "reset Operation failed!");
    }
}

void RoboxComWidget::createFolderSlot()
{
   int ret = CreateFolder(m_folderPath, m_deviceId);
   if(ret != ERROR_OK)
   {
       QMessageBox::information(this, "error", "Create folder error!");
   }
}

void RoboxComWidget::lineEditCreateFolderSlot(const QString &)
{
     m_folderPath =  ui->lineEditCreateFolder->text().toStdString();
}

void RoboxComWidget::on_pushButtonDeleteFolder_clicked()
{
    qDebug() << QString::fromStdString(m_folderPath);
    int ret = DeleteFolder(m_folderPath, m_deviceId);
    if (ret != ERROR_OK)
    {
        QMessageBox::information(this, "error", "Delete folder error");
    }
}

void RoboxComWidget::on_bmm_posAdd_btn_clicked()
{
    /*if(ui->bmm_posList->count() >= MAX_MULTI_MOVE)
    {
        QMessageBox::information(this, "warning", "Operation failed! Max quantity is 20!");
        return;
    }*/
    QString moveType = ui->bmm_movetype->currentText();
    QString posType = ui->bmm_postype->currentText();
    int flag = ui->bmm_flags->currentIndex();
    double speed = ui->bmm_speed->text().toDouble();
    double acc = ui->bmm_acc->text().toDouble();
    double dec = ui->bmm_dec->text().toDouble();
    double jerk = ui->bmm_jerk->text().toDouble();
    double overlap = ui->bmm_overlap->text().toDouble();
    double aoverlap = ui->bmm_auxoverlap->text().toDouble();

    if(moveType.toStdString() == "JOINT")
    {
        if(posType.toStdString() == "CARTESIAN")
        {
            QMessageBox::information(this, "warning", "JointMove with CPOS is not supported in current version");
            return;
        }
        for(int i=0; i<12; i++)
        {
            bmm_ap[0].j[i] = bmmPos1List[i]->text().toDouble();
        }
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap)
                .arg(aoverlap).arg(bmm_ap[0].j[0]).arg(bmm_ap[0].j[1]).arg(bmm_ap[0].j[2]).arg(bmm_ap[0].j[3])
                .arg(bmm_ap[0].j[4]).arg(bmm_ap[0].j[5]).arg(bmm_ap[0].j[6]).arg(bmm_ap[0].j[7]).arg(bmm_ap[0].j[8])
                .arg(bmm_ap[0].j[9]).arg(bmm_ap[0].j[10]).arg(bmm_ap[0].j[11]);
        ui->bmm_posList->addItem(str);
    }
    if(moveType.toStdString() == "LINE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "LineMove with JPOS is not supported in current version");
            return;
        }
        bmm_cp[0].x = ui->bmm_P1J1->text().toDouble();
        bmm_cp[0].y = ui->bmm_P1J2->text().toDouble();
        bmm_cp[0].z = ui->bmm_P1J3->text().toDouble();
        bmm_cp[0].a = ui->bmm_P1J4->text().toDouble();
        bmm_cp[0].b = ui->bmm_P1J5->text().toDouble();
        bmm_cp[0].c = ui->bmm_P1J6->text().toDouble();
        bmm_cp[0].ej1 = ui->bmm_P1ej1->text().toDouble();
        bmm_cp[0].ej2 = ui->bmm_P1ej2->text().toDouble();
        bmm_cp[0].ej3 = ui->bmm_P1ej3->text().toDouble();
        bmm_cp[0].ej4 = ui->bmm_P1ej4->text().toDouble();
        bmm_cp[0].ej5 = ui->bmm_P1ej5->text().toDouble();
        bmm_cp[0].ej6 = ui->bmm_P1ej6->text().toDouble();
        bmm_cp[0].cfgx = ui->bmm_P1Cfgx->text().toInt();
        bmm_cp[0].cfg1 = ui->bmm_P1Cfg1->text().toInt();
        bmm_cp[0].cfg4 = ui->bmm_P1Cfg4->text().toInt();
        bmm_cp[0].cfg6 = ui->bmm_P1Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap)
                .arg(aoverlap).arg(bmm_cp[0].x).arg(bmm_cp[0].y).arg(bmm_cp[0].z)
                .arg(bmm_cp[0].a).arg(bmm_cp[0].b).arg(bmm_cp[0].c).arg(bmm_cp[0].ej1).arg(bmm_cp[0].ej2)
                .arg(bmm_cp[0].ej3).arg(bmm_cp[0].ej4).arg(bmm_cp[0].ej5).arg(bmm_cp[0].ej6)
                .arg(bmm_cp[0].cfgx).arg(bmm_cp[0].cfg1).arg(bmm_cp[0].cfg4).arg(bmm_cp[0].cfg6);
        ui->bmm_posList->addItem(str);
    }
    if(moveType.toStdString() == "CIRCLE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "CircleMove with JPOS is not supported in current version");
            return;
        }
        bmm_cp[0].x = ui->bmm_P1J1->text().toDouble();
        bmm_cp[0].y = ui->bmm_P1J2->text().toDouble();
        bmm_cp[0].z = ui->bmm_P1J3->text().toDouble();
        bmm_cp[0].a = ui->bmm_P1J4->text().toDouble();
        bmm_cp[0].b = ui->bmm_P1J5->text().toDouble();
        bmm_cp[0].c = ui->bmm_P1J6->text().toDouble();
        bmm_cp[0].ej1 = ui->bmm_P1ej1->text().toDouble();
        bmm_cp[0].ej2 = ui->bmm_P1ej2->text().toDouble();
        bmm_cp[0].ej3 = ui->bmm_P1ej3->text().toDouble();
        bmm_cp[0].ej4 = ui->bmm_P1ej4->text().toDouble();
        bmm_cp[0].ej5 = ui->bmm_P1ej5->text().toDouble();
        bmm_cp[0].ej6 = ui->bmm_P1ej6->text().toDouble();
        bmm_cp[0].cfgx = ui->bmm_P1Cfgx->text().toInt();
        bmm_cp[0].cfg1 = ui->bmm_P1Cfg1->text().toInt();
        bmm_cp[0].cfg4 = ui->bmm_P1Cfg4->text().toInt();
        bmm_cp[0].cfg6 = ui->bmm_P1Cfg6->text().toInt();

        bmm_cp[1].x = ui->bmm_P2J1->text().toDouble();
        bmm_cp[1].y = ui->bmm_P2J2->text().toDouble();
        bmm_cp[1].z = ui->bmm_P2J3->text().toDouble();
        bmm_cp[1].a = ui->bmm_P2J4->text().toDouble();
        bmm_cp[1].b = ui->bmm_P2J5->text().toDouble();
        bmm_cp[1].c = ui->bmm_P2J6->text().toDouble();
        bmm_cp[1].ej1 = ui->bmm_P2ej1->text().toDouble();
        bmm_cp[1].ej2 = ui->bmm_P2ej2->text().toDouble();
        bmm_cp[1].ej3 = ui->bmm_P2ej3->text().toDouble();
        bmm_cp[1].ej4 = ui->bmm_P2ej4->text().toDouble();
        bmm_cp[1].ej5 = ui->bmm_P2ej5->text().toDouble();
        bmm_cp[1].ej6 = ui->bmm_P2ej6->text().toDouble();
        bmm_cp[1].cfgx = ui->bmm_P2Cfgx->text().toInt();
        bmm_cp[1].cfg1 = ui->bmm_P2Cfg1->text().toInt();
        bmm_cp[1].cfg4 = ui->bmm_P2Cfg4->text().toInt();
        bmm_cp[1].cfg6 = ui->bmm_P2Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25\n,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap).arg(aoverlap)
                .arg(bmm_cp[0].x).arg(bmm_cp[0].y).arg(bmm_cp[0].z)
                .arg(bmm_cp[0].a).arg(bmm_cp[0].b).arg(bmm_cp[0].c).arg(bmm_cp[0].ej1).arg(bmm_cp[0].ej2)
                .arg(bmm_cp[0].ej3).arg(bmm_cp[0].ej4).arg(bmm_cp[0].ej5).arg(bmm_cp[0].ej6)
                .arg(bmm_cp[0].cfgx).arg(bmm_cp[0].cfg1).arg(bmm_cp[0].cfg4).arg(bmm_cp[0].cfg6)
                .arg(bmm_cp[1].x).arg(bmm_cp[1].y).arg(bmm_cp[1].z)
                .arg(bmm_cp[1].a).arg(bmm_cp[1].b).arg(bmm_cp[1].c).arg(bmm_cp[1].ej1).arg(bmm_cp[1].ej2)
                .arg(bmm_cp[1].ej3).arg(bmm_cp[1].ej4).arg(bmm_cp[1].ej5).arg(bmm_cp[1].ej6)
                .arg(bmm_cp[1].cfgx).arg(bmm_cp[1].cfg1).arg(bmm_cp[1].cfg4).arg(bmm_cp[1].cfg6);
        ui->bmm_posList->addItem(str);
    }
    if(moveType.toStdString() == "ACIRCLE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "ACircleMove with JPOS is not supported in current version");
            return;
        }
        bmm_cp[0].x = ui->bmm_P1J1->text().toDouble();
        bmm_cp[0].y = ui->bmm_P1J2->text().toDouble();
        bmm_cp[0].z = ui->bmm_P1J3->text().toDouble();
        bmm_cp[0].a = ui->bmm_P1J4->text().toDouble();
        bmm_cp[0].b = ui->bmm_P1J5->text().toDouble();
        bmm_cp[0].c = ui->bmm_P1J6->text().toDouble();
        bmm_cp[0].ej1 = ui->bmm_P1ej1->text().toDouble();
        bmm_cp[0].ej2 = ui->bmm_P1ej2->text().toDouble();
        bmm_cp[0].ej3 = ui->bmm_P1ej3->text().toDouble();
        bmm_cp[0].ej4 = ui->bmm_P1ej4->text().toDouble();
        bmm_cp[0].ej5 = ui->bmm_P1ej5->text().toDouble();
        bmm_cp[0].ej6 = ui->bmm_P1ej6->text().toDouble();
        bmm_cp[0].cfgx = ui->bmm_P1Cfgx->text().toInt();
        bmm_cp[0].cfg1 = ui->bmm_P1Cfg1->text().toInt();
        bmm_cp[0].cfg4 = ui->bmm_P1Cfg4->text().toInt();
        bmm_cp[0].cfg6 = ui->bmm_P1Cfg6->text().toInt();

        bmm_cp[1].x = ui->bmm_P2J1->text().toDouble();
        bmm_cp[1].y = ui->bmm_P2J2->text().toDouble();
        bmm_cp[1].z = ui->bmm_P2J3->text().toDouble();
        bmm_cp[1].a = ui->bmm_P2J4->text().toDouble();
        bmm_cp[1].b = ui->bmm_P2J5->text().toDouble();
        bmm_cp[1].c = ui->bmm_P2J6->text().toDouble();
        bmm_cp[1].ej1 = ui->bmm_P2ej1->text().toDouble();
        bmm_cp[1].ej2 = ui->bmm_P2ej2->text().toDouble();
        bmm_cp[1].ej3 = ui->bmm_P2ej3->text().toDouble();
        bmm_cp[1].ej4 = ui->bmm_P2ej4->text().toDouble();
        bmm_cp[1].ej5 = ui->bmm_P2ej5->text().toDouble();
        bmm_cp[1].ej6 = ui->bmm_P2ej6->text().toDouble();
        bmm_cp[1].cfgx = ui->bmm_P2Cfgx->text().toInt();
        bmm_cp[1].cfg1 = ui->bmm_P2Cfg1->text().toInt();
        bmm_cp[1].cfg4 = ui->bmm_P2Cfg4->text().toInt();
        bmm_cp[1].cfg6 = ui->bmm_P2Cfg6->text().toInt();

        bmm_cp[2].x = ui->bmm_P3J1->text().toDouble();
        bmm_cp[2].y = ui->bmm_P3J2->text().toDouble();
        bmm_cp[2].z = ui->bmm_P3J3->text().toDouble();
        bmm_cp[2].a = ui->bmm_P3J4->text().toDouble();
        bmm_cp[2].b = ui->bmm_P3J5->text().toDouble();
        bmm_cp[2].c = ui->bmm_P3J6->text().toDouble();
        bmm_cp[2].ej1 = ui->bmm_P3ej1->text().toDouble();
        bmm_cp[2].ej2 = ui->bmm_P3ej2->text().toDouble();
        bmm_cp[2].ej3 = ui->bmm_P3ej3->text().toDouble();
        bmm_cp[2].ej4 = ui->bmm_P3ej4->text().toDouble();
        bmm_cp[2].ej5 = ui->bmm_P3ej5->text().toDouble();
        bmm_cp[2].ej6 = ui->bmm_P3ej6->text().toDouble();
        bmm_cp[2].cfgx = ui->bmm_P3Cfgx->text().toInt();
        bmm_cp[2].cfg1 = ui->bmm_P3Cfg1->text().toInt();
        bmm_cp[2].cfg4 = ui->bmm_P3Cfg4->text().toInt();
        bmm_cp[2].cfg6 = ui->bmm_P3Cfg6->text().toInt();

        bmm_cp[3].x = ui->bmm_P4J1->text().toDouble();
        bmm_cp[3].y = ui->bmm_P4J2->text().toDouble();
        bmm_cp[3].z = ui->bmm_P4J3->text().toDouble();
        bmm_cp[3].a = ui->bmm_P4J4->text().toDouble();
        bmm_cp[3].b = ui->bmm_P4J5->text().toDouble();
        bmm_cp[3].c = ui->bmm_P4J6->text().toDouble();
        bmm_cp[3].ej1 = ui->bmm_P4ej1->text().toDouble();
        bmm_cp[3].ej2 = ui->bmm_P4ej2->text().toDouble();
        bmm_cp[3].ej3 = ui->bmm_P4ej3->text().toDouble();
        bmm_cp[3].ej4 = ui->bmm_P4ej4->text().toDouble();
        bmm_cp[3].ej5 = ui->bmm_P4ej5->text().toDouble();
        bmm_cp[3].ej6 = ui->bmm_P4ej6->text().toDouble();
        bmm_cp[3].cfgx = ui->bmm_P4Cfgx->text().toInt();
        bmm_cp[3].cfg1 = ui->bmm_P4Cfg1->text().toInt();
        bmm_cp[3].cfg4 = ui->bmm_P4Cfg4->text().toInt();
        bmm_cp[3].cfg6 = ui->bmm_P4Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,"
                              "%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25\n,"
                              "%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41\n,"
                              "%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57\n,"
                              "%58,%59,%60,%61,%62,%63,%64,%65,%66,%67,%68,%69,%70,%71,%72,%73")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap).arg(aoverlap)
                .arg(bmm_cp[0].x).arg(bmm_cp[0].y).arg(bmm_cp[0].z)
                .arg(bmm_cp[0].a).arg(bmm_cp[0].b).arg(bmm_cp[0].c).arg(bmm_cp[0].ej1).arg(bmm_cp[0].ej2)
                .arg(bmm_cp[0].ej3).arg(bmm_cp[0].ej4).arg(bmm_cp[0].ej5).arg(bmm_cp[0].ej6)
                .arg(bmm_cp[0].cfgx).arg(bmm_cp[0].cfg1).arg(bmm_cp[0].cfg4).arg(bmm_cp[0].cfg6)
                .arg(bmm_cp[1].x).arg(bmm_cp[1].y).arg(bmm_cp[1].z)
                .arg(bmm_cp[1].a).arg(bmm_cp[1].b).arg(bmm_cp[1].c).arg(bmm_cp[1].ej1).arg(bmm_cp[1].ej2)
                .arg(bmm_cp[1].ej3).arg(bmm_cp[1].ej4).arg(bmm_cp[1].ej5).arg(bmm_cp[1].ej6)
                .arg(bmm_cp[1].cfgx).arg(bmm_cp[1].cfg1).arg(bmm_cp[1].cfg4).arg(bmm_cp[1].cfg6)
                .arg(bmm_cp[2].x).arg(bmm_cp[2].y).arg(bmm_cp[2].z)
                .arg(bmm_cp[2].a).arg(bmm_cp[2].b).arg(bmm_cp[2].c).arg(bmm_cp[2].ej1).arg(bmm_cp[2].ej2)
                .arg(bmm_cp[2].ej3).arg(bmm_cp[2].ej4).arg(bmm_cp[2].ej5).arg(bmm_cp[2].ej6)
                .arg(bmm_cp[2].cfgx).arg(bmm_cp[2].cfg1).arg(bmm_cp[2].cfg4).arg(bmm_cp[2].cfg6)
                .arg(bmm_cp[3].x).arg(bmm_cp[3].y).arg(bmm_cp[3].z)
                .arg(bmm_cp[3].a).arg(bmm_cp[3].b).arg(bmm_cp[3].c).arg(bmm_cp[3].ej1).arg(bmm_cp[3].ej2)
                .arg(bmm_cp[3].ej3).arg(bmm_cp[3].ej4).arg(bmm_cp[3].ej5).arg(bmm_cp[3].ej6)
                .arg(bmm_cp[3].cfgx).arg(bmm_cp[3].cfg1).arg(bmm_cp[3].cfg4).arg(bmm_cp[3].cfg6);
        ui->bmm_posList->addItem(str);
    }
    return;
}

void RoboxComWidget::on_bmm_postype_currentIndexChanged(int index)
{
    ui->bmm_P1Cfgx->setVisible(index == 0);
    ui->bmm_P1Cfg1->setVisible(index == 0);
    ui->bmm_P1Cfg4->setVisible(index == 0);
    ui->bmm_P1Cfg6->setVisible(index == 0);
    ui->label_p1Cfgx->setVisible(index == 0);
    ui->label_p1Cfg1->setVisible(index == 0);
    ui->label_p1Cfg4->setVisible(index == 0);
    ui->label_p1Cfg6->setVisible(index == 0);

    ui->bmm_P2Cfgx->setVisible(index == 0);
    ui->bmm_P2Cfg1->setVisible(index == 0);
    ui->bmm_P2Cfg4->setVisible(index == 0);
    ui->bmm_P2Cfg6->setVisible(index == 0);
    ui->label_p2Cfgx->setVisible(index == 0);
    ui->label_p2Cfg1->setVisible(index == 0);
    ui->label_p2Cfg4->setVisible(index == 0);
    ui->label_p2Cfg6->setVisible(index == 0);

    ui->bmm_P3Cfgx->setVisible(index == 0);
    ui->bmm_P3Cfg1->setVisible(index == 0);
    ui->bmm_P3Cfg4->setVisible(index == 0);
    ui->bmm_P3Cfg6->setVisible(index == 0);
    ui->label_p3Cfgx->setVisible(index == 0);
    ui->label_p3Cfg1->setVisible(index == 0);
    ui->label_p3Cfg4->setVisible(index == 0);
    ui->label_p3Cfg6->setVisible(index == 0);

    ui->bmm_P4Cfgx->setVisible(index == 0);
    ui->bmm_P4Cfg1->setVisible(index == 0);
    ui->bmm_P4Cfg4->setVisible(index == 0);
    ui->bmm_P4Cfg6->setVisible(index == 0);
    ui->label_p4Cfgx->setVisible(index == 0);
    ui->label_p4Cfg1->setVisible(index == 0);
    ui->label_p4Cfg4->setVisible(index == 0);
    ui->label_p4Cfg6->setVisible(index == 0);
    return;
}

void RoboxComWidget::on_bmm_posDel_btn_clicked()
{
    QListWidgetItem *cur = ui->bmm_posList->currentItem();
    if(cur == NULL)
    {
        QMessageBox::information(this, "warning", "Operation failed");
        return;
    }
    delete cur;
    return;
}

void RoboxComWidget::on_bmm_posClr_btn_clicked()
{
    ui->bmm_posList->clear();
    return;
}

void RoboxComWidget::on_bmm_Start_btn_clicked()
{
    vector<MultiMoveInfo2> list;
    int length = ui->bmm_posList->count();
    for(int i=0; i<length; i++)
    {
        MultiMoveInfo2 pos;
        QString text = ui->bmm_posList->item(i)->text();
        QStringList lst = text.split(",");
        if(lst.count() == 21)
        {
            pos.moveType = MULTI_MOVE2_MJOINT;
            pos.posType = TYPE_JOINT;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            for(int j=0; j<12; j++)
            {
                pos.ap[0].j[j] = lst[j+9].toDouble();
            }
        }
        else if(lst.count() == 25)
        {
            pos.moveType = MULTI_MOVE2_MLIN;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toDouble();
            pos.cp[0].cfg1 = lst[22].toDouble();
            pos.cp[0].cfg4 = lst[23].toDouble();
            pos.cp[0].cfg6 = lst[24].toDouble();
        }
        else if(lst.count() == 41)
        {
            pos.moveType = MULTI_MOVE2_MCIRC;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toDouble();
            pos.cp[0].cfg1 = lst[22].toDouble();
            pos.cp[0].cfg4 = lst[23].toDouble();
            pos.cp[0].cfg6 = lst[24].toDouble();

            pos.cp[1].x = lst[25].toDouble();
            pos.cp[1].y = lst[26].toDouble();
            pos.cp[1].z = lst[27].toDouble();
            pos.cp[1].a = lst[28].toDouble();
            pos.cp[1].b = lst[29].toDouble();
            pos.cp[1].c = lst[30].toDouble();
            pos.cp[1].ej1 = lst[31].toDouble();
            pos.cp[1].ej2 = lst[32].toDouble();
            pos.cp[1].ej3 = lst[33].toDouble();
            pos.cp[1].ej4 = lst[34].toDouble();
            pos.cp[1].ej5 = lst[35].toDouble();
            pos.cp[1].ej6 = lst[36].toDouble();
            pos.cp[1].cfgx = lst[37].toDouble();
            pos.cp[1].cfg1 = lst[38].toDouble();
            pos.cp[1].cfg4 = lst[39].toDouble();
            pos.cp[1].cfg6 = lst[40].toDouble();
        }
        else if(lst.count() == 73)
        {
            pos.moveType = MULTI_MOVE2_MCIRC360;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toInt();
            pos.cp[0].cfg1 = lst[22].toInt();
            pos.cp[0].cfg4 = lst[23].toInt();
            pos.cp[0].cfg6 = lst[24].toInt();

            pos.cp[1].x = lst[25].toDouble();
            pos.cp[1].y = lst[26].toDouble();
            pos.cp[1].z = lst[27].toDouble();
            pos.cp[1].a = lst[28].toDouble();
            pos.cp[1].b = lst[29].toDouble();
            pos.cp[1].c = lst[30].toDouble();
            pos.cp[1].ej1 = lst[31].toDouble();
            pos.cp[1].ej2 = lst[32].toDouble();
            pos.cp[1].ej3 = lst[33].toDouble();
            pos.cp[1].ej4 = lst[34].toDouble();
            pos.cp[1].ej5 = lst[35].toDouble();
            pos.cp[1].ej6 = lst[36].toDouble();
            pos.cp[1].cfgx = lst[37].toInt();
            pos.cp[1].cfg1 = lst[38].toInt();
            pos.cp[1].cfg4 = lst[39].toInt();
            pos.cp[1].cfg6 = lst[40].toInt();

            pos.cp[2].x = lst[41].toDouble();
            pos.cp[2].y = lst[42].toDouble();
            pos.cp[2].z = lst[43].toDouble();
            pos.cp[2].a = lst[44].toDouble();
            pos.cp[2].b = lst[45].toDouble();
            pos.cp[2].c = lst[46].toDouble();
            pos.cp[2].ej1 = lst[47].toDouble();
            pos.cp[2].ej2 = lst[48].toDouble();
            pos.cp[2].ej3 = lst[49].toDouble();
            pos.cp[2].ej4 = lst[50].toDouble();
            pos.cp[2].ej5 = lst[51].toDouble();
            pos.cp[2].ej6 = lst[52].toDouble();
            pos.cp[2].cfgx = lst[53].toInt();
            pos.cp[2].cfg1 = lst[54].toInt();
            pos.cp[2].cfg4 = lst[55].toInt();
            pos.cp[2].cfg6 = lst[56].toInt();

            pos.cp[3].x = lst[57].toDouble();
            pos.cp[3].y = lst[58].toDouble();
            pos.cp[3].z = lst[59].toDouble();
            pos.cp[3].a = lst[60].toDouble();
            pos.cp[3].b = lst[61].toDouble();
            pos.cp[3].c = lst[62].toDouble();
            pos.cp[3].ej1 = lst[63].toDouble();
            pos.cp[3].ej2 = lst[64].toDouble();
            pos.cp[3].ej3 = lst[65].toDouble();
            pos.cp[3].ej4 = lst[66].toDouble();
            pos.cp[3].ej5 = lst[67].toDouble();
            pos.cp[3].ej6 = lst[68].toDouble();
            pos.cp[3].cfgx = lst[69].toInt();
            pos.cp[3].cfg1 = lst[70].toInt();
            pos.cp[3].cfg4 = lst[71].toInt();
            pos.cp[3].cfg6 = lst[72].toInt();
        }
        list.push_back(pos);
    }

    std::cout << "BlockMultiMoveStart, List = " << list.size() << std::endl;   
    bmmStop_thread = new std::thread(BlockMultiMove, list, m_deviceId);
    bmmStop_thread->detach();
    return;
}

void RoboxComWidget::on_bmm_Stop_btn_clicked()
{
    int ret = BlockMultiMoveStop(m_deviceId);
    if(ret != ERROR_OK)
    {
        qDebug() << "BlockMultiMoveStop res = " << ret;
        QMessageBox::information(this, "warning", "BlockMultiMoveStop failed!");
    }

    if (bmmStop_thread)
    {
        bmmStop_thread->~thread();
    }
}

void RoboxComWidget::on_bmm_movetype_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
    case 1:
        ui->tabWidget_2->setTabEnabled(1,false);
        ui->tabWidget_2->setTabEnabled(2,false);
        ui->tabWidget_2->setTabEnabled(3,false);
        break;
    case 2:
        ui->tabWidget_2->setTabEnabled(1,true);
        ui->tabWidget_2->setTabEnabled(2,false);
        ui->tabWidget_2->setTabEnabled(3,false);
        break;
    case 3:
        ui->tabWidget_2->setTabEnabled(1,true);
        ui->tabWidget_2->setTabEnabled(2,true);
        ui->tabWidget_2->setTabEnabled(3,true);
        break;
    default:
        break;
    }
}


void RoboxComWidget::on_IkSolver_btn_clicked()
{
    std::string toolName = ui->Ik_toolName->text().toStdString();
    std::string wobjName = ui->Ik_wobjName->text().toStdString();
    RobotPos cp;
    cp.x = ui->Ik_x->text().toDouble();
    cp.y = ui->Ik_y->text().toDouble();
    cp.z = ui->Ik_z->text().toDouble();
    cp.a = ui->Ik_a->text().toDouble();
    cp.b = ui->Ik_b->text().toDouble();
    cp.c = ui->Ik_c->text().toDouble();
    cp.ej1 = ui->Ik_ej1->text().toDouble();
    cp.ej2 = ui->Ik_ej2->text().toDouble();
    cp.ej3 = ui->Ik_ej3->text().toDouble();
    cp.ej4 = ui->Ik_ej4->text().toDouble();
    cp.ej5 = ui->Ik_ej5->text().toDouble();
    cp.ej6 = ui->Ik_ej6->text().toDouble();
    cp.cfgx = ui->Ik_Cfgx->text().toInt();
    cp.cfg1 = ui->Ik_Cfg1->text().toInt();
    cp.cfg4 = ui->Ik_Cfg4->text().toInt();
    cp.cfg6 = ui->Ik_Cfg6->text().toInt();
    RobotJoint ap;
    int ret = IkSolver(cp, ap, toolName, wobjName, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "IkSolver Operation Failed");
        return;
    }
    ui->Ik_j1label->setText(QString::number(ap.j[0], 'f', 2));
    ui->Ik_j2label->setText(QString::number(ap.j[1], 'f', 2));
    ui->Ik_j3label->setText(QString::number(ap.j[2], 'f', 2));
    ui->Ik_j4label->setText(QString::number(ap.j[3], 'f', 2));
    ui->Ik_j5label->setText(QString::number(ap.j[4], 'f', 2));
    ui->Ik_j6label->setText(QString::number(ap.j[5], 'f', 2));
//    if(ap.j[6] != INIT_FLOAT_MAX_VALUE)
//    {
//        ui->Ik_ej1label->setText(QString::number(ap.j[6], 'f', 2));
//        ui->Ik_ej2label->setText(QString::number(ap.j[7], 'f', 2));
//        ui->Ik_ej3label->setText(QString::number(ap.j[8], 'f', 2));
//        ui->Ik_ej4label->setText(QString::number(ap.j[9], 'f', 2));
//        ui->Ik_ej5label->setText(QString::number(ap.j[10], 'f', 2));
//        ui->Ik_ej6label->setText(QString::number(ap.j[11], 'f', 2));
//    }
    return;
}

void RoboxComWidget::on_fkSolver_btn_clicked()
{
    std::string toolName = ui->fk_toolName->text().toStdString();
    std::string wobjName = ui->fk_wobjName->text().toStdString();
    RobotJoint ap;
    ap.j[0] = ui->fk_j1->text().toDouble();
    ap.j[1] = ui->fk_j2->text().toDouble();
    ap.j[2] = ui->fk_j3->text().toDouble();
    ap.j[3] = ui->fk_j4->text().toDouble();
    ap.j[4] = ui->fk_j5->text().toDouble();
    ap.j[5] = ui->fk_j6->text().toDouble();
    ap.j[6] = ui->fk_ej1->text().toDouble();
    ap.j[7] = ui->fk_ej2->text().toDouble();
    ap.j[8] = ui->fk_ej3->text().toDouble();
    ap.j[9] = ui->fk_ej4->text().toDouble();
    ap.j[10] = ui->fk_ej5->text().toDouble();
    ap.j[11] = ui->fk_ej6->text().toDouble();
    RobotPos cp;
    int ret = FkSolver(ap, cp, toolName, wobjName, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "FkSolver Operation Failed <%d>", ret);
        return;
    }
    ui->fk_xlabel->setText(QString::number(cp.x, 'f', 2));
    ui->fk_ylabel->setText(QString::number(cp.y, 'f', 2));
    ui->fk_zlabel->setText(QString::number(cp.z, 'f', 2));
    ui->fk_alabel->setText(QString::number(cp.a, 'f', 2));
    ui->fk_blabel->setText(QString::number(cp.b, 'f', 2));
    ui->fk_clabel->setText(QString::number(cp.c, 'f', 2));
    ui->fk_ej1label->setText(QString::number(cp.ej1, 'f', 2));
    ui->fk_ej2label->setText(QString::number(cp.ej2, 'f', 2));
    ui->fk_ej3label->setText(QString::number(cp.ej3, 'f', 2));
    ui->fk_ej4label->setText(QString::number(cp.ej4, 'f', 2));
    ui->fk_ej5label->setText(QString::number(cp.ej5, 'f', 2));
    ui->fk_ej6label->setText(QString::number(cp.ej6, 'f', 2));
    ui->fk_cfgxlabel->setText(QString::number(cp.cfgx));
    ui->fk_cfg1label->setText(QString::number(cp.cfg1));
    ui->fk_cfg4label->setText(QString::number(cp.cfg4));
    ui->fk_cfg6label->setText(QString::number(cp.cfg6));
    return;
}

void RoboxComWidget::on_mm2_postype_currentIndexChanged(int index)
{
    ui->mm2_P1Cfgx->setVisible(index == 0);
    ui->mm2_P1Cfg1->setVisible(index == 0);
    ui->mm2_P1Cfg4->setVisible(index == 0);
    ui->mm2_P1Cfg6->setVisible(index == 0);
    ui->label_p1Cfgx_2->setVisible(index == 0);
    ui->label_p1Cfg1_2->setVisible(index == 0);
    ui->label_p1Cfg4_2->setVisible(index == 0);
    ui->label_p1Cfg6_2->setVisible(index == 0);

    ui->mm2_P2Cfgx->setVisible(index == 0);
    ui->mm2_P2Cfg1->setVisible(index == 0);
    ui->mm2_P2Cfg4->setVisible(index == 0);
    ui->mm2_P2Cfg6->setVisible(index == 0);
    ui->label_p2Cfgx_2->setVisible(index == 0);
    ui->label_p2Cfg1_2->setVisible(index == 0);
    ui->label_p2Cfg4_2->setVisible(index == 0);
    ui->label_p2Cfg6_2->setVisible(index == 0);

    ui->mm2_P3Cfgx->setVisible(index == 0);
    ui->mm2_P3Cfg1->setVisible(index == 0);
    ui->mm2_P3Cfg4->setVisible(index == 0);
    ui->mm2_P3Cfg6->setVisible(index == 0);
    ui->label_p3Cfgx_2->setVisible(index == 0);
    ui->label_p3Cfg1_2->setVisible(index == 0);
    ui->label_p3Cfg4_2->setVisible(index == 0);
    ui->label_p3Cfg6_2->setVisible(index == 0);

    ui->mm2_P4Cfgx->setVisible(index == 0);
    ui->mm2_P4Cfg1->setVisible(index == 0);
    ui->mm2_P4Cfg4->setVisible(index == 0);
    ui->mm2_P4Cfg6->setVisible(index == 0);
    ui->label_p4Cfgx_2->setVisible(index == 0);
    ui->label_p4Cfg1_2->setVisible(index == 0);
    ui->label_p4Cfg4_2->setVisible(index == 0);
    ui->label_p4Cfg6_2->setVisible(index == 0);
}

void RoboxComWidget::on_mm2_movetype_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
    case 1:
        ui->tabWidget_3->setTabEnabled(1,false);
        ui->tabWidget_3->setTabEnabled(2,false);
        ui->tabWidget_3->setTabEnabled(3,false);
        break;
    case 2:
        ui->tabWidget_3->setTabEnabled(1,true);
        ui->tabWidget_3->setTabEnabled(2,false);
        ui->tabWidget_3->setTabEnabled(3,false);
        break;
    case 3:
        ui->tabWidget_3->setTabEnabled(1,true);
        ui->tabWidget_3->setTabEnabled(2,true);
        ui->tabWidget_3->setTabEnabled(3,true);
        break;
    default:
        break;
    }
}

void RoboxComWidget::on_mm2_add_btn_clicked()
{
    if(ui->bmm_posList->count() >= MAX_MULTI_MOVE)
    {
        QMessageBox::information(this, "warning", "Operation failed! Max quantity is 20!");
        return;
    }
    QString moveType = ui->mm2_movetype->currentText();
    QString posType = ui->mm2_postype->currentText();
    int flag = ui->mm2_flags->currentIndex();
    double speed = ui->mm2_speed->text().toDouble();
    double acc = ui->mm2_acc->text().toDouble();
    double dec = ui->mm2_dec->text().toDouble();
    double jerk = ui->mm2_jerk->text().toDouble();
    double overlap = ui->mm2_overlap->text().toDouble();
    double aoverlap = ui->mm2_auxoverlap->text().toDouble();

    if(moveType.toStdString() == "JOINT")
    {
        if(posType.toStdString() == "CARTESIAN")
        {
            QMessageBox::information(this, "warning", "JointMove with CPOS is not supported in current version");
            return;
        }
        for(int i=0; i<12; i++)
        {
            mm2_ap[0].j[i] = mm2Pos1List[i]->text().toDouble();
        }
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap)
                .arg(aoverlap).arg(mm2_ap[0].j[0]).arg(mm2_ap[0].j[1]).arg(mm2_ap[0].j[2]).arg(mm2_ap[0].j[3])
                .arg(mm2_ap[0].j[4]).arg(mm2_ap[0].j[5]).arg(mm2_ap[0].j[6]).arg(mm2_ap[0].j[7]).arg(mm2_ap[0].j[8])
                .arg(mm2_ap[0].j[9]).arg(mm2_ap[0].j[10]).arg(mm2_ap[0].j[11]);
        ui->mm2_poslist->addItem(str);
    }
    if(moveType.toStdString() == "LINE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "LineMove with JPOS is not supported in current version");
            return;
        }
        mm2_cp[0].x = ui->mm2_P1J1->text().toDouble();
        mm2_cp[0].y = ui->mm2_P1J2->text().toDouble();
        mm2_cp[0].z = ui->mm2_P1J3->text().toDouble();
        mm2_cp[0].a = ui->mm2_P1J4->text().toDouble();
        mm2_cp[0].b = ui->mm2_P1J5->text().toDouble();
        mm2_cp[0].c = ui->mm2_P1J6->text().toDouble();
        mm2_cp[0].ej1 = ui->mm2_P1ej1->text().toDouble();
        mm2_cp[0].ej2 = ui->mm2_P1ej2->text().toDouble();
        mm2_cp[0].ej3 = ui->mm2_P1ej3->text().toDouble();
        mm2_cp[0].ej4 = ui->mm2_P1ej4->text().toDouble();
        mm2_cp[0].ej5 = ui->mm2_P1ej5->text().toDouble();
        mm2_cp[0].ej6 = ui->mm2_P1ej6->text().toDouble();
        mm2_cp[0].cfgx = ui->mm2_P1Cfgx->text().toInt();
        mm2_cp[0].cfg1 = ui->mm2_P1Cfg1->text().toInt();
        mm2_cp[0].cfg4 = ui->mm2_P1Cfg4->text().toInt();
        mm2_cp[0].cfg6 = ui->mm2_P1Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap)
                .arg(aoverlap).arg(mm2_cp[0].x).arg(mm2_cp[0].y).arg(mm2_cp[0].z)
                .arg(mm2_cp[0].a).arg(mm2_cp[0].b).arg(mm2_cp[0].c).arg(mm2_cp[0].ej1).arg(mm2_cp[0].ej2)
                .arg(mm2_cp[0].ej3).arg(mm2_cp[0].ej4).arg(mm2_cp[0].ej5).arg(mm2_cp[0].ej6)
                .arg(mm2_cp[0].cfgx).arg(mm2_cp[0].cfg1).arg(mm2_cp[0].cfg4).arg(mm2_cp[0].cfg6);
        ui->mm2_poslist->addItem(str);
    }
    if(moveType.toStdString() == "CIRCLE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "CircleMove with JPOS is not supported in current version");
            return;
        }
        mm2_cp[0].x = ui->mm2_P1J1->text().toDouble();
        mm2_cp[0].y = ui->mm2_P1J2->text().toDouble();
        mm2_cp[0].z = ui->mm2_P1J3->text().toDouble();
        mm2_cp[0].a = ui->mm2_P1J4->text().toDouble();
        mm2_cp[0].b = ui->mm2_P1J5->text().toDouble();
        mm2_cp[0].c = ui->mm2_P1J6->text().toDouble();
        mm2_cp[0].ej1 = ui->mm2_P1ej1->text().toDouble();
        mm2_cp[0].ej2 = ui->mm2_P1ej2->text().toDouble();
        mm2_cp[0].ej3 = ui->mm2_P1ej3->text().toDouble();
        mm2_cp[0].ej4 = ui->mm2_P1ej4->text().toDouble();
        mm2_cp[0].ej5 = ui->mm2_P1ej5->text().toDouble();
        mm2_cp[0].ej6 = ui->mm2_P1ej6->text().toDouble();
        mm2_cp[0].cfgx = ui->mm2_P1Cfgx->text().toInt();
        mm2_cp[0].cfg1 = ui->mm2_P1Cfg1->text().toInt();
        mm2_cp[0].cfg4 = ui->mm2_P1Cfg4->text().toInt();
        mm2_cp[0].cfg6 = ui->mm2_P1Cfg6->text().toInt();

        mm2_cp[1].x = ui->mm2_P2J1->text().toDouble();
        mm2_cp[1].y = ui->mm2_P2J2->text().toDouble();
        mm2_cp[1].z = ui->mm2_P2J3->text().toDouble();
        mm2_cp[1].a = ui->mm2_P2J4->text().toDouble();
        mm2_cp[1].b = ui->mm2_P2J5->text().toDouble();
        mm2_cp[1].c = ui->mm2_P2J6->text().toDouble();
        mm2_cp[1].ej1 = ui->mm2_P2ej1->text().toDouble();
        mm2_cp[1].ej2 = ui->mm2_P2ej2->text().toDouble();
        mm2_cp[1].ej3 = ui->mm2_P2ej3->text().toDouble();
        mm2_cp[1].ej4 = ui->mm2_P2ej4->text().toDouble();
        mm2_cp[1].ej5 = ui->mm2_P2ej5->text().toDouble();
        mm2_cp[1].ej6 = ui->mm2_P2ej6->text().toDouble();
        mm2_cp[1].cfgx = ui->mm2_P2Cfgx->text().toInt();
        mm2_cp[1].cfg1 = ui->mm2_P2Cfg1->text().toInt();
        mm2_cp[1].cfg4 = ui->mm2_P2Cfg4->text().toInt();
        mm2_cp[1].cfg6 = ui->mm2_P2Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25\n,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap).arg(aoverlap)
                .arg(mm2_cp[0].x).arg(mm2_cp[0].y).arg(mm2_cp[0].z)
                .arg(mm2_cp[0].a).arg(mm2_cp[0].b).arg(mm2_cp[0].c).arg(mm2_cp[0].ej1).arg(mm2_cp[0].ej2)
                .arg(mm2_cp[0].ej3).arg(mm2_cp[0].ej4).arg(mm2_cp[0].ej5).arg(mm2_cp[0].ej6)
                .arg(mm2_cp[0].cfgx).arg(mm2_cp[0].cfg1).arg(mm2_cp[0].cfg4).arg(mm2_cp[0].cfg6)
                .arg(mm2_cp[1].x).arg(mm2_cp[1].y).arg(mm2_cp[1].z)
                .arg(mm2_cp[1].a).arg(mm2_cp[1].b).arg(mm2_cp[1].c).arg(mm2_cp[1].ej1).arg(mm2_cp[1].ej2)
                .arg(mm2_cp[1].ej3).arg(mm2_cp[1].ej4).arg(mm2_cp[1].ej5).arg(mm2_cp[1].ej6)
                .arg(mm2_cp[1].cfgx).arg(mm2_cp[1].cfg1).arg(mm2_cp[1].cfg4).arg(mm2_cp[1].cfg6);
        ui->mm2_poslist->addItem(str);
    }
    if(moveType.toStdString() == "ACIRCLE")
    {
        if(posType.toStdString() == "JOINT")
        {
            QMessageBox::information(this, "warning", "ACircleMove with JPOS is not supported in current version");
            return;
        }
        mm2_cp[0].x = ui->mm2_P1J1->text().toDouble();
        mm2_cp[0].y = ui->mm2_P1J2->text().toDouble();
        mm2_cp[0].z = ui->mm2_P1J3->text().toDouble();
        mm2_cp[0].a = ui->mm2_P1J4->text().toDouble();
        mm2_cp[0].b = ui->mm2_P1J5->text().toDouble();
        mm2_cp[0].c = ui->mm2_P1J6->text().toDouble();
        mm2_cp[0].ej1 = ui->mm2_P1ej1->text().toDouble();
        mm2_cp[0].ej2 = ui->mm2_P1ej2->text().toDouble();
        mm2_cp[0].ej3 = ui->mm2_P1ej3->text().toDouble();
        mm2_cp[0].ej4 = ui->mm2_P1ej4->text().toDouble();
        mm2_cp[0].ej5 = ui->mm2_P1ej5->text().toDouble();
        mm2_cp[0].ej6 = ui->mm2_P1ej6->text().toDouble();
        mm2_cp[0].cfgx = ui->mm2_P1Cfgx->text().toInt();
        mm2_cp[0].cfg1 = ui->mm2_P1Cfg1->text().toInt();
        mm2_cp[0].cfg4 = ui->mm2_P1Cfg4->text().toInt();
        mm2_cp[0].cfg6 = ui->mm2_P1Cfg6->text().toInt();

        mm2_cp[1].x = ui->mm2_P2J1->text().toDouble();
        mm2_cp[1].y = ui->mm2_P2J2->text().toDouble();
        mm2_cp[1].z = ui->mm2_P2J3->text().toDouble();
        mm2_cp[1].a = ui->mm2_P2J4->text().toDouble();
        mm2_cp[1].b = ui->mm2_P2J5->text().toDouble();
        mm2_cp[1].c = ui->mm2_P2J6->text().toDouble();
        mm2_cp[1].ej1 = ui->mm2_P2ej1->text().toDouble();
        mm2_cp[1].ej2 = ui->mm2_P2ej2->text().toDouble();
        mm2_cp[1].ej3 = ui->mm2_P2ej3->text().toDouble();
        mm2_cp[1].ej4 = ui->mm2_P2ej4->text().toDouble();
        mm2_cp[1].ej5 = ui->mm2_P2ej5->text().toDouble();
        mm2_cp[1].ej6 = ui->mm2_P2ej6->text().toDouble();
        mm2_cp[1].cfgx = ui->mm2_P2Cfgx->text().toInt();
        mm2_cp[1].cfg1 = ui->mm2_P2Cfg1->text().toInt();
        mm2_cp[1].cfg4 = ui->mm2_P2Cfg4->text().toInt();
        mm2_cp[1].cfg6 = ui->mm2_P2Cfg6->text().toInt();

        mm2_cp[2].x = ui->mm2_P3J1->text().toDouble();
        mm2_cp[2].y = ui->mm2_P3J2->text().toDouble();
        mm2_cp[2].z = ui->mm2_P3J3->text().toDouble();
        mm2_cp[2].a = ui->mm2_P3J4->text().toDouble();
        mm2_cp[2].b = ui->mm2_P3J5->text().toDouble();
        mm2_cp[2].c = ui->mm2_P3J6->text().toDouble();
        mm2_cp[2].ej1 = ui->mm2_P3ej1->text().toDouble();
        mm2_cp[2].ej2 = ui->mm2_P3ej2->text().toDouble();
        mm2_cp[2].ej3 = ui->mm2_P3ej3->text().toDouble();
        mm2_cp[2].ej4 = ui->mm2_P3ej4->text().toDouble();
        mm2_cp[2].ej5 = ui->mm2_P3ej5->text().toDouble();
        mm2_cp[2].ej6 = ui->mm2_P3ej6->text().toDouble();
        mm2_cp[2].cfgx = ui->mm2_P3Cfgx->text().toInt();
        mm2_cp[2].cfg1 = ui->mm2_P3Cfg1->text().toInt();
        mm2_cp[2].cfg4 = ui->mm2_P3Cfg4->text().toInt();
        mm2_cp[2].cfg6 = ui->mm2_P3Cfg6->text().toInt();

        mm2_cp[3].x = ui->mm2_P4J1->text().toDouble();
        mm2_cp[3].y = ui->mm2_P4J2->text().toDouble();
        mm2_cp[3].z = ui->mm2_P4J3->text().toDouble();
        mm2_cp[3].a = ui->mm2_P4J4->text().toDouble();
        mm2_cp[3].b = ui->mm2_P4J5->text().toDouble();
        mm2_cp[3].c = ui->mm2_P4J6->text().toDouble();
        mm2_cp[3].ej1 = ui->mm2_P4ej1->text().toDouble();
        mm2_cp[3].ej2 = ui->mm2_P4ej2->text().toDouble();
        mm2_cp[3].ej3 = ui->mm2_P4ej3->text().toDouble();
        mm2_cp[3].ej4 = ui->mm2_P4ej4->text().toDouble();
        mm2_cp[3].ej5 = ui->mm2_P4ej5->text().toDouble();
        mm2_cp[3].ej6 = ui->mm2_P4ej6->text().toDouble();
        mm2_cp[3].cfgx = ui->mm2_P4Cfgx->text().toInt();
        mm2_cp[3].cfg1 = ui->mm2_P4Cfg1->text().toInt();
        mm2_cp[3].cfg4 = ui->mm2_P4Cfg4->text().toInt();
        mm2_cp[3].cfg6 = ui->mm2_P4Cfg6->text().toInt();
        QString str = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n,"
                              "%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25\n,"
                              "%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38,%39,%40,%41\n,"
                              "%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57\n,"
                              "%58,%59,%60,%61,%62,%63,%64,%65,%66,%67,%68,%69,%70,%71,%72,%73")
                .arg(moveType).arg(posType).arg(flag).arg(speed)
                .arg(acc).arg(dec).arg(jerk).arg(overlap).arg(aoverlap)
                .arg(mm2_cp[0].x).arg(mm2_cp[0].y).arg(mm2_cp[0].z)
                .arg(mm2_cp[0].a).arg(mm2_cp[0].b).arg(mm2_cp[0].c).arg(mm2_cp[0].ej1).arg(mm2_cp[0].ej2)
                .arg(mm2_cp[0].ej3).arg(mm2_cp[0].ej4).arg(mm2_cp[0].ej5).arg(mm2_cp[0].ej6)
                .arg(mm2_cp[0].cfgx).arg(mm2_cp[0].cfg1).arg(mm2_cp[0].cfg4).arg(mm2_cp[0].cfg6)
                .arg(mm2_cp[1].x).arg(mm2_cp[1].y).arg(mm2_cp[1].z)
                .arg(mm2_cp[1].a).arg(mm2_cp[1].b).arg(mm2_cp[1].c).arg(mm2_cp[1].ej1).arg(mm2_cp[1].ej2)
                .arg(mm2_cp[1].ej3).arg(mm2_cp[1].ej4).arg(mm2_cp[1].ej5).arg(mm2_cp[1].ej6)
                .arg(mm2_cp[1].cfgx).arg(mm2_cp[1].cfg1).arg(mm2_cp[1].cfg4).arg(mm2_cp[1].cfg6)
                .arg(mm2_cp[2].x).arg(mm2_cp[2].y).arg(mm2_cp[2].z)
                .arg(mm2_cp[2].a).arg(mm2_cp[2].b).arg(mm2_cp[2].c).arg(mm2_cp[2].ej1).arg(mm2_cp[2].ej2)
                .arg(mm2_cp[2].ej3).arg(mm2_cp[2].ej4).arg(mm2_cp[2].ej5).arg(mm2_cp[2].ej6)
                .arg(mm2_cp[2].cfgx).arg(mm2_cp[2].cfg1).arg(mm2_cp[2].cfg4).arg(mm2_cp[2].cfg6)
                .arg(mm2_cp[3].x).arg(mm2_cp[3].y).arg(mm2_cp[3].z)
                .arg(mm2_cp[3].a).arg(mm2_cp[3].b).arg(mm2_cp[3].c).arg(mm2_cp[3].ej1).arg(mm2_cp[3].ej2)
                .arg(mm2_cp[3].ej3).arg(mm2_cp[3].ej4).arg(mm2_cp[3].ej5).arg(mm2_cp[3].ej6)
                .arg(mm2_cp[3].cfgx).arg(mm2_cp[3].cfg1).arg(mm2_cp[3].cfg4).arg(mm2_cp[3].cfg6);
        ui->mm2_poslist->addItem(str);
    }
    return;
}


void RoboxComWidget::on_mm2_del_btn_clicked()
{
    QListWidgetItem *cur = ui->mm2_poslist->currentItem();
    if(cur == NULL)
    {
        QMessageBox::information(this, "warning", "Operation failed");
        return;
    }
    delete cur;
    return;
}

void RoboxComWidget::on_mm2_clear_btn_clicked()
{
    ui->mm2_poslist->clear();
    return;
}

void RoboxComWidget::on_mm2_start_btn_clicked()
{
    vector<MultiMoveInfo2> list;
    int length = ui->mm2_poslist->count();
    for(int i=0; i<length; i++)
    {
        MultiMoveInfo2 pos;
        QString text = ui->mm2_poslist->item(i)->text();
        QStringList lst = text.split(",");
        if(lst.count() == 21)
        {
            pos.moveType = MULTI_MOVE2_MJOINT;
            pos.posType = TYPE_JOINT;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            for(int j=0; j<12; j++)
            {
                pos.ap[0].j[j] = lst[j+9].toDouble();
            }
        }
        else if(lst.count() == 25)
        {
            pos.moveType = MULTI_MOVE2_MLIN;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toDouble();
            pos.cp[0].cfg1 = lst[22].toDouble();
            pos.cp[0].cfg4 = lst[23].toDouble();
            pos.cp[0].cfg6 = lst[24].toDouble();
        }
        else if(lst.count() == 41)
        {
            pos.moveType = MULTI_MOVE2_MCIRC;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toDouble();
            pos.cp[0].cfg1 = lst[22].toDouble();
            pos.cp[0].cfg4 = lst[23].toDouble();
            pos.cp[0].cfg6 = lst[24].toDouble();

            pos.cp[1].x = lst[25].toDouble();
            pos.cp[1].y = lst[26].toDouble();
            pos.cp[1].z = lst[27].toDouble();
            pos.cp[1].a = lst[28].toDouble();
            pos.cp[1].b = lst[29].toDouble();
            pos.cp[1].c = lst[30].toDouble();
            pos.cp[1].ej1 = lst[31].toDouble();
            pos.cp[1].ej2 = lst[32].toDouble();
            pos.cp[1].ej3 = lst[33].toDouble();
            pos.cp[1].ej4 = lst[34].toDouble();
            pos.cp[1].ej5 = lst[35].toDouble();
            pos.cp[1].ej6 = lst[36].toDouble();
            pos.cp[1].cfgx = lst[37].toDouble();
            pos.cp[1].cfg1 = lst[38].toDouble();
            pos.cp[1].cfg4 = lst[39].toDouble();
            pos.cp[1].cfg6 = lst[40].toDouble();
        }
        else if(lst.count() == 73)
        {
            pos.moveType = MULTI_MOVE2_MCIRC360;
            pos.posType = TYPE_CART;
            pos.flags = lst[2].toInt();
            pos.speed = lst[3].toDouble();
            pos.acc = lst[4].toDouble();
            pos.dec = lst[5].toDouble();
            pos.jerk = lst[6].toDouble();
            pos.overlapping = lst[7].toDouble();
            pos.auxOverlapping = lst[8].toDouble();
            pos.cp[0].x = lst[9].toDouble();
            pos.cp[0].y = lst[10].toDouble();
            pos.cp[0].z = lst[11].toDouble();
            pos.cp[0].a = lst[12].toDouble();
            pos.cp[0].b = lst[13].toDouble();
            pos.cp[0].c = lst[14].toDouble();
            pos.cp[0].ej1 = lst[15].toDouble();
            pos.cp[0].ej2 = lst[16].toDouble();
            pos.cp[0].ej3 = lst[17].toDouble();
            pos.cp[0].ej4 = lst[18].toDouble();
            pos.cp[0].ej5 = lst[19].toDouble();
            pos.cp[0].ej6 = lst[20].toDouble();
            pos.cp[0].cfgx = lst[21].toInt();
            pos.cp[0].cfg1 = lst[22].toInt();
            pos.cp[0].cfg4 = lst[23].toInt();
            pos.cp[0].cfg6 = lst[24].toInt();

            pos.cp[1].x = lst[25].toDouble();
            pos.cp[1].y = lst[26].toDouble();
            pos.cp[1].z = lst[27].toDouble();
            pos.cp[1].a = lst[28].toDouble();
            pos.cp[1].b = lst[29].toDouble();
            pos.cp[1].c = lst[30].toDouble();
            pos.cp[1].ej1 = lst[31].toDouble();
            pos.cp[1].ej2 = lst[32].toDouble();
            pos.cp[1].ej3 = lst[33].toDouble();
            pos.cp[1].ej4 = lst[34].toDouble();
            pos.cp[1].ej5 = lst[35].toDouble();
            pos.cp[1].ej6 = lst[36].toDouble();
            pos.cp[1].cfgx = lst[37].toInt();
            pos.cp[1].cfg1 = lst[38].toInt();
            pos.cp[1].cfg4 = lst[39].toInt();
            pos.cp[1].cfg6 = lst[40].toInt();

            pos.cp[2].x = lst[41].toDouble();
            pos.cp[2].y = lst[42].toDouble();
            pos.cp[2].z = lst[43].toDouble();
            pos.cp[2].a = lst[44].toDouble();
            pos.cp[2].b = lst[45].toDouble();
            pos.cp[2].c = lst[46].toDouble();
            pos.cp[2].ej1 = lst[47].toDouble();
            pos.cp[2].ej2 = lst[48].toDouble();
            pos.cp[2].ej3 = lst[49].toDouble();
            pos.cp[2].ej4 = lst[50].toDouble();
            pos.cp[2].ej5 = lst[51].toDouble();
            pos.cp[2].ej6 = lst[52].toDouble();
            pos.cp[2].cfgx = lst[53].toInt();
            pos.cp[2].cfg1 = lst[54].toInt();
            pos.cp[2].cfg4 = lst[55].toInt();
            pos.cp[2].cfg6 = lst[56].toInt();

            pos.cp[3].x = lst[57].toDouble();
            pos.cp[3].y = lst[58].toDouble();
            pos.cp[3].z = lst[59].toDouble();
            pos.cp[3].a = lst[60].toDouble();
            pos.cp[3].b = lst[61].toDouble();
            pos.cp[3].c = lst[62].toDouble();
            pos.cp[3].ej1 = lst[63].toDouble();
            pos.cp[3].ej2 = lst[64].toDouble();
            pos.cp[3].ej3 = lst[65].toDouble();
            pos.cp[3].ej4 = lst[66].toDouble();
            pos.cp[3].ej5 = lst[67].toDouble();
            pos.cp[3].ej6 = lst[68].toDouble();
            pos.cp[3].cfgx = lst[69].toInt();
            pos.cp[3].cfg1 = lst[70].toInt();
            pos.cp[3].cfg4 = lst[71].toInt();
            pos.cp[3].cfg6 = lst[72].toInt();
        }
        list.push_back(pos);
    }
    std::cout << "MultiMove2Start, List = " << list.size() << std::endl;
    int ret = MultiMove2Start(list, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", tr("%1").arg(ret));
    }
    return;
}

void RoboxComWidget::on_mm2_hold_btn_clicked()
{
    int ret = MultiMove2Hold(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Multimove2 hold operation failed");
    }
    return;
}

void RoboxComWidget::on_mm2_resume_btn_clicked()
{
    int ret = MultiMove2Resume(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Multimove2 hold operation failed");
    }
    return;
}

void RoboxComWidget::on_mm2_reset_btn_clicked()
{
    int ret = MultiMove2Reset(m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Multimove2 reset operation failed");
    }
    return;
}

void RoboxComWidget::on_auxjCount_currentIndexChanged(int index)
{
    if((index == 0) || (index == 4))
    {
        ui->gantry->clear();
        ui->gantry->addItem(QString::fromLocal8Bit("无"));
    }
    else
    {
        ui->gantry->clear();
        ui->gantry->addItem(QString::fromLocal8Bit("无"));
        ui->gantry->addItem(QString::fromLocal8Bit("有（主从轴电机相向运动）"));
        ui->gantry->addItem(QString::fromLocal8Bit("有（主从轴电机同向运动）"));
    }
    int gtyEnable = ui->gantry->currentIndex();
    if(gtyEnable == 0)  // 无双驱轴
    {
        switch (index) {
        case 0:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            break;
        case 1:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            break;
        case 2:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            break;
        case 3:
        case 4:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y-Z轴直线插补"));
            break;
        default:
            break;
        }
    }
    else
    {
        switch (index) {
        case 1:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            break;
        case 2:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            break;
        case 3:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y-Z轴直线插补"));
            break;
        default:
            break;
        }
    }
}

void RoboxComWidget::on_gantry_currentIndexChanged(int index)
{
    ui->gantry_index->clear();
    int auxNum = ui->auxjCount->currentIndex();
    if(index == 0)  // 无双驱轴
    {
        switch (auxNum) {
        case 0:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            break;
        case 1:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            break;
        case 2:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            break;
        case 3:
        case 4:
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴旋转插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y-Z轴直线插补"));
            break;
        default:
            break;
        }
    }
    else
    {
        switch (auxNum) {
        case 1:
            ui->gantry_index->addItem("1");
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            break;
        case 2:
            ui->gantry_index->addItem("2");
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            break;
        case 3:
            ui->gantry_index->addItem("3");
            ui->systemType->clear();
            ui->systemType->addItem(QString::fromLocal8Bit("无插补轴"));
            ui->systemType->addItem(QString::fromLocal8Bit("X轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("Y-Z轴直线插补"));
            ui->systemType->addItem(QString::fromLocal8Bit("X-Y-Z轴直线插补"));
            break;
        default:
            break;
        }
    }
}

void RoboxComWidget::on_AuxjMove1_btn_clicked()
{
    int auxjNum = ui->auxMove2_combox->currentIndex() + 1;
    int ret = AuxjStepMove1(auxjNum, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "AuxMove1 Operation Failed");
    }
    return;
}

void RoboxComWidget::on_AuxjMove2_btn_clicked()
{
    int auxjNum = ui->auxMove1_combox->currentText().toInt();
    int ret = AuxjStepMove2(auxjNum, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "AuxMove2 Operation Failed");
    }
    return;
}

void RoboxComWidget::on_SetAuxj_btn_clicked()
{
    // int interpCount = ui->interpCount->currentText().toInt();
    int auxCount = ui->auxjCount->currentText().toInt();
    int sysType = ui->systemType->currentIndex();
    GantryInfo gantry;
    switch(ui->gantry->currentIndex())
    {
    case 0:
        gantry.enable = false;
        break;
    case 1:
        gantry.enable = true;
        gantry.sameDirection = false;
        gantry.maxError = ui->gantry_maxError->text().toDouble();
        break;
    case 2:
        gantry.enable = true;
        gantry.sameDirection = true;
        gantry.maxError = ui->gantry_maxError->text().toDouble();
        break;
    default:
        break;
    }

    int ret = SetAuxjSystemType2(gantry, auxCount, sysType, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "SetAuxj Operation Failed");
    }
    return;
}

void RoboxComWidget::on_setVoltage_btn_clicked()
{
    //double value = ui->voltage_edit->text().toDouble();
    //int ret = ArcWeldingSetTxVoltage(value, m_deviceId);
    //if(ret != ERROR_OK)
    //{
    //    QMessageBox::information(this, "warning", "Set Voltage Operation Failed");
    //}
    //return;
}

void RoboxComWidget::on_setCurrent_btn_clicked()
{
    //double value = ui->current_edit->text().toDouble();
    //int ret =ArcWeldingSetTxCurrent(value, m_deviceId);
    //if(ret != ERROR_OK)
    //{
    //    QMessageBox::information(this, "warning", "Set Current Operation Failed");
    //}
    //return;
}

void RoboxComWidget::on_setSpeed_btn_clicked()
{
    //double value = ui->speed_edit->text().toDouble();
    //int ret = ArcWeldingSetWeldingSpeed(value, m_deviceId);
    //if(ret != ERROR_OK)
    //{
    //    QMessageBox::information(this, "warning", "Set Speed Operation Failed");
    //}
    //return;
}

void RoboxComWidget::on_curTool_combox_currentTextChanged(const QString &arg1)
{
    int ret = SetCurrentToolByName(arg1.toStdString(), m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Set Tool Operation Failed");
    }
    return;
}

void RoboxComWidget::on_curWobj_combox_currentTextChanged(const QString &arg1)
{
    int ret = SetCurrentUframeByName(arg1.toStdString(), m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "Set Wobj Operation Failed");
    }
    return;
}
/*
void RoboxComWidget::on_interpCount_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->systemType->clear();
        ui->systemType->addItem("No Interpolation");
        break;
    case 1:
        ui->systemType->clear();
        ui->systemType->addItem("X Linear Interpolation");
        ui->systemType->addItem("Y Linear Interpolation");
        ui->systemType->addItem("Z Linear Interpolation");
        ui->systemType->addItem("X Circular Interpolation");
        ui->systemType->addItem("Y Circular Interpolation");
        ui->systemType->addItem("Z Circular Interpolation");
        break;
    case 2:
        ui->systemType->clear();
        ui->systemType->addItem("X-Y Linear Interpolation");
        ui->systemType->addItem("Y-X Linear Interpolation");
        ui->systemType->addItem("X-Z Linear Interpolation");
        ui->systemType->addItem("Z-X Linear Interpolation");
        ui->systemType->addItem("Y-Z Linear Interpolation");
        ui->systemType->addItem("Z-Y Linear Interpolation");
        break;
    case 3:
        ui->systemType->clear();
        ui->systemType->addItem("X-Y-Z Linear Interpolation");
        ui->systemType->addItem("X-Z-Y Linear Interpolation");
        ui->systemType->addItem("Y-X-Z Linear Interpolation");
        ui->systemType->addItem("Y-Z-X Linear Interpolation");
        ui->systemType->addItem("Z-X-Y Linear Interpolation");
        ui->systemType->addItem("Z-Y-X Linear Interpolation");
        break;
    default:
        break;
    }
}
*/
void RoboxComWidget::on_getbool_btn_clicked()
{
    int index = ui->boolIndex_edit->text().toInt();
    int len = ui->boolen_edit->text().toInt();
    int ret = 0;
    if(len == 1)
    {
        bool value = false;
        ret = GetBoolVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetBoolVariable Failed, errornumber: <%d>", ret);
            return;
        }
        ui->boolList->clear();
        if(value)
            ui->boolList->addItem("1");
        else
            ui->boolList->addItem("0");
    }
    else
    {
        bool *value = new bool[len];
        ret = GetBoolVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetBoolVariable Failed, errornumber: <%d>", ret);
            return;
        }
        ui->boolList->clear();
        for(int i=0; i<len; i++)
        {
            if(value[i])
                ui->boolList->addItem("index"+QString::number(index+i)+": 1");
            else
                ui->boolList->addItem("index"+QString::number(index+i)+": 0");
        }
        delete[] value;
    }
    return;
}

void RoboxComWidget::on_getint_btn_clicked()
{
    int index = ui->intindex_edit->text().toInt();
    int len = ui->intlen_edit->text().toInt();
    int ret = 0;
    if(len == 1)
    {
        int value = 0;
        ret = GetIntVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetIntVariable Failed");
            return;
        }
        ui->intList->clear();
        ui->intList->addItem(QString::number(value));
    }
    else
    {
        int *value = new int[len];
        ret = GetIntVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetIntVariable Failed");
            return;
        }
        ui->intList->clear();
        for(int i=0; i<len; i++)
        {
            ui->intList->addItem("index"+QString::number(index+i)+": "+QString::number(value[i]));
        }
        delete [] value;
    }
    return;
}

void RoboxComWidget::on_getdouble_btn_clicked()
{
    int index = ui->doubleindex_edit->text().toInt();
    int len = ui->doublelen_edit->text().toInt();
    int ret = 0;
    if(len == 1)
    {
        double value = 0;
        ret = GetDoubleVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetDoubleVariable Failed");
            return;
        }
        ui->doubleList->clear();
        ui->doubleList->addItem(QString::number(value, 'f', 6));
    }
    else
    {
        double *value = new double[len];
        ret = GetDoubleVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "GetDoubleVariable Failed");
            return;
        }
        ui->doubleList->clear();
        for(int i=0; i<len; i++)
        {
            ui->doubleList->addItem("index"+QString::number(index+i)+": "+QString::number(value[i], 'f', 6));
        }
        delete [] value;
    }
    return;
}

void RoboxComWidget::on_getjoint_btn_clicked()
{
    int index = ui->jointIndex_edit->text().toInt();
    RobotJoint value;
    int ret = GetJointVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetJointVariable Failed");
        return;
    }
    ui->jointList->clear();
    ui->jointList->addItem("j1:"+QString::number(value.j[0])+
            "; j2:"+QString::number(value.j[1])+
            "; j3:"+QString::number(value.j[2])+
            "; j4:"+QString::number(value.j[3])+
            "; j5:"+QString::number(value.j[4])+
            "; j6:"+QString::number(value.j[5])+
            "; ej1:"+QString::number(value.j[6])+
            "; ej2:"+QString::number(value.j[7])+
            "; ej3:"+QString::number(value.j[8])+
            "; ej4:"+QString::number(value.j[9])+
            "; ej5:"+QString::number(value.j[10])+
            "; ej6:"+QString::number(value.j[11]));
    return;
}

void RoboxComWidget::on_getpos_btn_clicked()
{
    int index = ui->posIndex_edit->text().toInt();
    // RobotPos value;
    int ret = GetPosVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetPosVariable Failed");
        return;
    }
    ui->posList->clear();
    ui->posList->addItem("X:"+QString::number(value.x)+
            "; Y:"+QString::number(value.y)+
            "; Z:"+QString::number(value.z)+
            "; A:"+QString::number(value.a)+
            "; B:"+QString::number(value.b)+
            "; C:"+QString::number(value.c)+
            "; ej1:"+QString::number(value.ej1)+
            "; ej2:"+QString::number(value.ej2)+
            "; ej3:"+QString::number(value.ej3)+
            "; ej4:"+QString::number(value.ej4)+
            "; ej5:"+QString::number(value.ej5)+
            "; ej6:"+QString::number(value.ej6)+
            "; cfgx:"+QString::number(value.cfgx)+
            "; cfg1:"+QString::number(value.cfg1)+
            "; cfg4:"+QString::number(value.cfg4)+
            "; cfg6:"+QString::number(value.cfg6));
    return;
}

void RoboxComWidget::on_setBool_btn_clicked()
{
    int index = ui->setBoolIndex_edit->text().toInt();
    QString text = ui->setBool_edit->text();
    QStringList lst = text.split(";");

    int len = lst.count();
    if(len == 1)
    {
        bool value;
        if(text.toInt())
        {
            value = true;
        }
        else
        {
            value = false;
        }
        int ret = SetBoolVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetBoolVariable Failed");
            return;
        }
    }
    else
    {
        bool *value = new bool[len];
        for(int i=0; i<len; i++)
        {
            if(lst[i].toInt())
            {
                value[i] = true;
            }
            else
            {
                value[i] = false;
            }
        }
        int ret = SetBoolVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetBoolVariable Failed");
            return;
        }
        delete [] value;
    }
}

void RoboxComWidget::on_setInt_btn_clicked()
{
    int index = ui->setIntIndex_edit->text().toInt();
    QString text = ui->setInt_edit->text();
    QStringList lst = text.split(";");

    int len = lst.count();
    if(len == 1)
    {
        int value = text.toInt();
        int ret = SetIntVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetBoolVariable Failed");
            return;
        }
    }
    else
    {
        int *value = new int[len];
        for(int i=0; i<len; i++)
        {
            value[i] = lst[i].toInt();
        }
        int ret = SetIntVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetIntBoolVariable Failed");
            return;
        }
        delete [] value;
    }
}

void RoboxComWidget::on_setDouble_btn_clicked()
{
    int index = ui->setDoubleIndex_edit->text().toInt();
    QString text = ui->setDouble_edit->text();
    QStringList lst = text.split(";");

    int len = lst.count();
    if(len == 1)
    {
        double value = text.toDouble();
        int ret = SetDoubleVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetDoubleVariable Failed");
            return;
        }
    }
    else
    {
        double *value = new double[len];
        for(int i=0; i<len; i++)
        {
            value[i] = lst[i].toDouble();
        }
        int ret = SetDoubleVariable(index, len, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetBoolVariable Failed");
            return;
        }
        delete [] value;
    }
}

void RoboxComWidget::on_setJoint_btn_clicked()
{
    int index = ui->setJointIndex_edit->text().toInt();
    QString text = ui->setJoint_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    RobotJoint value;
    if(len != 12)
    {
        QMessageBox::information(this, "warning", "Input Error");
        return;
    }
    else
    {
        for(int i=0; i<len; i++)
        {
            value.j[i] = lst[i].toInt();
        }
        int ret = SetJointVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warning", "SetJointVariable Failed");
        }
    }
    return;
}

void RoboxComWidget::on_setPos_btn_clicked()
{
    int index = ui->setPosIndex_edit->text().toInt();
    QString text = ui->setPos_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    // RobotPos value;
    if(len != 16)
    {
        QMessageBox::information(this, "warning", "SetPosVariable Failed");
        return;
    }
    else
    {
        value.x = lst[0].toDouble();
        value.y = lst[1].toDouble();
        value.z = lst[2].toDouble();
        value.a = lst[3].toDouble();
        value.b = lst[4].toDouble();
        value.c = lst[5].toDouble();
        value.ej1 = lst[6].toDouble();
        value.ej2 = lst[7].toDouble();
        value.ej3 = lst[8].toDouble();
        value.ej4 = lst[9].toDouble();
        value.ej5 = lst[10].toDouble();
        value.ej6 = lst[11].toDouble();
        value.cfgx = lst[12].toInt();
        value.cfg1 = lst[13].toInt();
        value.cfg4 = lst[14].toInt();
        value.cfg6 = lst[15].toInt();

        int ret = SetPosVariable(index, value, m_deviceId);
        if(ret != ERROR_OK)
        {
            QMessageBox::information(this, "warinng", "SetPosVariable Failed");
        }
    }
    return;
}

void RoboxComWidget::on_getpointj_btn_clicked()
{
    int index = ui->pointjIndex_edit->text().toInt();
    PointJ value;
    int ret = GetPointJVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetPointJVariable Failed");
        return;
    }
    ui->pointjList->clear();
    ui->pointjList->addItem("J1:"+QString::number(value.j1)+
                            "; J2:"+QString::number(value.j2)+
                            "; J3:"+QString::number(value.j3)+
                            "; J4:"+QString::number(value.j4)+
                            "; J5:"+QString::number(value.j5)+
                            "; J6:"+QString::number(value.j6));
    return;
}

void RoboxComWidget::on_getpointc_btn_clicked()
{
    int index = ui->pointcIndex_edit->text().toInt();
    PointC value;
    int ret = GetPointCVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetPointCVariable Failed");
        return;
    }
    ui->pointcList->clear();
    ui->pointcList->addItem("X:"+QString::number(value.x)+
                            "; Y:"+QString::number(value.y)+
                            "; Z:"+QString::number(value.z)+
                            "; A:"+QString::number(value.a)+
                            "; B:"+QString::number(value.b)+
                            "; C:"+QString::number(value.c)+
                            "; cfgx:"+QString::number(value.cfgx)+
                            "; cfg1:"+QString::number(value.cfg1)+
                            "; cfg4:"+QString::number(value.cfg4)+
                            "; cfg6:"+QString::number(value.cfg6));
    return;
}

void RoboxComWidget::on_setPointJ_btn_clicked()
{
    int index = ui->setPointJIndex_edit->text().toInt();
    PointJ value;
    QString text = ui->setPointJ_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    if(len != 6)
    {
        QMessageBox::information(this, "warning", "SetPointJVariable Failed");
        return;
    }
    else
    {
        value.j1 = lst[0].toDouble();
        value.j2 = lst[1].toDouble();
        value.j3 = lst[2].toDouble();
        value.j4 = lst[3].toDouble();
        value.j5 = lst[4].toDouble();
        value.j6 = lst[5].toDouble();
    }
    int ret = SetPointJVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "SetPointJVariable Failed");
    }
    return;
}

void RoboxComWidget::on_setPointC_btn_clicked()
{
    int index = ui->setPointCIndex_edit->text().toInt();
    PointC value;
    QString text = ui->setPointC_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    if(len != 10)
    {
        QMessageBox::information(this, "warning", "SetPointCVariable Failed");
        return;
    }
    else
    {
        value.x = lst[0].toDouble();
        value.y = lst[1].toDouble();
        value.z = lst[2].toDouble();
        value.a = lst[3].toDouble();
        value.b = lst[4].toDouble();
        value.c = lst[5].toDouble();
        value.cfgx = lst[6].toInt();
        value.cfg1 = lst[7].toInt();
        value.cfg4 = lst[8].toInt();
        value.cfg6 = lst[9].toInt();
    }
    int ret = SetPointCVariable(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "SetPointCVariable Failed");
    }
    return;
}

void RoboxComWidget::on_readEPointJ_btn_clicked()
{
    int index = ui->epointjIndex_edit->text().toInt();
    RobotJoint value;
    int ret = ReadEPointJ(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ReadEPointJ Failed");
        return;
    }
    ui->epointjList->clear();
    ui->epointjList->addItem("j1:"+QString::number(value.j[0])+
            "; j2:"+QString::number(value.j[1])+
            "; j3:"+QString::number(value.j[2])+
            "; j4:"+QString::number(value.j[3])+
            "; j5:"+QString::number(value.j[4])+
            "; j6:"+QString::number(value.j[5])+
            "; ej1:"+QString::number(value.j[6])+
            "; ej2:"+QString::number(value.j[7])+
            "; ej3:"+QString::number(value.j[8])+
            "; ej4:"+QString::number(value.j[9])+
            "; ej5:"+QString::number(value.j[10])+
            "; ej6:"+QString::number(value.j[11]));
    return;
}

void RoboxComWidget::on_readEPointC_btn_clicked()
{
    int index = ui->epointcIndex_edit->text().toInt();
    RobotPos value;
    int ret = ReadEPointC(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ReadEPointC Failed");
        return;
    }
    ui->epointcList->clear();
    ui->epointcList->addItem("X:"+QString::number(value.x)+
            "; Y:"+QString::number(value.y)+
            "; Z:"+QString::number(value.z)+
            "; A:"+QString::number(value.a)+
            "; B:"+QString::number(value.b)+
            "; C:"+QString::number(value.c)+
            "; ej1:"+QString::number(value.ej1)+
            "; ej2:"+QString::number(value.ej2)+
            "; ej3:"+QString::number(value.ej3)+
            "; ej4:"+QString::number(value.ej4)+
            "; ej5:"+QString::number(value.ej5)+
            "; ej6:"+QString::number(value.ej6)+
            "; cfgx:"+QString::number(value.cfgx)+
            "; cfg1:"+QString::number(value.cfg1)+
            "; cfg4:"+QString::number(value.cfg4)+
            "; cfg6:"+QString::number(value.cfg6));
    return;
}

void RoboxComWidget::on_writeEpointj_btn_clicked()
{
    int index = ui->writeEpointjIndex_edit->text().toInt();
    QString text = ui->writeEpointj_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    if(len != 12)
    {
        QMessageBox::information(this, "warning", "Input Error, Please input 12 element");
        return;
    }

    std::string pos = text.toStdString();
    int ret = WriteEPointJ(index, pos, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "WriteEPointJ Failed");
    }
    return;
}

void RoboxComWidget::on_writeEpointc_btn_clicked()
{
    int index = ui->writeEpointcindex_edit->text().toInt();
    QString text = ui->writeEpointc_edit->text();
    QStringList lst = text.split(";");
    int len = lst.count();
    if(len != 13)
    {
        QMessageBox::information(this, "warning", "Input Error, Please input 13 element");
        return;
    }

    std::string pos = text.toStdString();
    int ret = WriteEPointC(index, pos, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "WriteEPointC Failed");
    }
    return;
}

void RoboxComWidget::on_readDIn_btn_clicked()
{
    int index = ui->readInIndex_edit->text().toInt();
    bool value;
    int ret = ReadDIn(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ReadDIn Opeartion Failed");
        return;
    }
    ui->readInValue_edit->clear();
    if(value)
        ui->readInValue_edit->setText("1");
    else
        ui->readInValue_edit->setText("0");
    return;
}

void RoboxComWidget::on_readDOut_btn_clicked()
{
    int index = ui->readOutIndex_edit->text().toInt();
    bool value;
    int ret = ReadDOut(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ReadDOut Operation Failed");
        return;
    }
    ui->readOutValue_edit->clear();
    if(value)
        ui->readOutValue_edit->setText("1");
    else
        ui->readOutValue_edit->setText("0");
    return;
}

void RoboxComWidget::on_writeDOut_btn_clicked()
{
    int index = ui->writeDOutIndex_edit->text().toInt();
    int inputValue = ui->writeDOutValue_edit->text().toInt();
    bool value = false;
    if(inputValue)
        value = true;
    int ret = WriteDOut(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "WriteDOut Operation Failed");
    }
    return;
}

void RoboxComWidget::on_forceDIn_btn_clicked()
{
    int index = ui->forceDInIndex_edit->text().toInt();
    int inputValue = ui->forceDInValue_edit->text().toInt();
    bool value = false;
    if(inputValue)
        value = true;
    int ret = ForceDIn(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ForceDIn Operation Failed");
    }
    return;
}

void RoboxComWidget::on_cancelFDIn_btn_clicked()
{
    int index = ui->forceDInIndex_edit->text().toInt();
    int ret = CancelForceDIn(index, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "CancelForceDIn Opeartion Failed");
    }
    return;
}

void RoboxComWidget::on_forceDOut_btn_clicked()
{
    int index = ui->forceDOutIndex_edit->text().toInt();
    int inputValue = ui->forceDOutValue_edit->text().toInt();
    bool value = false;
    if(inputValue)
        value = true;
    int ret = ForceDOut(index, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ForceDOut Operation Failed");
    }
    return;
}

void RoboxComWidget::on_cancelFDOut_btn_clicked()
{
    int index = ui->forceDOutIndex_edit->text().toInt();
    int ret = CancelForceDOut(index, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "CancelForceDOut Operation Failed");
    }
    return;
}

void RoboxComWidget::on_getTool_btn_clicked()
{
    string name = ui->getTool_combox->currentText().toStdString();
    RobotTool value;
    int ret = GetToolCoordinate(name, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetToolCoordinate Opeartion Failed");
        return;
    }
    ui->lineEdit_ToolX->setText(QString::number(value.x));
    ui->lineEdit_ToolY->setText(QString::number(value.y));
    ui->lineEdit_ToolZ->setText(QString::number(value.z));
    ui->lineEdit_ToolA->setText(QString::number(value.a));
    ui->lineEdit_ToolB->setText(QString::number(value.b));
    ui->lineEdit_ToolC->setText(QString::number(value.c));
    if(value.fixed)
    {
        ui->lineEdit_ToolFixed->setText("1");
    }
    else
    {
        ui->lineEdit_ToolFixed->setText("0");
    }
    return;
}

void RoboxComWidget::on_getWobj_btn_clicked()
{
    string name = ui->getWobj_combox->currentText().toStdString();
    RobotWorkpiece value;
    int ret = GetUserCoordinate(name, value, m_deviceId);
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "GetUserCoordinate Operation Failed");
        return;
    }
    ui->lineEdit_WobjX->setText(QString::number(value.x));
    ui->lineEdit_WobjY->setText(QString::number(value.y));
    ui->lineEdit_WobjZ->setText(QString::number(value.z));
    ui->lineEdit_WobjA->setText(QString::number(value.a));
    ui->lineEdit_WobjB->setText(QString::number(value.b));
    ui->lineEdit_WobjC->setText(QString::number(value.c));
    if(value.onrobot)
    {
        ui->lineEdit_WobjOnRobot->setText("1");
    }
    else
    {
        ui->lineEdit_WobjOnRobot->setText("0");
    }
    return;
}

void RoboxComWidget::on_ClrAlmBtn_clicked()
{
    int ret = ClearAlarm();
    if(ret != ERROR_OK)
    {
        QMessageBox::information(this, "warning", "ClearAlarm Opeartion Failed");
    }
    return;
}

void RoboxComWidget::on_setTool_btn_clicked()
{
    RobotTool tool;
    tool.x = ui->lineEdit_ToolX->text().toDouble();
    tool.y = ui->lineEdit_ToolY->text().toDouble();
    tool.z = ui->lineEdit_ToolZ->text().toDouble();
    tool.a = ui->lineEdit_ToolA->text().toDouble();
    tool.b = ui->lineEdit_ToolB->text().toDouble();
    tool.c = ui->lineEdit_ToolC->text().toDouble();
    tool.fixed = (bool) ui->lineEdit_ToolFixed->text().toInt();
    std::string toolName = ui->getTool_combox->currentText().toStdString();
    int res = SetToolCoordinate(toolName, tool, m_deviceId);
    if (res != ERROR_OK)
    {
        QString msg = QString("SetToolCoordinate error, error code = %1 ").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_setWobj_btn_clicked()
{
    RobotWorkpiece wobj;
    wobj.x = ui->lineEdit_WobjX->text().toDouble();
    wobj.y = ui->lineEdit_WobjY->text().toDouble();
    wobj.z = ui->lineEdit_WobjZ->text().toDouble();
    wobj.a = ui->lineEdit_WobjA->text().toDouble();
    wobj.b = ui->lineEdit_WobjB->text().toDouble();
    wobj.c = ui->lineEdit_WobjC->text().toDouble();
    wobj.onrobot = (bool)ui->lineEdit_WobjOnRobot->text().toInt();

    std::string wobjName = ui->getWobj_combox->currentText().toStdString();
    int res = SetUserCoordinate(wobjName, wobj, m_deviceId);
    if (res != ERROR_OK)
    {
        QString msg = QString("SetUserCoordinate error. error code = %1").arg(res);
        QMessageBox::information(this, "waring", msg);
    }
}

void RoboxComWidget::on_RMJointButton_clicked()
{
    double pos[6];
    pos[0] = ui->rmjLineEdit->text().toDouble();
    pos[1] = ui->rmjLineEdit_2->text().toDouble();
    pos[2] = ui->rmjLineEdit_3->text().toDouble();
    pos[3] = ui->rmjLineEdit_4->text().toDouble();
    pos[4] = ui->rmjLineEdit_5->text().toDouble();
    pos[5] = ui->rmjLineEdit_6->text().toDouble();
    int speed = ui->rmjspeedLineEdit->text().toDouble();

    int res = RMJOINT(pos, speed, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("RMJOINT error. error code = %1").arg(res);
        QMessageBox::information(this, "waring", msg);
    }
}

void RoboxComWidget::on_RMLINButton_clicked()
{
    double pos[6];
    pos[0] = ui->rmlxLineEdit->text().toDouble();
    pos[1] = ui->rmlyLineEdit->text().toDouble();
    pos[2] = ui->rmlzLineEdit->text().toDouble();
    pos[3] = ui->rmlaLineEdit->text().toDouble();
    pos[4] = ui->rmlbLineEdit->text().toDouble();
    pos[5] = ui->rmlcLineEdit->text().toDouble();
    int speed = ui->rmlspeedLineEdit->text().toDouble();

    int res = RMLIN(pos, speed, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("RMLIN error. error code = %1").arg(res);
        QMessageBox::information(this, "waring", msg);
    }
}

void RoboxComWidget::on_loadFromFile_2_clicked()
{
    if(ui->loadProgramFileName->text().isEmpty())
    {
        QMessageBox::information(this, "warning", "fileName empty!");
        return;
    }

    string targetName = ui->loadProgramFileName->text().toStdString();

    int res = LoadWithStartPro(targetName, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("LoadWithStartPro error. error code = %1").arg(res);
        QMessageBox::information(this, "waring", msg);
    }
}

void RoboxComWidget::on_pushjointque_clicked()
{
    RobotJoint joint;
    joint.j[0] = ui->que_j1->text().toDouble();
    joint.j[1] = ui->que_j2->text().toDouble();
    joint.j[2] = ui->que_j3->text().toDouble();
    joint.j[3] = ui->que_j4->text().toDouble();
    joint.j[4] = ui->que_j5->text().toDouble();
    joint.j[5] = ui->que_j6->text().toDouble();
    joint.j[6] = ui->que_ej1->text().toDouble();
    joint.j[7] = ui->que_ej2->text().toDouble();
    joint.j[8] = ui->que_ej3->text().toDouble();
    joint.j[9] = ui->que_ej4->text().toDouble();
    joint.j[10] = ui->que_ej5->text().toDouble();
    joint.j[11] = ui->que_ej6->text().toDouble();

    int res = PushPointQueue(joint, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("Push Joint Point error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_pushcartque_clicked()
{
    RobotPos pos;
    pos.x = ui->que_j1->text().toDouble();
    pos.y = ui->que_j2->text().toDouble();
    pos.z = ui->que_j3->text().toDouble();
    pos.a = ui->que_j4->text().toDouble();
    pos.b = ui->que_j5->text().toDouble();
    pos.c = ui->que_j6->text().toDouble();
    pos.ej1 = ui->que_ej1->text().toDouble();
    pos.ej2 = ui->que_ej2->text().toDouble();
    pos.ej3 = ui->que_ej3->text().toDouble();
    pos.ej4 = ui->que_ej4->text().toDouble();
    pos.ej5 = ui->que_ej5->text().toDouble();
    pos.ej6 = ui->que_ej6->text().toDouble();
    pos.cfgx = ui->que_cfgx->text().toDouble();
    pos.cfg1 = ui->que_cfg1->text().toDouble();
    pos.cfg4 = ui->que_cfg4->text().toDouble();
    pos.cfg6 = ui->que_cfg6->text().toDouble();

    int res = PushPointQueue(pos, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("Push Cart Point error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_getPermButton_clicked()
{
    int res = GetPermission(m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("Get Permission error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_freePermButton_clicked()
{
    int res = ReleasePermission(m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("Release Permission error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_swModeButton_clicked()
{
    int mode = ui->mode_comboBox->currentIndex();
    int res = 0;
    switch (mode)
    {
    case 0:
        res = SwitchRoboxMode(RoboxKeyMode::ROBOX_MODE_MANUAL, m_deviceId);
        break;
    case 1:
        res = SwitchRoboxMode(RoboxKeyMode::ROBOX_MODE_MANUFAST, m_deviceId);
        break;
    case 2:
        res = SwitchRoboxMode(RoboxKeyMode::ROBOX_MODE_AUTO, m_deviceId);
        break;
    }
    if(res != ERROR_OK)
    {
        QString msg = QString("Switch Mode error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_holdMove_btn_clicked()
{
    int res = MOVEHOLD(m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("Hold move error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_powerOnManual_clicked()
{
    int res = PowerOnManual(m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("PowerOnManual error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_powerOffManual_clicked()
{
    int res = PowerOffManual(m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("PowerOffManual error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
}

void RoboxComWidget::on_sendShell_btn_clicked()
{
    string cmd = ui->cmdLineEdit->text().toStdString();
    vector<string> answer;
    int res;// = ShellCmdSend(cmd, answer, m_deviceId);
    if(res != ERROR_OK)
    {
        QString msg = QString("ShellCmdSend error, error code = %1").arg(res);
        QMessageBox::information(this, "warning", msg);
    }
    else
    {
        // ui->shell_answer->clear();
        // QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(answer));
        // ui->shell_answer->addItem(item);
        // cout << answer << endl;
    }
}
