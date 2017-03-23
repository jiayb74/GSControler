#ifndef _SERVER_H__
#define _SERVER_H__

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
#include "customthread.h"
#include "MyQTcpserver.h"

using namespace std;

class Server:public QThread
{
    Q_OBJECT
public:
	void Close();
    Server();
	~Server();
    void run();
public slots:
    void timerUpDate() ;
private:
    MyQTcpServer * TcpServer;
    FileToArm * arm;
    CustomThread * CustomObject;
    int online;


};





#endif
