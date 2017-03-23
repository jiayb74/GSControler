#ifndef UPDATEPROGRAM_H
#define UPDATEPROGRAM_H

#include <QThread>
#include <QObject>


class UpdateProgram:public QThread
{
    Q_OBJECT
public:
    UpdateProgram(int Module);
    ~UpdateProgram();
    virtual void run();
    int ID;


};

#endif // UPDATEPROGRAM_H
