#ifndef FILETOPC_H
#define FILETOPC_H

#include<QThread>
#include<QtNetwork>
#include<QtNetwork/qhostaddress.h>
#include<QtNetwork/QNetworkInterface>
#include<QNetworkInterface>
#include<QHostAddress>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

class FileToPc:public QThread
{
    Q_OBJECT
public:
    FileToPc(unsigned char _dat);
    ~FileToPc();
    void SendFileToPc();
    void run();
public slots:
    void FreePoint();
    void ThreadFinishedClear();
public:
    unsigned char dat;
     QTcpServer * FileFromPcToArm;
     QTcpSocket *client;
};

#endif // FILETOPC_H
