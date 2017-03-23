#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <QTimer>
#include "FIleToArm.h"
#include "server.h"
#include "GsCmdProcess.h"
#include "customthread.h"

bool ServerThreadStatus;


Server::Server()
{
    online = 0;
   // CustomObject = new CustomThread[10];
    ServerThreadStatus = true;
    //上位机与ARM消息传输线程
    start();
    //上位机向ARM传送文件线程
    arm = new FileToArm;
}

Server::~Server()
{

}


void Server::run()
{
      //cout<<"---------------------serever--"<<QThread::currentThreadId()<<endl;
      TcpServer = new MyQTcpServer();
      if(!TcpServer->listen(QHostAddress("192.168.47.100"),4755))
       {

       }  
       QTimer *timer = new QTimer();
       connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
       timer->start(1500);  //1.5S
       exec();
}

void Server::timerUpDate()
{
  //  cout<<"server timeout..."<<endl;
    ServerThreadStatus = true;

}

void Server::Close()
{

}



