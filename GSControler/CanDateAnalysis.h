#ifndef _CANTEST_H__
#define _CANTEST_H__

#include <QString>
#include <QThread>
#include <QObject>
#include "Status.h"
#include "LDKeeper.h"
#include "RegTable.h"

class Can_LD:public QThread
{
    Q_OBJECT
public:
    static int s; 
    CStatus * status;
    LDKeeper * lpLD;
    int LDNum;
public:
    Can_LD(LDKeeper *_lpLD,int _LDNum );
    Can_LD();
    void InitLD();
    int LDRank(int num);
    void Data_Analysis(struct can_frame *fr);
    void RecvCanData(void );
	void SetSwitch(int Node,unsigned char status);	
	void Set_Switch(int Node,unsigned char status);	
	int OpenCan(void);	
    virtual void run();
signals:
    void Fault_Alarm(char NODE);
    void LinkStatus(char NODE);
    void ModeLocked(bool state);
    void Int_Alarm(unsigned char data0,unsigned char data2);

};

#endif
