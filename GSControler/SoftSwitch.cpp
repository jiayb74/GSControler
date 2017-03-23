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
#include "mainwindow.h"
#include "LDKeeper.h"
#include "Status.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "HotSwap.h"
#include "PortState.h"
#include "SoftSwitch.h"
#include "Module.h"
#include "board.h"
#include <QMessageBox>


using namespace std;

extern Module module;       //模块对象
bool IsSanHeYi = false;     //是否时三合一
extern PortState * por;     //PIN
bool SwitchPthreadStatus;   //SWOITCH线程状态
LDKeeper * lpLD_temp;       //模块对象
float Curr_Temp = 0;        //电流缓存
bool FLAG = true;           //加减电流标志位
float ProgressBase;         //精度条百分比
int WaitTime = 0;           //锁膜等待时间
unsigned char InitFreq;     //初始化频率
unsigned int InitPlus;      //初始化脉冲
bool IsShutdown = false;    //是否关机
int restart;
int EnMOFlag = 0;
int EnPWFlag = 0;
float precurr = 0;
Board * boards;
Board * ExBoards;
int boardTempIndex = 0;
int boardLDTempIndex = 0;
int MachineMode = 0;

extern float MinPW   ;
extern int MaxPW   ;
extern float MaxPower   ;
extern int MinFreq   ;
extern int MaxFreq   ;
extern int MAXMJ     ;
extern float FreqLimit;
int WorkStatus = 0;
int WorkLock   = 1;
int AlarmStatus = 0;
int errorNumber = 0;
int errorMaxNumber = 3;
float pencent = 0;
int SystemOutPut = 1;


int stage = 0;
int ontime = 0;
int offtime = 0;
int times  = 0;
int curentRank = 9;
float aimcurrent[10];
float setcurrent = aimcurrent[0];
int aimcurrentindex = 0;
int slowindex = 0;
float Lockcurr = 0;
int checkProgress=0;
bool lockflag=false;
int FSwitch = 0;
CSoftSwitch::CSoftSwitch()
{
    m_bLaserOn      = false;
    m_bSelfScan     = false;
    m_bModeLocked   = false;
    m_bIsShutDown   = false;
    m_isOffLock     = false;
    m_bLOOP         = true;
    m_bIsRunThread  = true;
    m_SureShutdown  = false;
    m_nInitialWait  = 5;
    m_nWaitProgress = 0;
    m_nCurrentLDNo  = -1;
    SwitchPthreadStatus = true;
    this->m_nCurrentLDNo = 0;
    m_isUpdateUI = true;
    m_isScanUpdate = true;
    m_bModeLockedHandleFlag = true;
    restart = 0;
}

bool CSoftSwitch::InitInstance()
{
    CanDataStruct data;
    emit SystemLog("read inifile");
    CanRecv = new Can_LD();
    SwitchPortLevel = new PortState;
    por = SwitchPortLevel;
    //设备故障信号槽
    connect(CanRecv,SIGNAL(Fault_Alarm(char)),this,SLOT(Fault_Alarm(char)));
    connect(CanRecv,SIGNAL(Int_Alarm(unsigned char,unsigned char)),this,SLOT(Int_Alarm(unsigned char,unsigned char)));
    HardwarePlatform();
    InitNetworkStructure();
    SystemInit();


    //心跳机制
  //  Heartbeat = new CStatus(lpLD,this->ModuleNum);


    QObject::connect(SwitchPortLevel,SIGNAL(SwitchState(bool)),
                          this,SLOT(Power_Switch(bool)));

    QObject::connect(CanRecv,SIGNAL(ModeLocked(bool)),
                       SwitchPortLevel,SLOT(Change_Led_state(bool)));

    QObject::connect(CanRecv,SIGNAL(ModeLocked(bool)),
                       this,SLOT(ModeLocked(bool)));

    QObject::connect(this,SIGNAL(Protect(bool)),
                       SwitchPortLevel,SLOT(Protect(bool)));

    hotSwap = new CHotSwap();
    QObject::connect(this,SIGNAL(SetDivi()),
                     hotSwap,SLOT(SetDivi()));

    boards[0].LD[0].Read_Set_Reg(0,CheakModeLock,BlockingRead,0,0,0,0,NULL);//上电初始化读一次锁莫状态
    //init temperature
    for(int i=0; i<Board::GetBoardNum();i++){
        boards[i].InitBoardTemp();
    }
    //重启系统后进入的方式选择
    StartModeSelection();
}

void CSoftSwitch::InitNetworkStructure()
{
    char lpwSecName[15];
    char lpwSecNameTemp[15];
    char lpwKeyPxITargName[20];
    int Ld;
    int Level;
    int OutPut;
    int DisableLD;
    int m_nComm;
    int pulsCnt;
    int * DisableArray;
    int levelrank = 0;
    int outrank = 0;
    int BoardIndex = 0;
    int pulsMax;
    int pulsMin;
    int Freq;
    LDKeeper LDTemp;

    Freq = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_FREQ,0,GS_INI_FILE);
    sprintf(lpwSecNameTemp,"Freq%d",Freq);
    InitFreq = Freq;
    pulsCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,0,GS_INI_X_FILE)+
              GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT1,0,GS_INI_X_FILE)+
              GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT2,0,GS_INI_X_FILE);

    pulsMax = GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMAX,12,GS_INI_HARD_FILE);
    pulsMin = GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMIN,8,GS_INI_HARD_FILE);
    if(!((pulsCnt<= pulsMax) && (pulsCnt>= pulsMin))){
        pulsCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,8,GS_INI_FILE);
    }
    InitPlus = pulsCnt;

    Ld = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LD,4,GS_INI_HARD_FILE);
    Level  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LEVEL,1,GS_INI_HARD_FILE);
    OutPut  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_OUTPUT,1,GS_INI_HARD_FILE);
    SystemOutPut = OutPut;
    DisableLD  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_DISABLELD,0,GS_INI_HARD_FILE);
    if(OutPut<=1)
        m_nComm=Ld;
    else
        m_nComm=(OutPut*(Level+1)-Ld+DisableLD)/(OutPut-1);
    m_nTotalLD = Ld-DisableLD;
//    switch(MachineMode)
//    {
//    case MachineN06:boards = new Board[m_nTotalLD];
//                    Board::SetBoardNum(m_nTotalLD);
//                    break;
//    case MachineP03H:boards = new Board[m_nTotalLD-1];
//                    Board::SetBoardNum(m_nTotalLD-1);
//                    boards[0].SetLDNumber(2);
//                    break;
//    case MachineP08H:boards = new Board[m_nTotalLD-1];
//                    Board::SetBoardNum(m_nTotalLD-1);
//                    boards[0].SetLDNumber(2);
//                    break;
//    default:boards = new Board[m_nTotalLD-1];
//                    Board::SetBoardNum(m_nTotalLD-1);
//                    boards[0].SetLDNumber(2);
//                    break;
//    }

/*
    for(int i=0; i<m_nBoard; i++)
    {
        sprintf(lpwSecName,"%s%.1d","P",i);
        sprintf(lpwKeyPxITargName,("%sITarget_Pulse%d"),lpwSecName,pulsCnt);
        float InitCurr = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_I,-1,GS_INI_FILE);
        InitCurr = GetPrivateProfileDouble(lpwSecNameTemp,lpwKeyPxITargName,InitCurr,GS_INI_FILE);
        float temp = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_T,-1,GS_INI_FILE);
        LDTemp.SetInitCurr(InitCurr);
        LDTemp.SetInitTemp(temp);
        LDTemp.SetIndex(i);
        if(boards[BoardIndex].IsOverload())
        {
            BoardIndex++;
        }
        boards[BoardIndex].AddLD(LDTemp);
        if(levelrank < m_nComm){
            if(!boards[BoardIndex].IsSetID())
                boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
            levelrank++;
        }
        else
        {
            if(!boards[BoardIndex].IsSetID())
                boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
            outrank++;
            if(outrank == OutPut)
            {
                levelrank++;
                outrank = 0;
            }
        }
    }*/

}

void CSoftSwitch::SystemInit()
{
    int MType,EType,DriverType;
    int Phase;
    switch(MachineMode)
    {
    case MachineN06:    CanDataStruct data;
            MinFreq = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_FMIN,20,GS_INI_FILE);
            MaxFreq = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_FMAX,1000,GS_INI_FILE);
            MinPW = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_PMIN,3,GS_INI_FILE);
            MaxPW = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_PMAX,400,GS_INI_FILE);
            MAXMJ = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_ENERGY,1,GS_INI_FILE)*1000;
            MaxPower =  GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_MAXPOWER,50,GS_INI_FILE);
            FreqLimit = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_FREQLEVEL,250,GS_INI_FILE);
            this->BaisI = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_BAISEI,0,GS_INI_FILE);
            this->WorkMode = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_MODE,2,GS_INI_FILE);
            this->PRR_filter = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_PRR_FILTER,0,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,125,WriteReg,this->WorkMode&0xff,this->PRR_filter&0xff,0,0,NULL);
            //enable fpga interupt
            //boards[0].LD[0].Read_Set_Reg(0,4,WriteReg,0x7f,0xff,0,0,NULL);
            //read pulse and freq from ini file
            this->initPulse = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_PULSE,0.8,GS_INI_FILE);
            this->initFreq = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_FREQ,25,GS_INI_FILE);
            InitPlus = this->initPulse;
            InitFreq = this->initFreq;
            boards[0].LD[0].Init(InitPlus,InitFreq);
            boards[0].LD[0].SetMinFreq(this->initFreq);
            // usleep(100000);
            boards[0].LD[0].SetPulseWidth(this->initPulse);
            //usleep(100000);
            //clear all fpga alarm
            boards[0].LD[0].Read_Set_Reg(0,31,WriteReg,0,0,0,0,NULL);
            //offset pulse width
            this->basePulseWidth = GetPrivateProfileDouble(GS_INI_SECTION_FPGA,GS_INI_KEY_BASEPW,0,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,18,WriteReg,(int(this->basePulseWidth*8))&0xff,0,0,0,NULL);
            this->DccIni = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_DCCINI,0x0b,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,6,WriteReg,this->DccIni&0xff,0,0,0,NULL);
            //mo signel enable
            EnMOFlag = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_ENMO,0,GS_INI_FILE);
            //pulse width style
            EnPWFlag = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_ENPW,0,GS_INI_FILE);
            //open ld switch
            //set pulse and freq
            boards[2].LD[0].SwitchSet(ON);
            boards[3].LD[0].SwitchSet(ON);
            m_bModeLocked = true;
            Inittime = 1;
            WorkLock = LASER_LD0;
            WorkStatus = Work_N06;
            AlarmStatus = LASERERROR;
            break;
    case MachineP03H:
            MType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_MOTORTYPE,0,GS_INI_FILE);
            DriverType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_DRIVERTYPE,0,GS_INI_FILE);
            EType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_EOMTYPE,0,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,127,WriteReg,(DriverType<<1)|MType,EType,0,0,NULL);
            Inittime = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_INITTIME,0,GS_INI_FILE);
            Phase = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PGAP,1544,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,GS_PD_PHASE,WriteReg,Phase&0xff,(Phase>>8)&0xff,0,0,NULL);
            WorkLock = LASER_LD0;
            WorkStatus = Work_P03;
            AlarmStatus = CheckP0;
           // boards[0].LD[0].Read_Set_Reg(0,CheakModeLock,BlockingRead,0,0,0,0,NULL);//上电初始化读一次锁莫状态
            m_bModeLocked = true;
            Inittime=1;
            break;
    case MachineP08H:
            char in[5];
            MType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_MOTORTYPE,0,GS_INI_FILE);
            DriverType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_DRIVERTYPE,0,GS_INI_FILE);
            EType = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_EOMTYPE,0,GS_INI_FILE);

            boards[0].LD[0].Read_Set_Reg(0,127,WriteReg,(DriverType<<1)|MType,EType,0,0,NULL);
            Inittime = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_INITTIME,0,GS_INI_FILE);
            m_OnTime = GetPrivateProfileInt(GS_INI_SECTION_SC,GS_INI_KEY_ONTIME,30,GS_INI_FILE);
            m_OffTime = GetPrivateProfileInt(GS_INI_SECTION_SC,GS_INI_KEY_OFFTIME,5,GS_INI_FILE);
            m_Step = GetPrivateProfileInt(GS_INI_SECTION_SC,GS_INI_KEY_OFFTIME,1,GS_INI_FILE);
            Phase = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PGAP,0,GS_INI_FILE);
            FSwitch =  GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_FSWITCH,0,GS_INI_FILE);
            boards[0].LD[0].Read_Set_Reg(0,124,WriteReg,FSwitch,0,0,0,NULL);
            usleep(100000);
            boards[0].LD[0].Read_Set_Reg(0,GS_PD_PHASE,WriteReg,Phase&0xff,(Phase>>8)&0xff,0,0,NULL);
            aimcurrent[0] = GetPrivateProfileDouble(GS_INI_SECTION_P0,GS_INI_KEY_I,0,GS_INI_FILE);
            for(int i=1;i<=9;i++){
                sprintf(in,"I%d",i);
                aimcurrent[i] = GetPrivateProfileDouble(GS_INI_SECTION_SC,in,aimcurrent[i-1],GS_INI_FILE);
            }
            WorkLock = LASER_LD0H;
            WorkStatus = Work_P08H;  //xiugai
            AlarmStatus = OffP0;
            boards[0].LD[0].Read_Set_Reg(0,CheakModeLock,BlockingRead,0,0,0,0,&data);//上电初始化读一次锁莫状态
            if(data.s_data){
                this->ModeLocked(data.s_data);
                por->Change_Led_state(data.s_data);
            }
            Inittime=1;
            break;
    }

}

bool CSoftSwitch::InitIniFile()
{
    char lpwSecName[15];
    char lpwSecNameTemp[15];
    char lpwKeyPxITargName[20];
    int Ld;
    int Level;
    int OutPut;
    int DisableLD;
    int m_nComm;
    int * DisableArray;
    int pulsMax;
    int pulsMin;
    int pulsCnt;
    int Freq;
    Freq = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_FREQ,0,GS_INI_FILE);
    sprintf(lpwSecNameTemp,"Freq%d",Freq);
    InitFreq = Freq;
    pulsCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,0,GS_INI_X_FILE)+
              GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT1,0,GS_INI_X_FILE)+
              GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT2,0,GS_INI_X_FILE);

    pulsMax = GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMAX,12,GS_INI_HARD_FILE);
    pulsMin = GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMIN,8,GS_INI_HARD_FILE);
    if(!((pulsCnt<= pulsMax) && (pulsCnt>= pulsMin))){
        pulsCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,8,GS_INI_FILE);
    }
    InitPlus = pulsCnt;
    Ld = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LD,4,GS_INI_HARD_FILE);
    Level  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LEVEL,1,GS_INI_HARD_FILE);
    OutPut  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_OUTPUT,1,GS_INI_HARD_FILE);
    DisableLD  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_DISABLELD,0,GS_INI_HARD_FILE);
    if(OutPut<=1)
        m_nComm=Ld;
    else
        m_nComm=(OutPut*(Level+1)-Ld+DisableLD)/(OutPut-1);

    m_nTotalLD = Ld-DisableLD;
    this->ModuleNum = m_nTotalLD+1; //+1 seed
    //根据文件读取节点数动态创建对象
    lpLD = new LDKeeper[this->ModuleNum];  //实际LD数量 + 1是seed
    //lpLD_temp传递到regmap里作为全局变量
    lpLD_temp = lpLD;
    IsSanHeYi = 1;
//    if(IsSanHeYi){
//        boards = new Board[m_nTotalLD-1];
//        Board::SetBoardNum(m_nTotalLD-1);
//        boards[0].SetLDNumber(2);
//    }
//    else{
//        boards = new Board[m_nTotalLD];
//        Board::SetBoardNum(m_nTotalLD);
//    }
    int levelrank = 0;//ziji
    int outrank = 0;
    int BoardIndex = 0;
    LDKeeper LDTemp;
    if(DisableLD>0){
        DisableArray=new int[DisableLD];
        for(int j=0;j<DisableLD;j++)    //找到失能的LD编号
        {
            sprintf(lpwSecName,"%s%.1d","Disable",j);
            DisableArray[j]=GetPrivateProfileInt(GS_INI_SECTION_MODULE,lpwSecName,0,GS_INI_HARD_FILE);
        }
        int j;
        for(int i=0; i<Ld; i++){        //根据失能的编号确定应该要读的PX
            for(j=0; j<DisableLD; j++){
                if(i == DisableArray[j]){
                    break;
                }
            }
            if(j == DisableLD){
                sprintf(lpwSecName,"%s%.1d","P",i);
                sprintf(lpwKeyPxITargName,("%sITarget_Pulse%d"),lpwSecName,pulsCnt);
                float InitCurr = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_I,-1,GS_INI_FILE);
                InitCurr = GetPrivateProfileDouble(lpwSecNameTemp,lpwKeyPxITargName,InitCurr,GS_INI_FILE);
                float temp = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_T,-1,GS_INI_FILE);
                LDTemp.SetInitCurr(InitCurr);  //初始化LD电流--不含seed,seed单独分离
                LDTemp.SetInitTemp(temp);
                LDTemp.SetIndex(i);
                if(boards[BoardIndex].IsOverload()){
                    BoardIndex++;
                }
                boards[BoardIndex].AddLD(LDTemp);
                if(levelrank < m_nComm){
                    if(!boards[BoardIndex].IsSetID())
                        boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
                    levelrank++;
                }
                else{
                    if(!boards[BoardIndex].IsSetID())
                        boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
                    outrank++;                  
                    if(outrank == OutPut){
                        levelrank++;
                        outrank = 0;
                    }
                }
            }
        }
    }
    else{
        for(int i=0; i<Ld; i++){
            sprintf(lpwSecName,"%s%.1d","P",i);
            sprintf(lpwKeyPxITargName,("%sITarget_Pulse%d"),lpwSecName,pulsCnt);
            float InitCurr = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_I,-1,GS_INI_FILE);
            InitCurr = GetPrivateProfileDouble(lpwSecNameTemp,lpwKeyPxITargName,InitCurr,GS_INI_FILE);
            float temp = GetPrivateProfileDouble(lpwSecName,GS_INI_KEY_T,-1,GS_INI_FILE);
            LDTemp.SetInitCurr(InitCurr);
            LDTemp.SetInitTemp(temp);
            LDTemp.SetIndex(i);
            if(boards[BoardIndex].IsOverload()){
                BoardIndex++;
            }
            boards[BoardIndex].AddLD(LDTemp);
            if(levelrank < m_nComm){
                if(!boards[BoardIndex].IsSetID())
                    boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
                levelrank++;
            }
            else{
                if(!boards[BoardIndex].IsSetID())
                    boards[BoardIndex].SetBoardCanID(CalID(levelrank,outrank));
                outrank++;
                if(outrank == OutPut){
                    levelrank++;
                    outrank = 0;
                }
            }
        }
    }

    Inittime = GetPrivateProfileInt(GS_INI_SECTION_FPGA,GS_INI_KEY_INITTIME,0,GS_INI_FILE);

   //读取ini文件后初始化状态信息
  /*printf("read inifile result:\n");
    for(int i=0; i<m_nTotalLD+1; i++){
        printf("LD[%d]: Index:%d ID:%d InitCurr:%f\n",i,lpLD[i].index,lpLD[i].GetID(),lpLD[i].GetAimCurr());
   }*/
//    for(int i=0; i<Board::GetBoardNum();i++){
//        boards[i].OutInfo();
//    }
//    printf("-----------ini end-------\n");
}

void CSoftSwitch::Power_Switch(bool status)
{
    if(status)
        LaunchLaser();
    else
        ShutLaser();
    this->SwitchTimerProcessThread();
}

BYTE CSoftSwitch::CheckLaserStage()
{
	return(m_byLaserStage);
}

void CSoftSwitch::ForwardLaserStage(BYTE stage)
{
	m_byLaserStage = stage;
}

BOOL CSoftSwitch::IsLaserOn()
{
	return(m_bLaserOn);
}

void CSoftSwitch::LaunchLaser()
{
	m_bLaserOn = true;
}

void CSoftSwitch::ShutLaser()
{
	m_bLaserOn = false;
}

BOOL CSoftSwitch::IsScanning()
{
    return(m_bSelfScan);
}

void CSoftSwitch::GoSelfScan()
{
	m_bSelfScan = true;
}

void CSoftSwitch::ScanCompleted()
{
	m_bSelfScan = false;
}
void CSoftSwitch::SetShutDownValue(BOOL bValue)
{
	m_bIsShutDown = bValue;
}

BOOL CSoftSwitch::GetShutDownValue()
{
	return m_bIsShutDown;
}

BOOL CSoftSwitch::IsLocked()
{
	return(m_bModeLocked);
}

void CSoftSwitch::ModeLocked(bool state)
{
    m_bModeLocked = state;
    emit SetDivi(); //updata PD_DIV
    emit UartMolock(state);
    if((this-> CheckLaserStage() == LASER_SCAN) || (this-> CheckLaserStage() == OffP0)|| (this-> CheckLaserStage() ==LASER_LD0H)){
        emit UartMolock(state);
        return ;
    }
    if(!m_bModeLocked){
      //  if(!m_SureShutdown){    //not sure shutdown
            LockProtect();
      //  }
        m_isOffLock = true;
    }
  //  this->SwitchTimerProcessThread();

}

void CSoftSwitch::ChangeSwitchState(bool status)
{
    if(status)
        GoSelfScan();
    else
        ScanCompleted();
}
BOOL CSoftSwitch::MoSignal()
{
    return(this->MoStatus|m_bLaserOn);
}

BOOL CSoftSwitch::MoSignal(bool status)
{
    this->MoStatus = status;
    por->Change_Led_state(status);
    return(this->MoStatus);
}
bool CSoftSwitch::IsPreviousLDCurrentOk()
{
    float a =  boards[0].LD[0].GetRealCurrByReg();
    float b =  boards[1].LD[0].GetRealCurrByReg();
    if((a>LD0MinWorkCurr)&&(b>LD1MinWorkCurr)){
        return true;
    }
    return false;
}

bool CSoftSwitch::IsPreviousLDCurrentOkStatic()
{

    float a =  boards[0].LD[0].GetRealCurr();
    float b =  boards[0].LD[0].GetRealCurr();
    if((a>LD0MinWorkCurr)&&(b>LD1MinWorkCurr)){
        return true;
    }
    return IsPreviousLDCurrentOk();

}
void CSoftSwitch::Int_Alarm(unsigned char data0,unsigned char data2)
{
    boards[0].LD[0].HandleAlarm(data0,data2);
}
void CSoftSwitch::Fault_Alarm(char NODE)
{
    int index = -1;
    for(int i=0; i<=this->ModuleNum; i++){
        if(NODE == lpLD[i].GetID()){
           index = i;
     //      printf("%d moudel Alarm\n",i);
           for(int i=this->ModuleNum-1; i>=index; i--){
               if(i != 1){  //i == 1 is seed
          //         lpLD[i].Set_FloatSurrent(0);
          //         lpLD[i].SetSwitch(0);
               }
           }
           return;
        }
    }
//    printf("ALarm NODE error\n");
}

void CSoftSwitch::LoseLocking()
{

}
extern MainWindow *GsGuiPower;
void CSoftSwitch::UpdataUIPower()
{
    switch(MachineMode)
    {
    case (MachineP03H):
        if((this-> CheckLaserStage() == Work_P03 || m_SureShutdown) &&
                !(this-> CheckLaserStage() == OffP0) && m_isUpdateUI &&
                m_isScanUpdate){
            float PowerValue = this->hotSwap->GetRealPower();
            float PresetPower = this->hotSwap->GetPresetPower();
            emit UI_ShowPower(PresetPower,PowerValue);
        }
        break;
    case (MachineP08H):
        if((this-> CheckLaserStage() == Work_P08H || m_SureShutdown) &&
                !(this-> CheckLaserStage() == OffP0) ){
            float PowerValue = this->hotSwap->GetRealPower();
            float PresetPower = this->hotSwap->GetPresetPower();
            emit UI_ShowPower(PresetPower,PowerValue);
        }
        break;
    case MachineN06:
        //GetPresetPulse
        if((this-> CheckLaserStage() == Work_N06)||(this-> CheckLaserStage() == SlowOff)){
            float PresetPower = boards[0].LD[0].GetPresetPower();
            float PowerValue = boards[0].LD[0].GetPower();
            emit UI_ShowPower(PresetPower,PowerValue);
            emit UpdataPlusFreqPower(boards[0].LD[0].GetPresetPulse(),boards[0].LD[0].GetPresetFreq());
        }
        break;
    }
}
// return ratio that m_nWaitProgress over m_nInitialWait
double CSoftSwitch::CheckInitProgress(void)
{
	return ((double) m_nWaitProgress / (double) m_nInitialWait);
}

void CSoftSwitch::timerUpDate()
{
    SwitchPthreadStatus = true;
}

void CSoftSwitch::run()
{
    QTimer *timer = new QTimer();
    QTimer *timer1 = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(SwitchTimerProcessThread()),Qt::DirectConnection);
    connect(timer1,SIGNAL(timeout()),this,SLOT(UpdataUIPower()));
    timer->start(1000);
    timer1->start(1000);
    exec();
}

void CSoftSwitch::LockProtect()
{
    m_bIsRunThread = false;

//    if(!m_bModeLockedHandleFlag)
//        return;
//    m_bModeLockedHandleFlag = false;     //因为没有满足减电流，而且此时有没有检测按钮，因此不会改变状态，待完善。
    for(int i = Board::BoardNum; i>=1; i--){
           boards[i-1].OffCurrent();            //save pump0 current
    }
    Board::CurrentBoardIndex = 0;
    switch(MachineMode)
    {
    case MachineP08H:
                    break;
    default:if(boards[ Board::CurrentBoardIndex].LDNumber>1)
                boards[ Board::CurrentBoardIndex].CurrentLDIndex++; //next ld
            else
               Board::CurrentBoardIndex++; //      //next board
            break;
    }

    SwitchPortLevel->Change_Led_state(false);
    emit UI_Laser_State(ERROR);
    ProgressBase = 0;
    WaitTime = 0;
    FLAG = true;
    this ->ForwardLaserStage( AlarmStatus );
    m_bIsRunThread = true;
}

void  CSoftSwitch::SwitchTimerProcessThread()
{
    if(this->m_bIsRunThread)
    {
       //cout<<"------------switch--"<<QThread::currentThreadId()<<endl;
        Board * BoardTemp = &boards[Board::CurrentBoardIndex];
        LDKeeper * LDTemp  = &boards[Board::CurrentBoardIndex].LD[boards[Board::CurrentBoardIndex].CurrentLDIndex];
        SwitchPthreadStatus = true;
        //lpLD[SeedRank].Read_Set_Reg(0,CheakModeLock,Nonblockingread,0,0,0,0,NULL); //防止长时间和FPGA通讯异常
        usleep(10000);
        switch(this-> CheckLaserStage())
        {
            case LASER_IDLE:	//Laser system idle
                if(this ->IsLaserOn() || this->IsScanning())
                {
                    this ->ForwardLaserStage( WorkLock );
                    FLAG = true;
                    if(this->IsLaserOn()){
                        emit UI_Laser_State(LAUNCH);    //launch
                        emit Protect(true); //start power protect
                        stage=0;
                    }
                    else
                        emit UI_Laser_State(LASER_SCAN);
                }
            break;
            case LASER_LD0:// Increase pump0 voltage each CURRENTSTEP per second
                if(!(this ->IsLaserOn() || this->IsScanning()))
                {
                    if(!this->IsLaserOn()){
                        emit UI_Laser_State(SHUTDOWN);
                        m_SureShutdown = true;
                    }
                    else
                        emit UI_Laser_State(LASER_SCAN);
                    this ->ForwardLaserStage( CheckP0 );
                }
                else if(FLAG == true)
                {
                    cout<<LDTemp->GetRealCurr()<<" + "<<LDTemp->GetAimCurr()<<" + "<<ProgressBase<<endl;
                    if(LDTemp->GetAimCurr() <=0.01)
                         pencent = 1.0;
                    else
                        pencent = LDTemp->GetRealCurr()/LDTemp->GetAimCurr();
                    emit  updatabar("Stage 1 of 3",100*pencent);
                    LDTemp->SwitchSet(ON);    //lpThis->m_nCurrentLDNo = 0
                    usleep(10000);
                    LDTemp->SetCurrByReg(LDTemp->GetAimCurr(),CHANGESLOW);
                    FLAG = false;
                }
                else
                {
                    if(LDTemp->GetAimCurr() <=0.01)
                         pencent = 1.0;
                    else
                        pencent = LDTemp->GetRealCurr()/LDTemp->GetAimCurr();
                    emit  updatabar("Stage 1 of 3",100*pencent);
                    if(LDTemp->CurrOk())       //预设电流
                    {
                        emit  updatabar("Stage 1 of 3",100);
                        this ->ForwardLaserStage (LASER_INIT);
                        FLAG = true;
                        BoardTemp->CurrentLDIndex++;
                        if(BoardTemp->CurrentLDIndex>=BoardTemp->LDNumber){
                            Board::CurrentBoardIndex++; //next board
                            BoardTemp->CurrentLDIndex--;   //receve old status
                        }
                       //  BoardTemp->CurrentLDIndex--;   //receve old status  here error
                         boardTempIndex = BoardTemp->CurrentBoardIndex;
                         boardLDTempIndex = BoardTemp->CurrentLDIndex;

                    }
                }
                break;
            case LASER_LD0H:// Increase pump0 voltage each CURRENTSTEP per second
            if(!(this ->IsLaserOn() || this->IsScanning()))
            {
                if(!this->IsLaserOn()){
                    emit UI_Laser_State(SHUTDOWN);
                    m_SureShutdown = true;
                }
                this ->ForwardLaserStage( CheckP0 );
            }
            if( this ->IsLocked()){
                 if(stage!=6)
                    stage = 5;
                  if(!lockflag){
                    Lockcurr = aimcurrent[aimcurrentindex];
                    lockflag=true;
                  }

                 checkProgress=0;
                 emit  updatabar("Stage 1 of 3",10*10);
             }

            emit  updatabar("Stage 1 of 3",checkProgress*10+times);
            switch(stage)
            {
            case 0:LDTemp->SwitchSet(ON);stage = 1;break;
            case 1:LDTemp->SetCurrByReg(aimcurrent[aimcurrentindex],CHANGEFSAT);stage=2;break;
            case 2:ontime++;if(ontime>=m_OnTime){ontime=0;stage=3;}break;
            case 3:LDTemp->SetCurrByReg(0,CHANGEFSAT);stage=4;break;
            case 4:offtime++;if(offtime>=m_OffTime){
                    offtime=0;stage=1;times++;
                    if(times>=10){
                        times=0;
                        checkProgress++;
                        aimcurrentindex++;
                    }
                }break;
            case 5:slowindex++;
                if(slowindex>=m_Step){
                    slowindex=0;
                    if(aimcurrent[aimcurrentindex]>LDTemp->GetAimCurr()){

                        if((Lockcurr-LDTemp->GetAimCurr())<=0.05){
                            LDTemp->SetCurrByReg(LDTemp->GetAimCurr(),CHANGEFSAT);
                            stage=6;
                        }
                        else{
                            Lockcurr -= 0.05;
                            LDTemp->SetCurrByReg(Lockcurr,CHANGEFSAT);
                        }
                    }
                    else{

                        if((Lockcurr-LDTemp->GetAimCurr())>=0){
                            LDTemp->SetCurrByReg(LDTemp->GetAimCurr(),CHANGEFSAT);
                            stage=6;
                        }
                        else{
                            Lockcurr += 0.05;
                            LDTemp->SetCurrByReg(Lockcurr,CHANGEFSAT);
                        }
                    }
                }break;
            case 6: if(!this ->IsLocked()){
                     this->ForwardLaserStage(LASER_IDLE);
                    }
                else{
                    this->ForwardLaserStage( LASER_INIT+1 );

                }
                ontime=0;
                offtime=0;
                aimcurrentindex=0;
                checkProgress=0;
                stage=0;
                lockflag=false;
                FLAG = true;
                BoardTemp->CurrentLDIndex++;
                if(BoardTemp->CurrentLDIndex>=BoardTemp->LDNumber){
                    Board::CurrentBoardIndex++; //next board
                    BoardTemp->CurrentLDIndex--;   //receve old status
                }
                 boardTempIndex = BoardTemp->CurrentBoardIndex;
                 boardLDTempIndex = BoardTemp->CurrentLDIndex;
                break;
            }
            break;

            case LASER_INIT:// Wait for ModeLock status lasting long enough
                    if(this->IsScanning()){
                        this->ForwardLaserStage(LASER_SCAN);
                        emit UI_Laser_State(LASER_SCAN);
                        emit StartScan();
                     }
                    else if(! this->IsLaserOn() )
                    {
                        emit UI_Laser_State(SHUTDOWN);
                        m_SureShutdown = true;
                        this->ForwardLaserStage( CheckP0 );
                    }
                    else if(FLAG == true){
                        emit  updatabar("Stage 2 of 3",0);
                        emit UI_Laser_State(LAUNCH);
                        FLAG = false;
                    }
                    else
                    {
                        if( this ->IsLocked())
                        {
                            emit  updatabar("Stage 2 of 3",100*(WaitTime++/(Inittime*1.0)));
                            if(WaitTime > (Inittime) ){
                                WaitTime = 0;
                                Inittime = Inittime/6;
                                if(Inittime <= 6)
                                    Inittime = 6;
                                this ->ForwardLaserStage (LASER_INIT + 1);
                                FLAG = true;
                            }
                        }
                    }
                break;
            case LASER_INIT + 1:// Increase pump1 pump2 pump3 current each CURRENTSTEP per second
            //后续在做
            //电流时先判断是否符合，若符合则直接跳过，反之，加电流。防止后级有电流时前级电流又重新加。
            //将flag标志位改写成switch语句，设置多个状态。
                if((!this ->IsLaserOn()) || this->IsScanning())
                {
                    if(!this->IsLaserOn()){
                        emit UI_Laser_State(SHUTDOWN);
                         m_SureShutdown = true;
                         m_isUpdateUI = false;
                    }
                    else
                        emit UI_Laser_State(LASER_SCAN);
                    FLAG = true;
                    this ->ForwardLaserStage(SlowOff);
                    //从最后一级减。
                    BoardTemp->CurrentBoardIndex = Board::BoardNum-1;
                    BoardTemp->CurrentLDIndex = boards[BoardTemp->CurrentBoardIndex].LDNumber-1;
                    ProgressBase = 0;
                }
                else if(FLAG == true)
                {
                    if(LDTemp->GetAimCurr() <=0.01)
                         pencent = 1.0;
                    else
                        pencent = LDTemp->GetRealCurr()/LDTemp->GetAimCurr();
                    emit  updatabar("Stage 3 of 3",ProgressBase+100.0/
                                        (this->m_nTotalLD-1)*
                                        (pencent));

                    LDTemp->SwitchSet(ON);    //lpThis->m_nCurrentLDNo = 0
                    usleep(10000);
                    LDTemp->SetCurrByReg(LDTemp->GetAimCurr(),CHANGESLOW);
                    FLAG = false;
               }
               else {
                    if(LDTemp->GetAimCurr() <=0.01)
                         pencent = 1.0;
                    else
                        pencent = LDTemp->GetRealCurr()/LDTemp->GetAimCurr();
                    emit  updatabar("Stage 3 of 3",ProgressBase+100.0/
                                        (this->m_nTotalLD-1)*
                                        (pencent));
                        if(LDTemp->CurrOk())  //预设电流
                                                    {
                            if(LDTemp->GetAimCurr() <=0.01)
                                 pencent = 1.0;
                            else
                                pencent = LDTemp->GetRealCurr()/LDTemp->GetAimCurr();
                            emit  updatabar("Stage 3 of 3",ProgressBase+100.0/
                                                (this->m_nTotalLD-1)*
                                                (pencent));
                            FLAG = true;
                            ProgressBase = ProgressBase+100/(this->m_nTotalLD-1);
                            BoardTemp->CurrentLDIndex++;
                            if(BoardTemp->CurrentLDIndex>=BoardTemp->LDNumber){
                                Board::CurrentBoardIndex++; //next board
                                BoardTemp->CurrentLDIndex--;    //receve old status
                            }
                            if(Board::CurrentBoardIndex>=Board::BoardNum){
                                this ->ForwardLaserStage (WorkStatus);
                                emit UI_Laser_State(WorkStatus);    //面板显示出光功率
                                emit Set_ProgressBar(HIDEBAR);
                                emit Set_ProgressBar(RESETBAR);                               
                                emit UI_Laser_State(LASER_LASE);
                                ProgressBase = 0;
                                FLAG=true;
                                Board::CurrentBoardIndex--; //point last board[]
                            }
                       }
                }
                break;
        case Work_N06:
                     if(!this ->IsLaserOn())
                     {
                         emit UI_Laser_State(SHUTDOWN);
                          m_SureShutdown = true;
                         FLAG = true;
                         this ->ForwardLaserStage(SlowOff);
                     }
                     if(EnMOFlag){
                         if(this->MoSignal()){
                             if(!IsPreviousLDCurrentOkStatic())
                             {
                                 for(int i = Board::BoardNum; i>=1; i--){
                                        boards[i-1].OffCareCurrent();
                                 }
                             }
                         }
                         else{
                             for(int i = Board::BoardNum; i>=1; i--){
                                    boards[i-1].OffCareCurrent();
                             }
                         }
                     }
                     else{
                         if(!IsPreviousLDCurrentOkStatic())
                         {
                             errorNumber++;
                             if(errorNumber>=errorMaxNumber){
                                 boards[0].LD[0].SystemAlarm(SYSTEMALARM,true);
                                 FLAG = true;
                                 this ->ForwardLaserStage(FastOff);
                             }
                         }
                         else{
                             errorNumber = 0;
                         }
                     }

              break;
            case Work_P03:// Laser system output
                if(restart == 1 ){
                    emit UI_Laser_State(LASER_LASE);    //面板显示出光功率
                    restart = 0;
                }
                if((!this ->IsLaserOn()) || this->IsScanning())
                {
                    if(!this->IsLaserOn()){
                        emit UI_Laser_State(SHUTDOWN);
                        m_SureShutdown = true;
                    }
                    else
                        emit UI_Laser_State(LASER_SCAN);
                    this ->ForwardLaserStage(SlowOff);
                }
                break;
            case Work_P08H:// Laser system output
                if(restart == 1 ){
                    emit UI_Laser_State(LASER_LASE);    //面板显示出光功率
                    restart = 0;
                }
//                if(FLAG==true){
//                    FLAG=false;
//                    if(FSwitch==3){
//                     boards[0].LD[1].SwitchSet(OFF);
//                     boards[2].LD[0].SwitchSet(OFF);
//                    }
//                }
                if((!this ->IsLaserOn()) || this->IsScanning())
                {
                    if(!this->IsLaserOn()){
                        emit UI_Laser_State(SHUTDOWN);
                        m_SureShutdown = true;
                    }
                    else
                        emit UI_Laser_State(LASER_SCAN);
                    this ->ForwardLaserStage(SlowOff);
                    FLAG=true;
                }
                break;
            case SlowOff:// jianxiao present pump 3 pump2 pump1  current
            if(!((Board::CurrentBoardIndex==0) && (BoardTemp->CurrentLDIndex==0))){
                  if(FLAG == true)
                   {
                       Curr_Temp =  LDTemp->GetRealCurr();
                       if(Curr_Temp<=0.01){
                           pencent = 1.0;
                       }
                       else
                           pencent = LDTemp->GetRealCurr()/Curr_Temp;
                       emit  updatabar("Stage 1 of 2",
                                            ProgressBase+
                                            100.0/(this->m_nTotalLD-1)*
                                            (1-pencent));
                       LDTemp->SetCurrByReg(0,CHANGESLOW);
                       FLAG = false;
                  }
                   else{
                      if(Curr_Temp<=0.01){
                          pencent = 1.0;
                      }
                      else
                          pencent = LDTemp->GetRealCurr()/Curr_Temp;
                      emit  updatabar("Stage 1 of 2",
                                           ProgressBase+
                                           100.0/(this->m_nTotalLD-1)*
                                           (1-pencent));
                       if(LDTemp->GetRealCurr()<=0.05)
                       {

                           LDTemp->SwitchSet(OFF);
                           ProgressBase = ProgressBase+100.0/(this->m_nTotalLD-1);
                           emit  updatabar("Stage 1 of 2",
                                                ProgressBase+
                                                100.0/(this->m_nTotalLD-1)*
                                                (1-pencent));
                           FLAG = true;
                           if(BoardTemp->CurrentLDIndex>0){
                                BoardTemp->CurrentLDIndex--;
                           }
                            else
                               Board::CurrentBoardIndex--;
                           FLAG = true;
                       }
                   }
                }
                else{
                    ProgressBase = 0;
                    this ->ForwardLaserStage( CheckP0 );
                }
                break;

            /******************************/
            case FastOff:// jianxiao present pump 3 pump2 pump1  current      //程序界面可能会停留在这个界面，
            if(!m_bModeLockedHandleFlag)
                return;
            m_bModeLockedHandleFlag = false;     //因为没有满足减电流，而且此时有没有检测按钮，因此不会改变状态，待完善。
            for(int i = Board::BoardNum; i>=1; i--){
                   boards[i-1].OffCurrent();            //save pump0 current
            }
            Board::CurrentBoardIndex = 0;
            if(boards[ Board::CurrentBoardIndex].LDNumber>1)
                boards[ Board::CurrentBoardIndex].CurrentLDIndex++; //next ld
            else
               Board::CurrentBoardIndex++; //      //next board

            SwitchPortLevel->Change_Led_state(false);
            emit UI_Laser_State(ERROR);
            ProgressBase = 0;
            WaitTime = 0;
            this ->ForwardLaserStage( AlarmStatus );
            break;

            /******************************/

            case CheckP0:// Check present pump0 voltage
                this ->ForwardLaserStage( OffP0 );
                FLAG = true;
                if(!m_SureShutdown){
                    if(m_isOffLock){
                        if(this ->IsLaserOn()){
                            emit UI_Laser_State(LAUNCH);
                            this ->ForwardLaserStage( LASER_INIT );
                        }
                        else{
                            emit UI_Laser_State(LASER_IDLE);
                            this ->ForwardLaserStage( LASER_IDLE );
                        }
                        m_isOffLock = false;
                    }
                }
                break;
            case OffP0:// Decrease pump0 voltage each CURRENTSTEP per second
                if(FLAG == true)
                {
                    Curr_Temp = LDTemp->GetRealCurr();
                    if(Curr_Temp<=0.01){
                        pencent = 1.0;
                    }
                    else
                        pencent = LDTemp->GetRealCurr()/Curr_Temp;
                    emit  updatabar("Stage 2 of 2",100*(1-pencent));
                    LDTemp->SetCurrByReg(0,CHANGESLOW);
                    //正常关机，从新回到LASER_IDLE状态
                    FLAG = false;
                }
                else{
                    if(Curr_Temp<=0.01){
                        pencent = 1.0;
                    }
                    else
                        pencent = LDTemp->GetRealCurr()/Curr_Temp;
                    emit  updatabar("Stage 2 of 2",100*(1-pencent));
                    if(LDTemp->GetRealCurr()<=0.05)
                    {
                        emit  updatabar("Stage 2 of 2",100);
                        LDTemp->SwitchSet(OFF);
                        this ->ForwardLaserStage(LASER_IDLE);
                        emit Set_ProgressBar(HIDEBAR);
                        emit UI_Laser_State(LASER_IDLE);
                        m_SureShutdown = false;
                        m_isUpdateUI = true;
                        m_isScanUpdate = true;
                        emit Protect(false);
                    }
                }
                break;
            case LASER_SCAN:
                    if(! this ->IsScanning() )
                    {
                          FLAG = true;
                          this ->ForwardLaserStage( 2);
                          m_isScanUpdate = false;
                    }
                    break;
            case LASERERROR:
                for(int i = Board::BoardNum; i>=1; i--){
                       boards[i-1].OffAllCurrent();
                }
                if(this->WorkMode==0){
                     this ->ForwardLaserStage(LASER_IDLE);
                    emit UI_Laser_State(LASER_IDLE);

                }
                break;
            default:break;
            }
    }
    return ;
}


void CSoftSwitch::StartModeSelection()
{
     m_byLaserStage =  LASER_IDLE;
     return ;
    switch(MachineMode)
    {
    case MachineN06:
        if(IsPreviousLDCurrentOk()){
                        emit UI_Laser_State(LASER_LASE);
                        m_byLaserStage =  READY;
        }
        else if(( boards[0].LD[0].GetRealCurrByReg()+ boards[0].LD[0].GetRealCurrByReg())>0.3){
            FLAG = true;
            emit UI_Laser_State(SHUTDOWN);
            m_byLaserStage =  SlowOff;
        }
        else
             m_byLaserStage =  LASER_IDLE;
        break;
   case MachineP03H:
    m_byLaserStage =  LASER_IDLE;break;
    CanDataStruct data;
    float LDOCurr;
    emit SystemLog("Start Mode selection");
   // cout<<lpLD[0].ID<<endl;
     boards[0].LD[0].Read_Set_Reg(4,2,BlockingRead,0,0,0,0,&data);
    LDOCurr = data.f_data;
    if(SwitchPortLevel->GetSwitchStatus() == '0'){
        if(LDOCurr > LD0MinCurr){
            m_byLaserStage =  SlowOff;
            m_nCurrentLDNo = ModuleNum - 1;
            IsShutdown = true;
            emit SystemLog("LD0 has Current and The switch is not pressed ,Enter Shutdown Mode");
            //cout<<"LD0 has Current and The switch is not pressed ,Enter Shutdown Mode"<<endl;
        }
        else{
            m_byLaserStage = LASER_IDLE;
            emit SystemLog("LD0 hasn't Current and The switch is not pressed ,Enter Idle Mode");
            //cout<<"LD0 hasn't Current and The switch is not pressed ,Enter Idle Mode"<<endl;
        }
    }
    else{
        CanDataStruct data;
        //read lastpmp current
         boards[Board::BoardNum-1].LD[0].Read_Set_Reg(0x04,0x02,BlockingRead,0,0,0,0,&data);
        float LastPumpCurr = data.f_data;
       // printf("%f\n",LastPumpCurr);
        if(LastPumpCurr <= LasingMinCurr){
             m_byLaserStage = LASER_IDLE;
             emit SystemLog("The switch is  pressed ,Enter Idle Mode");
             //cout<<"The switch is  pressed ,Enter Idle Mode"<<endl;
        }
        else{
             //cout<<"12"<<endl;
            restart = 1;
             m_byLaserStage = Work_P03;
             m_nCurrentLDNo = ModuleNum - 1;
             emit SystemLog("The switch is  pressed and LastPumpCurr > 2.0A ,Enter Lase out Mode");
          //   cout<<"The switch is  pressed and LastPumpCurr > 2.0A ,Enter Lase out Mode"<<endl;
        }
    }
    }
}

bool CSoftSwitch::HardwarePlatform()
{
    int iTemp,i,j;
    char lpwSecName[20];
    MachineMode = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_REGMAP,MachineP08,GS_INI_HARD_FILE);
    iTemp=GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_KEY_LD,0,GS_INI_HARD_FILE);
    m_nBoard = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_MODULE_MODULE,iTemp,GS_INI_HARD_FILE);
    boards =new Board[m_nBoard];
    Board::SetBoardNum(m_nBoard);
    m_nLDcnt=0;
    for(i=0; i<m_nBoard; i++)
    {   sprintf(lpwSecName,"%s%.1d","Module",i);
        iTemp=GetPrivateProfileInt(lpwSecName,GS_KEY_LD,1,GS_INI_HARD_FILE);
        boards[i].ExistLD=iTemp;
        boards[i].LD=new LDKeeper[iTemp];
        boards[i].CAN_ID=i+2;
        m_nLDcnt+=iTemp;
    }
//    cout<<"m_nLDcnt="<<m_nLDcnt<<endl;
    m_bdBoardArray= new BoardIndex[m_nLDcnt];
    iTemp=0;
    for(i=0;i<m_nBoard;i++)
    {   for(j=0;j<boards[i].ExistLD;j++)
        {   m_bdBoardArray[iTemp+j].BoardNo=i;
            m_bdBoardArray[iTemp+j].LDNo=j;
   //         cout<<"\r\n m_bdBoardArray["<<iTemp+j<<"]:BoarNo:"<<m_bdBoardArray[iTemp+j].BoardNo<<",LDNo:"<<m_bdBoardArray[iTemp+j].LDNo<<endl;
        }
        iTemp+=boards[i].ExistLD;
    }
    m_nOutPut  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_OUTPUT,1,GS_INI_HARD_FILE);
    m_nLevel  = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LEVEL,1,GS_INI_HARD_FILE);
    if(m_nOutPut<=1)
    {
        m_nCommLD=m_nLDcnt;
 //       m_nCommModule=m_nLDcnt;
    }
    else
    {
        m_nCommLD=(m_nOutPut*(m_nLevel+1)-m_nLDcnt)/(m_nOutPut-1);
    }

    m_nSwapLevel=GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_MODULE_SWAPLEVEL,0,GS_INI_HARD_FILE);
    int nToModifyLDcnt=GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_MODULE_MODIF,0,GS_INI_HARD_FILE);
 //   cout<<"\r\n m_nCommLD="<<m_nCommLD<<",m_nOutPut="<<m_nOutPut<<",nToModifyLDcnt="<<nToModifyLDcnt<<",m_nSwapLevel="<<m_nSwapLevel<<endl;
    //    if((m_nLDcnt-m_nLevel-1)<(nToModifyLDcnt+1)*(m_nOutPut-1))
//        nToModifyLDcnt=(m_nLDcnt-m_nLevel-1)/(m_nOutPut-1)-1;
//    if(nToModifyLDcnt<0)
//        nToModifyLDcnt=0;
    m_nTomodifyPumpNo=nToModifyLDcnt;
    m_pLastButPump= new LDKeeper **[m_nOutPut];
    m_pLastPump=new LDKeeper **[m_nOutPut];
    m_nLastPumpNo=new int[m_nOutPut];

    for(i=0;i<m_nOutPut;i++)
    {   m_nLastPumpNo[i]=m_nLDcnt-m_nOutPut+i;
        m_pLastPump[i]=new LDKeeper *[m_nSwapLevel];
        m_pLastButPump[i]=new LDKeeper *[m_nTomodifyPumpNo];
        for(int j=0;j<=m_nSwapLevel;j++)
        {   iTemp=m_nLDcnt-m_nOutPut*(j+1)+i;
            m_pLastPump[i][j]=&boards[m_bdBoardArray[iTemp].BoardNo].LD[m_bdBoardArray[iTemp].LDNo];
  //          cout<<"\r\n Get Last Pump poiter ["<<i<<"]["<<j<<"]= "<<iTemp<<endl;
        }
        for(int j=0;j<m_nTomodifyPumpNo;j++)
        {   iTemp=m_nLDcnt-m_nOutPut+i-(nToModifyLDcnt-j)*m_nOutPut;
            m_pLastButPump[i][j]=&boards[m_bdBoardArray[iTemp].BoardNo].LD[m_bdBoardArray[iTemp].LDNo];
//            cout<<"\r\n Get Last ButPump poiter ["<<i<<"]["<<j<<"]= "<<iTemp<<endl;
        }
    }


 //  cout<<"\r\n m_nSwapLevel="<<m_nSwapLevel<<",m_nTomodifyPumpNo="<<m_nTomodifyPumpNo<<endl;
    return true;
}




void CSoftSwitch::ExitTimerThread()
{
	m_bIsRunThread = false;
}
void CSoftSwitch::SetLockSt(bool Status)
{
	m_bModeLocked=Status;
}

CSoftSwitch::~CSoftSwitch()
{
    m_hSwitchTimerProcessThread = NULL;
    m_bLaserOn = false;
    m_bSelfScan = false;
    m_bModeLocked = false;
    m_byLaserStage = LASER_IDLE;
    m_bIsRunThread = FALSE;
    m_nWaitProgress = 0;
    m_nTotalLD = 0;

    delete CanRecv;
    delete Heartbeat;
    delete SwitchPortLevel;
    delete[] lpLD;
    delete LD;
    delete hotSwap;
    delete LDMap;
}

