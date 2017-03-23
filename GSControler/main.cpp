#include <QtGui/QApplication>
#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QMessageBox>
#include <QFrame>
#include <QtDebug>
#include <QTextStream>
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
#include <pthread.h>
#include <QObject>
#include "FIleToArm.h"
#include "HotSwap.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include <QDebug>
#include "PortState.h"
#include "GSControler.h"

bool login_flage = true;        //登录标志位 true需输入密码 false无需输入密码
extern unsigned char InitFreq;  //初始化界面频率
extern unsigned int InitPlus;   //初始化界脉宽
extern bool IsShutdown;         //初始化检测开关状态
MainWindow *GsGuiPower;         //主界面对象
//http://maker.zlgmcu.com/forum.php?mod=viewthread&tid=403657
//http://maker.zlgmcu.com/forum.php?mod=forumdisplay&fid=160
/*
  *函数功能：将程序打印输出重定向
  *输入参数：消息类型、消息内容
  *输出参数：NONE
  *返 回 值：无
*/
void LogOutPut(QtMsgType type, const char* msg)
{
    QString qstrText;
    switch (type)
    {
    case QtDebugMsg:
        break;
    case QtWarningMsg:
        break;
    case QtCriticalMsg:
        break;
    case QtFatalMsg:
        exit(0);
    }

}

/*
  *函数功能：主函数，程序入口
  *输入参数：命令行参数
  *输出参数：NONE
  *返 回 值：成功返回0
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   //qInstallMsgHandler(LogOutPut); //daiceshi
   //cout<<"--main  --"<<QThread::currentThreadId()<<endl;

    //中文支持
    QTextCodec *codec = QTextCodec::codecForName(GLOBAL_CODEC);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    MainWindow GsGui;
    GsGuiPower = &GsGui;
    GsGui.setWindowFlags(Qt::FramelessWindowHint);
    GsGui.show();
    GScontroler gscontroler(&GsGui); 
    GsGui.InitGSUI(InitPlus,InitFreq,IsShutdown);
    return a.exec();
}

