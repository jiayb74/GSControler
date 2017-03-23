#include <stdio.h>
#include <queue>
#include <iostream>
#include <string.h>
#include "UpdateProgram.h"
#include "LDKeeper.h"

using namespace std;

UpdateProgram::UpdateProgram(int Module)
{
    ID = Module;
    run();  //not start pthread

}

void UpdateProgram::run()
{
    LDKeeper LD;
    LD.SetID(ID);
    //cout<<"ID:"<<LD.GetID()<<endl;
    //send update program cmd
    LD.SendUpdateDate(0xfe,0x23,0x54,0xcd,0x93,0x8d,0x43,0x00);
    //wait erease
    sleep(3);
    unsigned char Date[6] = {0};
    int j = 0;
    unsigned char buf[1024*60] = {0}; //60k
    short MemAddress = 0x0000;
    short MemMaxAddress = 0xb000;
    char filepath[20] = {0};
    sprintf(filepath,"%s%d%s","./doc/",ID,".bin");
    int fd = open(filepath,O_RDONLY);
    if(fd == -1){
        perror("Update:");
        return;
    }
    int ReadBytes = read(fd,buf,sizeof(buf));
    //cout<<"readbytes:"<<ReadBytes<<endl;

    for(int i=0; i< ReadBytes; i++){
        usleep(1);
        Date[j] = buf[i];
        j++;
        if(j == 6){
            LD.SendUpdateDate((MemAddress>>8)&0xff,MemAddress&0xff,Date[0],Date[1],Date[2],Date[3],Date[4],Date[5]);
            MemAddress = MemAddress + 6;
            memset(Date,0,sizeof(Date));
            j = 0;
        }
    }
    for(; j<6; j++){
        Date[j] = 0xFF;
        if(j == 5){
             MemAddress = MemAddress + 6;
             LD.SendUpdateDate((MemAddress>>8)&0xff,MemAddress&0xff,Date[0],Date[1],Date[2],Date[3],Date[4],Date[5]);
        }
    }
    //update success
    LD.SendUpdateDate(0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
}

UpdateProgram::~UpdateProgram()
{

}
