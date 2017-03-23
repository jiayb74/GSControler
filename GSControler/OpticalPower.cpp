/*************************

说明：主要实现配置串口。

**************************/

#include "OpticalPower.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "PortState.h"

//ofstream outp;
#define FrameHead   0xff
#define FrameInsert 0x00

using namespace std;

OpticalPower* OpticalPower::m_serialHandle = 0;
OpticalPower *OpticalPower::getInstance()
{

    if(m_serialHandle==0)
    {
        m_serialHandle = new OpticalPower();
        m_serialHandle->initSerialHandle();
    //    m_serialHandle->InitPowerArgByINI();
        m_serialHandle->RequestGetArgByUart();
    //    m_serialHandle->InitFreqSwitch();
        m_serialHandle->start();
    }
    return m_serialHandle;
}

OpticalPower::OpticalPower()
{
    for(int i=0; i<8;i++)
        m_OpticalValueObject[i] = 0;
    IsOpenPowerFile = false;
    IsSaveFile = false;
}

int OpticalPower::openSerial(char *cSerialName)
{
    int m_fd;
    struct termios opt;

    m_fd = open(cSerialName, O_RDWR | O_NOCTTY);
    if(m_fd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(m_fd, &opt);
    cfsetispeed(&opt, B57600);
    cfsetospeed(&opt, B57600);

    opt.c_cflag &= ~OPOST;   //使能原始输出
    opt.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    opt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
     opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
     opt.c_oflag &= ~(OPOST);
     opt.c_cflag &= ~(CSIZE | PARENB);


    opt.c_cflag |= PARENB;
    opt.c_cflag |= CREAD;
    opt.c_cflag |= CLOCAL;
    opt.c_cflag &= ~PARODD;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;

  //  opt.c_iflag |= (INPCK | ISTRIP);
    opt.c_iflag &=   ~ISTRIP;   //不要剔除字符第八位bit7
    opt.c_iflag |= (INPCK);
    opt.c_oflag &= ~OPOST;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    opt.c_cc[VMIN]   =   0xFF;
    opt.c_cc[VTIME]  =   10;    //1VTIME=0.1S

    if (tcsetattr(m_fd,   TCSANOW,   &opt)<0) {
        return   -1;
    }
    return m_fd;
}

void OpticalPower::run()
{
    ReadThreadFunc();
}
DWORD OpticalPower::ReadThreadFunc()
{
    unsigned char tmp[1];
    int len= 0;
    while(TRUE)
    {
        len = read(m_fd, tmp, sizeof(tmp));
     //   write(m_fd,"send",4);
        for(int i=0;i<len;i++){
         //   printf("%x-",tmp[i]);
        }
        fflush(stdout);
      //  printf("\n");
        for(int idx = 0; idx< len; idx++)
        {
          //  m_Qrecieve.push(tmp[idx]);
            ProtocolQrecieve.push(tmp[idx]);
        }

    }
}

//设置频率
void OpticalPower::SetFrequency(unsigned char frequency)
{
    unsigned char send[4];
    send[0] = 0xb1;
    send[1] = 0x80;         //bit7 = 1
    send[2] = frequency;
    send[3] = ((send[0]+send[1]+send[2])%256);
    ProtocolSendArray(send,4,send[0]);
 //   write(m_fd,send,sizeof(send));
    CurrentFreq = frequency;
}

//存储EEPROM
void OpticalPower::SetEEprom()
{
    unsigned char send[4];
    send[0] = 0xe1;
    send[1] = 0x00;         //bit7 = 1
    send[2] = 0x00;
    send[3] = ((send[0]+send[1]+send[2])%256);
    ProtocolSendArray(send,4,send[0]);
}

//读取当前频率
void OpticalPower::GetFrequency()
{
    unsigned char send[4];
    send[0] = 0xa1;
    send[1] = (1<<7);         //bit7 = 1
    send[2] = 0;
    send[3] = ((send[0]+send[1]+send[2])%256);
    //write(m_fd,send,sizeof(send));
    ProtocolSendArray(send,4,send[0]);
}

//设置ADC通道开关
void OpticalPower::SetAdcChannel(unsigned char channel, bool SwitchStatus)
{
    unsigned char send[4];
    send[0] = 0xb1;
    send[1] = (channel<<4);         //bit7 = 0
    send[2] = SwitchStatus;
    send[3] = ((send[0]+send[1]+send[2])%256);
    ProtocolSendArray(send,4,send[0]);
   // write(m_fd,send,sizeof(send));
    if(SwitchStatus)
        AllChannelSwitch |= (1<<channel);
    else
        AllChannelSwitch &= ~(1<<channel);
}

//获取当前ADC开光状态
unsigned char OpticalPower::GetAdcSwitch()
{
    unsigned char send[4];
    send[0] = 0xa1;
    send[1] = 0;         //bit7 = 0
    send[2] = 0;
    send[3] = ((send[0]+send[1]+send[2])%256);
    //write(m_fd,send,sizeof(send));
    ProtocolSendArray(send,4,send[0]);
}

int num = 0;
void OpticalPower::AdcToPower(unsigned char _channel, short _adcvalue)
{
    float powervalue = Arg[_channel].A*_adcvalue +Arg[_channel].B;
    //m_OpticalValueObject[_channel] = powervalue;
    m_OpticalValueObject[_channel] = powervalue;//(_adcvalue/1024.0)*3.4;
  //  printf("channel:%d %d value:%f\n",_channel,_adcvalue,(_adcvalue/1024.0)*3.4);
    num++;
    if(num>=8){
        WritePowerFile();
        num = 0;
    }
}

void OpticalPower::StartSaveFile()
{
    IsSaveFile = true;
}

void OpticalPower::StopSaveFile()
{
    IsSaveFile = false;
    IsOpenPowerFile = false;
    outp.close();

}

bool OpticalPower::IsStartSaveFIle()
{
    return IsSaveFile;
}
void OpticalPower::OpenPowerFile()
{
    outp.open("./doc/PowerResultData.txt");
    outp<<left;
    outp<<setw(8)<<"C0"<<setw(8)<<"C1"<<setw(8)<<"C2"<<setw(8)<<
         "C3"<<setw(8)<<"C4"<<setw(8)<<"C5"<<setw(8)<<"C6"<<setw(8)<<"C7"<<"\r\n"<<endl;
    IsOpenPowerFile = true;
}
void OpticalPower::WritePowerFile()
{
    if(IsSaveFile){
        if(!IsOpenPowerFile)
            OpenPowerFile();
        for(int i=0; i<8; i++){
             outp<<setw(8)<<m_OpticalValueObject[i];
        }
        outp<<"\r\n"<<endl;
    }
}

bool OpticalPower::GetChannelSwitchStatus(unsigned char channel)
{
    return ((AllChannelSwitch>>channel)&0x01);
}

float OpticalPower::GetChannelPower(unsigned char channnel)
{
    return m_OpticalValueObject[channnel];
}

void OpticalPower::InitPowerArgByINI()
{
    char section[10];
    for(int i=0; i<8; i++){
        AdjustingArg m_arg;
      //  sprintf(section,"%s%d","Channel",i);
        m_arg.A = GetPrivateProfileDouble(section,"A",0,GS_INI_POWERADJARG);
        m_arg.B = GetPrivateProfileDouble(section,"B",0,GS_INI_POWERADJARG);
        Arg[i] = m_arg;
    }
   // printarg();
}
void OpticalPower::InitPowerArgByUart(unsigned char byte1,unsigned char byte2)
{

    unsigned char channel;
    unsigned char AorB;
    unsigned char byteindex;
    channel = (byte1>>5);
    AorB = ((byte1>>4)&0x01);
    byteindex = ((byte1>>2)&0x03);
    if(AorB){
        unsigned char * p = (unsigned char*)&Arg[channel].A;
        *(p+byteindex) = byte2;
    }
    else{
        unsigned char * p = (unsigned char *)&Arg[channel].B;
        *(p+byteindex) = byte2;
    }
    if((channel == 7)&&(AorB == 0)&&(byteindex == 3)){      //recv ok
      //  printarg();
    }
}


void OpticalPower::printarg()
{
     map<unsigned char, AdjustingArg>::iterator iter;
     for(iter = Arg.begin(); iter != Arg.end(); iter++)
     {
         printf("channel:%d A:%f B:%f\n",iter->first,iter->second.A,iter->second.B);
     }
}

void OpticalPower::EmptyArg()
{
    AdjustingArg m_arg;
    m_arg.A = 0;
    m_arg.B = 0;
    for(int i=0; i<8; i++){
        Arg[i] = m_arg;
    }
}
void OpticalPower::RequestGetArgByUart()
{
    EmptyArg();
    unsigned char send[4];
    send[0] = 0xc1;
    send[1] = 0;         //bit7 = 1
    send[2] = 0;
    send[3] = ((send[0]+send[1]+send[2])%256);
    ProtocolSendArray(send,4,send[0]);
  //  write(m_fd,send,sizeof(send));
}

void OpticalPower::InitFreqSwitch()
{
    GetFrequency();
    GetAdcSwitch();
}

unsigned char OpticalPower::GetCurrentFreq()
{
    GetFrequency();
    return CurrentFreq;
}

void OpticalPower::UpdataCurrentFreq(unsigned char _freq)
{
    //printf("currentfreq:%d\n",_freq);
    CurrentFreq = _freq;
}


unsigned char OpticalPower::GetAllChannelSwitch()
{
    GetAdcSwitch();
    return AllChannelSwitch;
}

void OpticalPower::UpdataAllChannelSwitch(unsigned char _switch)
{
   // printf("AllChannelSwitch:%x\n",_switch);
    AllChannelSwitch = _switch;
}

DWORD OpticalPower::WriteThreadFunc()
{
    unsigned char* p_ch;
    while(TRUE)
    {
        while(m_Qsend.empty()!=true)
        {
            p_ch = &(m_Qsend.front());
            write(m_fd,p_ch,1);
            m_Qsend.pop();
        }
    }
}

bool OpticalPower::WriteSyncPort(const char* buf)
{
    int iHasWritten = 0;
    const char* p_ch = buf;
    while(true)
    {
        if(iHasWritten == strlen(buf))
        {
            break;
        }
        write(m_fd,p_ch,1);
        iHasWritten++;
        p_ch++;
    }
}
int OpticalPower::closeSerial()
{
    close(m_fd);
}
DWORD OpticalPower::initSerialHandle()
{
    m_fd = openSerial("/dev/ttySP1");
    if(m_fd < 0)
    {
        return 0xFF;
    }
}

void OpticalPower::InitP118ToIn()
{
    int fd = open(LaserSwitchDirection_PATH,O_RDWR);
    if(fd < 0){
      //  perror(DEV_PATH);
    }
    lseek(fd,0,SEEK_SET);
    write(fd,"in",2);
    close(fd);
}


OpticalPower::~OpticalPower()
{
    delete PdHandleObject;
    delete this;
}

/*****************************************新协议**************************/



unsigned char OpticalPower::ProtocolSendArray(unsigned char  *ucSendData, int nLength, unsigned char ucOperateCode)
{
    unsigned int nIndex = 0;
    queue<unsigned char>qSendQ;
    unsigned char ucReturnCode;
    if(ucSendData == 0x00) return 0x01;  //Wrong pointer
    while(nIndex < nLength)
    {
        qSendQ.push(*(ucSendData+nIndex++));
    }
    ucReturnCode = ProtocolSendQueue(&qSendQ, ucOperateCode);
    return (ucReturnCode);
}

unsigned char OpticalPower::ProtocolSendQueue(queue<unsigned char>* sendQueue, unsigned char ucOperateCode)
{
    unsigned char Frame[16];
    if(!sendQueue->size())return 0x02;				//Error: No data to send.
    if((ucOperateCode == FrameHead)||(ucOperateCode == 0x00)) return 0x03; //Error: Wrong operate code;
    do{
        if(PackFrame(sendQueue,Frame,ucOperateCode))
        {
            for(int i=0; i<16; i++){
                printf("%0x ",Frame[i]);
            }
            printf("\n");
            write(m_fd,Frame,sizeof(Frame));
        }
        else
            return 0x04;  //Error: Send error.
    }while(sendQueue->size());
    return 0;					//Error: Successed sent data.
}


unsigned char OpticalPower::PackFrame(queue<unsigned char>* SendQueue, unsigned char *PackedFrame, unsigned char ucOperateCode)
{
    unsigned char index = 3;
    unsigned char ucData;
    if(!SendQueue->size()) return 0; //nothing to send

    PackedFrame[0] = FrameHead;    //set frame head
    PackedFrame[1] = FrameHead;			//set frame head
    PackedFrame[2] = ucOperateCode;		//set operacte code

    do
    {
        ucData = SendQueue->front();
        SendQueue->pop();
        if(ucData != FrameHead)								//check the data
            PackedFrame[index++] = ucData;			//if not equate the head code 0xFF, send this data
        else
        {		                       //else, we need more process
            if(index == 14)											//if is the last byte data
            {
                PackedFrame[index++] = ucData;		//send this data
            }
            else
            {												//data equate with the head code 0xFF
                PackedFrame[index++] = ucData;				//put the data
                PackedFrame[index++] = FrameInsert;		//follow by the data, insert 0 byte
            }
        }
        if(index == 15)						//check it is full frame
        {
            if(SendQueue->size())     //still have more data need to send
                PackedFrame[index] = 13;	//set flag
            else
                PackedFrame[index] = 12;  //else set actual data length
            return 1;
        }
  }while(SendQueue->size());    //loop it, untill send all data

    PackedFrame[15] = index-3;     //set actual data length
    return 1;
}


unsigned char OpticalPower::ProtocolReceive(queue<unsigned char >* receivedQueue, queue<unsigned char >* receivedData, unsigned char  * ucpOperateCode)
{
        unsigned char  ucReceivedFrame[16];
        unsigned char ucDataLength = 0;
        bool		bIsNotLastFrame = true;
        if(receivedQueue->size()<16) return 0x80; //Error: Receive error.
        while(bIsNotLastFrame/*&&(receivedQueue->size()>16)*/){
            usleep(1);
            if(receivedQueue->size()>16)
            {
                    ucDataLength = GetFrame(receivedQueue, ucReceivedFrame);
                    if(ucDataLength == 0)return 0x81; //Error: Get a frame Error.
                    if(ucDataLength < 13)
                    {
                        bIsNotLastFrame = false;
                     //   break;
                    }
                    bIsNotLastFrame = false;
                    UnpackFrame(ucReceivedFrame, receivedData, ucpOperateCode);
            }
        }
        if(!bIsNotLastFrame) return 0x83; //Error: Received queue is empty.

        return 0;
}

unsigned char OpticalPower::GetFrame(queue<unsigned char >* ReceivedQueue, unsigned char  *ReceivedFrame)
{
    unsigned char ucIndex = 2;
    bool 	bGetHead = false;
    ReceivedFrame[0] = ReceivedQueue->front();
    ReceivedQueue->pop();
    ReceivedFrame[1] = ReceivedQueue->front();
    ReceivedQueue->pop();
    if((ReceivedFrame[0] == FrameHead)&&(ReceivedFrame[1] == FrameHead))
        bGetHead = true;
    while((ReceivedQueue->size())&&(bGetHead == false))
    {
        ReceivedFrame[0] = ReceivedFrame[1];
        ReceivedFrame[1] = ReceivedQueue->front();
        ReceivedQueue->pop();
        if((ReceivedFrame[0] == FrameHead)&&(ReceivedFrame[1] == FrameHead))
            bGetHead = true;
    }
    if(bGetHead != true)
        return 0;

    while(ucIndex <= 15){
        if(ReceivedQueue->size()){
            ReceivedFrame[ucIndex++] = ReceivedQueue->front();
            ReceivedQueue->pop();
        }

    }
    return ReceivedFrame[15];
}

unsigned char OpticalPower::UnpackFrame(unsigned char  *ReceviedFrame, queue<unsigned char>* ReceivedData, unsigned char * ucpOperateCode)
{
    unsigned char ucDataLength;
    unsigned char ucIndex = 3;
    unsigned char ucData;
    bool bIsLastFrame = true;

    *ucpOperateCode = ReceviedFrame[2];
    ucDataLength = ReceviedFrame[15];
    if(ucDataLength<=0)return 0;

    if(ucDataLength == 13)
    {
        ucDataLength = 12;
        bIsLastFrame = false;
    }
    while((ucIndex - 3) < ucDataLength)
    {
        ucData = ReceviedFrame[ucIndex++];
        if(ucData!= FrameHead)
        {
            ReceivedData->push(ucData);
        }
        else
        {
            ReceivedData->push(ucData);
            ucIndex++;
        }
    }
    return bIsLastFrame;
}

/****************************************新协议*****************************/
