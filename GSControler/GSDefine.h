#ifndef GSDEFINE
#define GSDEFINE
#define DBG
//#include "win2linuxtypdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// for module system
#define GS_MODULE_BOARD   "Board"
#define GS_MODULE_EXBOARD "ExBoard"
#define GS_MODULE_LEVEL   "Level"
#define GS_MODULE_OUTPUT  "OutPut"
#define GS_MODULE_MODIF   "Modif"
//selfdefition
#define    GS_INI_KEY_PDCNT   "PDCnt"



////?ARM coding version
#ifndef		GS_CODE_VERSION
#define		GS_CODE_VERSION				L"R3-03-17"
#define		GS_CODE_RELEASE_VER			_T("V 3.0.0.0")
//???
//#define     GS_REGVERSION_P03H             0xC2
#define     GS_REGVERSION_P03H             0xC3
#define		GS_REGVERSION_P07			   0xD1
#define		GS_REGVERSION_P08			   0xD2
#define		GS_REGVERSION_P07HM			   0xD3
#define		GS_REGVERSION_P06			   0xD4
#endif //GS_CODE_VERSION
//????(W)
#define GS_SWITCH_DEFAULT_POWER		0.2
#define GS_DATA_LENGTH				20
//define model type
#define		GS_P03	L"Y12-P03"
#define		GS_P03H L"Y12-P03H"
#define		GS_P50	L"Y12-P50"
#define		GS_P07	L"Y12-P07"
#define		GS_P07H L"Y12-P07H"
#define		GS_P07M L"Y12-P07M"
#define		GS_P06  L"Y12-P06"
#define		GS_P06H L"Y12-P06H"

//#define		GS_P51  L"Y12-P51"
#define		GS_P08	L"Y12-P08"
#define		GS_F01  L"Y15-F01"

#define     P03HFPGA 20000
//Define the data type which be used in GS code
#ifndef		U32
#define		U32		unsigned int
#endif
#ifndef		U16
#define		U16		unsigned short
#endif
#ifndef		S32
#define		S32		int
#endif
#ifndef		S16
#define		S16		short int
#endif
#ifndef		U8
#define		U8		unsigned char
#endif
#ifndef		S8
#define		S8		char
#endif


////??
#ifndef		GS_ADAPTER_KEY_NAME
#define		GS_ADAPTER_KEY_NAME			L"Comm\\DM9CE1\\Parms\\TcpIp"
#endif //GS_ADAPTER_KEY_NAME

//Hardware check-loop
#ifndef		__GS_CHECK_LOOP__
#define		__GS_CHECK_LOOP__
//define maximum trial to check loop
#define		GS_MAX_LOOP_TIMES			3
//GPIO Actions code
#define		IO_CTL_CHECKLOOP			0


#endif //__GS_CHECK_LOOP__

//For LoopDriver actions
#ifndef		__GS_CTL_LOOP__
#define    __GS_CTL_LOOP__

//GPIO Actions code
//Check Loop
#define		IO_CTL_LOP_CHECKLOOP		0x00
////Read GPIO GPF6
//#define		IO_CTL_LOP_READ_GPF6		0x01
//Write GPIO GPHF6
////#define		IO_CTL_LOP_WRITE_GPF6		0x02
//Read GPIO GPG3(LOOPUP)
#define		IO_CTL_LOP_READ_LOOPUP		0x01
//Write GPIO GPG3(LOOPUP)
#define		IO_CTL_LOP_WRITE_LOOPUP		0x02
////Read GPIO GPH10
//#define		IO_CTL_LOP_READ_GPH10		0x03
////Write GPIO GPH10
//#define		IO_CTL_LOP_WRITE_GPH10		0x04
//Read GPIO GPH14
#define		IO_CTL_LOP_READ_LOOPDN		0x03
//Write GPIO GPH14
#define		IO_CTL_LOP_WRITE_LOOPDN		0x04

#endif   //__GS_CTL_LOOP__
//for MotorDriver
#ifndef  _GS_CTL_MOTOR_
#define	 _GS_CTL_MOTOR_
#define IO_CTL_MOTOR_ENABLE		1
#define IO_CTL_MOTOR_DRIVER		2
#define IO_CTL_MOTOR_RESET3		4
#define IO_CTL_MOTOR_RESET4		5
#define IO_CTL_MOTOR_STOP		6
#define IO_CTL_MOTOR_CW			7
#define IO_CTL_MOTOR_CCW		8

#define	IO_CTL_GET_STATUS		10
#define IO_CTL_GET_POS			11
#define IO_CTL_GET_UPOS			12
#define IO_CTL_GET_VPOS			13
#define IO_CTL_GET_TARGET		14
#define	IO_CTL_GET_UTARG		15
#define	IO_CTL_GET_VTARG		16

#define IO_CTL_SET_POS			17
#define IO_CTL_SET_UPOS			18
#define IO_CTL_SET_VPOS			19
#define IO_CTL_SET_TARG			20
#define IO_CTL_SET_UTARG		21
#define IO_CTL_SET_VTARG		22

#define		ControlBitM1	0x01
#define		ControlBitM2	0x02
#define		ControlBitM3	0x04
#define		ControlBitM4	0x08
#define		ControlBitAll	0x0F
#define		MicroStep		8
//for p50\p07\p08
#define		GS_MOTOR_ENABLE_P5X 		194
#define		GS_MOTORUV_STATE_P5X		195
#define		GS_MOTORU_TARG_P5X			196
#define		GS_MOTORV_TARG_P5X			198
#define		GS_MOTORU_POSITION_P5X		200
#define		GS_MOTORV_POSITION_P5X		202
//for p03h
#define		GS_MOTORU_TARG			206
#define		GS_MOTORV_TARG			208
#define		GS_MOTORU_POSITION		210
#define		GS_MOTORV_POSITION		212
#define		GS_MOTOR_ENABLE 		214
#define		GS_MOTORUV_STATE		215

#endif  //_GS_CTL_MOTOR_

//GS Message codes
#ifndef		__GS_WM_MSG__
#define		__GS_WM_MSG__
#define		WM_GSMSG_EINT18				WM_USER + 0x100
#define		WM_GSMSG_THERMO				WM_USER + 0x101
#define		WM_GSMSG_TEMPERATURE		WM_USER + 0x103
#define		WM_GSMSG_TEMPERATURE_UI		WM_USER + 0x104
#define		WM_GSMSG_TEMP_TO_UI			WM_USER + 0x105
#define		WM_GSMSG_TCP_DATA_ARRAY		WM_USER + 0x106
#define		WM_GSMSG_UPLOAD				WM_USER + 0x107
#define		WM_GSMSG_PANEL_OPER			WM_USER + 0x108
#define		WM_GSMSG_HANS_REPLY			WM_USER + 0x002
#define		WM_GSMSG_LS_CHANGE			WM_USER + 0x009
#define		WM_GSMSG_PARAM_CHANGE		WM_USER + 0x00A
#define		WM_GSMSG_SELFX				WM_USER + 0x00B
//#define		GS_IAP_MSG_PRINT_INFOR		WM_USER + 0x200
#define		GS_IAP_MSG_PRINT_INFOR		WM_USER + 0x020
#endif //__GS_WM_MSG__

//
typedef struct GStagCOPYDATASTRUCTdb
{
    double dTargetData;
    double dActualData;
    void * lpData;
} GSCOPYDATASTRUCT, *PGSCOPYDATASTRUCT;

typedef struct GScomDCBstruct
{
    unsigned int		nPort;
    unsigned int		nBaudRate;
    unsigned int		nDataBits;
    unsigned int		nParity;
    unsigned int		nStop;
    unsigned int		nFlowCtrl;
}GSCOMDCB, *PGSCOMDCB;


#ifndef		__GS_HANS_COM_
#define		__GS_HANS_COM_
//define serial port used between gs and hans
#define		GS_HANS_COM					1
//define ACSII code used in Han's protocol
#define		GS_HANS_CR					0x0D	//Enter as terminal character
#define		GS_HANS_NAK					0x15	//Negative Acknowledge when cmd is non-acknowledgable
#define		GS_HANS_QMK					0x3F	//Question Mark
#define		GS_HANS_LF					0x0A	//
//define wParam for WM_GSMSG_ASK_LSTATUS/WM_GSMSG_ANS_LSTATUS
#define		HANS_L_QUES					0x01
#define		HANS_SPO_QUES				0x02
#define		HANS_F_QUES					0x03
#define		HANS_SETP_QUES				0x04
#define     HANS_SETC_QUES				0x05
#define		HANS_XC_I_QUES			    0x06
#define		HANS_XC_LDT_QUES			0x07
#define		HANS_XC_THT_QUES			0x08
#define		HANS_SPO_SET				0x09
#define		HANS_F_SET					0x10
#define		HANS_SETP_SET				0x20
#define		HANS_RSET					0x40
#define		HANS_SETC_SET				0x80
#define		HANS_XC_I_SET				0x90
#define		HANS_XC_LDT_SET				0x91
#define		HANS_XC_THT_SET				0x92

//define HANS error bit
#define		HANSERR_FORMAT				0x01	//
#define		HANSERR_PWROVER				0x02	//
#define		HANSERR_FREQOVER			0x04	//
#define		HANSERR_CNTOVER				0x08	//
#define		HANSERR_NOTRDY				0x10	//
#define 	HANSERR_NOTLCK				0x20
#define		HANSERR_TEMPOVER			0x40
#endif //__GS_HANS_COM_

#ifndef		__GS_REG_MSG_
#define		__GS_REG_MSG_
#define		GS_REG_MSG_P0				0x00
#define		GS_REG_MSG_P1				0x01
#define		GS_REG_MSG_P2				0x02
#define		GS_REG_MSG_P3				0x03
#define		GS_REG_MSG_C4				0x04 //Y12-P03
#define		GS_REG_MSG_C5				0x05 //Y12-P03
#define		GS_REG_MSG_P4				0x04 //Y12-P21
#define		GS_REG_MSG_BOX				0x06 //Y12-P21
#define		GS_REG_MSG_AIR				0x07 //Y12-P21
#define		GS_REG_MSG_P5				0x08 //Y12-P21
#define		GS_REG_MSG_P6				0x07
#define		GS_REG_MSG_P7				0x08
#define		GS_REG_MSG_P8				0x09
#define		GS_REG_MSG_P9				0x0A
//p5x
#define		GS_REG_MSG_P0_P5X			0x00
#define		GS_REG_MSG_P1_P5X			0x01
#define		GS_REG_MSG_P2_P5X			0x02
#define		GS_REG_MSG_P3_P5X			0x03
#define		GS_REG_MSG_P4_P5X			0x04
#define		GS_REG_MSG_P5_P5X			0x05
#define		GS_REG_MSG_P6_P5X			0x06
#define		GS_REG_MSG_P7_P5X			0x07
#define		GS_REG_MSG_P8_P5X			0x08
#define		GS_REG_MSG_P9_P5X			0x09
#define		GS_REG_MSG_P10_P5X			0x0A
//#define		GS_REG_MSG_BOX_P5X			0x0B
//#define		GS_REG_MSG_AIR_P5X			0x0C

#define		GS_REG_MSG_SCAN_ENB			0x01
#define		GS_REG_MSG_X_START			0x02
#define		GS_REG_MSG_X_END			0x03
#define		GS_REG_MSG_X_STEP			0x04
#define		GS_REG_MSG_Y_START			0x05
#define		GS_REG_MSG_Y_END			0x06
#define		GS_REG_MSG_Y_STEP			0x07
#define		GS_REG_MSG_SCAN_STAGE		0x08
#define		GS_REG_MSG_SAVE_POS			0x09
#define		GS_REG_MSG_SAVE_DATA		0x0A
#define		GS_REG_MSG_UV_RESET			0x0B
#define     GS_REG_MSG_UV_STATE			0x0C
#define		GS_REG_MSG_UV_ENABLE		0x0D
#define		GS_REG_MSG_VTARG			0x0E
#define		GS_REG_MSG_UTARG			0x0F
#define		GS_REG_MSG_UV_MOVE			0x10
#define		GS_REG_MSG_UPOS				0x11
#define		GS_REG_MSG_VPOS				0x12

#endif //__GS_REG_MSG_


#ifndef		__GS_COMMUNICATE_CMD_
#define		__GS_COMMUNICATE_CMD_
#define		GS_CMD_HEAD					0x81	//HEADER fixed part of data series
#define		GS_CMD_ONE_BYTE				0x00	//Signal is composed of 1-byte data
#define		GS_CMD_TWO_BYTE				0x04	//Signal is composed of 2-byte data
#define		GS_CMD_TRI_BYTE				0x08	//Signal is composed of 3-byte data
#define		GS_CMD_QUA_BYTE				0x0C	//Signal is composed of 4-byte data
#define		GS_CMD_PC_WRITE				0x10	//PC sends Write signal
#define		GS_CMD_PC_READ				0x00	//PC sends Read Signal
#define		GS_CMD_BLOCK_OP				0x20	//PC sneds Block Operation
#define		GS_CMD_INCR_MODE			0x80	//PC Block operation which automatically increase address
#define		GS_CMD_NACK					0xFF
#endif //__GS_COMMUNICATE_CMD_


#ifndef		__GS_INI_DEF_
#define		__GS_INI_DEF_
//cert.ini
#ifdef DBG
#define		GS_CERT_FILE			"/mnt/GSControler/ARMTarget/Debug/doc/Cert.ini"
#else
#define		GS_CERT_FILE					"./doc/Cert.ini"
#endif
#define     GS_INI_SECTION_CHLG         "Chlg"
#define     GS_INI_SECTION_CERT         "Cert"
//PowerAdjArg.ini
#ifdef DBG
#define		GS_INI_POWERADJARG			"/mnt/GSControler/TargetARM/Debug/doc/PowerAdjArg.ini"
#else
#define     GS_INI_POWERADJARG          "./doc/PowerAdjArg.ini"
#endif
//GSControler.ini
#ifdef DBG
#define		GS_INI_FILE			"/mnt/GSControler/ARMTarget/Debug/doc/GSControler.ini"
#else
#define		GS_INI_FILE					"./doc/GSControler.ini"
#endif
#define		GS_INI_SECTION_SC			"SELFCHECK"
#define		GS_INI_KEY_I				"I"
#define		GS_INI_KEY_ONTIME			"ONTIME"
#define		GS_INI_KEY_OFFTIME			"OFFTIME"
#define		GS_INI_KEY_STEP				"STEP"
#define		GS_INI_SECTION_MOT			"Location"
#define		GS_INI_KEY_X				"X"
#define		GS_INI_KEY_Y				"Y"
#define		GS_INI_SECTION_PEA			"PeakPwr"
#define		GS_INI_KEY_THD				"PDThrshd"
#define		GS_INI_KEY_BIAS				"PDBias"
#define		GS_INI_KEY_PULCNT			"Pulse"
#define		GS_INI_KEY_PULCNT0			"Pulse0"
#define		GS_INI_KEY_PULCNT1			"Pulse1"
#define		GS_INI_KEY_PULCNT2			"Pulse2"
#define		GS_INI_KEY_INTEVAL1			"Interval1"
#define		GS_INI_KEY_INTEVAL2			"Interval2"
#define		GS_INI_KEY_DIV				"Divider"
#define		GS_INI_KEY_FREQ				"Freq"
#define		GS_INI_KEY_PGAP				"Phase"
#define		GS_INI_KEY_FSWITCH			"FSWITCH"
#define		GS_INI_SECTION_PRO			"ProductID"
#define		GS_INI_KEY_ID				"ID"
#define     GS_INI_KEY_MODEL            "MODEL"
#define		GS_INI_SECTION_FREQ			"Freq"
#define		GS_INI_KEY_R_A				"ReadA"
#define		GS_INI_KEY_R_B				"ReadB"
#define		GS_INI_KEY_R_C				"ReadC"
#define		GS_INI_KEY_R_D				"ReadD"
#define		GS_INI_KEY_R_MIN			"ReadMin"
#define		GS_INI_KEY_W_A				"WriteA"
#define		GS_INI_KEY_W_B				"WriteB"
#define		GS_INI_KEY_W_C				"WriteC"
#define		GS_INI_KEY_W_D				"WriteD"
#define		GS_INI_KEY_W_MIN			"WriteMin"
#define		GS_INI_KEY_W_MAX			"WriteMax"
#define		GS_INI_SECTION_SPO          "SPO"
#define		GS_INI_KEY_P1LIMIT			"P1LIMIT"
#define		GS_INI_KEY_W_LA				"LWriteA"
#define		GS_INI_KEY_W_LB				"LWriteB"
#define		GS_INI_KEY_W_LC				"LWriteC"
#define		GS_INI_KEY_W_LD				"LWriteD"
#define		GS_INI_KEY_W_LMIN			"LWriteMin"
#define		GS_INI_KEY_W_LMAX			"LWriteMax"

#define		GS_INI_KEY_R_LA				"LReadA"
#define		GS_INI_KEY_R_LB				"LReadB"
#define		GS_INI_KEY_R_LC				"LReadC"
#define		GS_INI_KEY_R_LD				"LReadD"
#define		GS_INI_KEY_R_LMIN			"LReadMin"

#define		GS_INI_KEY_W_HA				"HWriteA"
#define		GS_INI_KEY_W_HB				"HWriteB"
#define		GS_INI_KEY_W_HC				"HWriteC"
#define		GS_INI_KEY_W_HD				"HWriteD"
#define		GS_INI_KEY_W_HMIN			"HWriteMin"
#define		GS_INI_KEY_W_HMAX			"HWriteMax"

#define		GS_INI_KEY_R_HA				"HReadA"
#define		GS_INI_KEY_R_HB				"HReadB"
#define		GS_INI_KEY_R_HC				"HReadC"
#define		GS_INI_KEY_R_HD				"HReadD"
#define		GS_INI_KEY_R_HMIN			"HReadMin"

#define		GS_INI_KEY_PMAX				"Pmax"
#define		GS_INI_KEY_ORDER			"3rdOrder"
#define		GS_INI_KEY_PULADJ			"AdjPulse"
#define		GS_INI_SECTION_FPGA			"FPGAType"
#define		GS_INI_KEY_DRIVERTYPE		"DriverType"
#define		GS_INI_KEY_MOTORTYPE		"MType"
#define		GS_INI_KEY_EOMTYPE			"EType"
#define     GS_INI_KEY_INITTIME         "InitTime"
#define     GS_INI_KEY_MODE             "Mode"
#define     GS_INI_KEY_BASEPW            "BasePW"
#define     GS_INI_KEY_DCCINI            "DCCIni"
#define     GS_INI_KEY_ENMO            "ENMO"
#define     GS_INI_KEY_ENPW            "ENPW"
#define     GS_INI_KEY_PRR_FILTER        "PRRFilter"
#define     GS_INI_KEY_FMIN            "Fmin"
#define     GS_INI_KEY_FMAX            "Fmax"
#define     GS_INI_KEY_PMIN            "Pmin"
#define     GS_INI_KEY_PMAX            "Pmax"
#define     GS_INI_KEY_ENERGY            "Energy"
#define     GS_INI_KEY_MAXPOWER          "MaxPower"
#define     GS_INI_KEY_BAISEI          "BaiseI"
#define     GS_INI_KEY_FREQLEVEL          "FLevel"
#define     GS_INI_KEY_PULSE            "Pulse"
#define     GS_INI_KEY_FREQ             "Freq"
#define		GS_INI_SECTION_REGISTER		"FPGARegister"
#define		GS_INI_KEY_ADDRESS			"Address_"
#define		GS_INI_KEY_LATCH			"LatchEnable"
#define		GS_INI_KEY_DBMODEL			"DBModel"
#define		GS_INI_KEY_ESTOP			"EStop"
#define		GS_INI_KEY_LATCHSOURCE      "LatchSource"
#define		GS_INI_KEY_LATCHWIDTH		"LatchWidth"
#define		GS_INI_KEY_EOM2PHASE		"EOM2Phase"
#define		GS_INI_KEY_EOM2WIDTH		"EOM2Width"
#define     GS_INI_SECTION_FREQ50       "Freq50"
#define     GS_INI_SECTION_FREQ100      "Freq100"
#define     GS_INI_SECTION_FREQ150      "Freq150"
#define     GS_INI_SECTION_FREQ200      "Freq200"
#define     GS_INI_SECTION_FREQ250      "Freq250"
#define     GS_INI_SECTION_FREQ300      "Freq300"
#define     GS_INI_SECTION_PMAX_PULSE6  "Pmax_Pulse6"
#define     GS_INI_SECTION_PMAX_PULSE7  "Pmax_Pulse7"
#define     GS_INI_SECTION_PMAX_PULSE8  "Pmax_Pulse8"
#define     GS_INI_SECTION_PMAX_PULSE9  "Pmax_Pulse9"
#define     GS_INI_SECTION_PMAX_PULSE10 "Pmax_Pulse10"
#define     GS_INI_SECTION_PMAX_PULSE11 "Pmax_Pulse11"
#define     GS_INI_SECTION_PMAX_PULSE12 "Pmax_Pulse12"
#define     GS_INI_SECTION_PMAX_PULSE13 "Pmax_Pulse13"
#define     GS_INI_SECTION_P2ITarget_Pulse6 "P2ITarget_Pulse6"
#define     GS_INI_SECTION_P2ITarget_Pulse7 "P2ITarget_Pulse7"
#define     GS_INI_SECTION_P2ITarget_Pulse8 "P2ITarget_Pulse6"
#define     GS_INI_SECTION_P2ITarget_Pulse9 "P2ITarget_Pulse9"
#define     GS_INI_SECTION_P2ITarget_Pulse10 "P2ITarget_Pulse10"
#define     GS_INI_SECTION_P2ITarget_Pulse11 "P2ITarget_Pulse11"
#define     GS_INI_SECTION_P2ITarget_Pulse12 "P2ITarget_Pulse12"
#define     GS_INI_SECTION_P2ITarget_Pulse13 "P2ITarget_Pulse13"

#define     GS_INI_SECTION_P3ITarget_Pulse6 "P3ITarget_Pulse6"
#define     GS_INI_SECTION_P3ITarget_Pulse7 "P3ITarget_Pulse7"
#define     GS_INI_SECTION_P3ITarget_Pulse8 "P3ITarget_Pulse6"
#define     GS_INI_SECTION_P3ITarget_Pulse9 "P3ITarget_Pulse9"
#define     GS_INI_SECTION_P3ITarget_Pulse10 "P3ITarget_Pulse10"
#define     GS_INI_SECTION_P3ITarget_Pulse11 "P3ITarget_Pulse11"
#define     GS_INI_SECTION_P3ITarget_Pulse12 "P3ITarget_Pulse12"
#define     GS_INI_SECTION_P3ITarget_Pulse13 "P3ITarget_Pulse13"
#define     GS_INI_SECTION_P0    "P0"
#define     GS_INI_SECTION_P1    "P1"
#define     GS_INI_SECTION_P2    "P2"
#define     GS_INI_SECTION_P3    "P3"
#define     GS_INI_SECTION_P4    "P4"
#define     GS_INI_SECTION_P5    "P5"
#define     GS_INI_SECTION_P6    "P6"
#define     GS_INI_SECTION_P7    "P7"
#define     GS_INI_SECTION_P8    "P8"
#define     GS_INI_SECTION_P9    "P9"
#define     GS_INI_SECTION_P10   "P10"
#define     GS_INI_SECTION_P11   "P11"
#define     GS_INI_SECTION_P12   "P12"

#define     GS_INI_SECTION_T0    "T0"
#define     GS_INI_SECTION_T1    "T1"
#define     GS_INI_SECTION_T2    "T2"
#define     GS_INI_SECTION_T3    "T3"
#define     GS_INI_SECTION_T4    "T4"

#define     GS_INI_KEY_TEMPXC    "TempXC"


//HardwareConfig.ini
#ifdef DBG
#define		GS_INI_HARD_FILE			"/mnt/GSControler/ARMTarget/Debug/doc/HardwareConfig.ini"
#else
#define		GS_INI_HARD_FILE			"./doc/HardwareConfig.ini"
#endif
#define     GS_INI_SECTION_TCP_Socket   "TCP_Socket"
#define		GS_INI_SECTION_THERM		"Thermostat"
#define		GS_INI_SECTION_DISABLE      "Disable"
#define		GS_INI_KEY_INTERVAL			"TimerInterval"
//#define		GS_INI_KEY_SAMPLE			"SamplePeriod"
#define     GS_INI_KEY_PortLocal         "PortLocal "
#define		GS_INI_KEY_HansPort         "HansPort"
#define		GS_INI_KEY_ADJ				"AdjustPeriod"
#define		GS_INI_KEY_KP				"Kp"
#define		GS_INI_KEY_KI				"Ki"
#define		GS_INI_KEY_KD				"Kd"
#define		GS_INI_KEY_ENAKP			"EnableKp"
#define		GS_INI_KEY_ENAKI			"EnableKi"
#define		GS_INI_KEY_ENAKD			"EnableKd"
#define		GS_INI_KEY_ECR				"Ecr"
#define		GS_INI_KEY_ENAU0			"EnableU0"
#define		GS_INI_SECTION_SET			"SetRange"
#define		GS_INI_KEY_PULMAX			"Pulse_Max"
#define		GS_INI_KEY_PULMIN			"Pulse_Min"
#define		GS_INI_KEY_FREQMAX			"Freq_Max"
#define		GS_INI_KEY_FREQMIN			"Freq_Min"
#define		GS_INI_KEY_PWRMED			"Power_Med"
#define		GS_INI_KEY_ROOTMIN			"Root_Min"
#define		GS_INI_KEY_ROOTMAX			"Root_Max"
#define		GS_INI_SECTION_IAP			"FPGADat"
#define		GS_INI_KEY_IAPCNT			"IAPCnt"
#define		GS_INI_SECTION_MODULE		"Module"
#define		GS_INI_KEY_REGMAP			"RegMapVersion"
#define		GS_INI_KEY_LD				"LD"
#define		GS_INI_KEY_DISABLELD		"DisableLD"
#define		GS_INI_KEY_DISABLE0         "Disable0"
#define		GS_INI_KEY_DISABLE1         "Disable1"
#define		GS_INI_KEY_DISABLE2         "Disable2"
#define		GS_INI_KEY_LEVEL			"Level"
#define		GS_INI_KEY_OUTPUT			"OutPut"
#define		GS_INI_KEY_THERM			"Therm"
#define		GS_INI_KEY_MODIF			"Modif"
#define		GS_INI_SECTION_LD			"P"
#define		GS_INI_KEY_I				"Current"
#define		GS_INI_KEY_IL				"CurrentLimit"
#define		GS_INI_KEY_T				"Temperature"
#define		GS_INI_KEY_REGADJ			"RegAdj"
#define		GS_INI_KEY_TADC				"TempADC"
#define		GS_INI_KEY_IADC				"CurrADC"
#define		GS_INI_KEY_VADC				"VotageADC"
#define		GS_INI_KEY_FAST				"FastSwitchEnble"
#define		GS_INI_KEY_REGPMPI			"RegPmpI"
#define		GS_INI_KEY_REGVSET			"RegVset"
#define		GS_INI_KEY_HIGHDRV			"Decode"
#define		GS_INI_KEY_OPENBIT			"OpenBit"
#define		GS_INI_KEY_TLBIT			"TLBit"
#define		GS_INI_SECTION_SEEDCFG      "SeedConfig"
#define		GS_INI_KEY_FREQFACTOR		"FreqFactor"
#define		GS_INI_KEY_WIDTHC			"WidthCoarse"
#define		GS_INI_KEY_WIDTHF			"WidthFine"
#define		GS_INI_KEY_WIDTH			"Width"
//#define		GS_INI_KEY_DCCINI			"DccIni"
#define		GS_INI_KEY_PHFRONT			"PhaseFront"
#define		GS_INI_KEY_PHTRAIL			"PhaseTrail"
#define		GS_INI_KEY_TEMPCTRLEN		"TempControlEnable"
#define		GS_INI_KEY_CURRCTRLEN		"CurrentControlEnable"
#define		GS_INI_KEY_EOM2ENABLE		"EOM2Enable"
#define		GS_INI_KEY_I_DELTA			"IDelta"
#define		GS_INI_KEY_T_DELTA			"TDelta"

#define     GS_INI_SECTION_ADJUST      "ADJUST"
#define		GS_INI_KEY_T_P2CA			"P2CA"
#define		GS_INI_KEY_T_P2CB			"P2CB"
#define		GS_INI_KEY_T_P2CC			"P2CC"
#define		GS_INI_KEY_T_P2CD			"P2CD"
#define		GS_INI_KEY_T_C2PA			"C2PA"
#define		GS_INI_KEY_T_C2PB			"C2PB"
#define		GS_INI_KEY_T_C2PC			"C2PC"
#define		GS_INI_KEY_T_C2PD			"C2PD"
#define     GS_INI_KEY_T_MINC			"MinC"

//XC.ini
#ifdef DBG
#define		GS_INI_X_FILE			"/mnt/GSControler/ARMTarget/Debug/doc/XC.ini"
#else
#define		GS_INI_X_FILE				"./doc/XC.ini"
#endif
#define		GS_INI_SECTION_LOG			"SystemMon"
#define		GS_INI_KEY_WDRESET			"WDResetCnt"
//Y12-P03Inspector.ini
#define		GS_INI_SCAN_FILE			"./doc/Inspector.ini"
#define		GS_INI_SCAN_SECTION			"ScanConfig"
#define		GS_INI_SCAN_KEY_ROUND		"TotalSch"
#define		GS_INI_SCAN_KEY_SCH			"Schematic"
#define		GS_INI_SCAN_KEY_XSTART		"Para1Zero"
#define		GS_INI_SCAN_KEY_XEND		"Para1Omeg"
#define		GS_INI_SCAN_KEY_XSTEP		"Para1Step"
#define		GS_INI_SCAN_KEY_YSTART		"Para2Zero"
#define		GS_INI_SCAN_KEY_YEND		"Para2Omeg"
#define		GS_INI_SCAN_KEY_YSTEP		"Para2Step"
//CalInfo.ini
#ifdef DBG
#define		GS_INI_CALI_FILE			"/mnt/GSControler/ARMTarget/Debug/doc/CalInfo.ini"
#else
#define		GS_INI_CALI_FILE			"CalInfo.ini"
#endif
#define     GS_INI_SECTION__PCB               "PCB"
#define      GS_INI_KEY_CURRXC               "CurrXC"
#define      GS_INI_KEY_TEMPXC               "TempXC"
#define     GS_INI_KEY_EBAKIWI0011       "EBAKIWI0011"
#define     GS_INI_KEY_EBAKIWI0015       "EBAKIWI0015"
#define     GS_INI_KEY_EBAKIWI0016       "EBAKIWI0016"
//#define		GS_INI_CALI_FILE			L"\\Program Files\\GSControler\\CalInfo.ini"
//#define		GS_INI_CALI_FILE_CHAR		"\\Program Files\\GSControler\\CalInfo.ini"
#define		GS_INI_KEY_ISETA			"CurSetA"
#define		GS_INI_KEY_ISETB			"CurSetB"
#define		GS_INI_KEY_IREADA			"CurReadA"
#define		GS_INI_KEY_IREADB			"CurReadB"
//#define		GS_INI_KEY_TSETA			"TemSetA"
//#define		GS_INI_KEY_TSETB			"TemSetB"
#define		GS_INI_KEY_TREADA			"TemReadA"
#define		GS_INI_KEY_TREADB			"TemReadB"
#define		GS_INI_KEY_VTHA				"VthA"
#define		GS_INI_KEY_VTHB				"VthB"
#define		GS_INI_KEY_RTHA				"RthA"
#define		GS_INI_KEY_RTHB				"RthB"
#define		GS_INI_KEY_PSETA			"PwrSetA"
#define		GS_INI_KEY_PSETB			"PwrSetB"
#define		GS_INI_KEY_PREADA			"PwrReadA"
#define		GS_INI_KEY_PREADB			"PwrReadB"
#define		GS_INI_KEY_VSETIA			"VSetIA"
#define		GS_INI_KEY_VSETIB			"VSetIB"
#define		GS_INI_KEY_VSETVA			"VSetVA"
#define		GS_INI_KEY_VSETVB			"VSetVB"
#define		GS_INI_KEY_VSETIVA			"VSetIVA"
#define		GS_INI_KEY_VSETIVB			"VSetIVB"
#define		GS_INI_KEY_CURRENTMAX		"CurrentMax"
#define		GS_INI_KEY_TIMERES			"TimeRes"
#endif //__GS_INI_DEF_

#ifndef		__GS_SCAN_AND_MOTOR_
#define		__GS_SCAN_AND_MOTOR_
//_GS_SCAN_SCHEMATIC
#define		GS_X_COMB					0x01
#define		GS_X_SNAKE					0x02
#define		GS_Y_COMB					0x10
#define		GS_Y_SNAKE					0x20
#define		GS_HOME						0x11
#define		GS_ABORT_SCAN				0x00
//_GS_MOTOR
#define		GS_X						1
#define		GS_Y						0
#endif //_GS_SCAN_AND_MOTOR


#ifndef		__GS_LD_DRIVE_
#define		__GS_LD_DRIVE_
//define power range
#define		GS_PMAX						0.8
#define		GS_PMIN						0.01
//define __GS_HOTSWAP_ERROR_
#define		SWAPERR_OK					0
#define		SWAPERR_NOTRDY				1
#define		SWAPERR_OVER				2
#endif //__GS_LD_DRIVE_



#ifndef		__GS_LOG_DEF_
#define		__GS_LOG_DEF_
#define		LOG_EVENT_ERROR				0
#define		LOG_EVENT_WARNING			1
#define		LOG_EVENT_INFO				2
#define		LOG_EVENT_SYSERR			3
#define		LOG_INFO_START				"Starting..."
#define		LOG_INFO_EXIT				"Exiting...\r\n"
#define		LOG_INFO_INTERRUPT			"FPGA-interrupt"
#define		LOG_INFO_LASING				"Lasing"
#define		LOG_INFO_LAUNCH				"Launching"
#define		LOG_INFO_SHUT				"Shutting down"
#define		LOG_INFO_LOCK				"Locked"
#define		LOG_INFO_REGCMD				"REG-cmd:"
#define		LOG_INFO_HANSCMD			"COM-cmd:"
#define		LOG_INFO_UPLOAD				"Uploading:"
#define		LOG_INFO_DOWNLOAD			"Downloading:"
#define		LOG_ERROR_UNLOCK			"Unlocked"
#define		LOG_ERROR_REGCMD			"REG-cmd error"
#define		LOG_ERROR_HANSCMD			"COM-cmd over-flow:"
#define		LOG_ERROR_LOOP				"Failed to check loop\r\n"
#define		LOG_ERROR_COM				"COM doesn't exist.ErrCode"
#define		LOG_ERROR_MOHANDLE			"Invalid MotorEvent handle.ErrCode"
#define		LOG_ERROR_UPLOAD			"Failed to upload.ErrCode"
#endif //__GS_LOG_DEF_

#ifndef		__GS_FILE_NAME_
#define		__GS_FILE_NAME_
#define		GS_TXT_SCAN_FILE			"ScanResultData.txt"
#define		GS_TXT_LOG1_FILE			"GSControllerLogs1.txt"
#define		GS_TXT_LOG2_FILE			"GSControllerLogs2.txt"
//#define		GS_TXT_SCAN_FILE			L"\\Program Files\\GSControler\\ScanResultData.txt"
//#define		GS_TXT_SCAN_FILE_CHAR		"\\Program Files\\GSControler\\ScanResultData.txt"
//#define		GS_TXT_LOG1_FILE			L"\\Storage Card\\GS Files\\GSControllerLogs1.txt"
//#define		GS_TXT_LOG1_FILE_CHAR		"\\Storage Card\\GS Files\\GSControllerLogs1.txt"
//#define		GS_TXT_LOG2_FILE			L"\\Storage Card\\GS Files\\GSControllerLogs2.txt"
//#define		GS_TXT_LOG2_FILE_CHAR		"\\Storage Card\\GS Files\\GSControllerLogs2.txt"
#endif //__GS_FILE_

//FPGA register bitwise signal
#ifndef		__GS_FPGA_BIT_
#define		__GS_FPGA_BIT_
//define MotorState/MotorMov Bit
#define		MOTOR_STOP					0x04
#define		MOTOR_RST					0x05
#define		MOTOR_MOVE					0x06
#define		MOTOR_SCAN					0x07
#define		MOTOR_Y						0x08
#define		MOTOR_DONE					0x10
#define		MOTOR_DATA					0x20
#define		MOTOR_DATA_FULL				0x40
#define		MOTOR_ERROR_RST				0x80
//define LDEnb Bit
#define		VOLT_TEC					0x20
#define		VOLT_FAN					0x40
#define		VOLT_TEC_P07				0x40
#define		VOLT_TEC_P08				0x20
#define		VOLT_FAN_P07				0x80
#define		VOLT_FAN_P08				0x40
#define		VOLT_P03					0x60
#define		VOLT_P07					0xC0
//define IntEnb Bit
#define		INTRPT_PLOSS				0x0001	//interrupt with regards to PLOSS
#define		INTRPT_STOP					0x0002	//interrupt concerning ESTOP
#define		INTRPT_MLCK					0x0004	//interrupt resulted from PD_LCK
#define		INTRPT_KNOB					0x0008
#define		INTRPT_TEC					0x0010	//TEC voltage interrup
#define		INTRPT_PMP3					0x0020	//PMP3 power interrupt
#define		INTRPT_MOTOR				0x0080	//interrupt resulted from Motor
#define		INTRPT_DATA					0x0100
#define		INTRPT_MO					0x0200	//interrupt concerning DB25sw_MO
#define		INTRPT_AP					0x0400	//interrupt concerning DB25sw_AP
//define FPGAMode Bit
#define		MOTOR_JAPAN					0x10
#define		DRIVER_DIY					0x20
#define		EOM_DIY						0x08
#define		EOM_120M					0x04
//define healthy_status
#define		STATUS_P3NG					0x0020
#define		STATUS_P3IL					0x0040
#define		STATUS_PLOSS				0x0080
#define		STATUS_LOCK					0x8000
//define DB25sw
#define		DB25SW_MO					0x01
#define		DB25SW_AP					0x02
#define		DB25SW_LATCH				0x04
#define		DB25SW_FREQ					0x08
#define		DB25SW_STOP					0x10
#define		DB25SW_REDL					0x20
//define DAC_Status
#define		DAC_BUSY					0x08
#endif      //__GS_FPGA_BIT_

#ifndef		_GS_LASER_STATUS_
#define		_GS_LASER_STATUS
#define		GS_LS_IDLE					0x00
#define		GS_LS_LAUNCH				0x01
#define		GS_LS_LASING				0x02
#define		GS_LS_SHUT					0x03
#define		GS_LS_SCAN					0x04
#define		GS_LS_ERROR					0x05
#define		GS_LS_INIT					0x06
#define		GS_LS_TOINIT				0x07
#define		GS_LS_PULSECHANGED			0x08
#define		GS_LS_FREQCHANGED			0x09
#define		GS_LS_LAUNCHING				0x0A
#define		GS_LS_TOLAUNCHPUMP			0x0B
#define		GS_LS_LAUNCHPUMP			0x0C
#define		GS_LS_SHUTPROGRESS1			0x0D
#define		GS_LS_SHUTPROGRESS2			0x0E
#define		GS_LS_TOSHUTPROGRESS2		0x10
#define		GS_LS_CHANGESC				0x11
#define		GS_LS_FPGAPROGRESS			0x12
#define		GS_LS_TOERASEFPGA			0x13
#define		GS_LS_TOPROGRAMFPGA			0x14
#define		GS_LS_TOVERFY				0x15
#define		GS_LS_TOINITIAL				0x16
#define		GS_LS_INITIAL				0x17
#define		GS_LS_HIDEPROCESS			0x18
#endif //_GS_LASER_STATUS

#ifndef		__GS_LASER_STAGE_
#define		__GS_LASER_STAGE_
#define		LASER_IDLE					0
#define     LASER_LD0                   1
#define     LASER_LD0H                  4
#define		LASER_INIT					2
#define		LASER_LASE					6
#define		LASER_SCAN					15
#define     LASER_MO_ON                 16
#define     LASER_MO_OFF                17
#define     LASER_AP_ON                 18
#define     LASER_AP_OFF                19
#define     OLD0                        20
#define     CLD0                        21
#define     OLD1                        22
#define     CLD1                        23
#define     OLD2                        24
#define     CLD2                        25
#define     OLD3                        26
#define     CLD3                        27
#define     OLD4                        28
#define     CLD4                        29
#define     READY                       30
#define     RLD3                        31
#define     WPF                         32
#define     InitTemp                    33
#define     READYOFF                    34
#define     LASERERROR                  35
#define     LASERRESET                  36
#define     StatusCheck                 37
#define     Work_P03                    38
#define     Work_N06                    39
#define     SlowOff                     40
#define     FastOff                     41
#define     CheckP0                     42
#define     OffP0                       43
#define     Work_P08H                   44
#endif //__GS_LASER_STATUS_

//FPGA register address
#ifndef		__GS_FPGA_REG_ADD_
#define		__GS_FPGA_REG_ADD_
#define		GS_FPGA_VER					1
#define		GS_FPGA_REV					2
#define     GS_SEED_DIV					4
#define		GS_WIDTH_COARSE				7
#define     GS_WIDTH_FINE				8
#define		GS_LD_ENABLE				10
#define     GS_LD_FASTSWITCH			11
#define		GS_FPGA_TIMERES				14
#define		GS_HARDWARE_STATUS			32
#define		GS_FANP_ADJ					34
#define		GS_FANP_STS					35
#define		GS_BUFIN					36
#define		GS_BUFOUT					38
#define		GS_INT_REG					40
#define		GS_INT_ENB					42
#define		GS_PD_CNTR					10  //48->10
#define		GS_PD_DIVIDER				11  //50->11
#define		GS_PD_PHASE                 12
#define		GS_PD_PWIDTH				53
#define		GS_PD_SYNCDIV				54
#define		GS_PD_SYNCPHA				56
#define		GS_PD_SYNCWID				58
#define		GS_SMAOUT_CFG				59
#define		GS_FS_PANEL					60
#define     GS_DCCINI					61
#define		GS_MOTOR_MOV				61
#define		GS_EXT_MODE					62
#define		GS_DB25_DIN					63
#define		GS_DB25_SW					64
#define		GS_DB25_ALARM				65
#define		GS_MOTOR_X_TARG				66
#define		GS_MOTOR_Y_TARG				68
#define		GS_MOTOR_X_POS				70
#define		GS_MOTOR_Y_POS				72
#define		GS_EOM2_PHASE				70
#define		GS_EOM2_WIDTH				72
#define		GS_MODE_LOCK_POS_X			74
#define		GS_MODE_LOCK_POS_Y			76
#define		GS_MOROT_STATE				78
#define		GS_DAC_REG_BASE				79
#define		GS_P0_CURTAG				80
#define		GS_P1_CURTAG				82
#define		GS_P2_CURTAG				84
#define		GS_P3_CURTAG				86
#define		GS_P4_CURTAG				88
#define		GS_P6_CURTAG				90	//Y12-P21
#define		GS_PD_BIAS					90	//Y12-P03
#define		GS_P8_CURTAG				92	//Y12-P21
#define		GS_REDLSET					92	//Y12-P03
#define		GS_DACLOOP					94
//#define		GS_CCCSTS					110
#define		GS_MAXFREQ					110
#define		GS_FREQ						44

#define		GS_PULSE_INTERVAL1			54
#define		GS_PULSE_CNT1				55
#define		GS_PULSE_INTERVAL2			56
#define		GS_PULSE_CNT2				57

#define		GS_HARD_RESET				126
#define		GS_FPGA_MODE				127
#endif		//__GS_FPGA_REG_ADD_

//other register address other than thosove maintained by FPGA
#ifndef		__GS_MCU_REG_ADD_
#define		__GS_MCU_REG_ADD_
#define		GS_VER						129
#define		GS_BOX_TEMPTAG				130
#define		GS_P0_TEMPTAG				132
#define		GS_P1_TEMPTAG				134
#define		GS_P2_TEMPTAG				136
#define		GS_P3_TEMPTAG				138
#define		GS_P4_TEMPTAG				140	//Y12-P21
#define		GS_P5_TEMPTAG				142	//Y12-P21
#define		GS_P6_TEMPTAG				144	//Y12-P21
#define		GS_P7_TEMPTAG				146	//Y12-P21
#define		GS_P8_TEMPTAG				148	//Y12-P21
#define		GS_P9_TEMPTAG				150	//Y12-P21

#define		GS_C4_TEMPTAG				140
#define		GS_P0_CUR					142
#define		GS_P0_TEMP					144
#define		GS_PD0_PW					146
#define		GS_TEC_0_A					148
#define		GS_LD0_UP					150
#define		GS_T4_TEMP					152
#define		GS_TEC_4_A					154
#define		GS_L24V_A					156
#define		GS_P1_CUR					158
#define		GS_P1_TEMP					160
#define		GS_PD1_PW					162
#define		GS_TEC_1_A					164
#define		GS_LD1_UP					166
#define		GS_T5_TEMP					168
#define		GS_PD_BIAS_BACK				170
#define		GS_D12V_A					172
#define		GS_P2_CUR					174
#define		GS_P2_TEMP					176
#define		GS_PD2_PW					178
#define		GS_TEC_2_A					180
#define		GS_LD2_UP					182
#define		GS_T24V_A					184
#define		GS_D5V0_A					186
#define		GS_P3_CUR					188
#define		GS_P3_TEMP					190
#define		GS_PD3_PW					192
#define		GS_TEC_3_A					194
#define		GS_LD3_UP_A					196
#define		GS_LD3_UN					198
#define		GS_T5V0_A					200
#define		GS_D3V3_A					202
#define		GS_PD4_PW					204
#define		GS_PD5_PW					206
#define		GS_ANA_IN					208
#define		GS_VREF						210
#define		GS_ENB						216
#define		GS_ENBSAMPLE				217
#define		GS_SAVE_POS					218
#define		GS_SAVE_INI					219
#define		GS_SCAN_ENB					220
#define		GS_SCAN_STAGE				221
#define		GS_X_START					222
#define		GS_X_END					224
#define		GS_Y_START					226
#define		GS_Y_END					228
#define		GS_X_STEP					230
#define		GS_Y_STEP					232
#define		GS_PID_SELECT				236
#define		GS_PID_DATA					237
#define		GS_GET_CALI					240
#define		GS_CALINFO_0				242
#define		GS_CALINFO_1				243
#define		GS_CALINFO_2				244
#define		GS_CALINFO_3				245
#define		GS_CALI_VOLTAGE				246
#define		GS_F_SWITCH					252
#define		GS_AUTORUN					253
#define		GS_UPDATE_DOWNLOAD			254
#define		GS_L_STATUS					255
//for P07
#define		GS_ADC_SELECT			    	132
#define		GS_ADC_RESULT				    134
#define		GS_P0_TEMPTAG_P5X				136
#define		GS_P1_TEMPTAG_P5X				138
#define		GS_P2_TEMPTAG_P5X				140
#define		GS_P3_TEMPTAG_P5X				142
#define		GS_P4_TEMPTAG_P5X				144
#define		GS_BOX_TEMPTAG_P5X				146
#define		GS_P5_TEMPTAG_P5X				148
#define		GS_P6_TEMPTAG_P5X				150
#define		GS_P7_TEMPTAG_P5X				152
#define		GS_P8_TEMPTAG_P5X				154
#define		GS_P9_TEMPTAG_P5X				156

#define		PMP0_TEMP_P5X					0
#define		PMP1_TEMP_P5X					1
#define		PMP3_TEMP_P5X					1
#define		PMP4_TEMP_P5X					1
#define		PMP5_TEMP_P5X					2
#define     PMP7_TEMP_P5X					2

#define		PMP0_TEMP_P08					0
#define		PMP1_TEMP_P08					1
#define		PMP2_TEMP_P08					2
#define		PMP3_TEMP_P08					3
#define		PMP4_TEMP_P08					4
#define		PMP5_TEMP_P08					5
#define		BOX_TEMP_P08					6
#define		AIR_TEMP_P08					7
#endif	//	__GS_MCU_REG_ADD_

//ADC MUX
#ifndef		__GS_ADC_MUX_REG_ADD_
#define		__GS_ADC_MUX_REG_ADD_
#define		PMP0_CURRENT				0
#define		PMP0_TEMP					1
#define		PMP0_PD_POWER				2
#define		TEC0A_VOLTAGE				3
#define		LD0A_VOLTAGE				4
#define		SEEDBOX_AIR_TEMP			5
#define		L24VA_VOLTAGE				7
#define		PMP1_CURRENT				8
#define		PMP1_TEMP					9
#define		PMP1_PD_POWER				10
#define		TEC1A_VOLTAGE				11
#define		LD1A_VOLTAGE				12
#define		SEEDBOX_CHASSIS_TEMP		13
#define		PDBIAS_VOLTAGE				14
#define		D12VA_VOLTAGE				15
#define		PMP2_CURRENT				16
#define		PMP2_TEMP					17
#define		PMP2_PD_POWER				18
#define		TEC2A_VOLTAGE				19
#define		LD2A_VOLTAGE				20
#define		AMBIENT_TEMP				21
#define		T24VA_VOLTAGE				22
#define		D5V0A_VOLTAGE				23
#define		PMP3_CURRENT				24
#define		PMP3_TEMP					25
#define		PMP3_PD_POWER				26
#define		TEC3A_VOLTAGE				27
#define		LD3A_VOLTAGE				28
#define		LD3B_VOLTAGE				29
#define		T5V0A_VOLTAGE				30
#define		D3V3A_VOLTAGE				31
#define		PMP4_CURRENT				32 //Y12-P21
#define		PMP4_TEMP					33 //Y12-P21
#define		PMP5_TEMP					38 //Y12-P21
#define		PMP4_PD_POWER				39
#define		PMP5_PD_POWER				47
#endif //__GS_ADC_MUX_REG_ADD_

#ifdef Y12_P2x

//FPGA register address
#define		GS_PD_THRES					94
//end of __GS_FPGA_REG_ADD_
//ADC MUX
#define		PD0_POWER					2
#define		PD1_POWER					10
#define		PD2_POWER					18
#define		PD3_POWER					26
#define		PMP4_CURRENT				32
#define		PMP4_TEMP					33
#define		PD4_POWER					34
#define		TEC4A_VOLTAGE				35
#define		LD4A_VOLTAGE				36
#define		PMP5_TEMP					38
#define		TEC5A_VOLTAGE				39
#define		PMP6_CURRENT				40
#define		PMP6_TEMP					41
#define		PD5_POWER					42
#define		TEC6A_VOLTAGE				43
#define		PMP7_TEMP					46
#define		TEC7A_VOLLTAGE				47
#define		PMP8_CURRENT				48
#define		PMP8_TEMP					49
#define		ANALOG_INPUT_VOLTAGE		50
#define		TEC8A_VOLTAGE				51
#define		LD8A_VOLTAGE				52
#define		PMP9_TEMP					54
#define		TEC9A_VOLTAGE				55
#define		VREF_VOLTAGE				56
//end of __GS_ADC_MUX_REG_ADD_
//FPGA mov bit
#define		MOTOR_READ					0x08
//end of FPGA move bit


#else // ifdef Y12_P2x
//ADC MUX
#define		TEC4A_VOLTAGE				6
//end of ADC MUX
//FPGA register address
#define		GS_PD_THRES					88
#define		GS_PD_THRES_P08				94
//end of __GS_FPGA_REG_ADD_
//FPGA mov bit
#define		MOTOR_READ					0x20
//end of FPGA move bit
//ADC MUX
#define		ANALOG_INPUT_VOLTAGE		55
#define		VREF_VOLTAGE				63
//end of __GS_ADC_MUX_REG_ADD_

#endif //Y12_P2x


union CaliInfo
{
    float	fValue;
    unsigned char	byValueArray[4];
};



#ifdef __cplusplus
}
#endif


#endif // GSDEFINE

