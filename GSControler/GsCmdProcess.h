#ifndef _GSCMDPROCESS_H__
#define _GSCMDPROCESS_H__

#include<queue>
#include<QThread>
#include<QtNetwork>
#include<QtNetwork/qhostaddress.h>
#include<QtNetwork/QNetworkInterface>
#include<QNetworkInterface>
#include<QHostAddress>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>

//typedef unsigned char BYTE;
using namespace std;

class CGsCmdProcess
{
public:
	CGsCmdProcess();
	virtual ~CGsCmdProcess();

typedef enum __GS_CMD_OPERATION_TYPE_
{
	BlockWrite = 0,
	BlockRead,
	SingleWrite,
	SingleRead,
    DoubleWrite,
    DoubleRead,
    FourWrite,
    FourRead,
	NoOperation
}GsCmdOperationType;

typedef struct __GS_CMD_STRUCT_
{
    unsigned char		*pByCmdArray;	//¿¿¿
	int			nCmdLength;		//¿¿¿¿
	int			nCurrentPos;	//¿¿¿¿
	GsCmdOperationType tCurrentOperation;//¿¿¿¿
}GsCmdStruct, *pGsCmdStruct;

private:
	GsCmdStruct		m_cmdCurrentCMD;
public:
    void DataArraveProcess(queue <unsigned char>	*pDataQueue,QTcpSocket * NewSocket);
    void DataArraveProcess(queue <unsigned char>*pDataQueue,	int &sockfd,struct sockaddr_in &cliaddr,socklen_t &len);
    unsigned char CheckDigit(unsigned char* pbyData,unsigned char length);
    bool CheckCmdHead(unsigned char byData);
	bool SendData(const char * buf, int len,int new_fd);
	void ClearCmdStruct(void);
};

#endif
