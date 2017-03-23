#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<linux/watchdog.h>
#include<pthread.h>
#include "watchdog.h"

extern bool ServerThreadStatus;
extern bool SwitchPthreadStatus;

using namespace std;

WatchDog::WatchDog(int _timeout)
{
    this->timeout = _timeout;
    this->fd = open("/dev/watchdog",O_RDWR);
    if(this->fd < 0)
    {
        perror("open watch");
        exit(0);
    }
    rc = ioctl(this->fd,WDIOC_SETTIMEOUT,&timeout);
    if(rc == -1){
        perror("watchdog timeout");
        exit(0);
    }

}

int WatchDog::Feed_WatchDog()
{
    rc = ioctl(fd,WDIOC_KEEPALIVE,0);
 // printf("rc = %d\n",rc);
}

int WatchDog::Dis_WatchDog()
{
    if (write(this->fd, "V", 1) != 1) {
//         printf("write WDT_OK_TO_CLOSE failed!");
    }
   int opts = WDIOS_DISABLECARD;
   rc = ioctl(this->fd,WDIOC_SETOPTIONS,&opts);
   if(rc == -1){
       perror("watchdog disable");
       exit(0);
   }

   close(this->fd);
}

void WatchDog::run()
{
    emit SystemLog("Start WatchDog......");
    while(1){
   //   cout<<QThread::idealThreadCount () <<endl;
        sleep(this->timeout-1);
        if(ServerThreadStatus && SwitchPthreadStatus){
            Feed_WatchDog();
            ServerThreadStatus = false;
            SwitchPthreadStatus = false;
        }
        else{
            emit SystemLog("Program exception, start watchdog and restart system\n");
            system("sync");
        }
    }
}

WatchDog::~WatchDog()
{
      Dis_WatchDog();

}




