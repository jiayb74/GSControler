#include "MyQTcpserver.h"
#include <QThread>
#include <iostream>
#include "MyClient.h"
using namespace std;


MyQTcpServer::MyQTcpServer()
{
}


void MyQTcpServer::incomingConnection(int socketDescriptor)
{
    client = new  MyClient(socketDescriptor);
}


//http://blog.csdn.net/cibiren2011/article/details/7235880
