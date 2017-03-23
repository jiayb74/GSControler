#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
#include "CanDateAnalysis.h"
#include "iostream"
#include "Status.h"
#include "LDKeeper.h"
#include "CanDateAnalysis.h"
#include "SoftSwitch.h"
#include "RegTable.h"
#include "board.h"

/*
Board::Board(boardtype type,int id)
{
    BType = type;
    switch(BType)
    {
    case (ThreeInOne):  LDNumber = 2;
         LD = new LDKeeper[LDNumber];
         for(int i=0; i<LDNumber;i++){
             LD[i]->SetID(id);
             LD[i].SetLDRowColumn(id);

         }
        break;
    case (SeedSource):  LDNumber = 1; break;
    case (BaseBoard) :  LDNumber = 1; break;
    }
}
*/
/*
  *
  *
  *
  *
 */

int Board::BoardNum = 0;
int Board::CurrentBoardIndex = 0;

Board::Board(int ldnumber,int id)
{
    CAN_ID = id;
    LDNumber = ldnumber;
    LD = new LDKeeper[LDNumber];
    for(int i=0; i<LDNumber;i++,id = id+8){
         LD[i].SetID(id);
         LD[i].SetLDRowColumn(id);
     }
}

Board::Board()
{
    CAN_ID = 0;
    LD = NULL;
    ExistLD = 0;
    SetIDNum = 0;
    LDNumber = 1;
    CurrentLDIndex = 0;
    BoxAirTemp  = 0;
    BoxPadTemp = 0;
    InitBoard(1,0);
}

void Board::InitBoard(int ldnumber,int id)
{
    CAN_ID = id;
    LDNumber = ldnumber;
    if(LD != NULL)
        delete []LD;
    LD = new LDKeeper[LDNumber];
    for(int i=0; i<LDNumber;i++,id = id+8){
         LD[i].SetID(id);
         LD[i].SetLDRowColumn(id);
     }
}

int Board::AddLD(LDKeeper  ld)
{
    if(ExistLD >=LDNumber){
        return -1;
    }
    ld.SetID(LD[ExistLD].GetID());
    ld.SetLevel(LD[ExistLD].GetLevel());
    ld.SetOutput(LD[ExistLD].GetOutput());
    LD[ExistLD] = ld;     //copy function
    ExistLD++;
    return 0;
}

bool Board::IsOverload()
{
    if(LDNumber<=ExistLD)
        return true;
    else
        return false;
}

void Board::SetBoardCanID(int id)
{
    CAN_ID = id;
    for(int i=0; i<LDNumber;i++){
        if(i!=0)
            id = 0;
        LD[i].SetID(CAN_ID);
        LD[i].SetLDRowColumn(i*8+id);

    }
    SetIDNum++;
}

int Board::IsSetID(void)
{
    return CAN_ID;
}

int Board::GetBoardID()
{
    return CAN_ID;
}
void Board::SetBoardNum(int num)
{
    BoardNum = num;
}


int Board::GetBoardNum()
{
    return BoardNum;
}

void Board::OutInfo()
{
    printf("ID:%d   LDnum:%d    ",CAN_ID,LDNumber);
    for(int i=0; i<LDNumber;i++){
        printf("level:%d    output:%d   curr:%.2f   temp:%.2f   index:%d", LD[i].GetLevel(), LD[i].GetOutput(),LD[i].GetAimCurr(),LD[i].GetAimTemp(),LD[i].index);
    }
    printf("\n");
}

void Board::SetLDNumber(int num)
{
    LDNumber = num;
    if(LD != NULL)
        delete []LD;
    LD = new LDKeeper[LDNumber];
}
int Board::GetLDNumber()
{
    return LDNumber;
}
void Board::OffCurrent()
{
//    for(int i=LDNumber-1;i>=0;i--){
//        LD[i].SwitchSet(OFF);
//        LD[i].SetCurrByReg(0);
//    }
    LD[LDNumber-1].SwitchSet(OFF);
    LD[LDNumber-1].SetCurrByReg(0);
    CurrentLDIndex = 0;
}

void Board::OffAllCurrent()
{
    for(int i=LDNumber-1;i>=0;i--){
        LD[i].SwitchSet(OFF);
        LD[i].SetCurrByReg(0);
    }
    CurrentLDIndex = 0;
}

void Board::OffCareCurrent()
{
    for(int i=LDNumber-1;i>=0;i--){
        LD[i].SetCurrByReg(0);
    }
    CurrentLDIndex = 0;

}
void Board::SetAirTemp(float temp)
{
    BoxAirTemp = temp;
}

float Board::GetAirTemp()
{
    return BoxAirTemp;
}

void Board::SetPadTemp(float temp)
{
    BoxPadTemp = temp;
}

float Board::GetPadTemp()
{
    return BoxPadTemp;
}

void Board::InitBoardTemp()
{
    for(int i=0; i<LDNumber; i++){
        LD[i].SetTempByReg(LD[i].GetAimTemp());
    }
}






