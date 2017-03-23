#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "CanDateAnalysis.h"
#include "iostream"
#include "Status.h"
#include "Module.h"
#include "PortState.h"
#include "SoftSwitch.h"
#include "board.h"

int LockNumber=0;

CSoftSwitch * cs;
PortState * por;
Module module;
using namespace std;
bool PreviousModeLock = false;
extern bool IsSanHeYi;
extern LDKeeper SecondSeedBos;
int Can_LD::s = -1;
extern  Board * boards;
extern int MachineMode;

Can_LD::Can_LD( LDKeeper * _lpLD,int _LDNum)
{
    module.SetID(2);
    lpLD = _lpLD;
    //当前节点数
    LDNum = _LDNum;
    //打开CAN设备
    OpenCan();
    //初始化节点---自动获取ID号
    //InitLD(); //guang 使用
    //开始监测CAN数据
    this->start();
}

Can_LD::Can_LD()
{
    //打开CAN设备
    OpenCan();
    //初始化节点---自动获取ID号
    //InitLD(); //guang 使用
    //开始监测CAN数据
    this->start();
}

int Can_LD::OpenCan(void)
{
	int ret;
	struct sockaddr_can addr;
    struct ifreq ifr;
    srand(time(NULL));
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
		perror("socket PF_CAN failed");
        return -1;
	}
	strcpy(ifr.ifr_name, "can0");
	ret = ioctl(s, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		perror("ioctl failed");
        return -1;
	}
	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		perror("bind failed");
		return 1;
	}
	return 0;
}


void Can_LD::run()
{
    while(1){
    RecvCanData();
    }
}

int Can_LD::LDRank(int num)
{
    char n;
    n = ((num&0x78)>>3);
    return n;
}

//初始化个节点，自动获取ID号
void Can_LD::InitLD()
{
    unsigned int ret;
    struct can_frame  frdup;
    struct timeval tv;
    fd_set rset;
    int i;
    int LDid;
    int n;

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&rset);
    FD_SET(s, &rset);

    for(i=0; i<LDNum;){
    ret = read(s, &frdup, sizeof(frdup));
    if (ret < sizeof(frdup)) {
        perror("read failed");
           return ;
    }
   LDid = (frdup.can_id&NODE_ID);
   n = LDRank(LDid);
   if(i == n){
       lpLD[n].SetID(LDid);
       i++;
   }
   }

}
void Can_LD::Data_Analysis(struct can_frame *fr)
{
	int i;
    float temp=0,ele = 0;  	//温度电流
    char  * data;
    char *  data1;
/*
  printf("%08x  ", fr->can_id & CAN_EFF_MASK);
	printf("%d  ", fr->can_dlc);
	for (i = 0; i < fr->can_dlc; i++)
		printf("%02x ", fr->data[i]);
    printf("\n");
*/
    //解析接收到帧数据
	switch(fr->can_id>>7)
	{
	//电流
	case RECV_CUR:
        for(int i=0; i<Board::BoardNum; i++){
            if((fr->can_id&NODE_ID) == boards[i].GetBoardID()){
                if(boards[i].GetLDNumber()>1){
                        float buff;
                        float buff1;
                        data = (char *)&buff;
                        data1 = (char *)&buff1;
                        data[0] = fr->data[0];
                        data[1] = fr->data[1];
                        data[2] = fr->data[2];
                        data[3] = fr->data[3];

                        data1[0] = fr->data[4];
                        data1[1] = fr->data[5];
                        data1[2] = fr->data[6];
                        data1[3] = fr->data[7];
                        boards[i].LD[0].SetRealCurr(buff);
                        boards[i].LD[1].SetRealCurr(buff1);
                }
                else {
                    float buff;
                    data = (char *)&buff;
                    data[0] = fr->data[0];
                    data[1] = fr->data[1];
                    data[2] = fr->data[2];
                    data[3] = fr->data[3];
                    boards[i].LD[0].SetRealCurr(buff);
                }
                break;
            }
        }
		break;
	//温度
    case RECV_TEMP:
        for(int i=0; i<Board::BoardNum; i++){
            if((fr->can_id&NODE_ID) == boards[i].GetBoardID()){
                if(boards[i].GetLDNumber()>1){
                        float buff;
                        float buff1;

                        data = (char *)&buff;
                        data1 = (char *)&buff1;
                        data[0] = fr->data[0];
                        data[1] = fr->data[1];
                        data[2] = fr->data[2];
                        data[3] = fr->data[3];

                        data1[0] = fr->data[4];
                        data1[1] = fr->data[5];
                        data1[2] = fr->data[6];
                        data1[3] = fr->data[7];

                        if(fr->data[4] == 0xff){         //Air
                            boards[i].SetAirTemp(buff);
                        }
                        else if(fr->data[4] == 0x0){    //pad
                            boards[i].SetPadTemp(buff);
                        }
                        else{
                            boards[i].LD[0].SetRealTemp(buff);
                            boards[i].LD[1].SetRealTemp(buff1);
                        }
                }
                else {
                    float buff;                   
                    data = (char *)&buff;
                    data[0] = fr->data[0];
                    data[1] = fr->data[1];
                    data[2] = fr->data[2];
                    data[3] = fr->data[3];
                    boards[i].LD[0].SetRealCurr(buff);
                    data[0] = fr->data[4];
                    data[1] = fr->data[5];
                    data[2] = fr->data[6];
                    data[3] = fr->data[7];
                    boards[i].LD[0].SetRealTemp(buff);
                }

                break;
            }
        }
		break;

    //寄存器值
	case RECV_REG:	
        for(int i=0; i<Board::BoardNum; i++){
            if((fr->can_id&NODE_ID) == boards[i].GetBoardID()){
                if(boards[i].GetLDNumber()>1){
                    boards[i].LD[0].RegInfo(fr->data[0],fr->data[1],fr->data[4],fr->data[5],fr->data[6],fr->data[7]);
                    boards[i].LD[1].RegInfo(fr->data[0],fr->data[1],fr->data[4],fr->data[5],fr->data[6],fr->data[7]);
                }
                else
                    boards[i].LD[0].RegInfo(fr->data[0],fr->data[1],fr->data[4],fr->data[5],fr->data[6],fr->data[7]);
              break;
            }
        }
        break;

     //设备故障
     case FAULT_ALARM:
        switch(MachineMode)
        {
        case MachineN06:
            for(int i=0; i<Board::BoardNum; i++){
                if((fr->can_id&NODE_ID) == boards[i].GetBoardID()){
                  emit Int_Alarm(fr->data[0],fr->data[1]);
                  break;
                }
            }break;
        case MachineP03H:
            switch(fr->data[0])
            {
            case 1:
                cs->ModeLocked(fr->data[1]);
                por->Change_Led_state(fr->data[1]);
                 break;

            }
        case MachineP08H:
            switch(fr->data[0])
            {
            case 1:
                cs->ModeLocked(fr->data[1]);
                por->Change_Led_state(fr->data[1]);
                 break;

            }
        }
    break;
    default:      
    //printf("Data Analysis Error\n");
    break;
	}
}

void Can_LD::RecvCanData(void)
{
    unsigned int ret;
	struct can_frame  frdup;
	struct timeval tv;
	fd_set rset;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&rset);
    FD_SET(s, &rset); 
    ret = read(s, &frdup, sizeof(frdup));
    if (ret < sizeof(frdup)) {
    perror("read failed");
           return ;
    }
 //解析接收到都CAN数据
   Data_Analysis(&frdup);
   return 	;
}





//设置各级开关
void Can_LD::SetSwitch(int Node,unsigned char  status)
{
    struct can_frame frdup;
	frdup.can_id = Node;
	frdup.can_dlc = 1;
	frdup.data[0] = status;
	int n = write(s,&frdup,sizeof(frdup));
    if(n == -1)
		perror("error:");
}

