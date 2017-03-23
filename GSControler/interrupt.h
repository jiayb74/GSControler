#ifndef INTERRUPT_H
#define INTERRUPT_H

#include<QThread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

class Key_Interrupt:public QThread
{
     Q_OBJECT
public:
    Key_Interrupt();
    ~Key_Interrupt();
    //打开激光器开光
    void Interrupt_LaserOn();
    //关闭激光器开光
    void Interrupt_LaserOff();
    //激光器开光状态改变
    void Power_Signal(bool status);
signals:
    //激光器开光状态改变信号
    void Power_Switch(bool status);
};


#endif // INTERRUPT_H
