#ifndef PDHANDLETHREAD_H
#define PDHANDLETHREAD_H

#include <QThread>
#include <QtNetwork>

class PdHandleThread:public QThread
{
    Q_OBJECT
public:
    PdHandleThread();
    virtual void run();
    void DataMatching();
    bool NewDataMatching();
    bool IsValid(unsigned char _check[]);
    void AnalyticData(unsigned char _channeldata[]);
};

#endif // PDHANDLETHREAD_H
