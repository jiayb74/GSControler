#ifndef CUSTOMTHREAD_H
#define CUSTOMTHREAD_H

#include<stdio.h>
#include<cstring>
#include<fcntl.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<iostream>
#include<stdlib.h>
#include<QThread>
#include<QtNetwork>
#include<QtNetwork/qhostaddress.h>
#include<QtNetwork/QNetworkInterface>
#include<QNetworkInterface>
#include<QHostAddress>
#include"GsCmdProcess.h"
#include<queue>
#include "FIleToArm.h"

class CustomThread:public  QThread
{
    Q_OBJECT
public:
    void Close();
    CustomThread();
    ~CustomThread();
    void run();
    int CustomID;
public slots:
    void LinkDisconnect();
public:

signals:



};

#endif // CUSTOMTHREAD_H
