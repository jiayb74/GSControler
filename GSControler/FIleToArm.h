#ifndef FILETOARM_H
#define FILETOARM_H

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

class FileToArm:public QThread
{
    Q_OBJECT
public:
    FileToArm();
    ~FileToArm();
    void run();
public slots:
    void FileClient();
    void SendFileToArm(void);
    void FreePoint();
private:
    QTcpServer * FileFromPcToArm;
    QTcpSocket * NewFileFromPcToArm;
};

#endif // FILETOARM_H
