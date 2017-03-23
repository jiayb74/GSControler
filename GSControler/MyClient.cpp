#include "MyClient.h"
#include <iostream>
#include <queue>
#include "GsCmdProcess.h"
using namespace std;

MyClient::MyClient()
{
}

MyClient::MyClient(int _socketDescriptor)
{
    socketDescriptor = _socketDescriptor;
    this->start();

}

void MyClient::run()
{
       TcpSocket = new QTcpSocket;
       if(!TcpSocket->setSocketDescriptor(socketDescriptor)){
           cout<<"error"<<endl;
       }
       connect(TcpSocket,SIGNAL(readyRead()),this,SLOT(DataHandle()),Qt::DirectConnection);
       connect(TcpSocket,SIGNAL(disconnected()),this,SLOT(LinkDisconnect()));
       connect(this,SIGNAL(finished()),this,SLOT(QthreadFinishedHandle()));
       exec();

}

void MyClient::DataHandle()
{
   //cout<<"-----readdata--"<<QThread::currentThreadId()<<endl;
    char buf[20] = {0};
    int n = TcpSocket->read(buf,sizeof(buf));
    int i;
    queue<unsigned char> que;
    for(i=0; i<n; i++){
  //      printf("\n\nrecv: %.2x ",buf[i]);
        que.push(buf[i]);
    }
  //  printf("\n");
    CGsCmdProcess cmd;
    cmd.DataArraveProcess(&que,TcpSocket);

}

void MyClient::LinkDisconnect()
{
    TcpSocket->deleteLater();
    this->quit();   //必须退出时间循环后this->deleteLater才有效，从而析构函数才能执行。

}

void MyClient::QthreadFinishedHandle()
{
       this->deleteLater();
}
