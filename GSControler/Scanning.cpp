#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include "CanDateAnalysis.h"
#include <stdlib.h>
#include <iostream>
#include <QTimer>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "LDKeeper.h"
#include "Status.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "HotSwap.h"
#include "PortState.h"
#include "SoftSwitch.h"
#include "Scanning.h"
#include "RegTable.h"
//ifconfig eth0 25
ofstream out;
using namespace std;
extern LDKeeper * lpLD_temp;

SCANNING::SCANNING()
{
    IsRun = false;
    ScanSource = 0;
    ScanArg = new int[6];
}

void SCANNING::run()
{
    bool flag = true;
    int n = 0;
    unsigned char m = 0;
    bool CusFlag = true;
    unsigned int Num = TotalSch;
    CumulativeStep = 0;
    TotalStep = 0;
    GetScanTotalStep();
   // cout<<"++++++++++++: "<<TotalStep<<endl;
    out.open("./doc/ScanResultData.txt");
	while(IsRun){
		switch(ScanSource)
		{
            case 5: GetScanInfo();
                    ScanSource = 6;
                    break;
            case 6: sleep(1);
                    ReadScanData();
                    if(IsScanComplete()){
                        ReadScanDataEnd();//-------
                        IsRun = false;
                        out.close();
                    }
                    break;
			case 0: sleep(1);
					GetScanPos();
					ReadScanData();
					break;
			case 1:
					emit UpDateUi(Num,1);
					while(TotalSch){
						sleep(1);
						switch(n){
							case 0: if(m == 0){
										if(GetScanConfigFile(n)){
											ScanStyleInfo();
											On_Reset_Slot();									
											m = 1;
										}
										else{
											n++;
										}
									}
									else if(m == 1){
										if(IsResetOk()){
											CusScan();
											m = 2;
										}

									}
									else if(m == 2){
										if(IsScanComplete()){
											ReadScanDataEnd();//-------
											UpDataCumulativeStep(n);
											n++;
											m = 0;
										}
										else{
											ReadScanData();
											GetXorYCurrPos(n);
											UpDateProcessBar();
										}
									}
									break;
							case 1: if(m == 0){
										if(GetScanConfigFile(n)){
											ScanStyleInfo();
											On_Reset_Slot();
											m = 1;
										}
										else{
											n++;
										}
									}
									else if(m == 1){
										if(IsResetOk()){
											m = 2;
											CusScan();
										}

									}
									else if(m == 2){
										if(IsScanComplete()){
											ReadScanDataEnd();
											UpDataCumulativeStep(n);
											n++;
											m = 0;
										}
										else{
											ReadScanData();
											GetXorYCurrPos(n);
											UpDateProcessBar();
										}
									}
									break;
							case 2: if(m == 0){
										if(GetScanConfigFile(n)){
											ScanStyleInfo();
											On_Reset_Slot();
											m = 1;
										}
										else{
											n++;
										}
									}
									else if(m == 1){
										if(IsResetOk()){
											m = 2;
											CusScan();
										}

									}
									else if(m == 2){
										if(IsScanComplete()){
											ReadScanDataEnd();
											UpDataCumulativeStep(n);
											n++;
											m = 0;
										}
										else{
											ReadScanData();
											GetXorYCurrPos(n);
											UpDateProcessBar();
										}
									}

									break;
							case 3:
									if(m == 0){
										if(GetScanConfigFile(n)){
											ScanStyleInfo();
											On_Reset_Slot();
											m = 1;
										}
										else{
											n++;
										}
									}
									else if(m == 1){
										if(IsResetOk()){
											m = 2;
											CusScan();
										}

									}
									else if(m == 2){
										if(IsScanComplete()){
											ReadScanDataEnd();
											UpDataCumulativeStep(n);
											n++;
											m = 0;
										}
										else{
											ReadScanData();
											GetXorYCurrPos(n);
											UpDateProcessBar();
										}
									}

									break;
							default:TotalSch--;
									n = 0;
									CumulativeStep = 0;
									if(TotalSch != 0)
										UpDateUi(Num,Num-TotalSch+1);
									break;
						}
					}
					ScanSource = 3;
					UpDateUi(0,1);
					RecoveryPos();
					break;
			case 3:
					if(IsRecoveryComplete()){
						UpDateUi(1,0);
						IsRun = false;
						out.close();
					}
					break;
		}
	}

}

void SCANNING::On_Setpos_Slot(unsigned short _x_target, unsigned short _y_target)
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Targ,0,_x_target&0xff,(_x_target>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Targ,0,_y_target&0xff,(_y_target>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,0,0x06,0,0,0,NULL);
}

void SCANNING::On_Reset_Slot()
{
     lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,0,0x05,0,0,0,NULL);
}

void SCANNING::On_Scan_Slot(unsigned short _x_start, unsigned short _x_end, unsigned short _y_start, unsigned short _y_end, unsigned short _step, unsigned char ScanStyle)
{
    unsigned short y_step = _step;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_start,0,_x_start&0xff,(_x_start>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_stop,0,_x_end&0xff,(_x_end>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_step,0,_step&0xff,(_step>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_start,0,_y_start&0xff,(_y_start>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_stop,0,_y_end&0xff,(_y_end>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_step,0,y_step&0xff,(y_step>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[1].Read_Set_Reg(0,ScanSTS,0,0x03,0,0,0,NULL);
 //   cout<<_x_start<<" "<<_x_end<<" "<<_y_start<<" "<<_y_end<<" "<<_step<<" "<<ScanStyle<<endl;
  //  printf("%d\n",ScanStyle);
    strcpy(Schematic, ScanStyle == 0x02?"Y2":"Y1");
    *ScanArg = _x_start;
   *(ScanArg+1) = _x_end;
    *(ScanArg+2) = _step;
    *(ScanArg+3) = _y_start;
    *(ScanArg+4) = _y_end;
    *(ScanArg+5) = _step;
  //  printf("%d %d %d %d %d %d\n",*ScanArg,(ScanArg + 1),*(ScanArg + 2),*(ScanArg + 3),*(ScanArg + 4),*(ScanArg + 5));
    ScanStyleInfo();
    this->start();  //here maybe problumer
    IsRun = true;
    ScanSource = 0;

}

void SCANNING::StartWebScan()
{

    IsRun = true;
    ScanSource = 5;
    this->start();  //here maybe problumer
}

void SCANNING::GetScanInfo()
{
    CanDataStruct data;
    unsigned short ScanStyle = 0;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_start,BlockingRead,0,0,0,0,&data);
    *ScanArg = data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_stop,BlockingRead,0,0,0,0,&data);
    *(ScanArg+1) =data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_step,BlockingRead,0,0,0,0,&data);
    *(ScanArg+2) =data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_start,BlockingRead,0,0,0,0,&data);
    *(ScanArg+3) = data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_stop,BlockingRead,0,0,0,0,&data);
    *(ScanArg+4) = data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_step,BlockingRead,0,0,0,0,&data);
    *(ScanArg+5) = data.s_data;

    lpLD_temp[1].Read_Set_Reg(0,ScanSTS,BlockingRead,0,0,0,0,&data);
    ScanStyle = data.s_data;
 //   cout<<_x_start<<" "<<_x_end<<" "<<_y_start<<" "<<_y_end<<" "<<_step<<" "<<ScanStyle<<endl;
  //  printf("%d\n",ScanStyle);
    strcpy(Schematic, ScanStyle == 0x02?"Y2":"Y1");

  //  printf("%d %d %d %d %d %d\n",*ScanArg,(ScanArg + 1),*(ScanArg + 2),*(ScanArg + 3),*(ScanArg + 4),*(ScanArg + 5));
    ScanStyleInfo();

}

void SCANNING::On_Getpos_Slot()
{
    CanDataStruct data;
    unsigned short curr_x;
    unsigned short curr_y;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Pos,BlockingRead,0,0,0,0,&data);
    curr_x = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Pos,BlockingRead,0,0,0,0,&data);
    curr_y = data.s_data;
    emit Curr_Pos(curr_x,curr_y);
}

void SCANNING::GetScanPos()
{
    CanDataStruct data;
    unsigned short curr_x;
    unsigned short curr_y;
    unsigned short tar_x;
    unsigned short tar_y;
    unsigned short scanstatus;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Pos,BlockingRead,0,0,0,0,&data);
    curr_x = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Pos,BlockingRead,0,0,0,0,&data);
    curr_y = data.s_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Targ,BlockingRead,0,0,0,0,&data);
    tar_x = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Targ,BlockingRead,0,0,0,0,&data);
    tar_y = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,ScanSTS,BlockingRead,0,0,0,0,&data);
    scanstatus = data.s_data;
    if(!(scanstatus&0x0004))
        IsRun = false;
    emit On_ScanPos_Signels(curr_x,curr_y,tar_x,tar_y,scanstatus&0x0004);

}


void SCANNING::On_Abort_Slot()
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,0,0x04,0,0,0,NULL);
}

void SCANNING::SetScanSource(unsigned char _ScanSource)
{
    ScanSource = _ScanSource;
}

bool SCANNING::GetScanConfigFile(unsigned char Number)
{
    char m_TotalSch[20];
    char m_Schematic[20];
    char m_Para1Zero[20];
    char m_Para1Omeg[20];
    char m_Para1Step[20];
    char m_Para2Zero[20];
    char m_Para2Omeg[20];
    char m_Para2Step[20];
    sprintf(m_Schematic,"%s%d",GS_INI_SCAN_KEY_SCH,Number);
    sprintf(m_Para1Zero,"%s%d",GS_INI_SCAN_KEY_XSTART,Number);
    sprintf(m_Para1Omeg,"%s%d",GS_INI_SCAN_KEY_XEND,Number);
    sprintf(m_Para1Step,"%s%d",GS_INI_SCAN_KEY_XSTEP,Number);
    sprintf(m_Para2Zero,"%s%d",GS_INI_SCAN_KEY_YSTART,Number);
    sprintf(m_Para2Omeg,"%s%d",GS_INI_SCAN_KEY_YEND,Number);
    sprintf(m_Para2Step,"%s%d",GS_INI_SCAN_KEY_YSTEP,Number);

    GetPrivateProfileString(GS_INI_SCAN_SECTION,m_Schematic,"NO",Schematic,3,GS_INI_SCAN_FILE);

    if(!strcmp(Schematic,"NO"))
        return false;
    *ScanArg = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para1Zero,12,GS_INI_SCAN_FILE);
    *(ScanArg+1) = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para1Omeg,12,GS_INI_SCAN_FILE);
    *(ScanArg+2) = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para1Step,12,GS_INI_SCAN_FILE);
    *(ScanArg+3) = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para2Zero,12,GS_INI_SCAN_FILE);
    *(ScanArg+4) = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para2Omeg,12,GS_INI_SCAN_FILE);
    *(ScanArg+5) = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para2Step,12,GS_INI_SCAN_FILE);
//    printf("%s %d %d %d %d %d %d %d\n",Schematic,TotalSch,*ScanArg,*(ScanArg+1),*(ScanArg+2),*(ScanArg+3),
 //          *(ScanArg+4),*(ScanArg+5));
    if(strcmp(Schematic,"X1"))
        ScanType = 0x03;
    else if(strcmp(Schematic,"X2"))
        ScanType = 0x03;
    else if(strcmp(Schematic,"Y1"))
        ScanType = 0x03;
    else if(strcmp(Schematic,"Y2"))
        ScanType = 0x02;
    return true;
}

void SCANNING::CusScan()
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_start,0,(*ScanArg)&0xff,((*ScanArg)>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_stop,0,(*(ScanArg+1))&0xff,((*(ScanArg+1))>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepX_step,0,(*(ScanArg+2))&0xff,((*(ScanArg+2))>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_start,0,(*(ScanArg+3))&0xff,((*(ScanArg+3))>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_stop,0,(*(ScanArg+4))&0xff,((*(ScanArg+4))>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,SweepY_step,0,(*(ScanArg+5))&0xff,((*(ScanArg+5))>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,ScanSTS,0,ScanType,0,0,0,NULL);

}

bool SCANNING::IsScanComplete()
{
    unsigned short scanstatus ;
    CanDataStruct data;
    lpLD_temp[SeedRank].Read_Set_Reg(0,ScanSTS,BlockingRead,0,0,0,0,&data);
    scanstatus = data.s_data;
    if(!(scanstatus&0x0004))
        return true;
    else
        return false;
}

void SCANNING::StartCusScan()
{
    CanDataStruct data;
    IsRun = true;
    ScanSource = 1;

    TotalSch = GetPrivateProfileInt(GS_INI_SCAN_SECTION,GS_INI_SCAN_KEY_ROUND,1,GS_INI_SCAN_FILE);
    //cout<<"Ini TotalSch:"<<TotalSch<<endl;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Pos,BlockingRead,0,0,0,0,&data);
    SaveCurrXPos = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Pos,BlockingRead,0,0,0,0,&data);
    SaveCurrYPos = data.s_data;
    this->start();
}

void SCANNING::RecoveryPos()
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Targ,0,SaveCurrXPos&0xff,(SaveCurrXPos>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Targ,0,SaveCurrYPos&0xff,(SaveCurrYPos>>8)&0xff,0,0,NULL);
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,0,0x06,0,0,0,NULL);
}

bool SCANNING::IsRecoveryComplete()
{
    CanDataStruct data;
    unsigned short curr_x;
    unsigned short curr_y;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Pos,BlockingRead,0,0,0,0,&data);
    curr_x = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Pos,BlockingRead,0,0,0,0,&data);
    curr_y = data.s_data;
    if((SaveCurrXPos == curr_x) && (SaveCurrYPos == curr_y))
        return true;
    else
        return false;
}

void SCANNING::GetScanTotalStep()
{
      unsigned int step;
      for(int i=0; i<4; i++){
          if(GetScanConfigFile(i)){
              if(strcmp(Schematic,"X1") == 0){
                  step = *(ScanArg+4)-*(ScanArg+3);
              }
              else if(strcmp(Schematic,"X2") == 0){
                  step = *(ScanArg+1)-*(ScanArg);//step = *(ScanArg+4)-*(ScanArg+3);
              }
              else if(strcmp(Schematic,"Y1") == 0){
                  step = *(ScanArg+1)-*ScanArg;
              }
              else if(strcmp(Schematic,"Y2") == 0){
                  step = *(ScanArg+1)-*ScanArg;
              }
              TotalStep = TotalStep + step;
          }

      }
}

void SCANNING::GetXorYCurrPos(unsigned char index)
{
    CanDataStruct data;
    char m_Para1Zero[20];
    char m_Para2Zero[20];
    sprintf(m_Para1Zero,"%s%d",GS_INI_SCAN_KEY_XSTART,index);
    sprintf(m_Para2Zero,"%s%d",GS_INI_SCAN_KEY_YSTART,index);

    char m_Schematic[20];
    sprintf(m_Schematic,"%s%d",GS_INI_SCAN_KEY_SCH,index);
    GetPrivateProfileString(GS_INI_SCAN_SECTION,m_Schematic,"NO",Schematic,3,GS_INI_SCAN_FILE);
    if(strncmp(Schematic,"X",1) == 0){
        lpLD_temp[SeedRank].Read_Set_Reg(0,0x18,BlockingRead,0,0,0,0,&data);
        int s = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para1Zero,12,GS_INI_SCAN_FILE);
        CurrXorYPos = ((data.s_data - s)>0?(data.s_data - s):0);
     }
    else if(strncmp(Schematic,"Y",1) == 0){
        lpLD_temp[SeedRank].Read_Set_Reg(0,0x19,BlockingRead,0,0,0,0,&data);
        int s = GetPrivateProfileInt(GS_INI_SCAN_SECTION,m_Para1Zero,12,GS_INI_SCAN_FILE);
        CurrXorYPos = ((data.s_data - s)>0?(data.s_data - s):0);
     }
}

void SCANNING::UpDateProcessBar()
{
 //   cout<<"---------------:"<<CurrXorYPos<<endl;
    float Percentage = (float)(CumulativeStep + CurrXorYPos)/TotalStep;
    emit UpDataBar(Percentage);
}

void SCANNING::UpDataCumulativeStep(unsigned int n)
{
     CanDataStruct data;
     char m_Schematic[20];
     sprintf(m_Schematic,"%s%d",GS_INI_SCAN_KEY_SCH,n);
     GetPrivateProfileString(GS_INI_SCAN_SECTION,m_Schematic,"NO",Schematic,3,GS_INI_SCAN_FILE);
     if(strncmp(Schematic,"X",1) == 0){
          CumulativeStep = CumulativeStep + *(ScanArg+4)-*(ScanArg+3);
     }
     else if(strncmp(Schematic,"Y",1) == 0){
          CumulativeStep = CumulativeStep + *(ScanArg+1)-*ScanArg;
     }
}

bool SCANNING::IsResetOk()
{
    CanDataStruct data;
    unsigned short curr_x;
    unsigned short curr_y;
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1X_Pos,BlockingRead,0,0,0,0,&data);
    curr_x = data.s_data;
    usleep(1000);
    lpLD_temp[SeedRank].Read_Set_Reg(0,Motor1Y_Pos,BlockingRead,0,0,0,0,&data);
    curr_y = data.s_data;
    if((curr_x == 0) &&(curr_y == 0))
        return true;
    else
        return false;
}

void SCANNING::ReadScanData()
{
    CanDataStruct data;
    unsigned short Xraw;
    unsigned short Yraw;
    unsigned short Xpos;
    unsigned short Ypos;
    string Active;
    string Dir;
    string Mode;

    lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,BlockingRead,0,0,0,0,&data);
    if(((data.s_data&0x40) != 0x40)||(data.f_data == -1)){
        return;
    }
    for(int i=0; i<250; i++){
        lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,BlockingRead,0,0,0,0,&data);
        if((data.s_data&0x20) != 0x20)
            return;

        lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,WriteReg,0x23,0,0,0,NULL);
        usleep(1000);
        lpLD_temp[SeedRank].Read_Set_Reg(0,ModeLockPosX,BlockingRead,0,0,0,0,&data);
        Xraw = data.s_data;
        usleep(1000);
        lpLD_temp[SeedRank].Read_Set_Reg(0,ModeLockPosY,BlockingRead,0,0,0,0,&data);
        Yraw = data.s_data;
        //cout<<"xraw "<<Xraw<<"yraw "<<Yraw<<endl;
        Xpos = Xraw&0x3fff;
        Ypos = Yraw&0x3fff;
        Active = (((Xraw>>15) == 1)?"X":"Y");
        Dir = (((Yraw>>14) == 0x03)?"CW":"CCW");
        Mode = (((Xraw>>14) == 0x00)?"Locked":"Unlckd");
        out<<setw(8)<<Xraw<<setw(8)<<Yraw<<setw(8)<<Xpos<<setw(8)<<Ypos<<
             setw(8)<<Active<<setw(8)<<Dir<<setw(8)<<Mode<<"\r"<<endl;

        ScanPoint.X_Point = Xpos;
        ScanPoint.Y_Point = Ypos;
        ScanPoint.Direction = (((Yraw>>14) == 0x03)?MoveIsUp:MoveIsDown);
        ScanPoint.Islock = !(Xraw>>14);
        ScanPointVector.push_back(ScanPoint);
    }
}

void SCANNING::ReadScanDataEnd()
{
    CanDataStruct data;
    unsigned short Xraw;
    unsigned short Yraw;
    unsigned short Xpos;
    unsigned short Ypos;
    string Active;
    string Dir;
    string Mode;

    for(int i=0; i<250; i++){
        lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,BlockingRead,0,0,0,0,&data);
        if((data.s_data&0x20) != 0x20)
            break;
        lpLD_temp[SeedRank].Read_Set_Reg(0,MotorSTS,WriteReg,0x23,0,0,0,NULL);
        usleep(1000);
        lpLD_temp[SeedRank].Read_Set_Reg(0,ModeLockPosX,BlockingRead,0,0,0,0,&data);
        Xraw = data.s_data;
        usleep(1000);
        lpLD_temp[SeedRank].Read_Set_Reg(0,ModeLockPosY,BlockingRead,0,0,0,0,&data);
        Yraw = data.s_data;
        //cout<<"xraw "<<Xraw<<"yraw "<<Yraw<<endl;
        Xpos = Xraw&0x3fff;
        Ypos = Yraw&0x3fff;
        Active = (((Xraw>>15) == 1)?"X":"Y");
        Dir = (((Yraw>>14) == 0x03)?"CW":"CCW");
        Mode = (((Xraw>>14) == 0x00)?"Locked":"Unlckd");
        out<<setw(8)<<Xraw<<setw(8)<<Yraw<<setw(8)<<Xpos<<setw(8)<<Ypos<<
             setw(8)<<Active<<setw(8)<<Dir<<setw(8)<<Mode<<"\r"<<endl;

        ScanPoint.X_Point = Xpos;
        ScanPoint.Y_Point = Ypos;
        ScanPoint.Direction = (((Yraw>>14) == 0x03)?MoveIsUp:MoveIsDown);
        ScanPoint.Islock = !(Xraw>>14);
        ScanPointVector.push_back(ScanPoint);
    }

}

void SCANNING::ScanStyleInfo()
{
    CanDataStruct data;
    out<<left;
    out<<endl;
    out<<endl;
    float seedDriverCurrent = lpLD_temp[0].Real_current;
    float seedTemperature = lpLD_temp[0].Real_temperature;
    float seedBoxBase = lpLD_temp[SeedRank].Real_temperature;
    lpLD_temp[SeedRank].Read_Set_Reg(0x04,0x04,BlockingRead,0,0,0,0,&data);
    float AirTemperature = data.f_data;
    lpLD_temp[SeedRank].Read_Set_Reg(0x03,0x02,BlockingRead,0,0,0,0,&data);
    float TargetTemp = data.f_data;
    out<<setw(35)<<"Seed Driver Current (A):"<<setw(10)<<seedDriverCurrent<<"\r"<<endl;
    out<<setw(35)<<"Seed Temperature (~C):"<<setw(10)<<seedTemperature<<"\r"<<endl;
    out<<setw(35)<<"SeedBox Base Temperature (~C):"<<setw(10)<<seedBoxBase<<"\r"<<endl;
    out<<setw(35)<<"SeedBox Target Temperature (~C):"<<setw(10)<<TargetTemp<<"\r"<<endl;
    out<<"\r"<<endl;
    out<<"\r"<<endl;
    out<<setw(15)<<"Scan TYpe:"<<setw(15)<<Schematic<<"\r"<<endl;
    out<<setw(15)<<"X Start:"<<setw(15)<<*ScanArg<<"\r"<<endl;
    out<<setw(15)<<"X End:"<<setw(15)<<*(ScanArg+1)<<"\r"<<endl;
    out<<setw(15)<<"X Step:"<<setw(15)<<*(ScanArg+2)<<"\r"<<endl;
    out<<setw(15)<<"Y Start:"<<setw(15)<<*(ScanArg+3)<<"\r"<<endl;
    out<<setw(15)<<"Y End:"<<setw(15)<<*(ScanArg+4)<<"\r"<<endl;
    out<<setw(15)<<"Y Step:"<<setw(15)<<*(ScanArg+5)<<"\r"<<endl;
    out<<"\r"<<endl;
    out<<"\r"<<endl;
    out<<setw(8)<<"Xraw"<<setw(8)<<"Yraw"<<setw(8)<<"Xpos"<<setw(8)<<
         "Ypos"<<setw(8)<<"Active"<<setw(8)<<"Dir"<<setw(8)<<"Mode"<<"\r"<<endl;
}

void SCANNING::CancelScan()
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,ScanSTS,WriteReg,0,0,0,0,NULL);
    IsRun = false;
    UpDateUi(1,0);
}

//xunzhao best workpoint
void SCANNING::SearchWorkingPoint()
{
    int j = 0;
    for(int i=0; i<ScanPointVector.size();){
        ScanPoint = ScanPointVector.at(i);
        for(j=i+1; j<ScanPointVector.size();j++){
            if(ScanPoint.X_Point != ScanPointVector.at(j).X_Point){
                i = j;//
                break;//
            }
        }
        //if(j == ScanPointVector.size())
        //    break;
        if((j-i) != 4)
            continue;
        SCANPOINT PointGroup[4] = { ScanPointVector.at(i-4),
                                    ScanPointVector.at(i-3),
                                    ScanPointVector.at(i-2),
                                    ScanPointVector.at(i-1)};
        if((PointGroup[0].Y_Point > PointGroup[3].Y_Point) &&(PointGroup[1].Y_Point > PointGroup[2].Y_Point) &&(PointGroup[2].Y_Point > PointGroup[0].Y_Point)){
            ScanValidPoint.X_Point = PointGroup[0].X_Point;
            ScanValidPoint.lineStart = PointGroup[0].Y_Point;
            ScanValidPoint.lineEnd = PointGroup[2].Y_Point;
            ValidPointVector.push_back(ScanValidPoint);
            continue;
        }
        if((PointGroup[1].Y_Point > PointGroup[2].Y_Point) &&(PointGroup[2].Y_Point > PointGroup[3].Y_Point) &&(PointGroup[3].Y_Point > PointGroup[0].Y_Point)){
            ScanValidPoint.X_Point = PointGroup[0].X_Point;
            ScanValidPoint.lineStart = PointGroup[3].Y_Point;
            ScanValidPoint.lineEnd = PointGroup[2].Y_Point;
            ValidPointVector.push_back(ScanValidPoint);
            continue;
        }
        if((PointGroup[2].Y_Point > PointGroup[1].Y_Point) &&(PointGroup[1].Y_Point > PointGroup[3].Y_Point) && (PointGroup[3].Y_Point > PointGroup[0].Y_Point)){
            ScanValidPoint.X_Point = PointGroup[0].X_Point;
            ScanValidPoint.lineStart = PointGroup[3].Y_Point;
            ScanValidPoint.lineEnd = PointGroup[1].Y_Point;
            ValidPointVector.push_back(ScanValidPoint);
            continue;
        }

    }
    SearchMaxAcre();

}
//xunzhao meiyou de dian
void SCANNING::SearchMaxAcre()
{
    int MaxStart = 0;
    int MaxNum   = 0;
    int j = 0;
    int Step = *(ScanArg+2);
    MaxStart = ValidPointVector.at(0).X_Point;
    ValidPoint validpoint = ValidPointVector.at(0);
    for(int i=1; i<ValidPointVector.size(); i++){
        j++;
        if(ValidPointVector.at(i).X_Point != (validpoint.X_Point + Step*j)){
            acre.X_Start = i-j;//validpoint.X_Point;
            acre.Length  = j;
            j = 0;
            validpoint = ValidPointVector.at(i);
            WuXiaoDian.push_back(acre);
        }
    }

    for(int i =0 ; i< WuXiaoDian.size();i++){
        acre = WuXiaoDian.at(i);
        if(acre.Length>MaxNum){
            MaxNum = acre.Length;
            MaxStart = acre.X_Start;
        }
    }
    int topmin;
    int downmin;
    int temp = 0;
    int temp1 = 0;
    for(int i=0 ;i <MaxNum; i++){
        if(ValidPointVector.at(MaxStart).lineStart<temp){
            temp = ValidPointVector.at(MaxStart).lineStart;
        }
        if(ValidPointVector.at(MaxStart).lineEnd<temp1){
            temp1 = ValidPointVector.at(MaxStart).lineEnd;
        }
    }
    int X;
    int Y;
    X = (ValidPointVector.at(MaxStart).X_Point + ValidPointVector.at(MaxStart+MaxNum-1).X_Point)/2 ;
    Y = (temp + temp1)/2;
    if((X>0&&X<60000) && (Y>0&&Y<60000)&&(temp1<temp)){
        //to do set xy motor location
    }

}


SCANNING::~SCANNING()
{
    lpLD_temp[SeedRank].Read_Set_Reg(0,ScanSTS,WriteReg,0,0,0,0,NULL);
    TotalSch = 0;
    IsRun = false;
    delete[] ScanArg;
}
