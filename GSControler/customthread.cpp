#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>>
#include "customthread.h"

CustomThread::CustomThread()
{
    CustomID = -1;
    start();
}

CustomThread::~CustomThread()
{

}

void CustomThread::run()
{
   int sockfd;
   socklen_t len;
   int addr_len;
   struct sockaddr_in servaddr,cliaddr;
   struct sockaddr ss;
   sockfd = socket(AF_INET,SOCK_DGRAM,0);
 //  bzero((&servaddr,sizeof(servaddr)));
   addr_len=sizeof(struct sockaddr_in);
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(4700);
   if(bind(sockfd,(struct sockaddr*)(&servaddr),sizeof(servaddr)) == -1)
   {
       perror("bind error");
   }
   char buf[20] = {0};
      CGsCmdProcess cmd;
   for(;;)
   {
       int n = recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&cliaddr,&len);
  //     cout<<"----------------@@-------"<<endl;
       queue<unsigned char> que;
       for(int i=0; i<n; i++){
      //     printf("\n\nrecv: %.2x ",buf[i]);
           que.push(buf[i]);
       }
       cmd.DataArraveProcess(&que,sockfd, cliaddr,len);
  //     cout<<"----------------00-------"<<endl;

   }

}

void CustomThread::LinkDisconnect()
{
    cout<<"-----UDP LinkDisconnect!"<<endl;
}

void CustomThread::Close()
{

}
