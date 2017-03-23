#include "PdHandleThread.h"
#include "OpticalPower.h"
#include <iostream>
using namespace std;

PdHandleThread::PdHandleThread()
{
    this->start();
}


void PdHandleThread::run()
{
  //  DataMatching();     //初始化匹配数据，执行完成后队列里的数据都是四个字节有效的了。  新协议取消
    unsigned char dat;
    unsigned char ChannelData[4];
    while(true)
    {
        unsigned char operationcode;
        OpticalPower::getInstance()->ProtocolReceive(&OpticalPower::getInstance()->ProtocolQrecieve,
                                                     &OpticalPower::getInstance()->m_Qrecieve,&operationcode);
            while(OpticalPower::getInstance()->m_Qrecieve.size()>=4) //7+1
            {
                    dat = OpticalPower::getInstance()->m_Qrecieve.front();
                    //OpticalPower::getInstance()->m_Qrecieve.pop();
                    if((dat == 0x81)||(dat == 0xb1)||(dat == 0xa1)||(dat == 0xc1)||(dat == 0xe1)) //adc value
                    {
                        OpticalPower::getInstance()->m_Qrecieve.pop();
                        ChannelData[0] = dat;
                        for(int i=1; i<=3; i++){
                            ChannelData[i] = OpticalPower::getInstance()->m_Qrecieve.front();
                            OpticalPower::getInstance()->m_Qrecieve.pop();
                        }
                        if(IsValid(ChannelData))
                            AnalyticData(ChannelData);
                        else {
                               continue;
                          //     NewDataMatching(); //重新匹配数据 新协议取消
                        }

                    }
                    else
                    {
                        OpticalPower::getInstance()->m_Qrecieve.pop();
                      //  NewDataMatching(); //重新匹配数据  新协议取消
                    }
            }
    }
}

void  PdHandleThread::AnalyticData(unsigned char _channeldata[])
{
    unsigned char channel = 0;
    short AdcValue = 0;
    switch(_channeldata[0])
    {
    case 0x81:  channel = ((_channeldata[1]>>4)&0x07);
                AdcValue = (((_channeldata[1]&0x03)<<8)+_channeldata[2]);
                OpticalPower::getInstance()->AdcToPower(channel,AdcValue);
                break;
    case 0xa1:  if((_channeldata[1]>>7)){       //read frequent return
                    OpticalPower::getInstance()->UpdataCurrentFreq(_channeldata[2]);
                }
                else{                           //adc switch
                    OpticalPower::getInstance()->UpdataAllChannelSwitch(_channeldata[2]);
                }
                break;
    case 0xc1:  OpticalPower::getInstance()->InitPowerArgByUart(_channeldata[1],_channeldata[2]);
                break;
    case 0xb1:  //cout<<"write ack"<<endl;
                break;
    case 0xe1:  //cout<<"eeprom ack"<<endl;
                break;
    default:break;
    }
}


bool PdHandleThread::NewDataMatching()
{
    unsigned char check[4];
    unsigned char count = 0;
    while(OpticalPower::getInstance()->m_Qrecieve.size()>=7){
            while(count<7){       //7个字节当中必定有四个字节是对应一个通道的
                count++;
                check[0] = OpticalPower::getInstance()->m_Qrecieve.front();               
                if((check[0] == 0x81)||(check[0] == 0xa1)||(check[0] == 0xb1)||(check[0] == 0xc1)||(check[0] == 0xe1))        //再次验证是否为四个字节有效数据
                {
                    if(OpticalPower::getInstance()->m_Qrecieve.size()>=3){
                        OpticalPower::getInstance()->m_Qrecieve.pop();  //delete head
                        for(int i=1; i<=3; i++){
                            check[i] =  OpticalPower::getInstance()->m_Qrecieve.front();
                            OpticalPower::getInstance()->m_Qrecieve.pop();
                        }
                        if(IsValid(check)){
                            AnalyticData(check);
                            return true;
                        }
                    }
                    else{
                        return false; //not delete head, save queue head
                    }
                }
                else{
                    OpticalPower::getInstance()->m_Qrecieve.pop();  //delete element
                }
            }
            count = 0;
    }
    return false;
}
void PdHandleThread::DataMatching()
{
    unsigned char check[4];
    while(true)
    {
        if(!(OpticalPower::getInstance()->m_Qrecieve.empty()))
        {
            if(OpticalPower::getInstance()->m_Qrecieve.size()>=7){       //7个字节当中必定有四个字节是对应一个通道的
                check[0] = OpticalPower::getInstance()->m_Qrecieve.front();
                OpticalPower::getInstance()->m_Qrecieve.pop();
                if((check[0] == 0x81)||(check[0] == 0xa1)||(check[0] == 0xb1)||(check[0] == 0xc1)||(check[0] == 0xe1))        //再次验证是否为四个字节有效数据
                {
                    for(int i=1; i<=3; i++){
                        check[i] =  OpticalPower::getInstance()->m_Qrecieve.front();
                        OpticalPower::getInstance()->m_Qrecieve.pop();
                    }
                    if(IsValid(check)){
                        AnalyticData(check);
                        break;
                    }
                    else
                        continue;
                }
            }
        }
    }
}

bool PdHandleThread::IsValid(unsigned char _check[])
{
    if(_check[3] == ((_check[0]+_check[1]+_check[2])%256)){
        return true;
    }
    return false;
}
