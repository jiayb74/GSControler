#include <iostream>
#include <iomanip>
#include <fstream>
#include "SystemLog.h"

ofstream outq;

SystemLog::SystemLog()
{
    outq.open("./doc/GSControllerLogs1.txt",ios_base::app);
}

void SystemLog::run()
{
   exec();
}

void SystemLog::WriteSystemLog(string info)
{
    outq<<"SystemLog:"<<info<<"\r"<<endl;
}

void SystemLog::Systemlog(string info)
{
    outq<<"SystemLog:"<<info<<"\r"<<endl;
}

SystemLog::~SystemLog()
{
    outq.close();
}
