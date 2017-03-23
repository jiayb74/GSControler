#include "hanscmd.h"
#include <string>
#include <iostream>
#include "RegTable.h"
#include "CanDateAnalysis.h"
#include "GsApi.h"
#include "board.h"
using namespace std;
extern LDKeeper * lpLD_temp;
extern Board * boards;
extern int WorkStatus;

CHansCMD::CHansCMD(CSoftSwitch *_GsSoftswitch)
{
    GsSoftswitch = _GsSoftswitch;
    this->start();
    m_bTLSealed = false;
    m_bUnLcked = false;
    m_byError = 0;
}
void CHansCMD::run()
{
    dataProcessThreadFun();
}

void CHansCMD::dataProcessThreadFun()
{
    char dat;
    string recBuf;
    while(true)
    {
        if(!(CSerialHandle::getInstance()->m_Qrecieve.empty()))
        {
            dat = CSerialHandle::getInstance()->m_Qrecieve.front();
            CSerialHandle::getInstance()->m_Qrecieve.pop();
         // printf("%0x %c\n",dat,dat);
            if((dat == 0x0d) || dat == 0x0a)
            {
                processCMD(recBuf);
                recBuf.clear();
            }
            else
            {
                recBuf += dat;

                if(recBuf.length() > HANS_CMD_MAXLENGTH)
                {
                    //add error print
                    recBuf.clear();
                }
            }
        }
    }
}

void CHansCMD::processCMD(string str)
{
    char *p = NULL;
    char strBuf[128];
    char strBuf1[128];
    string strCmd = str;
    //set "busy" flag
    //m_bIsProcessing = true;
    //All converted to uppercase. example spo -> SPO
    str2upper(strCmd);

  //cout<<"strcmd:"<<strCmd<<endl;
  //fetch the first character of message
    if( strCmd.compare("VER?") == 0) //return VERxx**-**-**
    {
        CanDataStruct date;
        QString byte0;
        QString byte1;
        lpLD_temp[SeedRank].Read_Set_Reg(0,1,BlockingRead,0,0,0,0,&date);
        byte0 = QString::number(date.s_data&0xff, 16).toUpper();
        byte1 = QString::number((date.s_data>>8)&0xff, 16).toUpper();
        QString FPGAVersion = "Fversion:";
        FPGAVersion = "F:" + byte1 + "-" + byte0 + "\n\r";
        GetPrivateProfileString("ProductID","ID","HAVEANICEDAY",strBuf,20,GS_INI_FILE);
    //  sprintf(strBuf1,"\r\nVER%s\nF%s\nID%s\r",GSVERSION,FPGAVersion,strBuf);
     //   m_sReplyMsg = m_sReplyMsg + "\n";
        m_sReplyMsg= GSVERSION ;
        m_sReplyMsg = m_sReplyMsg + "\r\n" + FPGAVersion.toStdString() ;
        m_sReplyMsg = m_sReplyMsg + "\r\n" + "ID:" + strBuf + "\r\n";
;
    }
    else if( strCmd.compare("I1?") == 0) //return I1xx**.** [A]
    {
        //TODO:get the current
        //double SeedCurrent = m_pLDdriver->GetLDCurrent();
        //sprintf(strBuf,"\r\n%sOK%5.3lf\r","I1",SeedCurrent);
        double SeedCurrent = boards[2].LD[0].GetRealCurr();//GsSoftswitch->lpLD[GsSoftswitch->ModuleNum-1].GetRealCurr();
        sprintf(strBuf,"\r\n%sOK%5.3lf\r","I1",SeedCurrent);
        m_sReplyMsg= strBuf;
    }
    else if( strCmd.compare("L?") == 0) //return Lxx*
    {
        //get the status of Laser
        double progress = 0;
        BYTE Lstatus = OnHansResolved(HANS_L_QUES, &progress,NULL);
        AnswerLStatus(Lstatus,progress);
    }
    else if((strCmd.compare("SPO?")==0)  ||
            (strCmd.compare("SPOA?")==0) ||
            (strCmd.compare("SPOB?")==0))
    {
        bool state = GsSoftswitch->hotSwap->IsSingleSpot();
        if(state && ((strCmd.find("SPOA") == 0) || (strCmd.find("SPOB") == 0))){
            NotAcknowledge();
        }
        else{
            double OpticalPower = 0;
            if(strCmd.compare("SPOB?")==0)
                GsSoftswitch->hotSwap->SetOutPutAorB(OUTB);
            else
                GsSoftswitch->hotSwap->SetOutPutAorB(OUTA);
            OnHansResolved(HANS_SPO_QUES, &OpticalPower,NULL);
            sprintf(strBuf,"\r\n%sOK%6.3lf\r",strCmd.substr(0,strCmd.size()-1).c_str(),OpticalPower);
            m_sReplyMsg= strBuf;
        //  cout<<m_sReplyMsg<<endl;

       }
    }
    else if(strCmd.compare("F?")==0)
    {
        UINT freqValue = 0;
        OnHansResolved(HANS_F_QUES, NULL, &freqValue);
        sprintf(strBuf,"\r\n%sOK%.3d\r","F",freqValue);
        m_sReplyMsg= strBuf;
    }
    else if(strCmd.substr(0,2) == "XC")
    {
        uint Node=0;
        string Value="";
        int pos = strCmd.find('=',0);
        if((pos == 5) && (strCmd.length() > 6)){  //XCXXX=YYY
            string xxx = strCmd.substr(2,3);
            string yyy = strCmd.substr(6,((strCmd.size()-6)>5)?5:(strCmd.size()-6));
            GetSetValue(xxx,1,4,&Node);
            GsSoftswitch->hotSwap->FineTuning(Node,yyy);
            sprintf(strBuf,"\r\nXC%dOK%s\r",Node,yyy.c_str());
             m_sReplyMsg= strBuf;
        }
        else if((strCmd.length() == 6) && (strCmd.find('?',0) == 5)){
            string xxx = strCmd.substr(2,3);
            GetSetValue(xxx,1,4,&Node);
            Value = GsSoftswitch->hotSwap->FineTuning(Node);
            sprintf(strBuf,"\r\nXC%dOK%s\r",Node,Value.c_str());
            m_sReplyMsg= strBuf;
        }
        else{
            NotAcknowledge();
        }
    }
    else if(strCmd.compare("CLRXC")==0){
        if(ClrXCIni()){
            sprintf(strBuf,"\r\nCRXCOK\r");
        }
        else
            sprintf(strBuf,"\r\nCRXCNO\r");
        m_sReplyMsg= strBuf;
    }
    else if(strCmd.compare("SETP?")==0)
    {
        UINT pWidth = 0;
        OnHansResolved(HANS_SETP_QUES, NULL,&pWidth);
        sprintf(strBuf,"\r\n%sOK%d\r","SETP",pWidth);
        m_sReplyMsg= strBuf;
    }
    else if(strCmd.compare("SETC?")==0)
    {
        unsigned char * arg = new unsigned char[10];
        GsSoftswitch->hotSwap->GetSetcInfo(arg);
        sprintf(strBuf,"\r\n%sOK%d,%d,%d,%d,%d\r","SETC",*arg,
                                                        *(arg+1),
                                                        *(arg+2),
                                                        *(arg+3),
                                                        *(arg+4));
        m_sReplyMsg= strBuf;
        delete[] arg;
    }
    else if( strCmd.compare("ERRLASER?") == 0)
    {
        if(m_byError == 0) m_sReplyMsg = "\r\nERRLASEROK999\r";
        else
        {
            if(m_byError & HANSERR_FORMAT) m_sReplyMsg+="\r\nERRLASEROK000\r";
            if(m_byError & HANSERR_PWROVER) m_sReplyMsg+="\r\nERRLASEROK001\r";
            if(m_byError & HANSERR_FREQOVER) m_sReplyMsg +="\r\nERRLASEROK002\r";
            if(m_byError & HANSERR_CNTOVER) m_sReplyMsg +="\r\nERRLASEROK003\r";
            if(m_byError & HANSERR_NOTRDY) m_sReplyMsg +="\r\nERRLASEROK004\r";
            if(m_byError & HANSERR_NOTLCK) m_sReplyMsg +="\r\nERRLASEROK005\r";
            if(m_byError & HANSERR_TEMPOVER) m_sReplyMsg +="\r\nERRLASEROK006\r";
            m_byError = 0;
            if(m_bUnLcked) m_byError |= HANSERR_NOTLCK;
            if(m_bTLSealed) m_byError |= HANSERR_TEMPOVER;
        }
    }
    else if(strCmd.compare("ERRLASERRSET")==0)
    {
        m_byError = 0;
        m_sReplyMsg = "\r\nERRLASERRSETOK\r";
    }
    else if(strCmd.compare("SELFCHECK")==0)
    {
        ResolveSELFXCmd();
    }
    else if(strCmd.compare("AUTOALLOC")==0)
    {
        ResolveSELFXCmd(TRUE);
    }
    else if( strCmd.find("SPO") == 0)   //spo?? meybe error
    {
        string temp;
        bool state = GsSoftswitch->hotSwap->IsSingleSpot();
        if(state && ((strCmd.find("SPOA") == 0) || (strCmd.find("SPOB") == 0))){
            NotAcknowledge();
        }
        else{
            if(strCmd.find("SPOA") == 0){
           //   cout<<"SPOA"<<endl;
                temp = "SPOA";
                sprintf(strBuf,"SPOA");
                strCmd.erase(0,4);
                GsSoftswitch->hotSwap->SetOutPutAorB(OUTA);
            }
            else if(strCmd.find("SPOB") == 0){
            //  cout<<"SPOB"<<endl;
                temp = "SPOB";
                sprintf(strBuf,"SPOB");
                strCmd.erase(0,4);
                GsSoftswitch->hotSwap->SetOutPutAorB(OUTB);
            }
            else if(strCmd.find("SPO") == 0){
                sprintf(strBuf,"SPO");
            //  cout<<"SPO"<<endl;
                temp = "SPO";
                strCmd.erase(0,3);
                GsSoftswitch->hotSwap->SetOutPutAorB(OUTA);
            }
          //  cout<<strCmd<<endl;
            ResolveSPOCmd(strCmd);

            m_sReplyMsg.erase(0,5);
            sprintf(strBuf,"\r\n%s%s",temp.c_str(),m_sReplyMsg.c_str());
            m_sReplyMsg = strBuf;
        }

    }
    else if( strCmd.find(("F")) == 0)
    {
        //get characters rather than "F"
        strCmd.erase(0,1);
        // resolve command left xxx
        ResolveFCmd(strCmd);
    }
    else if( strCmd.find(("SETP")) == 0)
    {
        //get characters rather than "SETP"
        strCmd.erase(0,4);
        // resolve command left x
        ResolveSTEPCmd(strCmd);
    }
    else if( strCmd.find(("SETC")) == 0)
    {
        if(strCmd.length() == 13){
            char m_strCmdTemp[13] = {0};
            strncpy(m_strCmdTemp,strCmd.c_str(),13);
            int j = 0;
            for(int i=0; i<13; i++){
                if(m_strCmdTemp[i] == ',')
                    j++;
            }
            if(j == 4){
                strCmd.erase(0,4);
                int pos = strCmd.find(',',0);
                string pwdth1 = strCmd.substr(0,pos);
                strCmd.erase(0,pos+1);

                pos = strCmd.find(',',0);
                string pgap1 = strCmd.substr(0,pos);
                strCmd.erase(0,pos+1);

                pos = strCmd.find(',',0);
                string pwdth2 = strCmd.substr(0,pos);
                strCmd.erase(0,pos+1);

                pos = strCmd.find(',',0);
                string pgap2 = strCmd.substr(0,pos);
                strCmd.erase(0,pos+1);

                pos = strCmd.find(',',0);
                string pwdth3 = strCmd.substr(0,pos);
                strCmd.erase(0,pos+1);

                UINT value1;
                GetSetValue(pwdth1,1,4,&value1);
                UINT value2;
                GetSetValue(pwdth2,1,4,&value2);
                UINT value3;
                GetSetValue(pwdth3,1,4,&value3);
                UINT value4;
                GetSetValue(pgap1,1,4,&value4);
                UINT value5;
                GetSetValue(pgap2,1,4,&value5);

                int num = value1+value2+value3;
                stringstream ss;
                ss<<num;
                strCmd = ss.str();
                GsSoftswitch->hotSwap->SetPulseGroupValue(value1,value4,value2,value5,value3);
                ResolveSTECCmd(strCmd);
            }
            else
                NotAcknowledge();
        }
        else{
             NotAcknowledge();
        }
    }
    else if(strCmd.find(("REGISTRY")) == 0)
    {
        strCmd.erase(0,8); 
        ResolveREGCmd(strCmd);
    }
    else if(strCmd.compare("RSETSYS")==0)
    {
        m_sReplyMsg = "\r\nRSETSYSOK\r";
        OnHansResolved(HANS_RSET, NULL,NULL);
    }
    else if(strCmd.find("SWAP")==0){
        bool state = GsSoftswitch->hotSwap->IsSingleSpot();
        if(state){
            NotAcknowledge();
        }
        else{
            if(strCmd.find("SWAP?")==0){
                   state = GsSoftswitch->hotSwap->GetSPOExchangeState();
                   if(state == true)
                        sprintf(strBuf,"\r\nSWAPOK1\r");
                   else
                        sprintf(strBuf,"\r\nSWAPOK0\r");
                   m_sReplyMsg = strBuf;
            }
            else{
            //    cout<<strCmd<<endl;
                strCmd = strCmd.substr(4,1);
            //    cout<<strCmd<<endl;
                if(strCmd == "1")
                    GsSoftswitch->hotSwap->SPOExchange(true);
                else
                    GsSoftswitch->hotSwap->SPOExchange(false);
            }
        }
    }
    else // report format error of received cmd
    {
        NotAcknowledge();
    }
    if( m_sReplyMsg.size()!=0 )
    {
        Send2serial();
    }
}

void CHansCMD::Send2serial()
{
    CSerialHandle::getInstance()->WriteSyncPort(m_sReplyMsg.c_str());
    m_sReplyMsg.clear();
}

//reply to command not recognized
void CHansCMD::NotAcknowledge()
{
    char tempBuf[64];
    BYTE NoACK = GS_HANS_NAK;
    m_byError |= HANSERR_FORMAT;
    sprintf(tempBuf,"\r\n%c\r",NoACK);
    m_sReplyMsg = tempBuf;
 // cout<<"error cmd"<<endl;
}

//TODO:
//if there is a modelock change, invoke this method.
//
void CHansCMD::ModeChanged(bool LCK)//ModeUnLck()
{
    if(LCK)
    {
        m_byError &= ~HANSERR_NOTLCK;
        m_bUnLcked = false;
    }
    else
    {
        m_byError |= HANSERR_NOTLCK;
        m_bUnLcked = true;

    }
}


void CHansCMD::AnswerLStatus(BYTE Lstatus, double progress)
{
    char tempBuf[64];
    switch(Lstatus)
    {
    case LASER_IDLE: m_sReplyMsg = "\r\nLOK0\r"; break;
    case LASER_LASE: m_sReplyMsg = "\r\nLOK1\r"; break;
    case LASER_SCAN: m_sReplyMsg = "\r\nLOK2\r"; break;		//Identify Self-Scan mode "LOK2"
    default: if(progress >=1)
                progress = 0.99;
        if(Lstatus == WorkStatus)
            progress=100;
        sprintf(tempBuf,"\r\nLOK%.2f\r",progress);
        m_sReplyMsg = tempBuf;
        break;	//Distinguished as intermediate stage
    }

}
void CHansCMD::ResolveSPOCmd(string strCmd)
{
    double OpticalPower = 0;
    if(GetSetValue(strCmd,1,7,&OpticalPower))
    {
        //cout<<m_bTLSealed<<endl;
        if(m_bTLSealed) m_sReplyMsg = "\r\nSPONG\r";
        else
        {
            BYTE errorCode = OnHansResolved(HANS_SPO_SET, &OpticalPower,NULL);
            switch(errorCode)
            {
            case SWAPERR_OK		: m_sReplyMsg = "\r\nSPOOK\r"; break;
            case SWAPERR_NOTRDY	: m_byError |= HANSERR_NOTRDY;
                                    m_sReplyMsg = "\r\nSPONG\r"; break;
            case SWAPERR_OVER	: m_byError |= HANSERR_PWROVER;
                                    m_sReplyMsg = "\r\nSPONG\r"; break;
            default				: m_sReplyMsg = "\r\nSPONG\r"; break;
        }
        }
    }
    else
        NotAcknowledge();
}

//
// resolve command Fxxx
void CHansCMD::ResolveFCmd(string strCmd)
{
    UINT freqValue = 0;
    if(GetSetValue(strCmd,1,4,&freqValue))
    {
        BYTE errorCode = OnHansResolved(HANS_F_SET, NULL, &freqValue);
        switch(errorCode)
        {
        case SWAPERR_OK		: m_sReplyMsg = "\r\nFOK\r"; break;
        case SWAPERR_NOTRDY	: m_byError |= HANSERR_NOTRDY;
                                m_sReplyMsg = "\r\nFNG\r"; break;
        case SWAPERR_OVER	: m_byError |= HANSERR_FREQOVER;;
                                m_sReplyMsg = "\r\nFNG\r"; break;
        default				: m_sReplyMsg = "\r\nFNG\r"; break;
        }
    }
    else
        NotAcknowledge();
}

// resolve command SETPx
void CHansCMD::ResolveSTEPCmd(string strCmd)
{
    //Check number format
    UINT pWidth = 0;
    if(GetSetValue(strCmd,1,3,&pWidth))
    {
        BYTE errorCode = OnHansResolved(HANS_SETP_SET, NULL,&pWidth);
        switch(errorCode)
        {
        case SWAPERR_OK		: m_sReplyMsg = "\r\nSETPOK\r"; break;
        case SWAPERR_NOTRDY	: m_byError |= HANSERR_NOTRDY;
                                m_sReplyMsg = "\r\nSETPNG\r"; break;
        case SWAPERR_OVER	: m_byError |= HANSERR_CNTOVER;
                                m_sReplyMsg = "\r\nSETPNG\r"; break;
        default				: m_sReplyMsg = "\r\nSETPNG\r"; break;
        }
    }
    else
        NotAcknowledge();
}

// resolve command SETPx
void CHansCMD::ResolveSTECCmd(string strCmd)
{
    //Check number format
    UINT pWidth = 0;
    if(GetSetValue(strCmd,1,3,&pWidth))
    {
        BYTE errorCode = OnHansResolved(HANS_SETC_SET, NULL,&pWidth);
        switch(errorCode)
        {
        case SWAPERR_OK		: m_sReplyMsg = "\r\nSETCOK\r"; break;
        case SWAPERR_NOTRDY	: m_byError |= HANSERR_NOTRDY;
                                m_sReplyMsg = "\r\nSETCNG\r"; break;
        case SWAPERR_OVER	: m_byError |= HANSERR_CNTOVER;
                                m_sReplyMsg = "\r\nSETCNG\r"; break;
        default				: m_sReplyMsg = "\r\nSETCNG\r"; break;
        }
    }
    else
        NotAcknowledge();
}


void CHansCMD::ResolveSELFXCmd(BOOL bToAutoAlloc)
{
    char tempBuf[64];
    double progress;
    bool ScanStyle;
    BYTE Lstatus = OnHansResolved(HANS_L_QUES, &progress,NULL);
    string cmd;
    cmd = bToAutoAlloc ?  "AUTOALLOC" : "SELFCHECK";
    ScanStyle =  bToAutoAlloc ?  0 : 1;
    if(Lstatus == LASER_SCAN)
    {
        m_byError |= HANSERR_NOTRDY;
        sprintf(tempBuf,"\r\n%sNG\r",cmd.c_str());
    }
    else
    {
        emit SelfCheck(ScanStyle);
        sprintf(tempBuf,"\r\n%sOK\r",cmd.c_str());
    }
    m_sReplyMsg= tempBuf;
}

// Handle command REGISTRYx
void CHansCMD::ResolveREGCmd(string strCmd)
{
    UINT keyValue = 0;
    if(GetSetValue(strCmd,1,2,&keyValue))
    {
        switch(keyValue)
        {
        case 0	: SetAutoRun(false); m_sReplyMsg = "\r\nREGISTRYOK\r"; break;
        case 1	: SetAutoRun(true); m_sReplyMsg = "\r\nREGISTRYOK\r"; break;
        default	: m_sReplyMsg = "\r\nREGISTRYNG\r"; break;
        }
    }
    else NotAcknowledge();
}

BOOL CHansCMD::GetSetValue(string strCmd, int BitCntMin, int BitCntMax, double * dValue)
{
    if((strCmd.length() < BitCntMin) || (strCmd.length() >= BitCntMax))
        return FALSE;
    else
    {
        char *num_c = new char[BitCntMax];
        memset(num_c,'\0',sizeof(num_c));
        strncpy(num_c,strCmd.c_str(),BitCntMax);
        int BitNo = 0;
        int PointNo = -1;
        *dValue = 0;
        while((num_c[BitNo] != '\0') && (BitNo <= BitCntMax))
        {
            if(isdigit(num_c[BitNo]))
            {
                *dValue *= 10;
                *dValue += (num_c[BitNo] - 0x30);	//transfer character to decimal digit
            }
            else if(num_c[BitNo] == '.')	//get the decimal point
            {
                PointNo = BitNo;
            }
            else							//illegal character
            {
                delete[] num_c;
                return FALSE;
            }
            BitNo++;
        }
        if(PointNo>=0)
        {
            while(BitNo != (PointNo+1))
            {
                *dValue /=10;
                BitNo --;
            }
        }
        delete[] num_c;
        return TRUE;
    }
}

// convert string from cmd to value
BOOL CHansCMD::GetSetValue(string strCmd, int BitCntMin, int BitCntMax, UINT * nValue)
{
    if((strCmd.length() < BitCntMin) || (strCmd.length() >= BitCntMax))
        return FALSE;
    else
    {
        char *num_c = new char[BitCntMax];
        memset(num_c,'\0',sizeof(num_c));
        strncpy(num_c,strCmd.c_str(),BitCntMax);
        int BitNo = 0;
        *nValue = 0;
        while((num_c[BitNo] != '\0') && (BitNo <= BitCntMax))
        {
            if(isdigit(num_c[BitNo]))
            {
                *nValue *= 10;
                *nValue += (num_c[BitNo] - 0x30);	//transfer character to decimal digit
            }
            else							//illegal character
            {
                delete[] num_c;
                return FALSE;
            }
            BitNo++;
        }
        delete[] num_c;
        return TRUE;
    }
}



BYTE CHansCMD::OnHansResolved(unsigned int uiParam, double *dValue, UINT *unValue)
{
    volatile BYTE byResult = 0;
    volatile BYTE Lstatus = 0;
    switch(uiParam)
    {
    //module HansCMD inquire L? to module SoftSwitch
    case HANS_L_QUES	:	Lstatus = GsSoftswitch->CheckLaserStage();
                            *dValue = 2;
                            byResult = Lstatus;
                            break;
    case HANS_SPO_QUES	:   *dValue = GsSoftswitch->hotSwap->GetRealPower();

                            break;
    case HANS_F_QUES	:	*unValue = GsSoftswitch->hotSwap->GetCurrentFreq();
                            break;
    case HANS_SETP_QUES	:   *unValue = GsSoftswitch->hotSwap->GetCurrentPlus();
                            break;
    case HANS_SPO_SET	:	Lstatus = GsSoftswitch->CheckLaserStage();
                            byResult = GsSoftswitch->hotSwap->HandlePowerSwap(*dValue, Lstatus);
                            GsSoftswitch->hotSwap->UpdataUI();
                            break;
    case HANS_F_SET		:	Lstatus = GsSoftswitch->CheckLaserStage();
                            byResult = GsSoftswitch->hotSwap->HandleFreqSwap(*unValue,Lstatus);
                            GsSoftswitch->hotSwap->UpdataUI();
                            break;
    case HANS_SETP_SET	:   Lstatus = GsSoftswitch->CheckLaserStage();
                            GsSoftswitch->hotSwap->SetPulseGroupValue(*unValue,0,0,0,0);
                            byResult = GsSoftswitch->hotSwap->HandlePWidthSwap(*unValue,Lstatus);
                            GsSoftswitch->hotSwap->UpdataUI();
                            break;
    case HANS_SETC_SET:     Lstatus = GsSoftswitch->CheckLaserStage();
                            byResult = GsSoftswitch->hotSwap->HandlePWidthSwap(*unValue,Lstatus);
                            GsSoftswitch->hotSwap->UpdataUI();
                            break;
    case HANS_RSET		:	//TODO:reset laser
                        //    cout<<"HANS_RSET"<<endl;
                        system("reboot");
                            break;
    default				:	break;
    }
    return byResult;
}
