#ifndef SERIALOptical
#define SERIALOptical

#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdio.h>
#include <QString>
#include <QThread>
#include <QObject>
#include <iostream>
#include <map>
#include <iomanip>
#include <fstream>
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "PdHandleThread.h"

//#define LaserSwitchDirection_PATH "/sys/devices/virtual/gpio/gpio50/direction"     //input cheak buttun status P1.18 direction set
#define LaserSwitchDirection_PATH "/sys/class/gpio/gpio50/direction"
using namespace std;

typedef struct PowerAdjustingArg{
    float A;
    float B;
}AdjustingArg;

class OpticalPower:public QThread
{
     Q_OBJECT
public:
    queue<unsigned char> m_Qrecieve;
    queue<unsigned char> ProtocolQrecieve;
    queue<BYTE> m_Qsend;
    map<unsigned char,float>m_OpticalValueObject;
    map<unsigned char,AdjustingArg>Arg;
    int openSerial(char *cSerialName);
    int closeSerial();
    DWORD ReadThreadFunc();
    DWORD WriteThreadFunc();
    bool WriteSyncPort(const char *buf);
    DWORD initSerialHandle();
    static OpticalPower* getInstance();
    virtual void run();
    void InitP118ToIn();
    void SetFrequency(unsigned char frequency);
    void SetEEprom();
    void GetFrequency();
    void SetAdcChannel(unsigned char channel,bool SwitchStatus);
    unsigned char GetAdcSwitch();
    void AdcToPower(unsigned char _channel,short _adcvalue);
    void InitPowerArgByINI();
    void InitFreqSwitch();
    unsigned char GetCurrentFreq();
    void UpdataCurrentFreq(unsigned char _freq);
    unsigned char GetAllChannelSwitch();
    void UpdataAllChannelSwitch(unsigned char _switch);
    void InitPowerArgByUart(unsigned char byte1, unsigned char byte2);
    void RequestGetArgByUart();
    void  EmptyArg();
    void printarg();
    float GetChannelPower(unsigned char channnel);
    bool GetChannelSwitchStatus(unsigned char channel);
    void WritePowerFile();
    void OpenPowerFile();
    void StartSaveFile();
    void StopSaveFile();
    bool IsStartSaveFIle();
    unsigned char PackFrame(queue<unsigned char> *SendQueue, unsigned char *PackedFrame, unsigned char ucOperateCode) ;
    unsigned char ProtocolSendQueue(queue<unsigned char> *sendQueue, unsigned char ucOperateCode);
    unsigned char ProtocolSendArray(unsigned char  *ucSendData, int nLength, unsigned char ucOperateCode);
    unsigned char ProtocolReceive(queue<unsigned char >* receivedQueue, queue<unsigned char >* receivedData, unsigned char  * ucpOperateCode);
    unsigned char GetFrame(queue<unsigned char >* ReceivedQueue, unsigned char  *ReceivedFrame);
    unsigned char UnpackFrame(unsigned char  *ReceviedFrame, queue<unsigned char>* ReceivedData, unsigned char * ucpOperateCode);
public:
    OpticalPower();
    ~OpticalPower();
    int m_fd;
    ofstream outp;
    bool IsOpenPowerFile;
    bool IsSaveFile;
    static OpticalPower* m_serialHandle;
    PdHandleThread * PdHandleObject;
    unsigned char CurrentFreq;
    unsigned char AllChannelSwitch;

};

#endif // SERIAL
