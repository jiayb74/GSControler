#ifndef _WATCHDOG_H__
#define _WATCHDOG_H__

#include<QThread>
#include<QObject>
#include<string>
#include<iostream>

#include <QThread>
#include "CanDateAnalysis.h"
#include "LDKeeper.h"
#include "HotSwap.h"
#include <map>
#include "PortState.h"
using namespace std;

class WatchDog:public QThread
{
    Q_OBJECT
public:
    WatchDog(int _timeout);
    ~WatchDog();
    int Feed_WatchDog();
    int Dis_WatchDog();
    void run();
    int fd;
    int rc;
    int timeout;
signals:
    void SystemLog(string info);
};

#endif
