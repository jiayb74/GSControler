#ifndef _HOTSWAP_H__
#define _HOTSWAP_H__

#include "GSDefine.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <map>
#include "LDKeeper.h"
#include "win2linuxtypdef.h"
#include "GsApi.h"
#include "GSDefine.h"

class CHotSwap:public QObject
{
    Q_OBJECT
public:
    CHotSwap(LDKeeper * _lpLD,int _LDNum, multimap<int, LDKeeper *> *_LDMap);
    CHotSwap();
	virtual ~CHotSwap(void);
public:
	virtual BOOL InitInstance();

public:
    LDKeeper * lpLD;
    LDKeeper * SeedBox;
    multimap<int, LDKeeper * > * LDMap;
    int LDNum;
    int LastFreqIndex;
    float PresetPower;
    unsigned char Pwidth1;
    unsigned char Pgap1;
    unsigned char Pwidth2;
    unsigned char Pgap2;
    unsigned char Pwidth3;
    LDKeeper	*m_pLastPumpA;
    LDKeeper	*m_pLastPumpB;
    unsigned char SwapAB;


    unsigned int	m_unCurrentFreq;
    unsigned int	m_unFreqMax;
    unsigned int	m_unFreqMin;
    unsigned int	m_unCurrentPulse;
    unsigned int    m_PDCnt;
    LDKeeper		*m_pLastPump;
//	CLDKeeper		*m_pLastButPump;
	int				m_nToModifyPumpNo;
	int				m_nLastPumpNo;
	double			*m_dWrite;
	double			*m_dRead;
    double			*m_dLWrite;
    double			*m_dLRead;
    double			*m_dHWrite;
    double			*m_dHRead;
	double			m_dWriteMin;
    double			m_dWriteMax;
	double			m_dReadMin;
    double			m_dLWriteMin;
    double			m_dLWriteMax;
    double			m_dLReadMin;
    double			m_dHWriteMin;
    double			m_dHWriteMax;
    double			m_dHReadMin;
	double			m_dPmax;
    double          m_P1PowerLimit;
	U16				m_unDivider;
    //U16			m_unP2ITarget;
    unsigned int	*m_unFreqOption;
	int				m_nFreqOptionCount;
    unsigned int	*m_unPulseOption;
	int				m_nPulseOptionCount;
	double			m_dPower;
	BOOL			m_bIs3rdOrder;
	BOOL			m_bPulCntAdj;
    unsigned int	m_unPulseMax;
    unsigned int	m_unPulseMin;
    //double		m_dPowerMedian;
	double			m_dRootMin;
	double			m_dRootMax;

	BOOL SearchFreqOptions();
	void InitOptions();
	// match frequency input to candidates
    BOOL MatchFreqOption(unsigned int freqValue);
	// set divider to FPGA registers
	void SetFreq(U16 divider);
	// alter working frequency
    void AlterFrequency(unsigned int freqValue, BOOL lasing);	
    void ConfigWRarray(const char * lpwSecName);
    void ConfigPmaxPxI(const char * lpwSecName, unsigned int pulseCnt, BOOL bPulseAdj, BOOL lasing,int index=-1);
	// set pulse width/count to FPGA register
    void SetPWidth(unsigned char pulseCnt);
	// alter pulse envelop to change peak power
    void AlterPulseWidth(unsigned char  pulseCnt, BOOL lasing);
    BOOL SearchPulseOption(const char * lpwFreqSection);
    BOOL MatchPulseOption(unsigned int widthValue);
	// output optical power to powerValue in unit of w
	BOOL SetOutputPower(double powerValue, double prvPower);
	void SetOutputCurrent(double currentValue, double prvCurrent);
	//ݹʼֵ
	double PowerToCurrent(double dPowerValue/*,double * m_dWrite,BOOL b3rdOrder*/);
	//ݵֵ㹦
	double CurrentToPower(double dCurrentValue/*, double * m_dRead,BOOL b3rdOrder*/);
    double CurrentToPower(double dCurrentValue,double ReadMin,double * dRead);
	double WriteCurrentToPower(double dCurrentValue);
    double PowerToCurrent(double dPowerValue,int * Real);
public:
	// got command to swap frequency, check whether it is proper timing and valid option, and then take action
    unsigned char HandleFreqSwap(unsigned int freqValue, unsigned char Lstatus);
	// got command to swap pulsewidth, check whether it is proper timing and valid option, and then take action
    unsigned char HandlePWidthSwap(unsigned int widthValue, unsigned char Lstatus);
	// get value of current output optical power
	double GetRealPower();
    void SetPower(double Power);
	// check whether it is proper timing and valid power value to output, and then take action
    unsigned char HandlePowerSwap(double powerValue, unsigned char Lstatus);
    unsigned char HandlePowerSwap(unsigned char Lstatus,unsigned char Ratio);
  //  unsigned char HandlePowerSwap(unsigned char Lstatus);
    int GetCurrentFreq();
    int GetCurrentPlus();
    void Set_PD_Dividerd(int Reg,unsigned short Value);
    bool IsMeetFreqRange(int Freq);
    int  SeekLastFreqIndex(int Freq);
    void SetPulseGroup(unsigned char pluswidth);
    void SetPulseGroupValue(unsigned char _width1,
                            unsigned char _pgap1,
                            unsigned char _width2,
                            unsigned char _pgap2,
                            unsigned char _width3
                            );
    void GetSetcInfo(unsigned char * arg);
    string FineTuning(int Node,string StrValue = "NO");
    void FineTuningAction(unsigned char  P,unsigned char flag,double Value);
    void SetOutPutAorB(unsigned char flag);
    void SPOExchange(bool state);
    bool GetSPOExchangeState();
    bool IsSingleSpot();
    unsigned int DefaultPlus();
    void UpdataUI();
    float GetPresetPower();
    unsigned int GetDiVer(unsigned int Freq);
    void InitSPOArg();
    void    SyncPlusInXCFile();
    void SetLPower(double power);
    void SetHPower(double power);
    BOOL SetOutputPowerUpper(double powerValue);
signals:
    void UpdataPlusFreqPower(float puls,float freq);
public slots:
   void SetDivi();
};

#endif
