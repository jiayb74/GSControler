#define		CURRENTSTEP			0xc8
#define		DEFAULTWAIT			1200
#define		SHORTWAIT			200
#define		SHUTDOWN_TIMEOUT	300		//in unit of second
#define		SWITCH_TIMER		1000
#ifndef     _SOFTSWITCH_H__
#define     _SOFTSWITCH_H__

#define BOOL bool
#define BYTE unsigned char

#include <QThread>
#include "CanDateAnalysis.h"
#include "LDKeeper.h"
#include "HotSwap.h"
#include <map>
#include "PortState.h"

#define GS_MODULE_MODULE   "Module"
#define GS_MODULE_EXMODULE "ExModule"
#define GS_MODULE_LEVEL   "Level"
#define GS_MODULE_OUTPUT  "OutPut"
#define GS_MODULE_MODIF   "Modif"
#define GS_MODULE_SWAPLEVEL   "SwapLevel"
#define GS_KEY_LD          "LD"
typedef struct
{
    int BoardNo;
    int LDNo;
    int TempNo;
}BoardIndex,*P_BoardIndex;
class CSoftSwitch : public QThread
{
    Q_OBJECT
public:
    CSoftSwitch();
	virtual ~CSoftSwitch();
    bool InitInstance();
    bool InitIniFile();
    void StartModeSelection();
    void run();
    void InitNetworkStructure();
    void SystemInit();
public slots:
    BYTE	CheckLaserStage();
	void	ForwardLaserStage(BYTE stage);
	BOOL	IsLaserOn();
	void	LaunchLaser();
	void	ShutLaser();
	BOOL	IsScanning();
	void	GoSelfScan();
	void	ScanCompleted();
	BOOL	IsLocked();
    void	ModeLocked(bool state);
	void	LoseLocking();
	void	SetShutDownValue(BOOL bValue);
	BOOL	GetShutDownValue();
	// return ratio that m_nWaitProgress over m_nInitialWait
	double	CheckInitProgress(void);
    void    SwitchTimerProcessThread();
    void    LockProtect();
	void	ExitTimerThread();
    void    Power_Switch(bool status);
    void    Fault_Alarm(char NODE);
    void    timerUpDate();
    void    UpdataUIPower();
    void    ChangeSwitchState(bool status);
    BOOL MoSignal(bool status);
    BOOL MoSignal();
    bool IsPreviousLDCurrentOk();
    bool   IsPreviousLDCurrentOkStatic();
    void Int_Alarm(unsigned char data0,unsigned char data2);
    bool HardwarePlatform();


public:
    Can_LD * CanRecv;
    CStatus * Heartbeat;
    PortState * SwitchPortLevel;
    bool    m_SureShutdown;
	int		m_nInitialWait;
	int		m_nWaitProgress;
	int		m_nTotalLD;
    int     m_nBoard;
    int     m_nLDcnt;
    int     m_nOutPut;
    int     m_nTomodifyPumpNo;
    int     m_nSwapLevel;
    int     *m_nLastPumpNo;
    BoardIndex *m_bdBoardArray;
    LDKeeper ***m_pLastButPump;
    LDKeeper ***m_pLastPump;


    int     m_nToModifyBoard;
    int     m_nLevel;
    int     m_nCommLD;
    int     m_nCommModule;
    int     m_nExBoard;
	int		m_nCurrentLDNo;
    int     m_hSwitchTimerProcessThread;
	BOOL	m_bLOOP;
	BOOL	m_bShutReady;
    BOOL    m_isOffLock;
    bool    m_isUpdateUI;
    bool    m_isScanUpdate;
    bool    m_bModeLockedHandleFlag;

public:
	BYTE	m_byLaserStage;
	BOOL	m_bLaserOn;
	BOOL	m_bIsShutDown;
	BOOL	m_bSelfScan;
	BOOL	m_bModeLocked;
	BOOL	m_bIsRunThread;
    BOOL    MoStatus;
    int     m_OnTime;
    int     m_OffTime;
    int     m_Step;

public:
    void SetLockSt(bool Status);
signals:
    void updatabar(const QString period,const char value);
    void Set_ProgressBar(char status);
    void Change_Led_state(bool state);
    void UI_Laser_State(char state);
    void UI_ShowPower(float presetpower,float power);
    void UpdataPlusFreqPower(float pulse,float freq);
    void Curr_Pos(unsigned short x,unsigned short y);
    void StartScan();
    void SystemLog(string info);
    void UartMolock(bool flag);
    void Protect(bool status);
    void SetDivi();

public:
    LDKeeper * lpLD;
    LDKeeper * LD;
    int ModuleNum;
    unsigned int Inittime;
    CHotSwap * hotSwap;
    multimap<int, LDKeeper  *> * LDMap;
    int WorkMode;
    double basePulseWidth;
    double BaisI;
    int DccIni;
    int PRR_filter;
    float initPulse;
    float initFreq;


};

#endif




	
