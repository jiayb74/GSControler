#ifndef SERIAL
#define SERIAL

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
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"

using namespace std;

class CSerialHandle:public QThread
{
     Q_OBJECT
public:
    queue<BYTE> m_Qrecieve;
    queue<BYTE> m_Qsend;
    int openSerial(char *cSerialName);
    int closeSerial();
    DWORD ReadThreadFunc();
    DWORD WriteThreadFunc();
    bool WriteSyncPort(const char *buf);
    DWORD initSerialHandle();
    static CSerialHandle* getInstance();
    virtual void run();
private:
    CSerialHandle()
    {
    }
    int m_fd;
    static CSerialHandle* m_serialHandle;
    ~CSerialHandle(){};

};

#endif // SERIAL

