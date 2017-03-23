#ifndef SCANNING_H
#define SCANNING_H

#include<QString>
#include<QThread>
#include<QObject>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

class SCANNING:public QThread
{
   Q_OBJECT
public:
    SCANNING();
    virtual ~SCANNING();
    void run();
    bool IsRun;
    unsigned char ScanSource;
    int * ScanArg;
    int TotalSch;
    float TotalTimeSeconds;
    unsigned int TotalStep;
    char  Schematic[3];
    unsigned char SchematicNum;
    unsigned short SaveCurrXPos;
    unsigned short SaveCurrYPos;
    unsigned short CurrXorYPos;
    unsigned short CumulativeStep;
    unsigned char  ScanType;
    typedef struct Point{
        int X_Point;
        int Y_Point;
        char Direction;
        bool Islock;
    }SCANPOINT,*PSCANPOINT;
    typedef struct ValidPoint{
        int X_Point;
        int lineStart;
        int lineEnd;
    }SCANVALIDPOINT,*PVALIDSCANPOINT;
    SCANPOINT ScanPoint ;
    SCANVALIDPOINT ScanValidPoint;
    vector<struct Point> ScanPointVector;
    vector<struct ValidPoint> ValidPointVector;

    typedef struct Acre{
        int X_Start;
        int Length;
    }ACRE;
    ACRE acre;
    vector<ACRE> WuXiaoDian;

    void SetScanSource(unsigned char _ScanSource);
    bool GetScanConfigFile(unsigned char Number);
    void CusScan();
    bool IsScanComplete();
    void RecoveryPos();
    bool IsRecoveryComplete();
    void GetXorYCurrPos(unsigned char index);
    void UpDateProcessBar();
    void GetScanTotalStep();
    void UpDataCumulativeStep(unsigned int n);
    bool IsResetOk();
    void ReadScanData();
    void ScanStyleInfo();
    void SearchWorkingPoint();
    void SearchMaxAcre();

public slots:
    void    On_Setpos_Slot(unsigned short _x_target,unsigned short _y_target);
    void    On_Reset_Slot();
    void    On_Scan_Slot(unsigned short _x_start,unsigned short _x_end,
                         unsigned short _y_start,unsigned short _y_end,
                         unsigned short _step,unsigned char ScanStyle);
    void    StartWebScan();
    void    GetScanInfo();
    void    On_Abort_Slot();
    void    On_Getpos_Slot();
    void    GetScanPos();
    void    StartCusScan();
    void    CancelScan();
    void    ReadScanDataEnd();
signals:
    void Curr_Pos(unsigned short x,unsigned short y);
    void UpDateUi(unsigned int _TotalSch,unsigned int _Curr);
    void On_ScanPos_Signels(unsigned short curr_x_pos,unsigned short curr_y_pos,
                            unsigned short tar_x_pos,unsigned short tar_y_pos,
                            unsigned char ScanStatue);
    void UpDataBar(float prec);
};

#endif // SCANNING_H
