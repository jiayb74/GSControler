#ifndef BOARD_H
#define BOARD_H

#include "LDKeeper.h"

typedef enum boardtype
{
    ThreeInOne = 1,
    SeedSource = 2,
    BaseBoard = 3
} BoardType;

class Board
{
public:
    int CAN_ID;        //每个板子有且只有一个唯一CAN_ID
    static int BoardNum;
    static int CurrentBoardIndex;
    LDKeeper  * LD;      //LD
    int LDNumber;
    int ExistLD;
    int SetIDNum;
    int CurrentLDIndex;
    float BoxAirTemp;
    float BoxPadTemp;
    BoardType BType;
    Board();
    Board(int ldnumber,int id);
    void InitBoard(int ldnumber,int id);
    int AddLD(LDKeeper ld);
    bool IsOverload();
    void SetBoardCanID(int id);
    int IsSetID(void);
    static void SetBoardNum(int num);
    static int GetBoardNum();
    void OutInfo();
    void SetLDNumber(int num);
    void OffCurrent();
    void OffAllCurrent();
    void OffCareCurrent();
    int GetBoardID();
    int GetLDNumber();
    void SetAirTemp(float temp);
    float GetAirTemp();
    void SetPadTemp(float temp);
    float GetPadTemp();
    void InitBoardTemp();

};

#endif // BOARD_H
