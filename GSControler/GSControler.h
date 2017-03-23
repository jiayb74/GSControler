#ifndef GSCONTROLER_H
#define GSCONTROLER_H

#include <QtGui/QApplication>
#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QMessageBox>
#include <QFrame>
#include "SoftSwitch.h"
#include "server.h"
#include "hanscmd.h"
#include "watchdog.h"
#include "mainwindow.h"
#include "Scanning.h"
#include "SystemLog.h"
#include "PdHandleThread.h"

class GScontroler
{
public:
    GScontroler(MainWindow *  _GsGui);
    ~GScontroler();
    void InitInstance();
private:
    MainWindow  *  GsGui;
    CSoftSwitch *  GsSoftswitch;
    Server      *  GsServer;
    CHansCMD    *  chanscmd;
    WatchDog    *  watchdog;
    SCANNING    *  Scanning;
    SystemLog   *  GS_SystemLog;
    PdHandleThread * PdHandleObject;

};


#endif // GSCONTROLER_H
