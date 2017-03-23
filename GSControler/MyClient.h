#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QThread>
#include <QtNetwork>

class MyClient: public QThread
{
    Q_OBJECT
public:
    MyClient();
    MyClient(int _socketDescriptor);
    int socketDescriptor;
    virtual void run();
    QTcpSocket * TcpSocket;
public slots:
    void QthreadFinishedHandle();
    void DataHandle();
    void LinkDisconnect();
};

#endif // MYCLIENT_H
