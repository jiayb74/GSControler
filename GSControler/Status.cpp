#include "Status.h"
#include "CanDateAnalysis.h"
#include <stdio.h>
#include <iostream>
#include <QMutex>
#include <QMessageBox>

//对节点状态变量操作时加锁。因为接收can数据线程cantest thread和status线程都用对共享资源操作。
//目前加锁后会出现死锁的情况，至今未解决。
QMutex mutex;

using namespace std;

CStatus::CStatus(LDKeeper *_lpLD, int _LdNum)
{
    this->lpLD = _lpLD;
    this->MoudleNum = _LdNum;
    this->start();
}

CStatus::~CStatus()
{

}

void CStatus::run()
{
    int t = 0;
    while(1)
    {
        sleep(1);     
  //     cout<<"------status--"<<QThread::currentThreadId()<<endl;
        //判断节点状态时不允许其他线程修改变量。
        //mutex.lock();

        for(int i=0 ; i<this->MoudleNum; i++){

            if(lpLD[i].GetLinkStatus() == false){
                //lpLD[i].SetRealCurr(-2); //GsColection面板显示0
                //lpLD[i].SetRealTemp(-2); //GsColection面板显示0
                //printf("-------moudle%d LinkStatus disconnect\n",i);
            }
        }
        for(int i=0; i<MoudleNum; i++){
            lpLD[i].DownLine();
        }
    }
}


