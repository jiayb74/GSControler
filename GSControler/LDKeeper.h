#ifndef LDKEEPER_H
#define LDKEEPER_H
#include "CRegMap.h"
#include <string>
#include <string.h>
#include "RegTable.h"

class LDKeeper
{
public:
    int ID;                 //唯一节点号
    int  level;
    int  output;
public:
    LDKeeper();
    LDKeeper(int _ID);      //
    ~LDKeeper();
    void SetLDRowColumn(int id);
    int GetLevel();
    int GetOutput();
    int SetLevel(int _level);
    int SetOutput(int _output);
    void SetIndex(int _index);
    int GetIndex();
    void Init(float pw,float freq);
    //设置节点ID
    void SetID(int _ID);
    //获得当前节点ID
    int GetID();
    //获得真实温度
    float GetRealTemp();
    //设置当前电流
    void SetRealCurr(float _curr);
    //设置当前温度
    void SetRealTemp(float _temp);
    //获得真实电流
    float GetRealCurr();
    //
    float GetCurrAimCurr();
    //获得真实电流
    float GetRealCurrByReg();
    //获得底盘温度
    float GetPadTemp();
    //获得空气温度
    float GetAirTemp();
    //设置底盘温度
    void SetPadTemp(float _PadTem);
    //设置空气温度
    void SetAirTemp(float _AirTemp);
    //设置初始化温度
    void SetInitTemp(float _temp);
    //设置初始化电路
    void SetInitCurr(float _curr);
    //设置节点开关
    void SetSwitch(unsigned char status);
    void SetSwitch(unsigned char SelectModule,unsigned char status);
    void SwitchSet(unsigned char status);
    void MolockSwitch(unsigned char status);
    //读写寄存器
    void Read_Set_Reg(unsigned char add1,unsigned char add0,unsigned char flag,unsigned char data1,
                       unsigned char data2,unsigned char data3,unsigned char data4,
                       P_CanDataStruct  data);
    //设置温度
    void Set_Temp(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4);
    void SetTempByReg(float temp);
    //设置电流
    void Set_Curr(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4);
    void SetCurrByReg(float curr,unsigned char flag = 0);
    void SetCurrByReg(unsigned char id,float curr,char flag);
    void SetCurrByReg(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4,unsigned char flag = 0);
     void SetTempByReg(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4,unsigned char flag = 0);
    //直接将浮点型作为参数
    void Set_FloatSurrent(float iTarget);
    void Set_FloatTemp(float tTarget);
    void SendUpdateDate(unsigned char addr1,unsigned char addr2,unsigned char data1,
                        unsigned char data2,unsigned char data3,unsigned char data4,
                        unsigned char data5, unsigned char data6);
    bool IsSanHeYi();

    void OnLine();      //在线
    void DownLine();    //下线
    bool GetLinkStatus(); //获得在线状态

    int Target_temperature; //预设目标温度
    int Target_current;     //预设目标电流

    float Init_temp;        //初始化温度
    float Init_curr;        //初始化电流

    float Real_temperature;   //当前温度
    float Real_current;       //当前电流

    float AirTemperature;
    float PadTemperature;

    bool LinkStatus;         //链接状态
    float PresetPower;
    float usersetpw;
    float usersetfreq;
    float P2C[4];
    float C2P[4];
    float MinC;

    float GetAimCurr();
    float GetAimTemp();
    bool CurrOk();

    int index;
    int Subscript;
    int Reg;
    float RegNum;
    CanDataStruct RegData;
    void RegInfo(unsigned char addr1,unsigned char addr0,
                 unsigned char data1,unsigned char data2,
                 unsigned char data3,unsigned char data4);
    void HandleAlarm(unsigned char data1,unsigned char data2);
    void Handle_INT_PW();
    void Handle_INT_FREQ();
    unsigned char Handle_Uart_INT_PW(float pw);
    void HandleLatch();
    void HandleFlimtALarm();
    void SetMinFreq(float freq);
    float GetMinFreq();
    void SetMaxFreq(float freq);
    float GetMaxFreq();
    void SetPulseWidth(float pulse);
    float CorrectPulseWidth(float pw);
    float GetFrequencyPeriod();
    float EqualityPulseWidth(float pw);
    float GetPulseWidth();
    float GetDB25Freq();
    float GetDB25PulseWidth();
    unsigned char GetDin();
    float power2current(float power);
    float current2power(float current);
    unsigned char SetPower(float power);
    unsigned char SetPowerPercentage(float percentage);
    float GetPower();
    float GetPresetPower();
    float GetPresetPulse();
    float GetPresetFreq();
    unsigned char GetDB25Alarm();
    unsigned char GetFlimtALarm();
    void SetDB25Alarm(unsigned char alarm);
    void SetLD0CurrentWithPw(float pw);
    float Pw2Current(float pw);
    void AlarmUnReady(unsigned char status);
     void  SystemAlarm(unsigned char source,bool status);
    float MaxPowerPw(float pw);
    void SetFreqLimitWithPW(float pw);
    unsigned char GetModeWithSinglePW(float SinglePulseWidth);
    float GetSinglePWPeriodWithMode(void);
    float GetSinglePWWithMode(void);
    unsigned long CalculatePeriod(unsigned long freq);
    unsigned long CalculateWritePeriod(unsigned long freq);
};

#endif // LDKEEPER_H
