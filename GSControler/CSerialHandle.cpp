/*************************

说明：主要实现配置串口。

**************************/

#include "CSerialHandle.h"
#include <string.h>
#include <iostream>
using namespace std;

CSerialHandle* CSerialHandle::m_serialHandle = 0;
CSerialHandle *CSerialHandle::getInstance()
{
    if(m_serialHandle==0)
    {
        m_serialHandle = new CSerialHandle();
        m_serialHandle->initSerialHandle();
        m_serialHandle->start();

    }

    return m_serialHandle;
}


int CSerialHandle::openSerial(char *cSerialName)
{
    int m_fd;
    struct termios opt;

    m_fd = open(cSerialName, O_RDWR | O_NOCTTY);
    if(m_fd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(m_fd, &opt);
    cfsetispeed(&opt, B9600);
    cfsetospeed(&opt, B9600);

    opt.c_cflag |= PARENB;
    opt.c_cflag |= CREAD;
    opt.c_cflag |= CLOCAL;
    opt.c_cflag &= ~PARODD;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;

    opt.c_iflag |= (INPCK | ISTRIP);
    opt.c_oflag &= ~OPOST;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    opt.c_cc[VMIN]   =   0xFF;
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(m_fd,   TCSANOW,   &opt)<0) {
        return   -1;
    }
    return m_fd;
}

void CSerialHandle::run()
{
    ReadThreadFunc();
}
DWORD CSerialHandle::ReadThreadFunc()
{
    char tmp[1024];
    int len= 0;
    while(TRUE)
    {
        len = read(m_fd, tmp, 0x01);
        for(int idx = 0; idx< len; idx++)
        {
            m_Qrecieve.push(tmp[idx]);       
        }

    }
}

DWORD CSerialHandle::WriteThreadFunc()
{
    unsigned char* p_ch;
    while(TRUE)
    {
        while(m_Qsend.empty()!=true)
        {
            p_ch = &(m_Qsend.front());
            write(m_fd,p_ch,1);
            m_Qsend.pop();
        }
    }
}

bool CSerialHandle::WriteSyncPort(const char* buf)
{
    int iHasWritten = 0;
    const char* p_ch = buf;
    while(true)
    {
        if(iHasWritten == strlen(buf))
        {
            break;
        }
        write(m_fd,p_ch,1);
        iHasWritten++;
        p_ch++;
    }
}
int CSerialHandle::closeSerial()
{
    close(m_fd);
}
DWORD CSerialHandle::initSerialHandle()
{
    m_fd = openSerial("/dev/ttySP0");
    if(m_fd < 0)
    {
        return 0xFF;
    }
}
