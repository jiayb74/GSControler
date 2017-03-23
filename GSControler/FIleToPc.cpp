#include "FileToPc.h"
#include <string>
#include <QString>
#include <sstream>

FileToPc::FileToPc(unsigned char _dat)
{
    this->dat = _dat;
    start();
}

void FileToPc::run()
{
    connect(this,SIGNAL(finished ()),this,SLOT(ThreadFinishedClear()));
    SendFileToPc();
    exec();
}
void FileToPc::ThreadFinishedClear()
{
    delete client;
    delete this;
}
void FileToPc::FreePoint()
{
    this->quit();
}

/*********************************
*函数功能：ARM发送文件到上位机 ARM为客户端
*输入参数：无
 //   QTcpSocket *client;
 // cout<<"-----arm to pc----"<<QThread::currentThreadId()<<endl;

*输出参数：无
*返 回 值：无
*********************************/
void FileToPc::SendFileToPc()
{
    client = new QTcpSocket();
    client->connectToHost(QHostAddress("192.168.47.99"),4756);
    client->waitForConnected();
    connect(client,SIGNAL(disconnected ()),this,SLOT(FreePoint()),Qt::DirectConnection);
    string FileName;
    switch(dat)
    {
    case 1:
        FileName = "GSControler.ini";
        break;
    case 2:
        FileName = "ScanResultData.txt";
        break;
    case 3:
        FileName = "Inspector.ini";
        break;
    case 4:
        FileName = "GSControllerLogs1.txt";
        break;
    case 5:
        FileName = "GSControllerLogs2.txt";
        break;
    case 6:
        FileName = "HardwareConfig.ini";
        break;
    case 7:
        FileName = "CalInfo.ini";
        break;
    case 8:
        FileName = "XC.ini";
        break;
    case 9:
        FileName = "PowerResultData.txt";
        break;
    }

    string FilePath = "./doc/";
    //文件路径
    FilePath = FilePath+FileName;
    //打开待发送文件
    int f_open = open(FilePath.c_str(),O_RDONLY);
    if(f_open == -1){
 //       perror("f_open error:");
        client->write("@00003-25",9); //no such file
        return;
    }
    //读取文件内容缓冲区，文件最大为40K字节
    char file_buf[4096*20] = {0};
  //char * file_buf = new char[1024*10];
    int n_bytes = read(f_open,file_buf,sizeof(file_buf));
    if(n_bytes == -1)
    {
  //    perror("read error:");
//		exit(0);
    }
    else if(n_bytes == 0){
        //The file exists but the content is empty
        client->write("@00003-25",9);
        return;
    }

 // cout<<"nbyte:"<<n_bytes<<endl;
    string file_bytes = "0000000000";
    stringstream num_bytes;
    num_bytes<<n_bytes;

    int len = num_bytes.str().size();

    file_bytes.erase(0,len);
    file_bytes = file_bytes + num_bytes.str();
 // cout<<file_bytes<<endl;
 // cout<<"4444444"<<endl;
 // 发送数据格式封装
    string send_file;
    int cmd_len = FileName.size()+13;
    stringstream cmd;
    cmd<<cmd_len;

    int len1 = cmd.str().size();
    string cmd_l = "00000";
    cmd_l.erase(0,len1);

    send_file = "@"+cmd_l+cmd.str()+"-16"+file_bytes+FileName;
   //cout<<send_file<<endl;

    //第一步 发送文件长度和文件名
    int n = client->write(send_file.c_str(),send_file.size());
    if(n == -1){
        perror("sendto error:");
   //   exit(0);
    }

    //接收缓冲区
    char buf[1024] = {0};
    //第二步 接收服务器消息 从哪里发送 默认开始从头发送
    client->waitForReadyRead();
    client->read(buf,sizeof(buf));
  //read(ToPc_fd,buf,sizeof(buf));
  //printf("%s\n",buf);

    //第三步 发送文件到服务器
    string s;
    string SendToPcBytes;
    unsigned int TotalSendBytes;
    unsigned int LeftSendBytes;
    unsigned int ReadySendBytes;
    TotalSendBytes = n_bytes;
    LeftSendBytes = n_bytes;
 //   char wt[1024];
    string aa ;
    string file_buf_temp = file_buf;
    if(n_bytes > 1024){
        while(LeftSendBytes > 0){
          ReadySendBytes = (LeftSendBytes < 1024?LeftSendBytes:1024);
          if(ReadySendBytes < 1024){
              string cmdlength;
              cmdlength = "00000";
              stringstream ss;
              ss<<(ReadySendBytes+3);
              string strCmd = ss.str();
              cmdlength.erase(0,strCmd.size());
              cmdlength = cmdlength + strCmd;
              aa = cmdlength;
          //  cout<<"aa"<<aa<<endl;
          }
          else
              aa = "01027";
          s = "@"+aa+"-19"+file_buf_temp.substr(0,ReadySendBytes);
          client->write(s.c_str(),ReadySendBytes+9);
          file_buf_temp.erase(0,ReadySendBytes);
          LeftSendBytes = LeftSendBytes - ReadySendBytes;
        }
    }
    else{
        ReadySendBytes = (LeftSendBytes < 1024?LeftSendBytes:1024);
        string cmdlength;
        cmdlength = "00000";
        stringstream ss;
        ss<<(ReadySendBytes+3);
        string strCmd = ss.str();
        cmdlength.erase(0,strCmd.size());
        cmdlength = cmdlength + strCmd;
        aa = cmdlength;
     // cout<<"aa"<<aa<<endl;
        s = "@"+aa+"-19"+file_buf_temp;
        client->write(s.c_str(),ReadySendBytes+9);

    }

    //第五步 接收服务器都响应  成功响应@00013-200000000000
    client->waitForReadyRead();
    client->read(buf,sizeof(buf));
  //printf("%s\n",buf);

}

FileToPc::~FileToPc()
{
 //  cout<<"----xigou arm to pc-----"<<QThread::currentThreadId()<<endl;
 //  printf("-----xigou-----\n");
}

