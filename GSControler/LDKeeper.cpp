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

extern float InitFreq;  //初始化界面频率
extern float InitPlus;   //初始化界脉宽
extern PortState * por;     //PIN

float MinPW  ;
int MaxPW   ;
float MaxPower  ;
int MinFreq   ;
int MaxFreq   ;
int MAXMJ     ;
float FreqLimit;
extern float precurr;
extern int EnMOFlag;
extern int EnPWFlag;
extern Board * boards;

//在发送数据时会判断数据是否发送成功，现在取消perror打印（No buffer space available）
using namespace std;
extern CSoftSwitch * cs;
extern LDKeeper * lpLD_temp;       //模块对象
LDKeeper::LDKeeper()
{
    this->Real_current = 0;
    this->Real_temperature = 0;
    this->Init_curr = 0;
    this->Init_temp = 0;
    this->PresetPower = 0;
    this->usersetpw = 0;
    this->usersetfreq = 0;
    this->ID = 0;

    P2C[0] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_P2CA,0,GS_INI_FILE);
    P2C[1] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_P2CB,0,GS_INI_FILE);
    P2C[2] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_P2CC,0,GS_INI_FILE);
    P2C[3] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_P2CD,0,GS_INI_FILE);
    C2P[0] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_C2PA,0,GS_INI_FILE);
    C2P[1] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_C2PB,0,GS_INI_FILE);
    C2P[2] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_C2PC,0,GS_INI_FILE);
    C2P[3] = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_C2PD,0,GS_INI_FILE);
    MinC = GetPrivateProfileDouble(GS_INI_SECTION_ADJUST,GS_INI_KEY_T_MINC,1.5,GS_INI_FILE);

}
/*
LDKeeper & LDKeeper::operator = (const LDKeeper& RightSides)
{
    if (this == &RightSides) // 如果自己给自己赋值则直接返回
    {
    return *this;
    }
    nSize=RightSides.nSize; //复制常规成员
    char *temp=new char[nSize]; //复制指针指向的内容
    memcpy(temp,RightSides.pBuffer,nSize*sizeof(char));
    delete []pBuffer; //删除原指针指向内容(将删除操作放在后面，避免X=X特殊情况下，内容的丢失)
    pBuffer=NULL;
    pBuffer=temp; //建立新指向
    return *this
}
*/
void LDKeeper::Init(float pw,float freq)
{
    this->usersetpw = pw;
    this->usersetfreq = freq;
}

LDKeeper::LDKeeper(int _ID)
{
    this->ID = _ID;
}


void LDKeeper::SetLDRowColumn(int id)
{
    if(id<8)
        output = 1;
    else
       output = id%8+1;
    level = id/8;
}

int LDKeeper::GetLevel()
{
    return level;
}

int LDKeeper::GetOutput()
{
    return output;
}

int LDKeeper::SetLevel(int _level)
{
   level = _level;
}

int LDKeeper::SetOutput(int _output)
{
    output = _output;
}

void LDKeeper::SetIndex(int _index)
{
    index = _index;
}
int LDKeeper::GetIndex()
{
    return index;
}

LDKeeper::~LDKeeper()
{

}

//设置各级开关
void LDKeeper::SetSwitch(unsigned char  status)
{
    struct can_frame frdup;
    frdup.can_id = ((SET_SWITCH_STATE<<7)+this->ID);
    frdup.can_dlc = 1;
    frdup.data[0] = status;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void LDKeeper::SetSwitch(unsigned char SelectModule, unsigned char status)
{
    struct can_frame  frdup;

    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectModule == MLD1)
        frdup.data[0] = 5;  //LD1
    else
        frdup.data[0] = 1;
    frdup.data[1] = 0;
    frdup.data[2] = 0;
    frdup.data[3] = 0;
    if(status == ON)
        frdup.data[4] = 0x0f;
    else
        frdup.data[4] = 0x02;
    frdup.data[5] = 0;
    frdup.data[6] = 0;
    frdup.data[7] = 0;
    write(Can_LD::s,&frdup,sizeof(frdup));
}
//
void LDKeeper::MolockSwitch(unsigned char status)
{
    struct can_frame frdup;
    frdup.can_id = ((SET_SWITCH_STATE<<7)+this->ID);

    frdup.can_dlc = 1;
  //  frdup.data[0] = status;
    if(status == ON)
        frdup.data[0] = 0x0f;
    else{
        if((Subscript == 2) && (ID == 2))      //sanheyi LD1
            frdup.data[0] = 0x2b;  //LD1
        else
            frdup.data[0] = 0x02;

    }
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void LDKeeper::SwitchSet(unsigned char status)
{
    struct can_frame  frdup;

    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if((index == 1) && ( ID == 2)){
        frdup.data[0] = 5;  //LD1
    }
    else
        frdup.data[0] = 1;
    frdup.data[1] = 0;
    frdup.data[2] = 0;
    frdup.data[3] = 0;
    if(status == ON)
        frdup.data[4] = 0x0f;
    else
        frdup.data[4] = 0x02;
    frdup.data[5] = 0;
    frdup.data[6] = 0;
    frdup.data[7] = 0;
    write(Can_LD::s,&frdup,sizeof(frdup));

}
//设置节点ID
void LDKeeper::SetID(int _ID)
{
    this->ID = _ID;
}
//返回节点ID
int LDKeeper::GetID()
{
    return this->ID;
}
//获得当前电流
float LDKeeper::GetRealCurr()
{
  //cout<<"this"<<this->Real_current<<endl;
    return this->Real_current;
}

float LDKeeper::GetCurrAimCurr()
{
    CanDataStruct data;
    Read_Set_Reg(3,0,BlockingRead,0,0,0,0,&data);
    return data.f_data;
}

//获得当前电流
float LDKeeper::GetRealCurrByReg()
{
   CanDataStruct data;
   Read_Set_Reg(4,2,BlockingRead,0,0,0,0,&data);
   return data.f_data;
}
//获得当前温度
float LDKeeper::GetRealTemp()
{
    return this->Real_temperature;
}
//设置当前电流
void LDKeeper::SetRealCurr(float _curr)
{
    this->Real_current = _curr;
}
//设置当前温度
void LDKeeper::SetRealTemp(float _temp)
{
    this->Real_temperature = _temp;
}
//设置初始化电流
void LDKeeper::SetInitCurr(float _curr)
{
    this->Init_curr = _curr;
}
//设置初始化温度
void LDKeeper::SetInitTemp(float _temp)
{
    this->Init_temp = _temp;
}

float LDKeeper::GetAimCurr()
{
    return this->Init_curr;
}

float LDKeeper::GetAimTemp()
{
    return this->Init_temp;
}
float LDKeeper::GetPadTemp()
{
    return this->PadTemperature;
}

float LDKeeper::GetAirTemp()
{
    return this->AirTemperature;
}

void LDKeeper::SetPadTemp(float _PadTemp)
{
    this->PadTemperature = _PadTemp;
}

void LDKeeper::SetAirTemp(float _AirTemp)
{
    this->AirTemperature = _AirTemp;
}

bool LDKeeper::CurrOk()
{
  //  if(((this->Real_current - this->Init_curr)>0.02 )||((this->Real_current - this->Init_curr)>-0.02 ) )
      if(((this->Real_current - this->Init_curr)>=0 ) || ((this->Init_curr - this->Real_current) < 0.1 ) )
        return true;
    return false;
}

//在线
void LDKeeper::OnLine()
{
    this->LinkStatus = true;
}

//下线
void LDKeeper::DownLine()
{
    this->LinkStatus = false;
}
//返回在线状态
bool LDKeeper::GetLinkStatus()
{
    return this->LinkStatus;
}
//设置/读寄存器 add为寄存器地址 data1 = 0xff 读寄存器 当data1 != 0xff data2 data3为设置寄存器值
void LDKeeper::Read_Set_Reg(unsigned char add1,unsigned char add0,
                             unsigned char flag,
                             unsigned char data1,
                             unsigned char data2,
                             unsigned char data3,
                             unsigned char data4,
                             P_CanDataStruct data)
{

  //  mutex.lock();
 //   cout<<"lock"<<endl;
  //  printf("%d %d\n",add1,add0);
   // printf("s--%0x\n",this->ID);
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    frdup.data[0] = add1;
    frdup.data[1] = add0;
    frdup.data[2] = flag;
    frdup.data[3] = 0;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
    if(flag == BlockingRead)       //Blocking read
    {
         int count = 0;
         while((this->Reg != ((add1<<8)+add0)) && (count <= TIMEOUTT)){    //1sm没有数据返回超时
             usleep(1000);   //1ms
             count++;
         }
         if(count > TIMEOUTT){
             if(data != NULL){
                 data->s_data = -1;
                 data->f_data = -1;
             }
        //   printf("---wait reg return error..%d. %d  %d....\n",count,this->ID,this->Subscript);
         }
         else{
         //  printf("---wait reg return ok..%d.....\n",count);
     //    this->Reg = -1;
             if(data != NULL){
                 data->s_data = this->RegData.s_data;
                 data->f_data = this->RegData.f_data;
             }
         }
       //  cout<<data->s_data<<endl;
     //    this->Reg = -1;
      //   return ;
    }
    this->Reg = -1;
 //   cout<<"unlock"<<endl;
 //   mutex.unlock();
}

void LDKeeper::RegInfo(unsigned char addr1,unsigned char addr0,
                       unsigned char data1,unsigned char data2,
                       unsigned char data3,unsigned char data4)
{
    char * temp;
    temp = (char *)&this->RegNum;
    temp[0] = data1;
    temp[1] = data2;
    temp[2] = data3;
    temp[3] = data4;
    this->RegData.s_data =  ((data2<<8)+data1);
    this->RegData.f_data = this->RegNum;
    this->Reg = ((addr1<<8)+addr0);
}
#define Int_ESTOP           0x01
#define Int_PLLunlock 		0x02
#define Int_pulse_width		0x04
#define Int_Freqlmt 		0x08
#define Int_Dinlat 			0x10
#define Int_MO    			0x20
#define Int_AP    			0x40
extern CSoftSwitch * cs;
void LDKeeper::HandleAlarm(unsigned char data1, unsigned char data2)
{
        if((data1&Int_MO&EnMOFlag) == Int_MO){
            cs->MoSignal(data2&0x01);
            if(data2&0x01){
              //  printf("MoSignal--1----\n");
                //set current to 0
                boards[0].LD[0].SetCurrByReg(boards[0].LD[0].GetAimCurr(),0);
                boards[1].LD[0].SetCurrByReg(boards[1].LD[0].GetAimCurr(),0);
                boards[2].LD[0].SetCurrByReg(precurr,CHANGEWITHAP);
                boards[3].LD[0].SetCurrByReg(precurr,CHANGEWITHAP);
            }
            else{
                //set current
             //    printf("MoSignal--0----\n");
                for(int i=Board::BoardNum; i>=1; i--){
                //    if(lpLD_temp[i].GetRealCurr()>0.1){
                        boards[i-1].LD[0].SetCurrByReg(0,0);
               //     }
                }
               // precurr = 0;
            }
        }
        if((data1&Int_ESTOP) == Int_ESTOP){
              //  Ints = 1;
        }
        if((data1&Int_PLLunlock) == Int_PLLunlock){
             //   Ints += (1<<1);
        }
        if((data1&Int_pulse_width) == Int_pulse_width){
         //   printf("Handle_INT_PW\n");
            Handle_INT_PW();
        }
        if((data1&Int_Freqlmt) == Int_Freqlmt){
         //   printf("HandleFlimtALarm\n");
            HandleFlimtALarm();
        }
        if((data1&Int_Dinlat) == Int_Dinlat){
        //    printf("HandleLatch\n");
            HandleLatch();
        }


        if((data1&Int_AP) == Int_AP){
             printf("ApSignal-%d\n",(data2>>1)&0x01);
       //     cs->ApSignal((data2>>1)&0x01);
        }
}
void LDKeeper::Handle_INT_PW()
{
    float pw;
    float newpw;
    float equpw;
    float Fmin;
    float Fmax;
    float freq;
    float power;
    float Pulseenergy;
    float percentage;
    //get pulse width
    pw = GetDB25PulseWidth();
    if(pw<0.5)
        pw = GetPulseWidth();
    if(pw<MinPW)
        pw=MinPW;
    else if(pw>MaxPW){
        pw = MaxPW;
    }
    //2 jiaozhun

    newpw = CorrectPulseWidth(pw);
    //3 set 12/13reg
    SetPulseWidth(newpw);
    //set ld0 current
    SetLD0CurrentWithPw(pw);
    //4 修改频率设定
    equpw = EqualityPulseWidth(pw);
    Fmin = 20 / (equpw * 0.1333) ;
    if(Fmin<MinFreq)
        Fmin = MinFreq;
    SetMinFreq(Fmin);
    //5 功率设定
    freq = GetDB25Freq();
     Pulseenergy=20*MaxPowerPw(pw);
     if(Pulseenergy  >MAXMJ){
             Pulseenergy=MAXMJ;
     }
     power = Pulseenergy *freq/1000;
     if(power>MaxPower){
         power = MaxPower;
     }
   // power = 20 * /*pw*/MaxPowerPw(pw) *freq;
    percentage = GetDin()/255.0;
    SetPower(power*percentage);

}

void LDKeeper::Handle_INT_FREQ()
{
    float pw;
    float freq;
    float power;
    float percentage;
    float Pulseenergy;
    //get pulse width
    pw = GetDB25PulseWidth();
    if(pw<0.5)
        pw = GetPulseWidth();
    if(pw<MinPW)
        pw=MinPW;
    else if(pw>MaxPW){
        pw = MaxPW;
    }
    //5 功率设定
    freq = GetDB25Freq();
    Pulseenergy=20*MaxPowerPw(pw);
    if(Pulseenergy  >MAXMJ){
            Pulseenergy=MAXMJ;
    }
    power = Pulseenergy *freq/1000;
    if(power>MaxPower){
        power = MaxPower;
    }
 //   power = 20 * /*pw*/MaxPowerPw(pw) *freq;
    percentage = GetDin()/255.0;
    SetPower(power*percentage);
}

unsigned char LDKeeper::Handle_Uart_INT_PW(float pw)
{
    float newpw;
    float equpw;
    float Fmin;
    float Fmax;
    float freq;
    float power;
    float Pulseenergy;
    float percentage;
    //get pulse width
    //pw = GetDB25PulseWidth();
    //2 jiaozhun
    if((pw<MinPW)||(pw>MaxPW))
        return 2;

    newpw = CorrectPulseWidth(pw);
    //3 set 12/13reg
    SetPulseWidth(newpw);
  //  usleep(100000);
    //set ld0 current
    SetLD0CurrentWithPw(pw);
 //    usleep(100000);
    //4 修改频率设定
    equpw = EqualityPulseWidth(pw);
    Fmin = 20 / (equpw * 0.1333) ;
    if(Fmin<20)
        Fmin = 20;
    SetMinFreq(Fmin);
 //    usleep(100000);
    //5 功率设定
    freq = GetDB25Freq();
    if(freq<GetMinFreq())
        freq = GetMinFreq();
    Pulseenergy=20*MaxPowerPw(pw);
    if(Pulseenergy  >MAXMJ){ //1250
            Pulseenergy=MAXMJ;
    }
    power = Pulseenergy *freq/1000;
    if(power>MaxPower){
        power = MaxPower;
    }//
   // power = 20 * pw *freq;
 //    usleep(100000);
    percentage = GetDin()/255.0;
    SetPower(power*percentage);
}

void LDKeeper::SetFreqLimitWithPW(float pw)
{
    float newpw;
    float equpw;
    float Fmin;
    float Fmax;
    if(pw<MinPW)
        pw=MinPW;
    else if(pw>MaxPW){
        pw = MaxPW;
    }
    newpw = CorrectPulseWidth(pw);
    //3 set 12/13reg
 //    printf("pw:%.3f new:%.3f\n",pw,newpw);
    SetPulseWidth(newpw);
    //4 修改频率设定
    equpw = EqualityPulseWidth(pw);
    Fmin = 20 / (equpw * 0.1333) ;
    if(Fmin<20)
        Fmin = 20;
    Fmax = 20 / 20 / pw*1000;
    if(Fmax>1000)
        Fmax = 1000;
    SetMinFreq(Fmin);
    SetMaxFreq(Fmax);
}
void LDKeeper::HandleLatch()
{
    float freq;
    float pw;
    float power;
    float Pulseenergy;
    unsigned char pre = GetDin();
    float percentage = pre/255.0;
    freq = GetDB25Freq();
    pw = GetDB25PulseWidth();
    if(pw<0.5)
        pw = GetPulseWidth();
    Pulseenergy=20*MaxPowerPw(pw);
    if(Pulseenergy  >MAXMJ){
            Pulseenergy=MAXMJ;
    }
    power = Pulseenergy *freq/1000;
    if(power>MaxPower){
        power = MaxPower;
    }
  //  power = 20 * pw *freq/1000;
    SetPower(power*percentage);
}
void LDKeeper::HandleFlimtALarm()
{
    unsigned alarm = 0;
    alarm = GetFlimtALarm();
    if(alarm&0x01){
        //lower alarm
    }
    else{
        //upper alarm
    }
}
//设置浮点型电流
void LDKeeper::Set_FloatSurrent(float iTarget)
{
    char * p;
    p = (char *)&iTarget;
    struct can_frame  frdup;
    frdup.can_id = ((SET_CURRENT<<7)+this->ID);
    frdup.can_dlc = 4;
    frdup.data[0] = *p;
    frdup.data[1] = *(p+1);
    frdup.data[2] = *(p+2);
    frdup.data[3] = *(p+3);
    write(Can_LD::s,&frdup,sizeof(frdup));

}

//设置浮点型电流
void LDKeeper::Set_FloatTemp(float tTarget)
{
    char * p;
    p = (char *)&tTarget;
    struct can_frame  frdup;
    frdup.can_id = ((SET_TEMP<<7)+this->ID);
    frdup.can_dlc = 4;
    frdup.data[0] = *p;
    frdup.data[1] = *(p+1);
    frdup.data[2] = *(p+2);
    frdup.data[3] = *(p+3);
    write(Can_LD::s,&frdup,sizeof(frdup));

}


//设置电流
void LDKeeper::Set_Curr(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4)
{
    struct can_frame  frdup;
    frdup.can_id = ((SET_CURRENT<<7)+this->ID) ;
    frdup.can_dlc = 4;
    frdup.data[0] = data1;
    frdup.data[1] = data2;
    frdup.data[2] = data3;
    frdup.data[3] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void LDKeeper::SetTempByReg(float temp)
{
    struct can_frame  frdup;
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&temp;
    data1 = *p;
    data2 = *(p+1);
    data3 = *(p+2);
    data4 = *(p+3);
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if((index == 1) && ( ID == 2)){
        frdup.data[0] = 7;  //LD1
    }
    else
        frdup.data[0] = 3;
    frdup.data[1] = 2;
    frdup.data[2] = 0;
    frdup.data[3] = 0;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));

}
void LDKeeper::SetCurrByReg(float curr,unsigned char flag)
{
    struct can_frame  frdup;
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&curr;
    data1 = *p;
    data2 = *(p+1);
    data3 = *(p+2);
    data4 = *(p+3);
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if((index == 1) && ( ID == 2)){
        frdup.data[0] = 7;  //LD1
    }
    else
        frdup.data[0] = 3;
    frdup.data[1] = 0;
    frdup.data[2] = 0;
    frdup.data[3] = flag;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));

}

void LDKeeper::SetCurrByReg(unsigned char id,float curr,char flag)
{
    struct can_frame  frdup;
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&curr;
    data1 = *p;
    data2 = *(p+1);
    data3 = *(p+2);
    data4 = *(p+3);
    frdup.can_id = ((SET_REG<<7)+id);
    frdup.can_dlc = 8;
    frdup.data[0] = 3;
    frdup.data[1] = 0;
    frdup.data[2] = 0;
    frdup.data[3] = flag;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));

}


void LDKeeper::SetCurrByReg(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4,unsigned char flag)
{
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if((index == 1) && ( ID == 2)){
        frdup.data[0] = 7;  //LD1
    }
    else{
        frdup.data[0] = 3;
    }
    frdup.data[1] = 0;
    frdup.data[2] = 0;
    frdup.data[3] = flag;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));

}

void LDKeeper::SetTempByReg(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4,unsigned char flag)
{
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if((index == 1) && ( ID == 2)){
        frdup.data[0] = 7;  //LD1
    }
    else{
        frdup.data[0] = 3;
    }
    frdup.data[1] = 2;
    frdup.data[2] = 0;
    frdup.data[3] = flag;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));

}


//设置温度
void LDKeeper::Set_Temp(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4)
{
   // printf("%0x\n",this->ID);
    struct can_frame  frdup;
    frdup.can_id = ((SET_TEMP<<7)+this->ID) ;
    frdup.can_dlc = 4;
    frdup.data[0] = data1;
    frdup.data[1] = data2;
    frdup.data[2] = data3;
    frdup.data[3] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

//update module program
void LDKeeper::SendUpdateDate(unsigned char addr1, unsigned char addr2,
                              unsigned char data1, unsigned char data2,
                              unsigned char data3, unsigned char data4,
                              unsigned char data5, unsigned char data6)
{
    struct can_frame  frdup;
    frdup.can_id = ((UPDATEPROGRAM<<7)+this->ID) ;
  //  frdup.can_id = ((0x0d<<7)+this->ID) ;
    frdup.can_dlc = 8;
    frdup.data[0] = addr1;
    frdup.data[1] = addr2;
    frdup.data[2] = data1;
    frdup.data[3] = data2;
    frdup.data[4] = data3;
    frdup.data[5] = data4;
    frdup.data[6] = data5;
    frdup.data[7] = data6;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

bool LDKeeper::IsSanHeYi()
{
    CanDataStruct data;
   // Read_Set_Reg(addr2,addr3,BlockingRead,0,0,0,0,&data);
    if(data.s_data == 0x0027){
        return true;
    }
    else
        return false;
}//5

void LDKeeper::SetMinFreq(float freq)
{
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&freq;
    freq = CalculateWritePeriod(freq); //new add by wt
    data1 = *p;
    data2 = *(p+1);
    data3 = *(p+2);
    data4 = *(p+3);
    Read_Set_Reg(13,0,WriteReg,data1,data2,data3,data4,NULL);

}
float LDKeeper::GetMinFreq()
{
    CanDataStruct data;
    Read_Set_Reg(13,0,BlockingRead,0,0,0,0,&data);
    data.f_data = CalculatePeriod(data.f_data); //bew add by wt
    return data.f_data;
}
void LDKeeper::SetMaxFreq(float freq)
{
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&freq;
    freq = CalculateWritePeriod(freq); //new add by wt
    data1 = *p;
    data2 = *(p+1);
    data3 = *(p+2);
    data4 = *(p+3);
    Read_Set_Reg(13,2,WriteReg,data1,data2,data3,data4,NULL);
}
float LDKeeper::GetMaxFreq()
{
    CanDataStruct data;
    Read_Set_Reg(13,2,BlockingRead,0,0,0,0,&data);
    data.f_data = CalculatePeriod(data.f_data); //bew add by wt
    return data.f_data;
}

void LDKeeper::SetPulseWidth(float pulse)
{
    unsigned char data1;
    unsigned char data2;
    unsigned char data3;
    unsigned char data4;
    unsigned char * p = (unsigned char *)&pulse;
    unsigned char burst = 0;
    unsigned char mode = 0;
    float SinglePulseWidth = 0;
    float RealWidth = pulse;
    if(pulse>8){	//8ns
        if(EnPWFlag == 0){		//old pulse mode
            Read_Set_Reg(0,50,WriteReg,1,data2,data3,data4,NULL);
            Read_Set_Reg(0,51,WriteReg,0,data2,data3,data4,NULL);
            data1 = *p;
            data2 = *(p+1);
            data3 = *(p+2);
            data4 = *(p+3);
            Read_Set_Reg(14,0,WriteReg,data1,data2,data3,data4,NULL);
            this->usersetpw = pulse;
        }
        else{												//new pulse mode
            burst = RealWidth/4.0f;
            SinglePulseWidth = RealWidth/burst;
            mode = GetModeWithSinglePW(SinglePulseWidth);
            Read_Set_Reg(0,50,WriteReg,burst,data2,data3,data4,NULL);
            Read_Set_Reg(0,51,WriteReg,mode,data2,data3,data4,NULL);
            this->usersetpw = pulse;
        }
    }
    else{
        Read_Set_Reg(0,50,WriteReg,1,data2,data3,data4,NULL);
        Read_Set_Reg(0,51,WriteReg,0,data2,data3,data4,NULL);
        data1 = *p;
        data2 = *(p+1);
        data3 = *(p+2);
        data4 = *(p+3);
        Read_Set_Reg(14,0,WriteReg,data1,data2,data3,data4,NULL);
        this->usersetpw = pulse;
    }
}

float LDKeeper::GetPulseWidth()
{
    CanDataStruct data;
    unsigned char burst = 0;
    float wd = 0;
    Read_Set_Reg(0,50,BlockingRead,0,0,0,0,&data);  //read fpga reg  R_Burst
    burst = data.s_data;
    if(!EnPWFlag){	//old mode
        Read_Set_Reg(14,0,BlockingRead,0,0,0,0,&data);
        return data.f_data;
    }
    else{
        wd = burst*GetSinglePWWithMode();
    }
    return wd;
}


float LDKeeper::GetDB25Freq()
{
    CanDataStruct data;
    float coefficient = 0;
    Read_Set_Reg(16,0,BlockingRead,0,0,0,0,&data);
    if(data.f_data<FreqLimit){
        data.f_data = GetFrequencyPeriod();
    }
    if(data.f_data<MinFreq)
        data.f_data = MinFreq;
    if(data.f_data>MaxFreq)
        data.f_data=MaxFreq;
    this->usersetfreq = CalculatePeriod(data.f_data);
    return this->usersetfreq;
}
float LDKeeper::GetDB25PulseWidth()
{
    CanDataStruct data;
    Read_Set_Reg(15,0,BlockingRead,0,0,0,0,&data);
    return data.f_data;
}
unsigned char LDKeeper::GetDin()
{
    CanDataStruct data;
    Read_Set_Reg(0,30,BlockingRead,0,0,0,0,&data);
    return data.s_data&0xff;
}

float  LDKeeper::EqualityPulseWidth(float pw)
 {
     unsigned int intpw;
     intpw = pw*10;
     if(pw>=3)
         return pw;
     else{
         switch(intpw)
         {
         case 8:    return 0.32;
         case 10:   return 0.5;
         case 12:   return 0.72;
         case 15:   return 1.05;
         case 19:   return 1.52;
         case 24:   return 2.16;
         default:   if(intpw<8)
                        return 0.32;
                     else if(intpw<10)
                         return 0.5;
                     else if(intpw<12)
                         return 0.72;
                     else if(intpw<15)
                         return 1.05;
                     else if(intpw<19)
                         return 1.52;
                     else if(intpw<24)
                         return 2.16;
         }
     }
 }

float LDKeeper::MaxPowerPw(float pw)
{
    return pw;
}


unsigned char LDKeeper::SetPowerPercentage(float percentage)
{
    float power;
    float pw = GetDB25PulseWidth();
    if(pw<0.5)
        pw = GetPulseWidth();
    if(pw<MinPW)
        pw=MinPW;
    else if(pw>MaxPW){
        pw = MaxPW;
    }
    //5 功率设定
    float freq = GetDB25Freq();
    float Pulseenergy=20*MaxPowerPw(pw);
    if(Pulseenergy  >MAXMJ){
            Pulseenergy=MAXMJ;
    }
    power = Pulseenergy *freq/1000;
  //  printf("%.3f %.3f %.3f\n",pw,freq,power);
    if(power>MaxPower){
        power = MaxPower;
    }

    SetPower(power*percentage);
    return 0;
}
//float precurr;

 unsigned char LDKeeper::SetPower(float power)
 {
     if(cs->CheckLaserStage()!=READY){
         AlarmUnReady(1);
         return 1;
     }
     if(!EnMOFlag){
         if(!cs->IsPreviousLDCurrentOk()){
             SystemAlarm(SYSTEMALARM,true);
             cs->ForwardLaserStage(READYOFF);
             return 1;
         }
     }
     this->PresetPower = power;
     float current;
     current = power2current(power);
     if(!EnMOFlag){
         boards[2].LD[0].SetCurrByReg(current,CHANGEWITHAP);
         boards[3].LD[0].SetCurrByReg(current,CHANGEWITHAP);
     }
     precurr = current;
     return 0;
}

float LDKeeper::GetPower()
{
     float current;
     float power;
     current = ( boards[2].LD[0].GetRealCurr()+ boards[3].LD[0].GetRealCurr())/2;
     power = current2power(current);
     return power;

}

float LDKeeper::GetPresetPower()
{
    return this->PresetPower;
}

float LDKeeper::GetPresetPulse()
{
    return this->usersetpw;
}

float LDKeeper::GetPresetFreq()
{
    return this->usersetfreq;
}

 unsigned char  LDKeeper::GetDB25Alarm()
 {
     CanDataStruct data;
     Read_Set_Reg(0,31,BlockingRead,0,0,0,0,&data);
     return data.s_data;
 }
 void  LDKeeper::SetDB25Alarm(unsigned char alarm)
 {
     Read_Set_Reg(0,31,WriteReg,alarm,0,0,0,NULL);
 }

 unsigned char  LDKeeper::GetFlimtALarm()
 {
     CanDataStruct data;
     Read_Set_Reg(0,32,BlockingRead,0,0,0,0,&data);
     return data.s_data;
 }

 void LDKeeper::SetLD0CurrentWithPw(float pw)
 {
     float current;
     current = Pw2Current(pw);
    // printf("set curr:%.3f",current);
     SetCurrByReg(current);
 }

 void  LDKeeper::AlarmUnReady(unsigned char status)
 {
     CanDataStruct data;
     Read_Set_Reg(0,31,BlockingRead,0,0,0,0,&data);
     if(status)
         data.s_data = data.s_data&(status<<3);
     else
         data.s_data = data.s_data&~(status<<3);
     Read_Set_Reg(0,31,WriteReg,data.s_data&0xff,0,0,0,&data);
 }

 void  LDKeeper::SystemAlarm(unsigned char source,bool status)
 {
     CanDataStruct data;
     unsigned char index=0;
     Read_Set_Reg(0,31,BlockingRead,0,0,0,0,&data);
     switch(source)
     {
     case 0: index=0;break;
     case 1: index=1;break;
     case 2: index=2;break;
     case 3: index=3;break;
     case 4: index=4;break;
     }
     if(status)
         data.s_data = data.s_data|(status<<index);
     else
         data.s_data = data.s_data&~(status<<index);
     Read_Set_Reg(0,31,WriteReg,data.s_data&0xff,0,0,0,&data);
 }


float LDKeeper::Pw2Current(float pw)
{
    if((0.8<=pw)&&(pw<3))
        return 3.1;
    else if((3<=pw)&&(pw<8))
        return 2.8;
    else if((8<=pw)&&(pw<20))
        return 2.6;
    else if((20<=pw)&&(pw<500))
        return 2.4;
    return 1.8;
}
float psz[17] = {1.02,3.66,6.62,9.98,13,16.26,19.34,22.96,25.9,29.37,32.33,35.2,38.5,41.4,44.3,47.5,49.9};
float csz[17] = {1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5};
float  LDKeeper::power2current(float power)
{
     float curr = P2C[0]*power*power*power+P2C[1]*power*power+P2C[2]*power+P2C[3];
     return curr;

}

float  LDKeeper::current2power(float current)
{
    if(current<MinC)
        current=MinC;
    float power = C2P[0]*current*current*current+C2P[1]*current*current+C2P[2]*current+C2P[3];
    return power;

}

float LDKeeper::CorrectPulseWidth(float pw)
{
   return pw;
}

float LDKeeper::GetFrequencyPeriod()
{
    CanDataStruct data;
    Read_Set_Reg(17,0,BlockingRead,0,0,0,0,&data);
    return data.f_data;

}

/*--------------new pulse width-----------------------*/

unsigned char LDKeeper::GetModeWithSinglePW(float SinglePulseWidth)
{
    //SinglePulseWidth = 4+ 0.2*mode;
    return (((SinglePulseWidth-4.0f)>0?(SinglePulseWidth-4.0f):0)*5)+0.5f;
}

float LDKeeper::GetSinglePWPeriodWithMode(void)
{
    //SinglePulsePeriod = 8+ 0.4*mode;
    CanDataStruct data;
    unsigned char mode = 0;
    Read_Set_Reg(0,51,BlockingRead,0,0,0,0,&data);  //read fpga reg  R_Mode
    mode = data.s_data;
    return (8+0.4f*mode);
}

float LDKeeper::GetSinglePWWithMode(void)
{
    //SinglePulsePeriod = 4+ 0.2*mode;
    CanDataStruct data;
    unsigned char mode = 0;
    Read_Set_Reg(0,51,BlockingRead,0,0,0,0,&data);  //read fpga reg  R_Mode
    mode = data.s_data;
    return (4+0.2f*mode);
}

unsigned long LDKeeper::CalculatePeriod(unsigned long freq)
{
    float coefficient = 0;
    coefficient = GetSinglePWPeriodWithMode();
    freq = freq*(1.0f/coefficient);
    return freq;
}

unsigned long LDKeeper::CalculateWritePeriod(unsigned long freq)
{
    float coefficient = 0;
    coefficient = GetSinglePWPeriodWithMode();
    freq = freq*(coefficient/64.0f);
    return freq;
}
