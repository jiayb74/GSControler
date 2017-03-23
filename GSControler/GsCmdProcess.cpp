#include "GSDefine.h"
#include "GsCmdProcess.h"
#include<stdio.h>
#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include"CRegMap.h"
#include<netdb.h>
#include<arpa/inet.h>

using namespace std;

CGsCmdProcess::CGsCmdProcess()
{
	//ClearCmdStruct();
	m_cmdCurrentCMD.pByCmdArray = NULL;
	m_cmdCurrentCMD.nCmdLength = 0;
	m_cmdCurrentCMD.nCurrentPos = 0xFF;
	m_cmdCurrentCMD.tCurrentOperation = NoOperation;
}

CGsCmdProcess::~CGsCmdProcess()
{
	ClearCmdStruct();
}

unsigned char CGsCmdProcess::CheckDigit(unsigned char *pbyData,unsigned char length)
{
	unsigned char i;
    unsigned char Xdigit = 0;
	for(i=0; i<length; i++){
		Xdigit += pbyData[i];	
	}
	return (Xdigit%256);
}

bool CGsCmdProcess::SendData(const char* buf, int len,int new_fd)
{
	int nBytes = 0;
	nBytes = send(new_fd,buf,len,0);
	if(nBytes == -1){
		perror("sendData:");	
	}
//	printf("---send  %d  byte ok----\n",nBytes);
	return true;	
}


// Process the data when reveived any data
  void CGsCmdProcess::DataArraveProcess(queue <unsigned char>*pDataQueue,	int &sockfd,struct sockaddr_in &cliaddr,socklen_t &len)
  {
      float itt = 0;
      char * p ;
      char addr1 = 0;
      char addr2 = 0;
      char addr3 = 0;

      char data1 = 0;
      char data2 = 0;
      char data3 = 0;
      char data4 = 0;
      short data;
      CRegMap reg;
      unsigned char byFront;
      byFront = pDataQueue->front();
      pDataQueue->pop();
      if(CheckCmdHead(byFront))
      {
          int i=0;
          for(i=1; i<m_cmdCurrentCMD.nCmdLength; i++){
              m_cmdCurrentCMD.pByCmdArray[i] = pDataQueue->front();
              pDataQueue->pop();
          }
  //		printf("~~~~~~~%d~~~~~%d\n",m_cmdCurrentCMD.pByCmdArray[m_cmdCurrentCMD.nCmdLength-1],CheckDigit(m_cmdCurrentCMD.pByCmdArray,m_cmdCurrentCMD.nCmdLength-1));
              //Get full command
          unsigned char *pSendData;
          pSendData = NULL;


          if(m_cmdCurrentCMD.pByCmdArray[m_cmdCurrentCMD.nCmdLength-1]
              != CheckDigit(m_cmdCurrentCMD.pByCmdArray,m_cmdCurrentCMD.nCmdLength-1))
          {
          //    printf("in gscmdprocess head cmd error\n");
          //    exit(-1);
          }
          else  //Analyze the received tcp command
          {
              unsigned char byAddress, byLen, i;
              byLen = ((m_cmdCurrentCMD.pByCmdArray[0]&0x0C)>>2) + 1;
              switch(m_cmdCurrentCMD.tCurrentOperation)
              {
              case	SingleWrite://	cout<<"-----ok singlewrite---"<<endl;
                                      pSendData = new unsigned char [2];
                                      pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                                      byAddress = m_cmdCurrentCMD.pByCmdArray[1];
                             //         cout<<"by:"<<byLen<<endl;
                                      for(i=0;i<byLen;i++)
                                      {
                                          reg.WriteSingleByte((byAddress+i),m_cmdCurrentCMD.pByCmdArray[i+2]);
                                      }
                                      pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                                      sendto(sockfd,(char *)pSendData,2,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
                                      delete [] pSendData;
                                      break;
              case DoubleRead:
           //      cout<<"----ok DoubleRead--"<<endl;
               pSendData = new unsigned char [4];//
               pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
               addr1 = m_cmdCurrentCMD.pByCmdArray[1];
               addr2 = m_cmdCurrentCMD.pByCmdArray[2];
               addr3 = m_cmdCurrentCMD.pByCmdArray[3];
               data = reg.ReadDoubleByte(addr1,addr2,addr3);
               if(-1 == data){
                   char ErrorSZ[4];
                   ErrorSZ[0] = m_cmdCurrentCMD.pByCmdArray[0];
                   ErrorSZ[1] = -1;
                   ErrorSZ[2] = -1;
                   ErrorSZ[3] =  ErrorSZ[0]-2;
                   sendto(sockfd,ErrorSZ,4,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
               }
               else{
                   pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                   pSendData[1] = data&0xff;
                   pSendData[2] = ((data>>8)&0xff);
                   pSendData[3] = CheckDigit(pSendData,3);
                   sendto(sockfd,(char *)pSendData,4,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
                    delete [] pSendData;
               }
               break;
               case DoubleWrite:
           //       cout<<"----ok DoubleWrite--"<<endl;
                    pSendData = new unsigned char [2];
                    pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                    addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                    addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                    addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                    data1 = m_cmdCurrentCMD.pByCmdArray[4];
                    data2 = m_cmdCurrentCMD.pByCmdArray[5];
                    reg.WriteDoubleByte(addr1,addr2,addr3,data1,data2);
                    pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                    sendto(sockfd,(char *)pSendData,2,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
              //    printf("%x %x\n",pSendData[0],pSendData[1]);
                    delete [] pSendData;
                    break;
                    case FourRead:
                       //cout<<"----ok FourRead--"<<endl;
                          pSendData = new unsigned char [6];
                          pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                          addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                          addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                          addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                          itt = reg.ReadFourByte(addr1,addr2,addr3);
                          p = (char *)&itt;
                          pSendData[1] = *p;
                          pSendData[2] = *(p+1);
                          pSendData[3] = *(p+2);
                          pSendData[4] = *(p+3);
                          pSendData[5] = CheckDigit(pSendData,5);
                          sendto(sockfd,(char *)pSendData,6,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
                          break;
                      case FourWrite:
            //              cout<<"----ok FourWrite--"<<endl;
                          pSendData = new unsigned char [2];
                          pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                          addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                          addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                          addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                          data1 = m_cmdCurrentCMD.pByCmdArray[4];
                          data2 = m_cmdCurrentCMD.pByCmdArray[5];
                          data3 = m_cmdCurrentCMD.pByCmdArray[6];
                          data4 = m_cmdCurrentCMD.pByCmdArray[7];
                          reg.WriteFourByte(addr1,addr2,addr3,data1,data2,data3,data4);
                          pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                          sendto(sockfd,(char *)pSendData,2,0,(struct sockaddr *)&cliaddr,(socklen_t)len);
                   //     printf("%x %x\n",pSendData[0],pSendData[1]);
                          delete [] pSendData;
                          break;
                      case	NoOperation:break;
                      default:
                        break;
                      }
                      ClearCmdStruct();
                  }
          }
  }


void CGsCmdProcess::DataArraveProcess(queue <unsigned char>	*pDataQueue,QTcpSocket *NewSocket)
{
    float itt = 0;
    char * p ;
    char addr1 = 0;
    char addr2 = 0;
    char addr3 = 0;

    char data1 = 0;
    char data2 = 0;
    char data3 = 0;
    char data4 = 0;
    short data;
    CRegMap reg;
    unsigned char byFront;
    byFront = pDataQueue->front();
    pDataQueue->pop();
    if(CheckCmdHead(byFront))
    {
        int i=0;
        for(i=1; i<m_cmdCurrentCMD.nCmdLength; i++){
            m_cmdCurrentCMD.pByCmdArray[i] = pDataQueue->front();
            pDataQueue->pop();
        }
//		printf("~~~~~~~%d~~~~~%d\n",m_cmdCurrentCMD.pByCmdArray[m_cmdCurrentCMD.nCmdLength-1],CheckDigit(m_cmdCurrentCMD.pByCmdArray,m_cmdCurrentCMD.nCmdLength-1));
			//Get full command
        unsigned char *pSendData;
        pSendData = NULL;


        if(m_cmdCurrentCMD.pByCmdArray[m_cmdCurrentCMD.nCmdLength-1]
            != CheckDigit(m_cmdCurrentCMD.pByCmdArray,m_cmdCurrentCMD.nCmdLength-1))
        {
        //    printf("in gscmdprocess head cmd error\n");
        //    exit(-1);
        }
        else  //Analyze the received tcp command
        {
            unsigned char byAddress, byLen, i;
            byLen = ((m_cmdCurrentCMD.pByCmdArray[0]&0x0C)>>2) + 1;
            switch(m_cmdCurrentCMD.tCurrentOperation)
            {
            case	SingleWrite://	cout<<"-----ok singlewrite---"<<endl;
                                    pSendData = new unsigned char [2];
                                    pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                                    byAddress = m_cmdCurrentCMD.pByCmdArray[1];
                           //         cout<<"by:"<<byLen<<endl;
                                    for(i=0;i<byLen;i++)
                                    {
                                        reg.WriteSingleByte((byAddress+i),m_cmdCurrentCMD.pByCmdArray[i+2]);
                                    }
                                    pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                                    NewSocket->write((char *)pSendData);
                                    delete [] pSendData;
                                    break;
            case DoubleRead:
         //      cout<<"----ok DoubleRead--"<<endl;
             pSendData = new unsigned char [4];//
             pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
             addr1 = m_cmdCurrentCMD.pByCmdArray[1];
             addr2 = m_cmdCurrentCMD.pByCmdArray[2];
             addr3 = m_cmdCurrentCMD.pByCmdArray[3];
             data = reg.ReadDoubleByte(addr1,addr2,addr3);
             if(-1 == data){
                 char ErrorSZ[4];
                 ErrorSZ[0] = m_cmdCurrentCMD.pByCmdArray[0];
                 ErrorSZ[1] = -1;
                 ErrorSZ[2] = -1;
                 ErrorSZ[3] =  ErrorSZ[0]-2;
                  NewSocket->write(ErrorSZ,4);
             }
             else{
                 pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                 pSendData[1] = data&0xff;
                 pSendData[2] = ((data>>8)&0xff);
                 pSendData[3] = CheckDigit(pSendData,3);
                 NewSocket->write((char*)pSendData,4);
                  delete [] pSendData;
             }
             break;
             case DoubleWrite:
         //       cout<<"----ok DoubleWrite--"<<endl;
                  pSendData = new unsigned char [2];
                  pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                  addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                  addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                  addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                  data1 = m_cmdCurrentCMD.pByCmdArray[4];
                  data2 = m_cmdCurrentCMD.pByCmdArray[5];
                  reg.WriteDoubleByte(addr1,addr2,addr3,data1,data2);
                  pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                  NewSocket->write((char *)pSendData,2);
            //    printf("%x %x\n",pSendData[0],pSendData[1]);
                  delete [] pSendData;
                  break;
                  case FourRead:
               //      cout<<"----ok FourRead--"<<endl;
                        pSendData = new unsigned char [6];
                        pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                        addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                        addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                        addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                        itt = reg.ReadFourByte(addr1,addr2,addr3);
                    //    itt = 0.53;
                        p = (char *)&itt;
                        pSendData[1] = *p;
                        pSendData[2] = *(p+1);
                        pSendData[3] = *(p+2);
                        pSendData[4] = *(p+3);
                        pSendData[5] = CheckDigit(pSendData,5);
                        NewSocket->write((char *)pSendData,6);
                        delete [] pSendData;
                        break;
                    case FourWrite:
          //              cout<<"----ok FourWrite--"<<endl;
                        pSendData = new unsigned char [2];
                        pSendData[0] = m_cmdCurrentCMD.pByCmdArray[0];
                        addr1 = m_cmdCurrentCMD.pByCmdArray[1];
                        addr2 = m_cmdCurrentCMD.pByCmdArray[2];
                        addr3 = m_cmdCurrentCMD.pByCmdArray[3];
                        data1 = m_cmdCurrentCMD.pByCmdArray[4];
                        data2 = m_cmdCurrentCMD.pByCmdArray[5];
                        data3 = m_cmdCurrentCMD.pByCmdArray[6];
                        data4 = m_cmdCurrentCMD.pByCmdArray[7];
                        reg.WriteFourByte(addr1,addr2,addr3,data1,data2,data3,data4);
                        pSendData[1] = m_cmdCurrentCMD.pByCmdArray[0];
                        NewSocket->write((char *)pSendData,2);
                 //     printf("%x %x\n",pSendData[0],pSendData[1]);
                        delete [] pSendData;
                        break;
                    case	NoOperation:break;
                    default:
                   NewSocket->write("eeeee",2);break;
					}
					ClearCmdStruct();
				}
		}
}

//
bool CGsCmdProcess::CheckCmdHead(unsigned char byData)
{
    if((m_cmdCurrentCMD.nCmdLength == 0) && ((byData & 0xC3) == GS_CMD_HEAD))
	{
		if(m_cmdCurrentCMD.pByCmdArray != NULL){
			delete [] m_cmdCurrentCMD.pByCmdArray;
			m_cmdCurrentCMD.pByCmdArray = NULL;
		}
		m_cmdCurrentCMD.nCmdLength = 0;
		m_cmdCurrentCMD.nCurrentPos = -1;
        //block operation
		if((byData & GS_CMD_BLOCK_OP)==GS_CMD_BLOCK_OP)
		{
			if((byData & GS_CMD_PC_WRITE)==GS_CMD_PC_WRITE){
				m_cmdCurrentCMD.nCmdLength = 1;
				m_cmdCurrentCMD.tCurrentOperation = BlockWrite;
			}
			else{
				m_cmdCurrentCMD.nCmdLength = 4;
				m_cmdCurrentCMD.tCurrentOperation = BlockRead;
			}
        }
        //signal operation
		else
        {
			if((byData & GS_CMD_PC_WRITE)==GS_CMD_PC_WRITE)
			{
                if((((byData & 0x0c)>>2) ==0x00) || (((byData & 0x0c)>>2) ==0x2)){
                    m_cmdCurrentCMD.nCmdLength = ((byData&0x0C)>>2)+4;
                    m_cmdCurrentCMD.tCurrentOperation = SingleWrite;
                }
                else if(((byData & 0x0c)>>2)==0x01){
                    m_cmdCurrentCMD.nCmdLength = ((byData&0x0C)>>2)+6;
                    m_cmdCurrentCMD.tCurrentOperation = DoubleWrite;
                }
                else if(((byData & 0x0c)>>2)==0x03){
                    m_cmdCurrentCMD.nCmdLength = ((byData&0x0C)>>2)+6;
                    m_cmdCurrentCMD.tCurrentOperation = FourWrite;
                }
            }
            else
            {
                if(((byData & 0x0c)>>2)==0x00 || (((byData & 0x0c)>>2) ==0x2)){
                    m_cmdCurrentCMD.nCmdLength = 3;
                    m_cmdCurrentCMD.tCurrentOperation = SingleRead;
                }
                else if(((byData & 0x0c)>>2)==0x01){
                    m_cmdCurrentCMD.nCmdLength = 5;
                    m_cmdCurrentCMD.tCurrentOperation = DoubleRead;
                }
                else if(((byData & 0x0c)>>2)==0x03){
                    m_cmdCurrentCMD.nCmdLength = 5;
                    m_cmdCurrentCMD.tCurrentOperation = FourRead;
                }

            }
		}
        m_cmdCurrentCMD.pByCmdArray = new unsigned char[m_cmdCurrentCMD.nCmdLength];
		m_cmdCurrentCMD.pByCmdArray[0] = byData;
		m_cmdCurrentCMD.nCurrentPos = 1;

		return true;
	}
  //  cout<<"------cheakcmdhead error--------"<<endl;

	return false;
}

void CGsCmdProcess::ClearCmdStruct(void)
{
	if(m_cmdCurrentCMD.pByCmdArray != NULL)
		delete [] m_cmdCurrentCMD.pByCmdArray;
	m_cmdCurrentCMD.pByCmdArray = NULL;
	m_cmdCurrentCMD.nCmdLength = 0;
	m_cmdCurrentCMD.nCurrentPos = 0xFF;
	m_cmdCurrentCMD.tCurrentOperation = NoOperation;
}

