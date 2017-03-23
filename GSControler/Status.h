#ifndef STATUS_H
#define STATUS_H
#include<QThread>
#include"LDKeeper.h"

class CStatus:public QThread
{
     Q_OBJECT
public:
    CStatus(LDKeeper * _lpLD,int _LdNum);
    ~CStatus();
    void run();
private:
    LDKeeper *lpLD;
    int MoudleNum;
};

#endif // STATUS_H
