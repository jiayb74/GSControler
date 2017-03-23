#include "FIleToArm.h"
#include <sstream>
#include <string>
#include <QFileInfo>
#include "UpdateProgram.h"
#include "RegTable.h"

FileToArm::FileToArm()
{
    start();
}

void FileToArm::FileClient()
{
  //与新链接都客户端通讯
    NewFileFromPcToArm = FileFromPcToArm->nextPendingConnection();
  //connect(NewFileFromPcToArm,SIGNAL(disconnected ()),this,SLOT(FreePoint()),Qt::DirectConnection);
  //有新都数据发来就执行ReadData()解析数据，在主线程中执行
  //Qt::DirectConnection在当前线程执行槽函数
    SendFileToArm();
  //connect(NewFileFromPcToArm,SIGNAL(readyRead()),this,SLOT(SendFileToArm()),Qt::DirectConnection);
}

void FileToArm::FreePoint()
{
    delete this;
}
void FileToArm::run()
{
  //cout<<"------fun file to arm-----"<<QThread::currentThreadId()<<endl;
  //创建文件服务器
    FileFromPcToArm = new QTcpServer();
    if(!FileFromPcToArm->listen(QHostAddress("192.168.47.100"),4757))
    {

    }
    connect(FileFromPcToArm,SIGNAL(newConnection()),this,SLOT(FileClient()),Qt::DirectConnection);
    exec();
}


void FileToArm::SendFileToArm(void)
{
    for(;;){
        char buf[FILEBUFFLENGTH] = {0};
        NewFileFromPcToArm->waitForReadyRead();
        NewFileFromPcToArm->read(buf,sizeof(buf));
        if(strncmp(buf,UPDATACOMPLETE,strlen(UPDATACOMPLETE)) == 0)break;
        char * FilePath = NULL;
        int f_fd;
        queue<char> m_Qsend;
        unsigned int TotalBytes = 0;
        unsigned int LeftBytes = 0;
        int i = 0;
        string Cmd;
        try{
            Cmd = buf;
            Cmd = Cmd.substr(9,10);
            stringstream ss(Cmd);
            ss>>TotalBytes;
            LeftBytes = TotalBytes;
        }
        catch(std::exception & e){
            cout<<"file exption"<<endl;
            return ;
        }
        FilePath = (char *)calloc(40+strlen("./doc/"),sizeof(char));
        strcat(FilePath,"./doc/");
        strcat(FilePath,buf+19);
        f_fd = open(FilePath,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
        if(f_fd == -1){
          //perror("open");
            return;
         }
        //发送请求上位机发送文件内容
        NewFileFromPcToArm->write("@00013-170000000000",19);
        memset(buf,0,sizeof(buf));
        while((LeftBytes >0)  && (LeftBytes <= TotalBytes)){
            NewFileFromPcToArm->waitForReadyRead();
            int RendBytes = NewFileFromPcToArm->read(buf,FILEBUFFLENGTH);
            if(RendBytes <= 1033){
                LeftBytes = LeftBytes - RendBytes + 9;
            }
            else{
                LeftBytes = LeftBytes - RendBytes + ((RendBytes+1032)/1033)*9;
            }
            while(i<RendBytes)
            {
                m_Qsend.push(buf[i]);
                i++;
            }
            i = 0;
            memset(buf,0,sizeof(buf));
        }
        NewFileFromPcToArm->write("@00003-20",10);

        char DateBuf[DATABUFF] = {0};
        int WriteBYtes = 0;
        int m = 0;
        int j = 0;
        int z = 0;

        WriteBYtes = m_Qsend.size();
        for(int n = 0;n< WriteBYtes; ){
            if(m_Qsend.size() < 1033){
                for(int f = 0; f< 9; f++){
                     m_Qsend.pop();
                     n++;
                }
                z = m_Qsend.size();
                m = 0;
                while(m < z){
                        DateBuf[j] = m_Qsend.front();
                        j++;
                        m_Qsend.pop();
                        n++;
                        m++;
                }
                m = 0;
                j = 0;
                write(f_fd,DateBuf,z);
                memset(DateBuf,0,sizeof(DateBuf));
            }
            else{
                while(m < 1033){
                    if(m < 9){
                        m_Qsend.pop();
                        n++;
                    }
                    else{
                        DateBuf[j] = m_Qsend.front();
                        j++;
                        m_Qsend.pop();
                        n++;
                    }
                    m++;
                }
                m = 0;
                j = 0;
                write(f_fd,DateBuf,1024);
                memset(DateBuf,0,sizeof(DateBuf));
            }
        }
        close(f_fd);
        //if need update 51 program
        QFileInfo fileInfo(FilePath);
        if(fileInfo.suffix() == "bin"){
            QString ID = fileInfo.baseName();
            UpdateProgram  update(ID.toInt());
            QFile::remove(FilePath);//刪除文件
        }
        free(FilePath);
    }
}

FileToArm::~FileToArm()
{


}
