#ifndef SYSTEMLOG_H
#define SYSTEMLOG_H
#include <QString>
#include <QThread>
#include <QObject>
#include <iomanip>
#include <fstream>
#include <iostream>

using namespace std;

class SystemLog:public QThread
{
    Q_OBJECT
public:
    SystemLog();
    ~SystemLog();
    virtual void run();

    static void Systemlog(string info);
public slots:
    void WriteSystemLog(string info);

};


#endif // SYSTEMLOG_H
