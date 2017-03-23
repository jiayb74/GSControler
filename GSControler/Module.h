#ifndef MODULE_H
#define MODULE_H
#include "LDKeeper.h"


class Module:public LDKeeper
{
public:
    float m_LD0Curr;
    float m_LD0Temp;
    float m_SeedBoxAirTemp;
    float m_SeedBoxPadTemp;
    float m_LD1Curr;
    float m_LD1Temp;
public:
    Module();
    virtual  ~Module();
    void SetSwitch(unsigned char SelectionLD,unsigned char  status);
    void ModuleRegReadWrite(unsigned char add1,unsigned char add0,unsigned char flag,unsigned char data1,
                            unsigned char data2,unsigned char data3,unsigned char data4,
                            P_CanDataStruct  data);
    void ModuleSetCurr(unsigned char SelectModule,float _current);
    void ModuleSetCurr(unsigned char SelectModule, unsigned char data1,unsigned char data2,
                       unsigned char data3,unsigned char data4);
    void ModuleSetTemp(unsigned char SelectModule,float _temp);

    void ModuleSetTemp(unsigned char SelectModule, unsigned char data1,unsigned char data2,
                       unsigned char data3,unsigned char data4);


    void ModuleSetRealCurr(unsigned char SelectModule,float _curr);
    void ModuleSetRealTemp(unsigned char SelectModule,float _temp);
    void ModeleSetSeedBoxTemp(unsigned char flag,float _temp);

    float ModuleGetCurr(unsigned char SelectModule);
    float ModuleGetTemp(unsigned char SelectModule);

};

#endif // MODULE_H
