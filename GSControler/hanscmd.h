#ifndef HANSCMD
#define HANSCMD

#include<QString>
#include<QThread>
#include<QObject>
#include <algorithm>
#include <string.h>
#include "CSerialHandle.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"
#include "SoftSwitch.h"

#define HANS_CMD_MAXLENGTH 30

class CHansCMD:public QThread
{
     Q_OBJECT
protected:
    BYTE m_byError;
    string m_sReplyMsg;
    BOOL	m_bTLSealed;
    BOOL	m_bUnLcked;
    CSoftSwitch * GsSoftswitch;

public:
    CHansCMD(CSoftSwitch * _GsSoftswitch);
    void dataProcessThreadFun();
    void processCMD(string str);
    BOOL GetSetValue(string strCmd, int BitCntMin, int BitCntMax, double * dValue);
    BOOL GetSetValue(string strCmd, int BitCntMin, int BitCntMax, UINT * nValue);
    void AnswerLStatus(BYTE Lstatus,double progress);
    BYTE OnHansResolved(unsigned int uiParam, double *dValue, UINT *unValue);    
    void Send2serial();
    // resolve command SPOxx.xxx
    void ResolveSPOCmd(string strCmd);
    // resolve command Fxxx
    void ResolveFCmd(string strCmd);
    // resolve command SETPx
    void ResolveSTEPCmd(string strCmd);
    // resolve command SETCx
    void ResolveSTECCmd(string strCmd);
    // resolve command SELFCHECK
    void ResolveSELFXCmd(BOOL bToAutoAlloc = FALSE);
    // Handle command REGISTRYx
    void ResolveREGCmd(string strCmd);
    //reply to command not recognized
    void NotAcknowledge();
    virtual void run();
signals:
    void SelfCheck(bool ScanStyle);
public slots:
    void ModeChanged(bool LCK);
};
#endif // HANSCMD

