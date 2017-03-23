
#include "Module.h"
#include "CanDateAnalysis.h"
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>


Module::Module()
{

}

void Module::SetSwitch(unsigned char SelectionLD,unsigned char  status)
{
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectionLD == MLD0){
        frdup.data[0] = 1;
        frdup.data[1] = 0;
    }
    else if(SelectionLD == MLD1){
        frdup.data[0] = 5;
        frdup.data[1] = 0;
    }
    frdup.data[2] = 0;
    frdup.data[3] = 0;
    if(status == ON)
        frdup.data[4] = 0xff;
    else
        frdup.data[4] = 0x02;

    frdup.data[5] = 0;
    frdup.data[6] = 0;
    frdup.data[7] = 0;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void Module::ModuleSetRealCurr(unsigned char SelectModule, float _curr)
{
    if(SelectModule == MLD0)
        this->m_LD0Curr = _curr;
    else if(SelectModule == MLD1)
        this->m_LD1Curr = _curr;
}

void Module::ModuleSetRealTemp(unsigned char SelectModule, float _temp)
{
    if(SelectModule == MLD0)
        this->m_LD0Temp = _temp;
    else if(SelectModule == MLD1)
        this->m_LD1Temp = _temp;

}

void Module::ModeleSetSeedBoxTemp(unsigned char flag, float _temp)
{
    if(flag == AIRTEMP)
        this->m_SeedBoxAirTemp = _temp;
    else if(flag == PADTEMP)
        this->m_SeedBoxPadTemp = _temp;

}

float Module::ModuleGetCurr(unsigned char SelectModule)
{
    if(SelectModule == MLD0)
        return this->m_LD0Curr;
    else if(SelectModule == MLD1)
        return this->m_LD1Curr;
}

float Module::ModuleGetTemp(unsigned char SelectModule)
{
    if(SelectModule == MLD0)
        return this->m_LD0Temp;
    else if(SelectModule == AIRTEMP)
        return this->m_SeedBoxAirTemp;
    else if(SelectModule == PADTEMP)
        return this->m_SeedBoxPadTemp;
    else if(SelectModule == MLD1)
        return this->m_LD1Temp;
}
void Module::ModuleRegReadWrite(unsigned char add1, unsigned char add0, unsigned char flag, unsigned char data1, unsigned char data2, unsigned char data3, unsigned char data4, P_CanDataStruct data)
{
   // cout<<"----------nnnnnnn"<<this<<endl;
  //
//    mutex.lock();
  //  cout<<"lock"<<endl;
  //   printf("add = %d\n",add1);
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    frdup.data[0] = add1;
    frdup.data[1] = add0;
    frdup.data[2] = flag;
    frdup.data[3] = 0;
    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
    if(flag == BlockingRead)       //Blocking read
    {
         int count = 0;
         while((this->Reg != ((add1<<8)+add0)) && (count <= TIMEOUTT)){    //1sm没有数据返回超时
             usleep(1000);   //1ms
             count++;
         }
         if(count > TIMEOUTT){
             if(data != NULL){
                 data->s_data = -1;
                 data->f_data = -1;
             }
       //    printf("-m--wait reg return error..%d.....\n",count);
         }
         else{
      //     printf("-m--wait reg return ok..%d.....\n",count);
     //    this->Reg = -1;
             if(data != NULL){
                 data->s_data = this->RegData.s_data;
                 data->f_data = this->RegData.f_data;
             }
         }
       //  cout<<data->s_data<<endl;
      //   this->Reg = -1;
      //   return ;
    }
    this->Reg = -1;
// cout<<"unlock"<<endl;
  //  mutex.unlock();

}

void Module::ModuleSetCurr(unsigned char SelectModule, float _current)
{
    char * p;
    p = (char *)&_current;
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectModule == MLD0){
        frdup.data[0] = 3;
        frdup.data[1] = 0;
    }
    else if(SelectModule == MLD1){
        frdup.data[0] = 7;
        frdup.data[1] = 0;
    }
    frdup.data[2] = 0;
    frdup.data[3] = 0;

    frdup.data[4] = *(p+3);
    frdup.data[5] = *(p+2);
    frdup.data[6] = *(p+1);
    frdup.data[7] = *p;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void Module::ModuleSetCurr(unsigned char SelectModule, unsigned char data1, unsigned char data2, unsigned char data3, unsigned char data4)
{

    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectModule == MLD0){
        frdup.data[0] = 3;
        frdup.data[1] = 0;
    }
    else if(SelectModule == MLD1){
        frdup.data[0] = 7;
        frdup.data[1] = 0;
    }
    frdup.data[2] = 0;
    frdup.data[3] = 0;

    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

void Module::ModuleSetTemp(unsigned char SelectModule, float _temp)
{
    char * p;
    p = (char *)&_temp;
    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectModule == MLD0){
        frdup.data[0] = 3;
        frdup.data[1] = 2;
    }
    else if(SelectModule == MLD1){
        frdup.data[0] = 7;
        frdup.data[1] = 2;
    }
    frdup.data[2] = 0;
    frdup.data[3] = 0;

    frdup.data[4] = *(p+3);
    frdup.data[5] = *(p+2);
    frdup.data[6] = *(p+1);
    frdup.data[7] = *p;
    write(Can_LD::s,&frdup,sizeof(frdup));
}


void Module::ModuleSetTemp(unsigned char SelectModule, unsigned char data1, unsigned char data2, unsigned char data3, unsigned char data4)
{

    struct can_frame  frdup;
    frdup.can_id = ((SET_REG<<7)+this->ID);
    frdup.can_dlc = 8;
    if(SelectModule == MLD0){
        frdup.data[0] = 3;
        frdup.data[1] = 2;
    }
    else if(SelectModule == MLD1){
        frdup.data[0] = 7;
        frdup.data[1] = 2;
    }
    frdup.data[2] = 0;
    frdup.data[3] = 0;

    frdup.data[4] = data1;
    frdup.data[5] = data2;
    frdup.data[6] = data3;
    frdup.data[7] = data4;
    write(Can_LD::s,&frdup,sizeof(frdup));
}

Module::~Module()
{


}
