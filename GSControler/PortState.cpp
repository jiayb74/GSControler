#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <iostream>
#include <QTimer>
#include "PortState.h"

using namespace  std;

PortState::PortState()
{
    isDisPower = false;
    discount = 0;
    PreviousValue = '0';
    SwitchFlag = false;
    fd = open(DEV_PATH,O_RDWR);
    if(fd < 0){
      //  perror(DEV_PATH);
    }
    lseek(fd,0,SEEK_SET);

    led_fd = open(LED_DEV_PATH,O_RDWR);
    if(led_fd < 0){
      //  perror(DEV_PATH);
    }
    lseek(led_fd,0,SEEK_SET);
    Change_Led_state(0);

    InitP24V();

    this->start();
}

void PortState::InitP24V()
{
    P24VPreviousValue = '0';
   // SwitchFlag = false;
    P24V_fd = open(POWERSTATUS,O_RDWR);
    if(P24V_fd < 0){
      //  perror(POWERSTATUS);
    }
    lseek(P24V_fd,0,SEEK_SET);

    Loop_fd = open(LOOPUP,O_RDWR);
    if(Loop_fd < 0){
      //  perror(LOOPUP);
    }
    lseek(Loop_fd,0,SEEK_SET);
    //noe need protect
    Protect(true);

}

void PortState::run()
{
    QTimer *timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(cheakport()),Qt::DirectConnection);
    timer->start(200);  //200MS
    QTimer *timer1 = new QTimer();
    connect(timer1,SIGNAL(timeout()),this,SLOT(cheakP24port()),Qt::DirectConnection);
    timer1->start(300);  //200MS
    QTimer *timer2 = new QTimer();
    connect(timer2,SIGNAL(timeout()),this,SLOT(DisPower()),Qt::DirectConnection);
    timer2->start(1000);  //1S
    exec();
}
void PortState::cheakport()
{
   // cout<<"-----PortState--"<<QThread::currentThreadId()<<endl;
    lseek(fd,0,SEEK_SET);
    read(fd,&value,1);
  //  cout<<"-------------------- "<<value<<endl;
    if(value != PreviousValue){
        //这里无需再加消除抖了，因为意义不大
        //只要进入if循环里，说明电平肯定有跳变
        //因为200MS检测一次，那么电平跳变有可能发生在第100MS时
        //那么当200MS到时，电平已稳定100MS了，相当与消抖100MS了。
        //最坏的情况就是快到200ms时，如果操作发送抖动，可能电平不变(不满足，因为
        //跳变总要想发送变化，那么在下一个200MS再判断肯定就行啦。响应不急时)或跳变（满足要求）。
           if(value == '0'){
   //             cout<<"guanji"<<endl;
                emit SwitchState(false);
           }
           else{
    //            cout<<"kaiji"<<endl;
                emit SwitchState(true);
           }
       }
    PreviousValue = value;
}

void PortState::cheakP24port()
{
   // cout<<"-----PortState--"<<QThread::currentThreadId()<<endl;
    lseek(P24V_fd,0,SEEK_SET);
    read(P24V_fd,&p24value,1);
  //  cout<<"-------------------- "<<value<<endl;
    if(p24value != P24VPreviousValue){
        //这里无需再加消除抖了，因为意义不大
        //只要进入if循环里，说明电平肯定有跳变
        //因为200MS检测一次，那么电平跳变有可能发生在第100MS时
        //那么当200MS到时，电平已稳定100MS了，相当与消抖100MS了。
        //最坏的情况就是快到200ms时，如果操作发送抖动，可能电平不变(不满足，因为
        //跳变总要想发送变化，那么在下一个200MS再判断肯定就行啦。响应不急时)或跳变（满足要求）。
           if(p24value == '0'){
               // cout<<"diao dian"<<endl;
                isDisPower = true;
               // to do idel shutdown
               emit SwitchState(false);
           }
           else{
              //  cout<<"kaiji"<<endl;
              //  emit SwitchState(true);
           }
       }
    P24VPreviousValue = p24value;
}

void PortState::DisPower()
{
    if(isDisPower){
        discount++;
        //cout<<"wait---"<<endl;
        if(discount >=5){
         //   cout<<"guanji---"<<endl;
            Protect(false);
            discount = 0;
            isDisPower = false;
        }
    }
}

void PortState::Change_Led_state(bool state)
{
    lseek(led_fd,0,SEEK_SET);
    if(state == true){
        lebvalue = '1';
         write(led_fd,&lebvalue,1);

    }
    else{

        lebvalue = '0';
         write(led_fd,&lebvalue,1);
    }


}

void PortState::Protect(bool state)
{
    lseek(Loop_fd,0,SEEK_SET);
    if(state == true){
        loopvalue = '1';
         write(Loop_fd,&loopvalue,1);
    }
    else{

        loopvalue = '0';
         write(Loop_fd,&loopvalue,1);
    }


}


unsigned char PortState::GetSwitchStatus()
{
    lseek(fd,0,SEEK_SET);
    read(fd,&value,1);
 //   cout<<"value:"<<value<<endl;
    return value;
}
PortState::~PortState()
{
    close(fd);
    close(led_fd);
}
