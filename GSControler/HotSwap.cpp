/*PDdiv = PDCnt*2/Freq
Example:
PDCnt=30887
Freq50:
PDdiv = 30887*2/50=1235=0x04D3
*/
#include "HotSwap.h"
#include "GsMath.h"
#include <stdlib.h>
#include "RegTable.h"
#include "CanDateAnalysis.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "board.h"
#include "SoftSwitch.h"

#define DefinitionFreq -1
extern Board * boards;
extern int SystemOutPut;
extern CSoftSwitch * cs;
extern int WorkStatus;
extern int MachineMode;
//constructor
//here need modifiy use board mode
CHotSwap::CHotSwap(LDKeeper *_lpLD, int _LDNum, multimap<int, LDKeeper *> *_LDMap)
{
/*
    lpLD            = _lpLD;
    LDNum           = _LDNum;
    SeedBox         = &lpLD[1];
    m_unFreqOption  = NULL;
	m_unPulseOption = NULL;
    m_dWrite        = NULL ;
    m_dRead         = NULL ;
    m_pLastPumpA    = NULL;
    m_pLastPumpB    = NULL;
    SwapAB          = 0;
    m_pLastPump     = &lpLD[LDNum-1];
    m_nLastPumpNo   = _LDNum;
    LastFreqIndex   = DefinitionFreq;
//	m_pLastButPump = NULL;
    LDMap           = _LDMap;

    Pwidth1         = 0;
    Pgap1           = 0;
    Pwidth2         = 0;
    Pgap2           = 0;
    Pwidth3         = 0;
    PresetPower = 0.000;

    multimap<int, LDKeeper *>::iterator  LDMapiter;

    multimap<int, LDKeeper *>::reverse_iterator  rLDMapiter = LDMap->rbegin();
    int OutPut = LDMap->count(rLDMapiter->first);
    if(OutPut == 1){
        m_pLastPumpA = rLDMapiter->second;
   //   cout<<m_pLastPumpA<<endl;
    }else if(OutPut == 2){
        m_pLastPumpB = rLDMapiter->second;
        rLDMapiter++;
        m_pLastPumpA = rLDMapiter->second;
    }
    m_pLastPump = m_pLastPumpA;
    m_nToModifyPumpNo = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_MODIF,-1,GS_INI_HARD_FILE);
    m_PDCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PDCNT,-1,GS_INI_FILE);
    InitInstance();
    */
}

CHotSwap::CHotSwap()
{

    m_unFreqOption  = NULL;
    m_unPulseOption = NULL;
    m_dWrite        = NULL ;
    m_dRead         = NULL ;
    m_pLastPumpA    = NULL;
    m_pLastPumpB    = NULL;
    SwapAB          = 0;
    m_nLastPumpNo   = 0;    //meybe
    LastFreqIndex   = DefinitionFreq;
//	m_pLastButPump = NULL;

    Pwidth1         = 0;
    Pgap1           = 0;
    Pwidth2         = 0;
    Pgap2           = 0;
    Pwidth3         = 0;
    PresetPower = 0.000;

    int level;
    int output;
    level = boards[Board::BoardNum-1].LD[0].GetLevel();
    output = boards[Board::BoardNum-1].LD[0].GetOutput();
    switch(SystemOutPut)
    {
    case 1: m_pLastPumpA = &boards[Board::BoardNum-1].LD[0];break;
    case 2: m_pLastPumpB = &boards[Board::BoardNum-1].LD[0];
        for(int i=Board::BoardNum-1;i>=0;i--){
            if(boards[i].LD[0].GetLevel() == level){
                m_pLastPumpA = &boards[i].LD[0];
                break;
            }
        }
        break;
    }
    m_pLastPump = m_pLastPumpA;
    m_nToModifyPumpNo = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_MODIF,-1,GS_INI_HARD_FILE);
    m_PDCnt = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PDCNT,-1,GS_INI_FILE);
    InitInstance();
    InitSPOArg();

}

//destructor
CHotSwap::~CHotSwap(void)
{
    if(m_unFreqOption != NULL) delete[] m_unFreqOption;
	if(m_unPulseOption != NULL) delete[] m_unPulseOption;
	if(m_dWrite != NULL) delete[] m_dWrite;
	if(m_dRead != NULL) delete[] m_dRead;
    if(m_dLWrite != NULL) delete[] m_dLWrite;
    if(m_dLRead != NULL) delete[] m_dLRead;
    if(m_dHWrite != NULL) delete[] m_dHWrite;
    if(m_dHRead != NULL) delete[] m_dHRead;
	//if(m_pLastPump != NULL) delete m_pLastPump;
	//if(m_pLastButPump != NULL) delete m_pLastButPump;

}

BOOL CHotSwap::InitInstance()
{
    //搜索ini文件里频率的选择 m_unFreqOption存放频率组
	if(SearchFreqOptions())
	{	
        //当前功率的最大值
        //当前功率三项拟合参数 分别存放在对应的数组里。
		InitOptions();
	}
	//else
		//TRACE(_T("None freq option found!\r\n"));
	m_dPower = 0;
    return true;
}

//if some freq option found,To be stored in the m_unFreqOption array.
//m_unFreqOption[m_nFreqOptionCount]
BOOL CHotSwap::SearchFreqOptions()
{
    BOOL bResult = false;
	//configure frequency and pulse range
    unsigned int unFreqMax, unFreqMin;
    //The maximum value and the minimum value of the frequency of queries
    unFreqMax =  GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_FREQMAX,999,GS_INI_HARD_FILE);
    unFreqMin =  GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_FREQMIN,1,GS_INI_HARD_FILE);
    m_unFreqMax = unFreqMax;
    m_unFreqMin = unFreqMin;
    //Query the pulse of the maximum and minimum values
    m_unPulseMax =  GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMAX,12,GS_INI_HARD_FILE);
    m_unPulseMin =  GetPrivateProfileInt(GS_INI_SECTION_SET,GS_INI_KEY_PULMIN,8,GS_INI_HARD_FILE);
  //  cout<<unFreqMax<<" "<<unFreqMin<<" "<<m_unPulseMax<<" "<<m_unPulseMin<<endl;
    //search freq opations
	int nFreq = unFreqMin;
	int nFreqOptionCnt = 0;
	int *nFreqOption = new int[unFreqMax - unFreqMin + 1];
	//confirm section name
	char  lpwSecName[10];
	//Search all Frequency alternative based on section name with pattern of "Freq**" saved in INI file 
	do{
		sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,nFreq);
        if(IsPrivateProfileSectionName(lpwSecName/*strFreq*/,GS_INI_FILE))
		{
			nFreqOption[nFreqOptionCnt] = nFreq;
			nFreqOptionCnt ++;
		}
		nFreq ++;
	}while(nFreq <= unFreqMax);
	//if some freq option found
	if(nFreqOptionCnt > 0)
	{
		m_nFreqOptionCount = nFreqOptionCnt;
		m_unFreqOption = new UINT[nFreqOptionCnt];
		for(int i = 0; i< nFreqOptionCnt; i++)
		{
			m_unFreqOption[i] = nFreqOption[i];
        //	cout<<nFreqOption[i]<<endl;
		}
		bResult = TRUE;
	}
	delete[] nFreqOption;
	return bResult;
}

//寻在在当前频率下，不同脉冲对应的最大功率值
BOOL CHotSwap::SearchPulseOption(LPCSTR lpwFreqSection)
{
	BOOL bResult = FALSE;
    int nPulse = m_unPulseMin;
	int nPulseOptionCnt = 0;
    UINT *nPulseOption = new UINT[m_unPulseMax - m_unPulseMin + 1];
	//confirm section name
	char  lpwPmaxPulse[15];
    sprintf(lpwPmaxPulse,("%s_%s"),GS_INI_KEY_PMAX,GS_INI_KEY_PULCNT);
    char lpwKeyName[15];
//	if(m_unPulseOption != NULL) delete[] m_unPulseOption;
	//Search all PulseWidth alternative based on section name with pattern of "Pulse**" saved in INI file 
	int i = 0;
	do{
		sprintf(lpwKeyName,("%s%d"),lpwPmaxPulse,nPulse);
        if(IsPrivateProfileKeyName(lpwKeyName/*strPulse*/,lpwFreqSection,GS_INI_FILE))
		{
			nPulseOption[i++] = nPulse;
			nPulseOptionCnt ++;
		}
		nPulse ++;
	}while(nPulse <= m_unPulseMax);
	//if some pulse option found
	m_nPulseOptionCount = nPulseOptionCnt;
	if(nPulseOptionCnt > 0)
	{
		m_unPulseOption = new UINT[nPulseOptionCnt];
		for(int i = 0; i< nPulseOptionCnt; i++)
		{
			m_unPulseOption[i] = nPulseOption[i];
		}
		bResult = TRUE;
	}
	delete []nPulseOption;
	return bResult;
}

//
void CHotSwap::InitOptions()
{
    //The frequency of default in inifile
    m_unCurrentFreq = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_FREQ,50,GS_INI_FILE);
    m_dRootMin = GetPrivateProfileDouble(GS_INI_SECTION_SET,GS_INI_KEY_ROOTMIN,0.1,GS_INI_HARD_FILE);
    m_dRootMax = GetPrivateProfileDouble(GS_INI_SECTION_SET,GS_INI_KEY_ROOTMAX,11,GS_INI_HARD_FILE);
  //  cout<<"m_unCurrentFreq:"<<m_unCurrentFreq<<" "<<" m_dRootMin:"<<m_dRootMin<<" m_dRootMax:"<<m_dRootMax<<endl;
	char lpwSecName[10]; 
	sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,m_unCurrentFreq);
    //config "write" and "read" based on current freq
	ConfigWRarray(lpwSecName);
    /*
    m_unDivider = GetPrivateProfileInt(lpwSecName,GS_INI_KEY_DIV,GetDiVer(m_unCurrentFreq),GS_INI_FILE);
    Set_PD_Dividerd(GS_PD_DIVIDER,m_unDivider); //初始化时不需要设置分频，只有锁莫才设置。
    */
    //The pulse of default in inifile
  //  m_unCurrentPulse = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,m_unPulseMin,GS_INI_FILE);
    //WByteFPGA(GS_PD_PWIDTH,m_unCurrentPulse);
    m_unCurrentPulse = DefaultPlus();
    SetPWidth(m_unCurrentPulse);


    if(SearchPulseOption(lpwSecName/*strFreq*/))
	{
		char lpwKeyName[15];
		sprintf(lpwKeyName,("%s_%s%d"),GS_INI_KEY_PMAX,GS_INI_KEY_PULCNT,m_unCurrentPulse);

        m_dPmax = GetPrivateProfileDouble(lpwSecName/*strFreq*/,lpwKeyName/*keyName*/,0.8,GS_INI_FILE);

    }
	else// to be compatible with old ini file
	{

        m_bPulCntAdj = GetPrivateProfileInt(lpwSecName/*strFreq*/,GS_INI_KEY_PULADJ,0,GS_INI_FILE);
        m_dPmax = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_PMAX,0.8,GS_INI_FILE);
	}
}

//got command to swap frequency,
//check whether it is proper timing and valid option, and then take action
BYTE CHotSwap::HandleFreqSwap(UINT freqValue, BYTE Lstatus)
{
	BYTE byResult = SWAPERR_OK;
    if((Lstatus == WorkStatus) || (Lstatus == LASER_IDLE))
	{
        if(IsMeetFreqRange(freqValue))
		{
			if(Lstatus == LASER_IDLE) AlterFrequency(freqValue, FALSE);
			else AlterFrequency(freqValue, TRUE);		
		}
		else byResult = SWAPERR_OVER;
	}
	else byResult = SWAPERR_NOTRDY;
	return byResult;
}

//
// match frequency input to candidates
BOOL/*BYTE*/ CHotSwap::MatchFreqOption(UINT freqValue)
{
	BOOL bResult = false;
	for(int i=0; i<m_nFreqOptionCount; i++)
	{
		if(m_unFreqOption[i] == freqValue) 
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}

//
// alter working frequency
void CHotSwap::AlterFrequency(unsigned int freqValue, BOOL lasing)
{
	char lpwSecName[10];

    int DividerTemp=0;
	if(freqValue!=m_unCurrentFreq)
	{
        m_unCurrentFreq = freqValue;
        LastFreqIndex = SeekLastFreqIndex(freqValue);
        if(LastFreqIndex == DefinitionFreq){
            sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,freqValue);
            m_unDivider = GetPrivateProfileInt(lpwSecName,GS_INI_KEY_DIV,GetDiVer(m_unCurrentFreq),GS_INI_FILE);
        }
        else{
            sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,m_unFreqOption[LastFreqIndex]);
            DividerTemp = GetPrivateProfileInt(lpwSecName,GS_INI_KEY_DIV,GetDiVer(m_unFreqOption[LastFreqIndex]),GS_INI_FILE);
            sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,m_unFreqOption[LastFreqIndex+1]);
            m_unDivider = GetPrivateProfileInt(lpwSecName,GS_INI_KEY_DIV,GetDiVer(m_unFreqOption[LastFreqIndex+1]),GS_INI_FILE);;
      /*      if(DividerTemp > m_unDivider)
            {
                int t;
                t = DividerTemp;
                DividerTemp = m_unDivider;
                m_unDivider = t;
            }
           m_unDivider = ((float)(m_unCurrentFreq-m_unFreqOption[LastFreqIndex])/
                        (m_unFreqOption[LastFreqIndex+1]-m_unFreqOption[LastFreqIndex]))
                        *(DividerTemp-m_unDivider)+m_unDivider;*/

        }
        m_unDivider = GetDiVer(m_unCurrentFreq);
		ConfigWRarray(lpwSecName);
        /**************************************/
        //是否修改脉冲数
        if(!SearchPulseOption(lpwSecName/*strFreq*/))
            m_bPulCntAdj = GetPrivateProfileInt(lpwSecName/*strFreq*/,GS_INI_KEY_PULADJ,0,GS_INI_FILE);
		if(!MatchPulseOption(m_unCurrentPulse))
		{
            int nPulse = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,m_unPulseMin,GS_INI_FILE);
			m_unCurrentPulse = nPulse;
            SetPWidth(m_unCurrentPulse);
        //	WByteFPGA(GS_PD_PWIDTH,m_unCurrentPulse);
		}
        /************************************/
        ConfigPmaxPxI(lpwSecName,m_unCurrentPulse,m_nPulseOptionCount,lasing,LastFreqIndex);
        if(lasing)  //出光状态下修改功率
		{
			double dPowerValue = GS_SWITCH_DEFAULT_POWER;
			dPowerValue = (dPowerValue <= m_dPmax)? dPowerValue : m_dPmax;
            //SetOutputPower(dPowerValue,m_dPower);
            SetOutputPowerUpper(dPowerValue);
			m_dPower = dPowerValue;       
		}
        //设置频率
		SetFreq(m_unDivider);    
	}
}

void CHotSwap::ConfigWRarray(LPCSTR lpwSecName)
{
    m_bIs3rdOrder = GetPrivateProfileInt(lpwSecName/*strFreq*/,GS_INI_KEY_ORDER,0,GS_INI_FILE);
	if(m_dWrite != NULL) delete[] m_dWrite;
	if(m_dRead != NULL) delete[] m_dRead;
		
	if(m_bIs3rdOrder)
	{
		m_dWrite = new double[4];
		m_dRead = new double[4];
        m_dWrite[3] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_A,1,GS_INI_FILE);
        m_dWrite[2] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_B,1,GS_INI_FILE);
        m_dWrite[1] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_C,1,GS_INI_FILE);
        m_dWrite[0] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_D,1,GS_INI_FILE);
        m_dWriteMin = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_MIN,1,GS_INI_FILE);
        m_dRead[3] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_A,1,GS_INI_FILE);
        m_dRead[2] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_B,1,GS_INI_FILE);
        m_dRead[1] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_C,1,GS_INI_FILE);
        m_dRead[0] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_D,1,GS_INI_FILE);
        m_dReadMin = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_MIN,1,GS_INI_FILE);
	}
	else
	{
		m_dWrite = new double[3];
		m_dRead = new double[3];
        m_dWrite[2] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_A,1,GS_INI_FILE);
        m_dWrite[1] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_B,1,GS_INI_FILE);
        m_dWrite[0] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_C,1,GS_INI_FILE);
        m_dWriteMin = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_W_MIN,1,GS_INI_FILE);
        m_dRead[2] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_A,1,GS_INI_FILE);
        m_dRead[1] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_B,1,GS_INI_FILE);
        m_dRead[0] = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_C,1,GS_INI_FILE);
        m_dReadMin = GetPrivateProfileDouble(lpwSecName/*strFreq*/,GS_INI_KEY_R_MIN,1,GS_INI_FILE);
	}
}

void CHotSwap::ConfigPmaxPxI(LPCSTR lpwSecName, UINT pulseCnt, BOOL bPulseAdj,BOOL lasing,int index)
{
	char lpwKeyPmaxName[20];
	char lpwPulse[10];
	sprintf(lpwKeyPmaxName,("%s"),GS_INI_KEY_PMAX);
    if(bPulseAdj){
		sprintf(lpwPulse,("_%s%d"),GS_INI_KEY_PULCNT,pulseCnt);
        strncat(lpwKeyPmaxName,lpwPulse,10);
    }
    else{
		sprintf(lpwPulse,("%s"),(""));
        strncat(lpwKeyPmaxName,lpwPulse,10);
	}
    if(index != DefinitionFreq){
        char lpwSecNameTemp[10];
        sprintf(lpwSecNameTemp,"Freq%d",m_unFreqOption[index]);
 //       cout<<lpwSecNameTemp<<" "<<lpwKeyPmaxName<<endl;
        double m_dPmaxTemp = GetPrivateProfileDouble(lpwSecNameTemp,lpwKeyPmaxName,0.8,GS_INI_FILE);
 //       cout<<m_dPmaxTemp<<endl;
        m_dPmaxTemp = GetPrivateProfileDouble(lpwSecNameTemp,GS_INI_KEY_PMAX,m_dPmaxTemp,GS_INI_FILE);
  //      cout<<m_dPmaxTemp<<endl;
        m_dPmax = GetPrivateProfileDouble(lpwSecName,lpwKeyPmaxName,0.8,GS_INI_FILE);
  //      cout<<m_dPmax<<endl;
         if(m_dPmaxTemp > m_dPmax){
             double t;
             t = m_dPmaxTemp;
             m_dPmaxTemp = m_dPmax;
             m_dPmax = t;
         }
         m_dPmax = ((float)(m_unCurrentFreq-m_unFreqOption[index])/(m_unFreqOption[index+1]-m_unFreqOption[index]))
                 *(m_dPmax-m_dPmaxTemp)+m_dPmaxTemp;
    }
    else
        m_dPmax = GetPrivateProfileDouble(lpwSecName,lpwKeyPmaxName,0.8,GS_INI_FILE);
   //     cout<<m_dPmax<<endl;
    double dDefCurrent,dCurrent = 0;
	char lpwKeyPxITargName[20];
	char lpwSecLDName[5];

    int sconedpumNo = 0;
    int lastLevel = boards[Board::BoardNum-1].LD[0].GetLevel() - 1;
    for(int i = 0; i< Board::BoardNum; i++){
        if(boards[i].LD[0].GetLevel() == lastLevel){
            sconedpumNo++;
        }
    }
    //int sconedpumNo = LDMap->count(rLDMapiter->first);
    int posion = Board::BoardNum-1;
	for(int i= 0; i < m_nToModifyPumpNo; i++)
	{
        for( int j=0;j< sconedpumNo ;j++)
        {
            for(int i = posion; i>=0; i--){
                if(boards[i].LD[0].GetLevel() == lastLevel){
                    posion = i-1;
                    break;
                }
            }
       //     sprintf(lpwSecLDName,("%s%d"),GS_INI_SECTION_LD,rLDMapiter->second->index);
            sprintf(lpwSecLDName,("%s%d"),GS_INI_SECTION_LD,boards[j].LD[0].GetIndex());
            //cout<<lpwSecLDName<<endl;
            dDefCurrent = GetPrivateProfileDouble(lpwSecLDName,GS_INI_KEY_I,0.14,GS_INI_FILE);
           // sprintf(lpwKeyPxITargName,("%s%dITarget%s"),GS_INI_SECTION_LD,rLDMapiter->second->index,lpwPulse);
            sprintf(lpwKeyPxITargName,("%s%dITarget%s"),GS_INI_SECTION_LD,boards[j].LD[0].GetIndex(),lpwPulse);
            dCurrent = GetPrivateProfileDouble(lpwSecName,lpwKeyPxITargName,dDefCurrent,GS_INI_FILE);
            //If the frequency is not defined in the INI file , Current value should take linear value in adjacent frequency
            if(index != DefinitionFreq){
                char lpwSecNameTemp[10];
                //cout<<"Not Sure"<<endl;
                sprintf(lpwSecNameTemp,("%s%d"),GS_INI_SECTION_FREQ,m_unFreqOption[index]);
                sprintf(lpwKeyPxITargName,("%s%dITarget%s"),GS_INI_SECTION_LD,boards[j].LD[0].GetIndex(),lpwPulse);
                //cout<<lpwSecNameTemp<<" "<<lpwKeyPxITargName<<endl;
                double ddCurrent = GetPrivateProfileDouble(lpwSecNameTemp,lpwKeyPxITargName,dDefCurrent,GS_INI_FILE);
                // dCurrent = req/(m_unFreqOption[index+1]-m_unFreqOption[index]);
                //cout<<dCurrent<<" "<<ddCurrent<<endl;
                if(dCurrent < ddCurrent)
                {
                    double t;
                    t = dCurrent;
                    dCurrent = ddCurrent;
                    ddCurrent = t;
                }
                dCurrent = ((float)(m_unCurrentFreq-m_unFreqOption[index])/(m_unFreqOption[index+1]-m_unFreqOption[index]))
                        *(dCurrent-ddCurrent)+ddCurrent;
            }
            //cout<<"setcurr:"<<dCurrent<<endl;
//            if(lasing)
//            {
//                boards[j].LD[0].Set_FloatSurrent(dCurrent);
//            }
      //      cout<<"modify currrent:"<<rLDMapiter->second.index<<endl;
        }
        lastLevel--;
        for(int i = 0; i< Board::BoardNum; i++){
            if(boards[i].LD[0].GetLevel() == lastLevel){
                sconedpumNo++;
            }
        }
      //  sconedpumNo = LDMap->count(rLDMapiter->first);
	}
}

//
// set divider to FPGA registers
void CHotSwap::SetFreq(U16 divider)
{
    if(cs->IsLocked()){
        Set_PD_Dividerd(GS_PD_DIVIDER,divider);
    }
}

// got command to swap pulsewidth, check whether it is proper timing and valid option, and then take action
BYTE CHotSwap::HandlePWidthSwap(UINT widthValue, BYTE Lstatus)
{
	BYTE byResult = SWAPERR_OK;
	if((Lstatus == LASER_LASE) || (Lstatus == LASER_IDLE))
	{
		if(MatchPulseOption(widthValue))
		{
			if(Lstatus == LASER_IDLE) AlterPulseWidth(widthValue, FALSE);
			else /*if(Lstatus == LASER_LASE) */AlterPulseWidth(widthValue, TRUE);
		}
		else byResult = SWAPERR_OVER;
	}
	else byResult = SWAPERR_NOTRDY;
	return byResult;
}

BOOL CHotSwap::MatchPulseOption(unsigned int widthValue)
{
	BOOL bResult = false;
    if(widthValue == GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,-1,GS_INI_FILE))
		bResult = true;
	else if(m_nPulseOptionCount)
	{
		for(int i=0; i<m_nPulseOptionCount; i++)
		{
    //        cout<<m_unPulseOption[i]<<endl;
			if(m_unPulseOption[i] == widthValue) 
			{
				bResult = true;
				break;
			}
		}
	}
	else if(m_bPulCntAdj)
	{
		if((widthValue >= m_unPulseMin) && (widthValue <= m_unPulseMax))
			bResult = true;
	}
	return bResult;
}

//
// alter pulse envelop to change peak power
void CHotSwap::AlterPulseWidth(unsigned char pulseCnt, BOOL lasing)
{
	char lpwSecName[10]; 
	sprintf(lpwSecName,("%s%d"),GS_INI_SECTION_FREQ,m_unCurrentFreq);

        if(m_nPulseOptionCount)
        {
            SetPWidth(pulseCnt);
  //          cout<<"set plus"<<endl;
            ConfigPmaxPxI(lpwSecName,pulseCnt,true,lasing,LastFreqIndex);
            m_unCurrentPulse = pulseCnt;
        }
        else if(m_bPulCntAdj)
        {
   //         cout<<"set11 plus"<<endl;
            SetPWidth(pulseCnt);
            m_unCurrentPulse = pulseCnt;
        }

}

//
// set pulse width/count to FPGA register
void CHotSwap::SetPWidth(unsigned char pulseCnt)
{
    SetPulseGroup(pulseCnt);
}



// check whether it is proper timing and valid power value to output, and then take action
BYTE CHotSwap::HandlePowerSwap(double powerValue, BYTE Lstatus)
{
	BYTE byResult = SWAPERR_OK;
	if((powerValue >= 0) && (powerValue <= m_dPmax))
	{
        if(Lstatus == WorkStatus ) //LASER_LASE->WorkStatus
		{
            switch(WorkStatus)
            {
            case Work_P08H:
                        if(SetOutputPowerUpper(powerValue))
                             m_dPower = powerValue;
                        break;
            default:if(SetOutputPower(powerValue, m_dPower))
                        m_dPower = powerValue;
                    else
                    {
                        m_dPower = GetRealPower();
                        byResult = SWAPERR_OVER;
                    }
                    break;
            }

		}
		else byResult = SWAPERR_NOTRDY;
	}
	else byResult = SWAPERR_OVER;
	return byResult;
}


// output optical power to powerValue in unit of w
BOOL CHotSwap::SetOutputPower(double powerValue, double prvPower)
{
    if(prvPower >= powerValue)
        SetPower(powerValue);
	else
	{
		double tempPower = prvPower + 0.05;
		while(tempPower < powerValue)
		{
			SetPower(tempPower);
            tempPower += 0.05;
		}
        SetPower(powerValue);
	}
    return true;
}

// output optical power to powerValue in unit of w
BOOL CHotSwap::SetOutputPowerUpper(double powerValue)
{
    if(powerValue<=m_P1PowerLimit){
        SetLPower(powerValue);
        //SetHPower(0);
        boards[2].LD[0].SetCurrByReg(m_dHWriteMin);

    }
    else{
        //SetLPower(m_P1PowerLimit);
        boards[0].LD[1].SetCurrByReg(m_dLWriteMax);
        SetHPower(powerValue);
    }
    return true;
}

void CHotSwap::SetLPower(double power)
{
    //save
    int Real = 0;
    double temp[4];
    double writeMin = m_dWriteMin;
     double writeMax = m_dWriteMax;
    for(int i=0; i<4; i++){
        temp[i] = m_dWrite[i];
    }
    //
    m_dWriteMin = m_dLWriteMin;
    m_dWriteMax = m_dLWriteMax;
    for(int i=0; i<4; i++){
        m_dWrite[i] = m_dLWrite[i];
    }
    double dCurrent = PowerToCurrent(power);
 //   printf("setL:%f\n",dCurrent);
    boards[0].LD[1].SetCurrByReg((float)dCurrent);
    //reseve
    m_dWriteMin = writeMin;
    m_dWriteMax = writeMax;
    for(int i=0; i<4; i++){
        m_dWrite[i] = temp[i];
    }
    PresetPower = power;
}

void CHotSwap::SetHPower(double power)
{
    //save
    double temp[4];
    double writeMin = m_dWriteMin;
    double writeMax = m_dWriteMax;
    for(int i=0; i<4; i++){
        temp[i] = m_dWrite[i];
    }
    //
    m_dWriteMin = m_dHWriteMin;
    m_dWriteMax = m_dHWriteMax;
    for(int i=0; i<4; i++){
        m_dWrite[i] = m_dHWrite[i];
    }
    double dCurrent = PowerToCurrent(power);
    boards[2].LD[0].SetCurrByReg((float)dCurrent);
   // boards[2].LD[0].SetCurrByReg(power);
    //reseve
    m_dWriteMin = writeMin;
    m_dWriteMax = writeMax;
    for(int i=0; i<4; i++){
        m_dWrite[i] = temp[i];
    }
    PresetPower = power;
}

// check whether it is proper timing and valid power value to output, and then take action
BYTE CHotSwap::HandlePowerSwap(BYTE Lstatus,BYTE Ratio)
{
	BYTE byResult = SWAPERR_OK;
	if(Lstatus == LASER_LASE)
	{
		//BYTE data_latch = RByteFPGA(GS_DB25_DIN);
		double powerValue = m_dPmax * Ratio / 255;
		SetOutputPower(powerValue, m_dPower);
		m_dPower = powerValue;
	}
	else byResult = SWAPERR_NOTRDY;
	return byResult;
}

//ݹʼֵ
double CHotSwap::PowerToCurrent(double dPowerValue)
{
	double dReturnValue = m_dWriteMin;
//	double Dist=fabs((m_dRootMax-m_dRootMin)/2);
//	double Middle=(m_dRootMax+m_dRootMin)/2;
    double Dist=fabs((m_dWriteMax-m_dWriteMin)/2);
    double Middle=(m_dWriteMax+m_dWriteMin)/2;
	double PowerMiddle=WriteCurrentToPower(Middle);
    if(1/*m_bIs3rdOrder*/)
	{

		double *param = new double[4];
		int nReRootCnt = 0;				//count of real root outputed
		int nImRootCnt = 0;				//count of imaginary root outputed
		double *dOutReRoot = new double[3];			//point to array of real roots
		double dOutImRootReal;		//point to real part of the imaginary root
		double dOutImRootImag;
		int i;
		for(i = 0; i < 4; i++)
			param[i] = m_dWrite[i];
		param[0] -= dPowerValue;

		fnGsCubicRoot(param,&nReRootCnt,&nImRootCnt,dOutReRoot,&dOutImRootReal,&dOutImRootImag);
        for(i=0;i<3&&i<nReRootCnt;i++)
		{
			if(dPowerValue<=PowerMiddle)
			{
				if((Middle-dOutReRoot[i])<Dist&&(Middle-dOutReRoot[i])>=0)
				{
					dReturnValue=dOutReRoot[i];
					Dist=Middle-dOutReRoot[i];
				}
			}
			else
			{
				if((dOutReRoot[i]-Middle)<Dist&&(dOutReRoot[i]-Middle)>=0)
				{
					dReturnValue=dOutReRoot[i];
					Dist=dOutReRoot[i]-Middle;
				}
			}
		}      
        delete[] param;
        delete[] dOutReRoot;

	}
    if(dReturnValue < m_dWriteMin) dReturnValue = m_dWriteMin;
	return dReturnValue;
}

double CHotSwap::PowerToCurrent(double dPowerValue,int * Real)
{
    double dReturnValue = m_dWriteMin;
//	double Dist=fabs((m_dRootMax-m_dRootMin)/2);
//	double Middle=(m_dRootMax+m_dRootMin)/2;
    double Dist=fabs((m_dWriteMax-m_dWriteMin)/2);
    double Middle=(m_dWriteMax+m_dWriteMin)/2;
    double PowerMiddle=WriteCurrentToPower(Middle);
    *Real = false;
    if(1/*m_bIs3rdOrder*/)
    {

        double *param = new double[4];
        int nReRootCnt = 0;				//count of real root outputed
        int nImRootCnt = 0;				//count of imaginary root outputed
        double *dOutReRoot = new double[3];			//point to array of real roots
        double dOutImRootReal;		//point to real part of the imaginary root
        double dOutImRootImag;
        int i;
        for(i = 0; i < 4; i++)
            param[i] = m_dWrite[i];
        param[0] -= dPowerValue;

        fnGsCubicRoot(param,&nReRootCnt,&nImRootCnt,dOutReRoot,&dOutImRootReal,&dOutImRootImag);
        for(i=0;i<3&&i<nReRootCnt;i++)
        {
            if(dPowerValue<=PowerMiddle)
            {
                if((Middle-dOutReRoot[i])<Dist&&(Middle-dOutReRoot[i])>=0)
                {
                    dReturnValue=dOutReRoot[i];
                    Dist=Middle-dOutReRoot[i];
                    *Real = true;
                }
            }
            else
            {
                if((dOutReRoot[i]-Middle)<Dist&&(dOutReRoot[i]-Middle)>=0)
                {
                    dReturnValue=dOutReRoot[i];
                    Dist=dOutReRoot[i]-Middle;
                    *Real = true;
                }
            }
        }
        delete[] param;
        delete[] dOutReRoot;

    }
    if(dReturnValue < m_dWriteMin) dReturnValue = m_dWriteMin;
    return dReturnValue;
}


//ݵֵ㹦
double CHotSwap::CurrentToPower(double dCurrentValue)
{
	double dReturnValue = 0;
	if(dCurrentValue <=  m_dReadMin)
		dReturnValue = 0;
	else
	{
		if(m_bIs3rdOrder)
		{
			dReturnValue = m_dRead[3] * dCurrentValue * dCurrentValue * dCurrentValue;
			dReturnValue += m_dRead[2] * dCurrentValue * dCurrentValue;
			dReturnValue += (m_dRead[1] * dCurrentValue);
			dReturnValue += m_dRead[0];
		}
		else
		{
		
			dReturnValue = m_dRead[2] * dCurrentValue * dCurrentValue;
			dReturnValue += (m_dRead[1] * dCurrentValue);
			dReturnValue += m_dRead[0];
		}
		if(dReturnValue < 0) dReturnValue = 0;
	}
	return dReturnValue;
}

double CHotSwap::CurrentToPower(double dCurrentValue,double ReadMin,double * dRead)
{
    double dReturnValue = 0;
    if(dCurrentValue <=  ReadMin)
        dReturnValue = 0;
    else
    {
        dReturnValue = dRead[3] * dCurrentValue * dCurrentValue * dCurrentValue;
        dReturnValue += dRead[2] * dCurrentValue * dCurrentValue;
        dReturnValue += (dRead[1] * dCurrentValue);
        dReturnValue += dRead[0];
        if(dReturnValue < 0) dReturnValue = 0;
    }
    return dReturnValue;
}

double CHotSwap::WriteCurrentToPower(double dCurrentValue)
{
	double dReturnValue = 0;
	if(dCurrentValue <=  m_dWriteMin)
		dReturnValue = 0;
	else
	{
        if(1/*m_bIs3rdOrder*/)
		{
			dReturnValue = m_dWrite[3] * dCurrentValue * dCurrentValue * dCurrentValue;
			dReturnValue += m_dWrite[2] * dCurrentValue * dCurrentValue;
			dReturnValue += (m_dWrite[1] * dCurrentValue);
			dReturnValue += m_dWrite[0];
		}
//		else
//		{
//            dReturnValue = m_dWrite[2] * dCurrentValue * dCurrentValue;
//			dReturnValue += (m_dWrite[1] * dCurrentValue);
//			dReturnValue += m_dWrite[0];
//		}
		if(dReturnValue < 0) dReturnValue = 0;
    }
	return dReturnValue;
}
double CHotSwap::GetRealPower()
{
    double dPower;
    double dCurrent;
    switch(MachineMode)
    {
    case MachineP08H:
        dCurrent = boards[2].LD->GetCurrAimCurr();
        if(dCurrent<=m_dHReadMin){
            dPower = CurrentToPower(boards[0].LD[1].GetRealCurr(),m_dLReadMin,m_dLRead);
        }
        else{
            dPower = CurrentToPower(dCurrent,m_dHReadMin,m_dHRead);
        }
        if(dPower < 0) dPower = 0;
        return (dPower);
    default:
        dCurrent = m_pLastPump->GetRealCurr();
        dPower = CurrentToPower(dCurrent);
        if(dPower < 0) dPower = 0;
        return (dPower);
    }


}

void CHotSwap::SetPower(double Power)
{
    if(Power <= GS_PMIN){
       //m_pLastPump->Set_FloatSurrent((float)m_dWriteMin);
        m_pLastPump->SetCurrByReg((float)m_dWriteMin);
        PresetPower = GS_PMIN;
    }
    else
    {
        PresetPower = Power;
		double dCurrent = PowerToCurrent(Power);
        m_pLastPump->SetCurrByReg((float)dCurrent);
    }
}

int CHotSwap::GetCurrentFreq()
{
    return m_unCurrentFreq;
}
int CHotSwap::GetCurrentPlus()
{
    return m_unCurrentPulse;
}

void CHotSwap::Set_PD_Dividerd(int Reg, unsigned short Value)
{
     boards[0].LD[0].Read_Set_Reg(0,GS_PD_DIVIDER,WriteReg,Value&0xff,(Value>>8)&0xff,0,0,NULL);
}

bool CHotSwap::IsMeetFreqRange(int Freq)
{
    if((Freq >= m_unFreqMin) && (Freq <= m_unFreqMax))
        return true;
    else
        return false;
}
int CHotSwap::SeekLastFreqIndex(int Freq)
{
    for(int i=0; i<m_nFreqOptionCount; i++)
    {
        if((m_unFreqOption[i] < Freq) && (m_unFreqOption[i+1] > Freq))
        {
            return i;
        }
    }
    return DefinitionFreq;
}

void CHotSwap::SetPulseGroup(unsigned char pluswidth)
{
    boards[0].LD[0].Read_Set_Reg(0,PWIDTH1,0,this->Pwidth1,0,0,0,NULL);
    usleep(500000);
    boards[0].LD[0].Read_Set_Reg(0,PGAP1,0,this->Pgap1,0,0,0,NULL);
    usleep(500000);
    boards[0].LD[0].Read_Set_Reg(0,PWIDTH2,0,this->Pwidth2,0,0,0,NULL);
    usleep(500000);
    boards[0].LD[0].Read_Set_Reg(0,PGAP2,0,this->Pgap2,0,0,0,NULL);
    usleep(500000);
    boards[0].LD[0].Read_Set_Reg(0,PWIDTH3,0,this->Pwidth3,0,0,0,NULL);
   // printf("send: %d %d %d %d %d %x\n",Pwidth1,Pgap1,Pwidth2,Pgap2,Pwidth3,SeedBox->GetID());
    SyncPlusInXCFile();


}

void CHotSwap::SyncPlusInXCFile()
{
    WritePrivateProfileInt(GS_INI_SECTION_PEA ,GS_INI_KEY_PULCNT,Pwidth1,GS_INI_X_FILE);
    WritePrivateProfileInt(GS_INI_SECTION_PEA ,GS_INI_KEY_INTEVAL1,Pgap1,GS_INI_X_FILE);
    WritePrivateProfileInt(GS_INI_SECTION_PEA ,GS_INI_KEY_PULCNT1,Pwidth2,GS_INI_X_FILE);
    WritePrivateProfileInt(GS_INI_SECTION_PEA ,GS_INI_KEY_INTEVAL2,Pgap2,GS_INI_X_FILE);
    WritePrivateProfileInt(GS_INI_SECTION_PEA ,GS_INI_KEY_PULCNT2,Pwidth3,GS_INI_X_FILE);
}
void CHotSwap::GetSetcInfo(unsigned char *arg)
{
    CanDataStruct data;

    boards[0].LD[0].Read_Set_Reg(0,PWIDTH1,BlockingRead,0,0,0,0,&data);
    *arg = data.s_data;
    arg++;
     boards[0].LD[0].Read_Set_Reg(0,PGAP1,BlockingRead,0,0,0,0,&data);
    *arg = data.s_data;
    arg++;
    boards[0].LD[0].Read_Set_Reg(0,PWIDTH2,BlockingRead,0,0,0,0,&data);
    *arg = data.s_data;
    arg++;
     boards[0].LD[0].Read_Set_Reg(0,PGAP2,BlockingRead,0,0,0,0,&data);
    *arg = data.s_data;
    arg++;
     boards[0].LD[0].Read_Set_Reg(0,PWIDTH3,BlockingRead,0,0,0,0,&data);
    *arg = data.s_data;
     arg++;

}

void CHotSwap::SetPulseGroupValue(unsigned char _width1, unsigned char _pgap1, unsigned char _width2, unsigned char _pgap2, unsigned char _width3)
{
    Pwidth1 = _width1;
    Pgap1   = _pgap1;
    Pwidth2 = _width2;
    Pgap2   = _pgap2;
    Pwidth3 = _width3;

}

string CHotSwap::FineTuning(int Node, string StrValue)
{

    char buf[10];
    char bufvlaue[10];
    string str ;
    double DValue;
    stringstream ss(StrValue);
    ss>>DValue;

    if(StrValue == "NO"){
        switch(Node/100){
        case 1: sprintf(buf,"%s%d","P",(Node%100));
                GetPrivateProfileString(buf,GS_INI_KEY_I,"0",bufvlaue,10,GS_INI_X_FILE);
                break;
        case 2: sprintf(buf,"%s%d","P",(Node%100));
                GetPrivateProfileString(buf,GS_INI_KEY_T,"0",bufvlaue,10,GS_INI_X_FILE);
                break;
        case 3:
        break;
        }

        str = bufvlaue;
        return str;
    }
    else{
        switch(Node/100){
        case 1: sprintf(buf,"%s%d","P",(Node%100));
                WritePrivateProfileString(buf, GS_INI_KEY_I, StrValue.c_str(), GS_INI_X_FILE);
                FineTuningAction(Node%100,CURRENT,DValue);
                break;
        case 2:sprintf(buf,"%s%d","P",(Node%100));
                WritePrivateProfileString(buf, GS_INI_KEY_T, StrValue.c_str(), GS_INI_X_FILE);
                FineTuningAction(Node%100,TEMP,DValue);
                break;
        case 3:
        break;
        }
    }
}

void CHotSwap::FineTuningAction(unsigned char P, unsigned char flag, double Value)
{
    char buf[10];
    double idelta;
    double tdelta;
    float current;
    float temp;
    int index = 0;
    for(;index<Board::BoardNum;index++){
        if(boards[index].LD->GetIndex() == P)
            break;
    }
    if(index>=Board::BoardNum){
        //no pipei
        return ;
    }
    switch(flag){
    case CURRENT:sprintf(buf,"%s%d","P",P);
                 idelta = GetPrivateProfileDouble(buf,GS_INI_KEY_I_DELTA,0,GS_INI_HARD_FILE);
                 current = boards[index].LD->GetAimCurr()+(((Value-100)/100.0)*idelta);
                 boards[index].LD->SetCurrByReg(current);   //Set_FloatSurrent(current);
                 break;

    case TEMP:  sprintf(buf,"%s%d","P",P);
                tdelta = GetPrivateProfileDouble(buf,GS_INI_KEY_T_DELTA,0,GS_INI_HARD_FILE);
                temp = boards[index].LD->GetAimTemp()+(((Value-100)/100.0)*tdelta);
                boards[index].LD->Set_FloatTemp(temp);
                break;
    default:break;
    }

}

void CHotSwap::SetOutPutAorB(unsigned char flag)
{
    if(flag == OUTA){
        if(!SwapAB){
            if(m_pLastPumpA != NULL){
            m_pLastPump = m_pLastPumpA;
     //       cout<<"m_pLastPump = m_pLastPumpA"<<endl;
            }
        }
        else{
            if(m_pLastPumpB != NULL){
                m_pLastPump = m_pLastPumpB;
   //         cout<<"m_pLastPump = m_pLastPumpB"<<endl;
            }
        }
    }
    else if(flag == OUTB){
        if(!SwapAB){
            if(m_pLastPumpB != NULL){
            m_pLastPump = m_pLastPumpB;
    //        cout<<"m_pLastPump = m_pLastPumpB"<<endl;
            }
        }
        else{
            if(m_pLastPumpA != NULL){
                m_pLastPump = m_pLastPumpA;
   //         cout<<"m_pLastPump = m_pLastPumpA"<<endl;
            }
        }
    }
}

void CHotSwap::SPOExchange(bool state)
{
 //   cout<<"Exchange:"<<state<<endl;
   SwapAB = state;
}

bool CHotSwap::GetSPOExchangeState()
{
    return SwapAB;
}
bool CHotSwap::IsSingleSpot()
{
    if(m_pLastPumpB == NULL)
        return true;
    else
        return false;
}

unsigned int CHotSwap::DefaultPlus()
{
    int pulsCntTemp;
    int plusTemp;
    int pgap1Temp;
    int plus1Temp;
    int pgap2Temp;
    int plus2Temp;

    plusTemp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,0,GS_INI_X_FILE);
    pgap1Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_INTEVAL1,0,GS_INI_X_FILE);
    plus1Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT1,0,GS_INI_X_FILE);
    pgap2Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_INTEVAL2,0,GS_INI_X_FILE);
    plus2Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT2,0,GS_INI_X_FILE);
    pulsCntTemp = plusTemp + plus1Temp + plus2Temp;

    if(!((pulsCntTemp<= m_unPulseMax) && (pulsCntTemp>= m_unPulseMin))){
        plusTemp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT,0,GS_INI_FILE);
        pgap1Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_INTEVAL1,0,GS_INI_FILE);
        plus1Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT1,0,GS_INI_FILE);
        pgap2Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_INTEVAL2,0,GS_INI_FILE);
        plus2Temp = GetPrivateProfileInt(GS_INI_SECTION_PEA,GS_INI_KEY_PULCNT2,0,GS_INI_FILE);
        pulsCntTemp = plusTemp + plus1Temp + plus2Temp;
    }

    SetPulseGroupValue(plusTemp,pgap1Temp,plus1Temp,pgap2Temp,plus2Temp);

    return pulsCntTemp;

}

void CHotSwap::UpdataUI()
{
    //update UI current pulse and freq
    emit UpdataPlusFreqPower(m_unCurrentPulse,m_unCurrentFreq);
}

float CHotSwap::GetPresetPower()
{
    return PresetPower;
}

unsigned int CHotSwap::GetDiVer(unsigned int Freq)
{
    CanDataStruct data;
    boards[0].LD[0].Read_Set_Reg(0,GS_PD_CNTR,BlockingRead,0,0,0,0,&data);
    return  data.s_data*2/Freq;
}

void CHotSwap::SetDivi()
{
    unsigned int DIVI = GetDiVer(m_unCurrentFreq);
    boards[0].LD[0].Read_Set_Reg(0,GS_PD_DIVIDER,WriteReg,DIVI&0xff,(DIVI>>8)&0xff,0,0,NULL);
}

void CHotSwap::InitSPOArg()
{
    m_dLWrite = new double[4];
    m_dLRead = new double[4];
    m_dHWrite = new double[4];
    m_dHRead = new double[4];

    m_P1PowerLimit = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_P1LIMIT,1,GS_INI_FILE);

    m_dLWrite[3] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LA,1,GS_INI_FILE);
    m_dLWrite[2] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LB,1,GS_INI_FILE);
    m_dLWrite[1] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LC,1,GS_INI_FILE);
    m_dLWrite[0] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LD,1,GS_INI_FILE);
    m_dLWriteMin = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LMIN,1,GS_INI_FILE);
    m_dLWriteMax = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_LMAX,1,GS_INI_FILE);
    m_dLRead[3] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_LA,1,GS_INI_FILE);
    m_dLRead[2] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_LB,1,GS_INI_FILE);
    m_dLRead[1] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_LC,1,GS_INI_FILE);
    m_dLRead[0] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_LD,1,GS_INI_FILE);
    m_dLReadMin = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_LMIN,1,GS_INI_FILE);

    m_dHWrite[3] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HA,1,GS_INI_FILE);
    m_dHWrite[2] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HB,1,GS_INI_FILE);
    m_dHWrite[1] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HC,1,GS_INI_FILE);
    m_dHWrite[0] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HD,1,GS_INI_FILE);
    m_dHWriteMin = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HMIN,1,GS_INI_FILE);
    m_dHWriteMax = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_W_HMAX,1,GS_INI_FILE);
    m_dHRead[3] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_HA,1,GS_INI_FILE);
    m_dHRead[2] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_HB,1,GS_INI_FILE);
    m_dHRead[1] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_HC,1,GS_INI_FILE);
    m_dHRead[0] = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_HD,1,GS_INI_FILE);
    m_dHReadMin = GetPrivateProfileDouble(GS_INI_SECTION_SPO,GS_INI_KEY_R_HMIN,1,GS_INI_FILE);
//    for(int i=0;i<4;i++){
//        printf("%.2f %.2f %.2f %.2f\n",m_dLWrite[i], m_dLRead[i],m_dHWrite[i],m_dHRead[i]);
//    }
}
