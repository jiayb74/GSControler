#ifndef _CREGMAP_H__
#define _CREGMAP_H__

#include "FileToPc.h"
#include"FIleToArm.h"
#include "GSDefine.h"

class CRegMap 
{
public:
	CRegMap(void);
    FileToPc  * fileTopc;
	virtual ~CRegMap(void);
public:
	virtual bool InitInstance();
    void WriteSingleByte(unsigned char addr1,unsigned char value);
    short ReadDoubleByte(unsigned char addr1,unsigned char addr2,unsigned char addr3);
    void WriteDoubleByte(unsigned char addr1,unsigned char addr2,unsigned char addr3,unsigned char data1,unsigned char data2);
    float ReadFourByte(unsigned char addr1,unsigned char addr2,unsigned char addr3);
    void  WriteFourByte(char addr1,char addr2,char addr3,char data1,char data2,char data3,char data4);
    void ReadWriteIniFile(unsigned char addr1,unsigned char addr2,unsigned char addr3,unsigned char data1,unsigned char data2);
};

#endif
