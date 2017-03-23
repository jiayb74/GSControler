#ifndef MYQTCPSERVER_H
#define MYQTCPSERVER_H

#include <QtNetwork>
#include "MyClient.h"

class MyQTcpServer:public QTcpServer
{
    Q_OBJECT
public:
    MyQTcpServer();
protected:
    void incomingConnection(int socketDescriptor);
public:
    MyClient *client;
};

#endif // MYQTCPSERVER_H
