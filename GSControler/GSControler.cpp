#include <QtGui/QApplication>
#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QMessageBox>
#include <QFrame>
#include <pthread.h>
#include <QObject>
#include <QDebug>
#include <iostream>
#include "mainwindow.h"
#include "keyboard.h"
#include "tcpip.h"
#include "this_config.h"
#include "CanDateAnalysis.h"
#include "SoftSwitch.h"
#include "interrupt.h"
#include "server.h"
#include "watchdog.h"
#include "SoftSwitch.h"
#include "FIleToArm.h"
#include "HotSwap.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "PortState.h"
#include "GSControler.h"
#include "server.h"
#include "hanscmd.h"
#include "Scanning.h"
#include "SystemLog.h"
#include "Module.h"
#include "customthread.h"
#include "OpticalPower.h"

extern CSoftSwitch * cs;
SCANNING * scantemp ;

GScontroler::GScontroler(MainWindow *  _GsGui)
{
    this->GsGui = _GsGui;
    InitInstance();
}

void GScontroler::InitInstance()
{
    PdHandleObject = new PdHandleThread;//光功率
    //激光器状态机
    GsSoftswitch = new CSoftSwitch();
    cs = GsSoftswitch;
    //系统日志功能
    GS_SystemLog = new SystemLog;
    QObject::connect(GsSoftswitch,
                     SIGNAL(SystemLog(string)),
                     GS_SystemLog,
                     SLOT(WriteSystemLog(string)));
    GS_SystemLog->WriteSystemLog("Start GSControler ");
    GsSoftswitch->InitInstance();
    GsSoftswitch->start();

    //GsSoftswitch->GsGui = this->GsGui;
    //进度条更新
    QObject::connect(GsSoftswitch,SIGNAL(updatabar(const QString, const char)),GsGui,SLOT(UpDataBar(const QString, const char )));
    //进度条都显示
    QObject::connect(GsSoftswitch,SIGNAL(Set_ProgressBar(char)),GsGui,SLOT(Set_ProgressBar(char)));
    QObject::connect(GsSoftswitch,SIGNAL(UI_Laser_State(char)),GsGui,SLOT(UI_Laser_State(char)));
    QObject::connect(GsSoftswitch,SIGNAL(UI_ShowPower(float,float)),GsGui,SLOT(UI_ShowPower(float,float)));
    QObject::connect(GsSoftswitch,
                     SIGNAL(UpdataPlusFreqPower(float,float)),
                     this->GsGui,
                     SLOT(UpdataPlusFreqPower(float,float)));
    Scanning = new SCANNING;
    scantemp = Scanning;
    QObject::connect(this->GsGui->EngineerMode->MotorScanner,
                     SIGNAL(On_Setpos_Signal(unsigned short,unsigned short)),
                     Scanning,
                     SLOT(On_Setpos_Slot(unsigned short,unsigned short)));
    QObject::connect(this->GsGui->EngineerMode->MotorScanner,
                     SIGNAL(On_Reset_Signal()),
                     Scanning,
                     SLOT(On_Reset_Slot()));
    QObject::connect(this->GsGui->EngineerMode->MotorScanner,
                     SIGNAL(On_Scan_Signal(unsigned short ,unsigned short ,
                                           unsigned short ,unsigned short ,
                                           unsigned short,unsigned char )),
                     Scanning,
                     SLOT(On_Scan_Slot(unsigned short ,unsigned short ,
                                       unsigned short ,unsigned short ,unsigned short,
                                       unsigned char)));
    QObject::connect(this->GsGui->EngineerMode->MotorScanner,
                     SIGNAL(On_Getpos_Signal()),
                     Scanning,
                     SLOT(On_Getpos_Slot()));
    QObject::connect(Scanning,
                     SIGNAL(Curr_Pos(unsigned short , unsigned short )),
                     this->GsGui->EngineerMode->MotorScanner,
                     SLOT(curr_pos(unsigned short , unsigned short )));
    QObject::connect(this->GsGui->EngineerMode->MotorScanner,
                     SIGNAL(On_Abort_Signal()),
                     Scanning,
                     SLOT(On_Abort_Slot()));
    QObject::connect(Scanning,
                     SIGNAL(On_ScanPos_Signels(unsigned short ,unsigned short ,
                                               unsigned short,unsigned short ,unsigned char)),
                     this->GsGui->EngineerMode->MotorScanner,
                     SLOT(On_ScanPos_Slot(unsigned short ,unsigned short ,
                                          unsigned short,unsigned short ,unsigned char )));

    QObject::connect(this->GsGui->customscanner,
                     SIGNAL(StartCusScan()),
                     Scanning,
                     SLOT(StartCusScan()));
    QObject::connect(Scanning,
                     SIGNAL(UpDateUi(unsigned int ,unsigned int)),
                     this->GsGui->customscanner,
                     SLOT(UpDateUi(unsigned int ,unsigned int)));
    QObject::connect(Scanning,
                     SIGNAL(UpDataBar(float)),
                     this->GsGui->customscanner,
                     SLOT(UpDataBar(float)));
    QObject::connect(this->GsGui->customscanner,
                     SIGNAL(ChangeSwitchState(bool)),
                     GsSoftswitch,
                     SLOT(ChangeSwitchState(bool)));
    QObject::connect(GsSoftswitch,
                     SIGNAL(StartScan()),
                     this->GsGui->customscanner,
                     SLOT(StartScan()));
    QObject::connect(this->GsGui->customscanner,
                     SIGNAL(CancelScan()),
                     Scanning,
                     SLOT(CancelScan()));


 //   watchdog = new WatchDog(5);
 //   QObject::connect(watchdog,
 //                    SIGNAL(SystemLog(string)),
 //                    GS_SystemLog,
 //                    SLOT(WriteSystemLog(string)));
 //   watchdog->start();


    chanscmd = new CHansCMD(GsSoftswitch);
    QObject::connect(chanscmd,
                     SIGNAL(SelfCheck(bool)),
                     this->GsGui->customscanner,
                     SLOT(ScanStyle(bool)));
    QObject::connect(GsSoftswitch->hotSwap,
                     SIGNAL(UpdataPlusFreqPower(float,float)),
                     this->GsGui,
                     SLOT(UpdataPlusFreqPower(float,float)));
    QObject::connect(GsSoftswitch,
                     SIGNAL(UartMolock(bool)),
                     chanscmd,
                     SLOT(ModeChanged(bool)));

    //建立服务器
    GsServer = new Server;
    CustomThread * cs = new CustomThread();
}

GScontroler::~GScontroler()
{
    delete GsSoftswitch;
    delete GsServer;
    delete watchdog;
    delete chanscmd;
    delete PdHandleObject;
}
