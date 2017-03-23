#include "GsApi.h"
#include <iostream>
#include <iomanip>
#include <fstream>

char		g_LPSTRCurrentPath[MAX_PATH]={0};

//初始化GS驱动的句柄
BOOL InitHandle()
{
    try
    {
        memset(g_LPSTRCurrentPath, 0, MAX_PATH);
        if(GetModuleFileName(NULL,g_LPSTRCurrentPath,MAX_PATH))
        {
            PathRemoveFileSpec(g_LPSTRCurrentPath);//获取文件路径
        }
    }
    catch(...)
    {
        //QMessageBox::critical(/*first argument should be a diglog*/, "error", "GS API: Unhandle error!\n\t Cannot active GS drivers!");
        //MessageBox(NULL,"GS API: Unhandle error!\n\t Cannot active GS drivers!", "Error",MB_ICONERROR | MB_OK);
    }
}



//Int to byte array
void ntob(int nDataIn,BYTE* pByteOut)
{
    if(sizeof(pByteOut)<4)
    {
        //SetLastError(87);
        errno = 22; //argument invalid
        return;
    }
    memset(pByteOut,0,sizeof(pByteOut));
    pByteOut[0] = (BYTE)(nDataIn >> 24);
    pByteOut[1] = (BYTE)(nDataIn >> 16);
    pByteOut[2] = (BYTE)(nDataIn >> 8);
    pByteOut[3] = (BYTE)(nDataIn >> 0);
}

//
//Byte array to int
int bton(BYTE* pbData)
{
    volatile int nResult = 0;
    nResult = (int)(((pbData[0]&0xFF) << 24) |
                    ((pbData[1]&0xFF) << 16) |
                    ((pbData[2]&0xFF) << 8) |
                    ((pbData[3]&0xFF) << 0) );
    return nResult;
}

double GsString2Double(std::string strValue)
{
    char *num;
    double dReturnValue;

    int nLen = strValue.length()+1;
    num = new char[2*nLen];
    memset(num, 0, 2*nLen);
    //ZeroMemory(num,2*nLen);
    //if(WideCharToMultiByte(CP_ACP, 0, strValue.GetBuffer(), strValue.GetLength(), num, 2*nLen,NULL, NULL)<=0)
    //    return 0;
    if(strValue.length()<= 0)
        return 0;
    dReturnValue = atof(num);
    delete[] num;
    return dReturnValue;
}




//////////////////////////////////////////////////////////////////////////////////////
//Start Read and Write Ini file function apis.
//////////////////////////////////////////////////////////////////////////
// 全局变量
//LPSTR g_pData = NULL;    // 存储整个INI文件数据的缓冲区

/************************************************************************
**函数：GetString
**功能：读INI文件
**参数：
        lpAppName[in]         - 字段名
        lpKeyName[in]         - 键名
        lpReturnedString[out] - 键值
        nSize[in]             - 键值缓冲区大小(in characters )
        lpFileName[in]        - 完整的INI文件名
**返回：Returns the number of bytes read.
**备注：
************************************************************************/
DWORD GetString( LPCSTR lpAppName,LPCSTR lpKeyName,
                LPSTR lpReturnedString, DWORD nSize,LPCSTR lpFileName, char** pData)
{
    //char szFileName[MAX_PATH];
    DWORD dwSize , cchCopied;
    DWORD dwOffset = 0;
    char pLine[MAX_PATH] = {0} ;
    DWORD dwValLen = 0;

    if (!lpAppName || !lpFileName)
        return 0;

    //打开文件，将文件数据拷贝到缓冲区g_pData中，返回INI文件大小
    //WaitForSingleObject(File_MUTEX, INFINITE);
    //pthread_mutex_lock(File_MUTEX); // need to del
    if (0 == (dwSize = ReadIniFile(lpFileName, pData)))
    {
        //ERRORMSG(1, (_T("GetString, Could not ReadIniFile INI file: %s\n"), lpFileName));
        std::string context;
        context = "GetString, Could not ReadIniFile INI file:";
        context += lpFileName;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        //ReleaseMutex(File_MUTEX);
        //phtread_mutex_unlock(File_MUTEX);  // need to del
        return 0;
    }

    cchCopied = 0;
    while ( 0 != (dwOffset = GetLine( pLine , dwOffset , dwSize ,pData)))
    {
        //RETAILMSG(1,(_T("%s\n"),szLine));
        // 是不是注释行
        if (IsComment(pLine))
            continue;

        // 是不是段名
        if (IsSection(pLine))
        {
            // 是不是我们要找的段名
            if (IsSectionName(pLine,lpAppName))
            {
                // 寻找我们要的键名
                while ( 0 != (dwOffset = GetLine(pLine , dwOffset , dwSize, pData)))
                {
                    LPSTR pValue=NULL;

                    if (IsSection(pLine))
                        break;

                    if (IsKey(pLine , lpKeyName, &pValue, &dwValLen))
                    {
                        cchCopied = min(dwValLen, nSize-1);
                        strncpy(lpReturnedString, pValue, cchCopied);
                        lpReturnedString[cchCopied] = 0;
                        // We're done.
                        break;
                    }
                }
                break;
            }
        }
    }
    //phtread_mutex_unlock(File_MUTEX);  //need to del
    //ReleaseMutex(File_MUTEX);
    return cchCopied;
}

/************************************************************************
**函数：ReadIniFile
**功能：打开文件，并将文件数据拷贝到一缓冲区g_pData中
**参数：
        lpFileName[in] - INI文件名，如果没有路径，默认路径为当前程序所在路径
**返回：
        非0 - 该INI文件的大小(in bytes)
        0   - 失败
**备注：
    1). 判断一个文本文件是不是UNICODE文件:读取文件前两个字节,如果是0XFF和0XFE
        则为UNICODE文件,否则为ASCII文件(注意根据CPU是大端还是小端格式),这里默
        认为小端格式.如果是UNICODE文件,则丢去前两个字节.
    2). 先将INI整个文件数据读到BYTE型缓冲区中，再转为char型,为什么?还没弄明白
************************************************************************/
int ReadIniFile(LPCSTR lpFileName, char** ppData)
{
    int nReturn = 0;
    char szFileName[MAX_PATH] ={0} ;
    //BY_HANDLE_FILE_INFORMATION fi = {0};
    FILE* pfile = NULL;
    DWORD dwSize , dwIO;
    BOOL bUnicode = FALSE, bRet = FALSE; // bUnicode - 标志INI文件是不是Unicode文件
    char* l_pData = NULL;
    std::string context;
    if (!lpFileName)
    {
        nReturn = 0;
        endFileRead(l_pData,  pfile);
        return nReturn;
    }


    if (strrchr(lpFileName, '\\'))
    {
        strcpy(szFileName, lpFileName);
        //strcpy_s(szFileName, MAX_PATH, lpFileName);
    }
    else
    {
        if(strlen(g_LPSTRCurrentPath)>0)
        {
            sprintf(szFileName,  "%s\\%s", g_LPSTRCurrentPath,lpFileName);
        }
        else
        {
            sprintf(szFileName,  "\\windows\\%s", lpFileName);
        }
    }

    // ReadIniFile the file.
    /*hfile = CreateFile(szFileName, GENERIC_READ,
        FILE_SHARE_READ,
        (LPSECURITY_ATTRIBUTES)0, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, (HANDLE)0);*/
    pfile = fopen(lpFileName, "r");
    if (pfile == NULL)
    {
        //ERRORMSG(1, (_T("ReadIniFile, CreateFile failed on INI file: %s, err %d\n"), szFileName, GetLastError()));
        context = "ReadIniFile, CreateFile failed on INI file:";
        context += szFileName ;
        context += " err ";
        context += errno;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        nReturn = 0;
        endFileRead(l_pData,  pfile);
        return nReturn;
    }
    struct stat fStat;
    // Get its size.
    //if (!GetFileInformationByHandle(hfile, &fi))
    if(0 != stat(lpFileName, &fStat))
    {
        //CloseHandle(hfile);

        //ERRORMSG(1, (_T("ReadIniFile, GetFileInformationByHandle failed on INI file: %s\n"), szFileName));
        context = "ReadIniFile, GetFileInformationByHandle failed on INI file:";
        context += szFileName;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        nReturn = 0;
        endFileRead(l_pData,  pfile);
        return nReturn;
    }

    //dwSize = (fi.nFileSizeLow + 1) & ~0x1; //keep it word aligned
    dwSize = (fStat.st_size + 1) & ~0x1;
    // Now check if the file is unicode.
    dwIO = 0;
    if (sizeof(WORD) <= dwSize)
    {
        WORD wByteOrderMark = 0;

        // See comment at the bottom of the file.
        //if ((ReadFile(pfile, &wByteOrderMark, sizeof(WORD), &dwIO, NULL)) &&
        if (fread(&wByteOrderMark, sizeof(WORD), 1, pfile)&&
            (dwIO == sizeof(WORD)) &&
            (0xfeff == wByteOrderMark))
        {
            dwSize -= sizeof(WORD);
            bUnicode = TRUE;
        }
        else
            //SetFilePointer(pfile, 0, NULL, FILE_BEGIN);
            fseek(pfile, 0, SEEK_SET);
    }

    // Next, load the data.
    //RETAILMSG(1, (_T("ReadIniFile, the size of ini file [%s] is [%d]\n"), szFileName, dwSize));
    if (0 < dwSize)
    {
        l_pData = (char *)malloc( dwSize + sizeof(char) );
        if (!l_pData)
        {
            //RETAILMSG(1, (_T("ReadIniFile, no enough memory\n")));
            GSLogEvent("ReadIniFile, no enough memory",LOG_EVENT_SYSERR);
            nReturn = 0;
            endFileRead(l_pData,  pfile);
            return nReturn;
        }
        memset(l_pData,0,dwSize + sizeof(char));
        //bRet = ReadFile(pfile, pData, dwSize, &dwIO, NULL);
        bRet = fread(l_pData, sizeof(WORD),dwSize, pfile);
        //CloseHandle(hfile);
        fclose(pfile);
        pfile = NULL;
        if (!bRet)
        {
            //RETAILMSG(1, (_T("ReadIniFile, ReadFile fail, err = [%d]\n"),GetLastError()));
            context = "ReadIniFile, ReadFile fail, err = [";
            context += errno;
            context += "]";
            GSLogEvent(context,LOG_EVENT_SYSERR);
            nReturn = 0;
            endFileRead(l_pData,  pfile);
            return nReturn;
        }
        // Create the buffer.
        if( *ppData )
        {
            free(*ppData);
            *ppData = NULL;
        }

        //dwSize = MultiByteToWideChar( CP_ACP,0, (char *)pData ,-1,NULL,0);
        dwSize = strlen(l_pData) +1;
        *ppData = (char *)malloc(dwSize*sizeof(char));
        if(!(*ppData))
        {
            //RETAILMSG(1, (_T("ReadIniFile, no enough momery\n")));
            GSLogEvent("ReadIniFile, no enough memory",LOG_EVENT_SYSERR);
            nReturn = 0;
            endFileRead(l_pData,  pfile);
            return nReturn;
        }
       // MultiByteToWideChar(CP_ACP,0,(char *)pData,-1,g_pData,dwSize);
        strcpy(*ppData, l_pData);

        dwSize--; // includes the NULL termination character
        nReturn = dwSize;
    }


    endFileRead(l_pData,  pfile);
    return nReturn;
}


/************************************************************************
**函数：GetLine
**功能：获取在g_pData中从dwOffset位置开始的一行数据并保存到pLine，同时把偏移量dwOffset
        移到下一行行首
**参数：
        pLine[out]   - 接收一行数据(不包括\r\n)
        dwOffset[in] - 要读取的那一行的开始位置
        dwSize[in]   - INI文件大小
**返回：
        正确 - 下一行行首的位置
        错误 - 0
**备注：
/************************************************************************/
DWORD GetLine(LPSTR pLine, DWORD dwOffset, DWORD dwSize, char** ppData)
{
    DWORD len = 0;
    DWORD len2 = 0;
    // Look for the end of the line.
    while ( dwOffset + len < dwSize
        && '\r' != (*ppData)[dwOffset+len] && '\n' != (*ppData)[dwOffset+len])
    {
        if( (*ppData)[dwOffset+len]==0 )
            break;

        pLine[len] = (*ppData)[dwOffset+len] ;
        ++len;
    }

    pLine[len] = 0 ;
    // Now push the internal offset past the newline.
    // (We assume \r\n pairs are always in this order)
    if (dwOffset + len + len2 < dwSize && '\r' == (*ppData)[dwOffset+len+len2])
        ++len2;
    if (dwOffset + len + len2+1 < dwSize && '\n' == (*ppData)[dwOffset+len+len2])
        ++len2;


    if (2 >= len + len2 && (dwOffset +2 >=dwSize) )
        return 0;

    dwOffset += len + len2;
    return dwOffset;
}

/************************************************************************
**函数：IsComment
**功能：判断是不是注释行
**参数：
        pLine[in] - INI的一行数据
**返回：
        1 - 注释行
        0 - 不是注释行
**备注：
    1). 空行也视为注释行
************************************************************************/
BOOL IsComment(LPCSTR pLine)
{
    if (!pLine || 0 == strlen(pLine) || ';' == *pLine)
        return TRUE;
    return FALSE;
}

/************************************************************************
**函数：IsSection
**功能：判断是不是段名
**参数：
        pLine[in] - INI的一行数据
**返回：
        1 - 是段名
        0 - 不是
************************************************************************/
BOOL IsSection(LPCSTR pLine)
{
    if (pLine && '[' == *pLine)
        return TRUE;
    return FALSE;
}

/************************************************************************
**函数：IsSectionName
**功能：判断是INI文件的一行(pLine)是不是我们要找的段名(pSection)
**参数：
        pLine[in]    - INI文件的一行数据
        pSection[in] - 要找的段名
**返回：
        1 - 是
        0 - 不是
**备注：
************************************************************************/
BOOL IsSectionName(LPCSTR pLine, LPCSTR pSection)
{
    if (IsSection(pLine))
    {
        DWORD len = strlen(pSection);
        if (strlen(pLine) - 2 == len && 0 == strncasecmp(pLine+1, pSection, len))
            return TRUE;
    }
    return FALSE;
}

/************************************************************************
**函数：IsKey
**功能：判断INI文件中一行的数据是不是我们要找的键名,如果是并读取键值
**参数：
        pLine[in] - INI文件某行数据
        pKeyName[in] - 要寻找的键名
        pValue[out] - 键值
        dwValLen[out] - 键值pValue大小(in bytes)
**返回：
        1 - 是，同时pValue返回键值
        0 - 不是，pValue为NULL
**备注：
************************************************************************/
BOOL IsKey(LPCSTR pLine , LPCSTR pKeyName, LPSTR* pValue, DWORD* dwValLen )
{
    LPCSTR pEqual = NULL;
    DWORD length = 0, len = 0;

    if(!pLine || !pKeyName)
        return FALSE;

    // pLine是不是注释行
    if (IsComment( pLine ))
        return FALSE;

    // 寻找"="号
    pEqual = strchr(pLine, '=' );
    if (!pEqual)
        return FALSE;
    // 寻找键名最后一字符的位置
    while (pEqual - 1 >= pLine && iswspace(*(pEqual-1)))
        --pEqual;
    // Badly formed file.
    if (pEqual - 1 < pLine)
        return FALSE;

    // 键名长度
    length = pEqual - pLine;

    len = strlen(pKeyName);
//	RETAILMSG(1,(TEXT("File KeyLength=%d,Read Key Len=%d \r\n"),length,len));
    if (len == length && 0 == strncasecmp(pLine, pKeyName, len))
//	if ( 0 == _wcsnicmp(pLine, pKeyName, len))
    {
        *pValue = (LPSTR)strchr(pLine, '=' );
        ++(*pValue);
        *dwValLen = strlen(pLine) - ((*pValue) - pLine);

        // 去掉紧跟在"="号后的所有空格
        while (0 < *dwValLen && iswspace(**pValue))
        {
            ++(*pValue);
            --(*dwValLen);
        }
        while (0 < *dwValLen && iswspace((*pValue)[*dwValLen-1]))
        {
            --(*dwValLen);
        }
        // If the string is surrounded by quotes, remove them.
        if ('"' == (**pValue))
        {
            ++(*pValue);
            --(*dwValLen);
            if ('"' == (*pValue)[*dwValLen-1])
            {
                --(*dwValLen);
            }
        }
        return TRUE;
    }
    else
    {
        *pValue = NULL;
        return FALSE;
    }
}

/************************************************************************
**函数：IsPrivateProfileKeyName
**功能：WCE下confirm whether a section name exists
**参数：
        lpKeyName[in]		  - 要寻找的键名
        lpAppName[in]         - points to section name
        lpFileName[in]        - points to initialization filename
**返回：1 exist
        0 not
************************************************************************/
BOOL	IsPrivateProfileKeyName(
                                            LPCSTR lpKeyName,
                                            LPCSTR lpAppName,
                                            LPCSTR lpFileName)
{
    //char szFileName[MAX_PATH];
    DWORD dwSize , cchCopied;
    DWORD dwOffset = 0;
    char pLine[MAX_PATH] = {0} ;
    DWORD dwValLen = 0;
    if (!lpAppName || !lpFileName)
        return 0;

    //打开文件，将文件数据拷贝到缓冲区g_pData中，返回INI文件大小
    char* pData = NULL;
    //WaitForSingleObject(File_MUTEX, INFINITE);
    if (0 == (dwSize = ReadIniFile(lpFileName,&pData)))
    {
        //ERRORMSG(1, (_T("GetString, Could not ReadIniFile INI file: %s\n"), lpFileName));
        std::string context;
        context = "GetString, Could not ReadIniFile INI file: s";
        context += lpFileName;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        //ReleaseMutex(File_MUTEX);
        return 0;
    }

    cchCopied = 0;
    while ( 0 != (dwOffset = GetLine( pLine , dwOffset , dwSize ,&pData)))
    {
        //RETAILMSG(1,(_T("%s\n"),szLine));
        // 是不是注释行
        if (IsComment(pLine))
            continue;

        // 是不是段名
        if (IsSection(pLine))
        {
            // 是不是我们要找的段名
            if (IsSectionName(pLine,lpAppName))
            {
                // 寻找我们要的键名
                while ( 0 != (dwOffset = GetLine(pLine , dwOffset , dwSize, &pData)))
                {
                    LPSTR pValue=NULL;

                    if (IsSection(pLine))
                        break;

                    if (IsKey(pLine , lpKeyName, &pValue, &dwValLen))
                    {
                        // We're done.
                        //ReleaseMutex(File_MUTEX);
                        if(pData != NULL)    //by wt
                             delete[] pData;
                        return 1;
                    }
                }
                break;
            }
        }
    }
    //ReleaseMutex(File_MUTEX);
    if(pData != NULL)    //by wt
         delete[] pData;
    return 0;
}



/************************************************************************
**函数：IsPrivateProfileSectionName
**功能：WCE下confirm whether a section name exists
**参数：
        lpAppName[in]         - points to section name
        lpFileName[in]        - points to initialization filename
**返回：1 exist
        0 not
************************************************************************/
BOOL	IsPrivateProfileSectionName(
                                            LPCSTR lpAppName,
                                            LPCSTR lpFileName)
{
    DWORD dwSize ;
    DWORD dwOffset = 0;
    char pLine[MAX_PATH] = {0} ;

    if (!lpAppName || !lpFileName)
        return 0;

    //打开文件，将文件数据拷贝到缓冲区g_pData中，返回INI文件大小
    char* pData = NULL;
    //WaitForSingleObject(File_MUTEX, INFINITE);
    if (0 == (dwSize = ReadIniFile(lpFileName, &pData)))
    {
        //ERRORMSG(1, (_T("GetString, Could not ReadIniFile INI file: %s\n"), lpFileName));
        std::string context;
        context = "GetString, Could not ReadIniFile INI file: %s";
        context += lpFileName;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        //ReleaseMutex(File_MUTEX);
        return 0;
    }

    while ( 0 != (dwOffset = GetLine( pLine , dwOffset , dwSize ,&pData)))
    {
        // 是不是段名
        if (IsSection(pLine))
        {
            // 是不是我们要找的段名
            if (IsSectionName(pLine,lpAppName))
            {
                //ReleaseMutex(File_MUTEX);
                return 1;
            }
            else continue;
        }
    }
    //ReleaseMutex(File_MUTEX);
    return 0;
}

/************************************************************************
**函数：GetPrivateProfileString
**功能：WCE下读取INI文件中某段名/键名的键值的字符串
**参数：
        lpAppName[in]         - points to section name
        lpKeyName[in]         - points to key name
        lpDefault[in]         - points to default string
        lpReturnedString[out] - points to destination buffer
        nSize[in]             - size of destination buffer "lpReturnedString"(in characters)
        lpFileName[in]        - points to initialization filename
**返回：The return value is the number of characters copied to the buffer,
        not including the terminating null character.
**备注：
    1). 如果INI文件没有你关心的数据，返回默认值lpDefault
************************************************************************/
DWORD GetPrivateProfileString(LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpDefault,
    LPSTR  lpReturnedString,
    DWORD   Size,
    LPCSTR lpFileName)
{
    DWORD dwRc = 0, dwReturn = 0;
    if(!lpAppName || !lpKeyName || !lpReturnedString || !lpFileName || Size<=0 )
        return 0;

    char* pData = NULL;
    dwRc = GetString(lpAppName,lpKeyName,lpReturnedString,Size,lpFileName, &pData);
    if(dwRc != 0)
    {
        dwReturn = dwRc;
    }
    else
    {
        if(lpDefault!= NULL)
        {
            strncpy(lpReturnedString, lpDefault, Size);
            //strncpy_s(lpReturnedString, dwRc, lpDefault, Size);
            lpReturnedString[Size-1] = NULL;
        }
        else
            *lpReturnedString = 0;
        dwReturn = strlen(lpReturnedString);
    }

    // 释放内存
    if (NULL != pData)
    {
        free( pData );
        pData = NULL;
    }

    return dwReturn;
}

/************************************************************************
**函数：GetPrivateProfileInt
**功能： retrieves an integer associated with a key in the
         specified section of the given initialization file
**参数：
        LPCSTR lpAppName,  // address of section name
        LPCSTR lpKeyName,  // address of key name
        INT nDefault,       // return value if key name is not found
        LPCSTR lpFileName  // address of initialization filename
**返回：
        The return value is the integer equivalent of the string following
        the specified key name in the specified initialization file. If the
        key is not found, the return value is the specified default value.
        If the value of the key is less than zero, the return value is zero.
**备注：
************************************************************************/
UINT GetPrivateProfileInt(
  LPCSTR lpAppName,
  LPCSTR lpKeyName,
  INT nDefault,
  LPCSTR lpFileName)
{
    char szRet[80] ={0};
    if(!lpAppName || !lpKeyName || !lpFileName )
        return 0;

    char* pData = NULL;
    DWORD cch = GetString(lpAppName, lpKeyName, szRet, sizeof(szRet)/sizeof(char), lpFileName, &pData);

    // 释放内存
    if (NULL != (pData))
    {
        free( pData );
        pData = NULL;
    }
    if (cch)
        return atoi(szRet);
    else
        return nDefault;
}

/************************************************************************
**函数：GetPrivateProfileDouble
**功能：WCE下读取INI文件中某段名/键名的键值的字符串
**参数：
        lpAppName[in]         - points to section name
        lpKeyName[in]         - points to key name
        dDefault[in]		  - default value
        lpFileName[in]        - points to initialization filename
**返回：The return value is the double equivalent of the string following
        the specified key name in the specified initialization file. If the
        key is not found, the return value is the specified default value.
**作者：WYY
**日期：06.14.13
************************************************************************/
double GetPrivateProfileDouble(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    double	dDefault,
    LPCSTR lpFileName)
{
    char c_str[20];
    memset(c_str,0,sizeof(c_str));
    std::string str= "";
    char *num;
    double dReturnValue;
    if(GetPrivateProfileString(lpAppName, lpKeyName, "", c_str, 20, lpFileName))
    {
        str = c_str;
        num = new char[str.length()+1];
        for(int i=0;i<str.length();i++)
        {
            num[i]=str.at(i);
        }
        num[str.length()]='\0';
        dReturnValue = atof(num);
        delete[] num;
    }
    else
    {
        dReturnValue = dDefault;
    }
    return dReturnValue;
}

/************************************************************************
**函数：WriteLine
**功能：向文件写入一行数据(包括回车换行符)
**参数：
        hOutput[in] - 已打开的文件句柄
        pLine[in]   - 要写入的一行数据
**日期：08.1.2
**备注：
    1). 写入一行数据，也包括写入行末的"\r\n"两个字符
    2). 注意区分是不是UNICODE版本的INI文件，如果不是，要将char转为char再写入INI文件
    3). 注意不要将结束符也写入文件
************************************************************************/
void WriteLine(FILE* pFile , LPCSTR pLine)
{
    char buffer[MAX_PATH] ={0};
    DWORD dwlen = strlen(pLine) ;
    BOOL bUnicode = FALSE ;
    if(bUnicode)
    {
        if (pLine)
        {
            //WriteFile(hOutput, pLine, strlen(pLine)*sizeof(char), &dwWrote, NULL);
            //WriteFile(hOutput, "\r\n", 2*sizeof(char), &dwWrote, NULL);
            fwrite(pLine, strlen(pLine)*sizeof(char), 1, pFile);
            fwrite("\r\n", 2*sizeof(char), 1, pFile);
        }
    }
    else
    {
        if (pLine)
        {
            int bsize ;
            char szTempLine[MAX_PATH] = {0} ;
            strncpy( szTempLine, pLine, dwlen);
            //strncpy_s( szTempLine, MAX_PATH, pLine, dwlen);
            bsize=strlen(szTempLine);
            strcpy(buffer, szTempLine);
            //buffer[bsize] = 0 ;

            //WriteFile(hOutput, buffer, bsize-1 , &dwWrote, NULL); // 注意不要将结束符也写入
            //WriteFile(hOutput, "\r\n", 2, &dwWrote, NULL);
            fwrite(buffer, bsize,1, pFile);
            fwrite("\r\n", 2,1, pFile);
        }
    }
}

/************************************************************************
**函数：WritePrivateProfileString
**功能：WCE环境下，向指定INI文件指定段名写入字符串数据
**参数：
    lpAppName[in]
        Pointer to a null-terminated string containing section name. If
        the section does not exit, it is created.
    lpKeyName[in]
        Pointer to a null-terminated string containing key name. If the
        key does not exit in the specified section pointed to by the lpAppName
        parameter, it's created. If this parameter is NULL, the ertire section,
        including all keys within the section, is deleted. When deleting a
        section, leave the comments intact.
    lpString[in]
        pointer to a null-terminated string to be written to the file.
        If this parameter is NULL, the key pointed to by the lpKeyName
        parameter is deleted.
    lpFileName[in]
        Pointer to a null-terminated string that names the initialization file.
**返回：
        FALSE - fail
        TRUE  - success
**备注：
    1). 先将要修改的INI文件的全部数据读取到全局内存g_pData中
    2). 在g_pData中定位到我们要修改的位置，将其它数据和我们修改的数据写入一临时ini文件
    3). 最后将临时ini文件覆盖原来的ini文件，再删除临时ini文件
    4). 主要的API函数：
        CreateFile、ReadFile、WriteFile、SetEndOfFile、CopyFile、DeleteFile
    5). 如果lpKeyName == NULL, 删除整个段, 如果lpString == NULL, 删除健
************************************************************************/
BOOL WritePrivateProfileString(LPCSTR lpAppName,
  LPCSTR lpKeyName,
  LPCSTR lpString,
  LPCSTR lpFileName)
{
    DWORD dwSize, dwOffset;                // dwSize - ini文件大小, dwOffset - 偏移量
    BOOL  bReadLine = TRUE;
    BOOL  bWrote = FALSE;
    char pszLine[MAX_PATH] = {0};         // 存储一行的数据
    char pszIniFileTemp[MAX_PATH] = {0};  // 临时ini文件的名称(包括路径)
    char pszIniFile[MAX_PATH] = {0};  // 临时ini文件的名称(包括路径)
    FILE* pFile;
    LPSTR pValue;
    DWORD dwValLen;
    //LPSTR pName;
    LPCSTR pName;

    dwOffset = 0;
    if (!lpFileName)
        return FALSE;

    // 读取INI文件内容到全局变量g_pData内存中
    char* pData = NULL;
    //WaitForSingleObject(File_MUTEX, INFINITE);
    dwSize = ReadIniFile(lpFileName, &pData);

    //RETAILMSG(1, (TEXT("lpFileName=[%s], dwSize=[%d]"), lpFileName, dwSize));

    // Create the output file.
  //  pName = (LPSTR)strrchr(lpFileName, '\\');
    pName = (LPSTR)strrchr(lpFileName, '/');

    if(pName)
    {
        pName++;
     //   sprintf(pszIniFileTemp,"\\Windows\\%s.tmp",pName);
        sprintf(pszIniFileTemp,"./doc/%s.tmp",pName);
        sprintf(pszIniFile,"%s",lpFileName);
    }
    else
    {
        if(strlen(g_LPSTRCurrentPath)>0)
        {
            sprintf(pszIniFileTemp, "%s/%s.tmp", g_LPSTRCurrentPath,lpFileName);
            sprintf(pszIniFile,  "%s/%s", g_LPSTRCurrentPath,lpFileName);
        }
        else
        {
            sprintf(pszIniFileTemp, "/windows/%s.tmp",lpFileName);
            sprintf(pszIniFile, "/windows/%s", lpFileName);
        }
    }

    /*hOutputFile = CreateFile(pszIniFileTemp,
        GENERIC_WRITE,
        0,(LPSECURITY_ATTRIBUTES)0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0);*/
    pFile = fopen(pszIniFileTemp,"w+");
    if (NULL == pFile)
    {
        //ERRORMSG(1, (TEXT("Could not open output file: %s\n"), pszIniFileTemp));
        std::string context;
        context ="Could not open output file: ";
        context += pszIniFileTemp;
        GSLogEvent(context,LOG_EVENT_SYSERR);
        //ReleaseMutex(File_MUTEX);
        return FALSE;
    }

    // 将所有数据写入临时ini文件
    for (;;)
    {
        // The bReadLine flag is used to not read a new line after we break
        // out of the inner loop. We've already got a line to process.
        if (bReadLine)
        {
            dwOffset = GetLine( pszLine , dwOffset , dwSize , &pData);
            if (!dwOffset)
                break;
        }
        bReadLine = TRUE;
        // Skip past comments.
        if (IsComment(pszLine))
        {
            WriteLine(pFile , pszLine);
            continue;
        }
        // Found a section name.
        if (IsSection(pszLine))
        {
            // It's the section we want.
            if (IsSectionName(pszLine , lpAppName))
            {
                // 以下如果lpKeyName为NULL，删除整个段
                if (lpKeyName)
                    WriteLine(pFile , pszLine);

                // Process the whole section.
                while (0 != (dwOffset = GetLine( pszLine , dwOffset , dwSize , &pData)))
                {
                    // Reached the end of the section.
                    if (IsSection(pszLine))
                    {
                        bReadLine = FALSE;
                        // This line will be written in the outer loop.
                        break;
                    }
                    // When deleting a section, leave the comments intact.
                    else if (IsComment(pszLine))
                    {
                        WriteLine(pFile , pszLine);
                        continue;
                    }
                    // Got the value we want.
                    if (!bWrote && IsKey(pszLine , lpKeyName, &pValue, &dwValLen))
                    {
                        bWrote = TRUE;
                        // 如果lpString为NULL，删除健lpKeyName
                        if(lpString)
                            WriteValue(pFile , NULL, lpKeyName, lpString);
                    }
                    else
                    {
                        if (lpKeyName)
                            WriteLine(pFile , pszLine);
                    }

                    if(dwOffset >= dwSize)
                        break ;
                }

                // 如果在段名lpAppName下键名lpKeyName不存在，则新建键名lpKeyName和键值lpString
                if (!bWrote)
                {
                    bWrote = TRUE;
                    WriteValue(pFile, NULL, lpKeyName, lpString);
                }
            }
            else
                WriteLine(pFile , pszLine);
        }
        else
            WriteLine(pFile , pszLine);

        if(dwOffset ==0)
            break;
    }
    // 如果指定的段名lpAppName不存在，则新建段名lpAppName及键名lpKeyName和键值lpString
    if (!bWrote && lpKeyName && lpString)
    {
        WriteValue(pFile , lpAppName, lpKeyName, lpString);
    }

    // 用临时ini文件覆盖原来的ini文件并删除临时ini文件
    if (NULL != pFile)
    {
        //SetEndOfFile(hOutputFile );
        fclose(pFile);
        pFile = NULL;
        CopyFile(pszIniFileTemp, pszIniFile, FALSE);
        remove(pszIniFileTemp);
    }
    if (NULL != pData)
    {
        free( pData ) ;
        pData = NULL ;
    }

    return TRUE;
}

/************************************************************************
**函数：WritePrivateProfileInt
**功能：WCE环境下，向指定INI文件指定段名写入整型数据
**参数：参考WritePrivateProfileString函数
**返回：
        FALSE - fail
        TRUE  - success
**备注：
************************************************************************/
BOOL WritePrivateProfileInt(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    INT     Value,
    LPCSTR lpFileName)
{
    char ValBuf[16]={0};

    sprintf( ValBuf, "%i" , Value);
    return( WritePrivateProfileString(lpAppName, lpKeyName, ValBuf, lpFileName) );
}
/************************************************************************
**函数：WritePrivateProfileDouble
**功能：WCE环境下，向指定INI文件指定段名写入整型数据
**参数：参考WritePrivateProfileString函数
**返回：
        FALSE - fail
        TRUE  - success
**备注：
************************************************************************/
BOOL WritePrivateProfileDouble(
                                            LPCSTR lpAppName,
                                            LPCSTR lpKeyName,
                                            double	dValue,
                                            LPCSTR lpFileName)
{
    char ValBuf[16]={0};
    sprintf( ValBuf,  "%.3f" , dValue);
    return( WritePrivateProfileString(lpAppName, lpKeyName, ValBuf, lpFileName) );

}

/************************************************************************
**函数：WriteValue
**功能：向指定INI文件中写入段名、键名和键值
**参数：
    m_hOutput[in]
        pointer to the handle of ini file.
    pAppName[in]
        Pointer to a null-terminated string containing the name of the section
        in which data is written. If this parameter is NULL, the WriteValue
        function just wirte the pKeyName and pString.
    pKeyName[in]
        Pointer to a null-terminated string containing the name of the key in
        which data is writtern.
    pString[in]
        Pointer to a null-terminated string to be written to the file.
**返回：NONE
**备注：
    1). 要成功写入INI文件，键名pKeyName和键值pString都不能为NULL。
    2). 如果段名pAppName为NULL，则只写入键名pKeyName和键值pString。
    3). 注意往INI文件写入字符串时，不要写入结束符。
************************************************************************/
void WriteValue(FILE* pFile, LPCSTR pAppName, LPCSTR pKeyName, LPCSTR pString)
{
    char buffer[MAX_PATH] = {0} ;
    DWORD dwWrote;
    BOOL m_bUnicode = FALSE ;

    if (pKeyName && pString)
    {
        //RETAILMSG( 1 , (TEXT("pKeyName : %s , pString : %s"), pKeyName , pString) ) ;

        if (pAppName)		// 写入段名
        {
            if (m_bUnicode)
            {
                //WriteFile(m_hOutput, "[", sizeof(char), &dwWrote, NULL);
                //WriteFile(m_hOutput, pAppName, strlen(pAppName)*sizeof(char), &dwWrote, NULL);
                //WriteFile(m_hOutput, "]\r\n", 3*sizeof(char), &dwWrote, NULL);
                fwrite("[", sizeof(char), 1, pFile);
                fwrite(pAppName, strlen(pAppName)*sizeof(char), 1, pFile);
            }
            else
            {
                int bsize , iRetLen;
                char szTempLine[256] ={0} ;

                strcpy( szTempLine , "[" ) ;
                strcat( szTempLine , pAppName ) ;
                strcat( szTempLine , "]" ) ;
                //strcpy_s( szTempLine , 256, TEXT("[") ) ;
                //strcat_s( szTempLine , 256, pAppName ) ;
                //strcat_s( szTempLine , 256, TEXT("]") ) ;

                //bsize=WideCharToMultiByte(CP_ACP,0,szTempLine,-1,NULL,0,NULL,NULL);
                bsize = strlen(szTempLine);
                //iRetLen = WideCharToMultiByte(CP_ACP,0,szTempLine,-1,buffer, bsize,NULL,NULL);
                iRetLen = bsize;
                strcpy(buffer, szTempLine);
                buffer[bsize] = 0 ;

                //WriteFile(m_hOutput, buffer , bsize-1 , &dwWrote, NULL);
                //WriteFile(m_hOutput, "\r\n", 2, &dwWrote, NULL);
                fwrite(buffer, bsize, 1, pFile);
                fwrite("\r\n",2,1,pFile);   //add by wt 2016-3-29 note:add huanhang

            }
        }

        if (m_bUnicode)		// 写入健名和键值
        {
            //WriteFile(m_hOutput, pKeyName, strlen(pKeyName)*sizeof(char), &dwWrote, NULL);
            //WriteFile(m_hOutput, "=", sizeof(char), &dwWrote, NULL);
            //WriteFile(m_hOutput, pString, strlen(pString)*sizeof(char), &dwWrote, NULL);
            //WriteFile(m_hOutput, "\r\n", 2*sizeof(char), &dwWrote, NULL);
            fwrite(pKeyName, strlen(pKeyName)*sizeof(char), 1, pFile);
            fwrite("=", sizeof(char), 1, pFile);
            fwrite(pString, strlen(pString)*sizeof(char), 1, pFile);
            fwrite("\r\n", sizeof(char), 1, pFile);
        }
        else
        {
            int bsize , iRetLen;
            char szTempLine[256] ={0} ;

            strcpy( szTempLine , pKeyName ) ;
            strcat( szTempLine , "=" ) ;
            strcat( szTempLine , pString )  ;
            //strcpy_s( szTempLine , 256, pKeyName ) ;
            //strcat_s( szTempLine , 256, TEXT("=") ) ;
            //strcat_s( szTempLine , 256, pString )  ;

            //bsize=WideCharToMultiByte(CP_ACP,0,szTempLine,-1,NULL,0,NULL,NULL);
            bsize = strlen(szTempLine);
            //iRetLen = WideCharToMultiByte(CP_ACP,0,szTempLine,-1,buffer, bsize,NULL,NULL);
            iRetLen = bsize;
            strcpy(buffer, szTempLine);
            buffer[bsize] = 0 ;

            //WriteFile(m_hOutput, buffer, bsize-1 , &dwWrote, NULL);
            //WriteFile(m_hOutput, "\r\n", 2, &dwWrote, NULL);

            fwrite(buffer, bsize, 1, pFile);
            fwrite("\r\n",2,1,pFile);
        }
    }
}





//Set Windows register Key value to auto run GSControler.exe (bIsAutorun = true) or explorer.exe (bIsAutorun = false)
bool SetAutoRun(bool bIsAutorun)
{
        int f_fd;
        f_fd = open("/root/AutoRestart",O_RDWR | O_CREAT, 0666);
        if(f_fd < 0){
            perror("AutoRestart error");
            return false;
        }
        if(bIsAutorun)
            write(f_fd,"1",1);
        else
            write(f_fd,"0",1);
        close(f_fd);
        return true;
}

bool ClrXCIni()
{
    ofstream fout;   
    fout.open("./doc/XC.ini");  
    if(!fout.is_open()){
        return false;
    }
    int Ld = GetPrivateProfileInt(GS_INI_SECTION_MODULE,GS_INI_KEY_LD,-1,GS_INI_HARD_FILE);
    fout<<"["<<GS_INI_SECTION_PEA<<"]"<<"\r"<<endl;
    fout<<GS_INI_KEY_PULCNT<<"="<<"\r"<<endl<<GS_INI_KEY_PULCNT1<<"="<<"\r"<<endl<<GS_INI_KEY_PULCNT2<<"="<<"\r"<<endl;
    fout<<GS_INI_KEY_INTEVAL1<<"="<<"\r"<<endl<<GS_INI_KEY_INTEVAL2<<"="<<"\r"<<endl<<"\r"<<endl;
    for(int i=0; i<Ld; i++){
        fout<<"[P"<<i<<"]"<<"\r"<<endl<<GS_INI_KEY_I<<"="<<"\r"<<endl<<GS_INI_KEY_T<<"="<<"\r"<<endl<<"\r"<<endl;
    }
    fout<<"["<<GS_INI_SECTION_THERM<<"0]"<<"\r"<<endl<<GS_INI_KEY_T<<"="<<"\r"<<endl<<"\r"<<endl;
    fout<<"["<<GS_INI_SECTION_THERM<<"1]"<<"\r"<<endl<<GS_INI_KEY_T<<"="<<"\r"<<endl<<"\r"<<endl;
    fout<<"[Freq50]"<<"\r"<<endl<<"P2ITarget="<<"\r"<<endl<<"\r"<<endl;
    fout<<"[Freq75]"<<"\r"<<endl<<"P2ITarget="<<"\r"<<endl<<"\r"<<endl;
    fout<<"[Freq100]"<<"\r"<<endl<<"P2ITarget="<<"\r"<<endl<<"\r"<<endl;
    fout<<"[Freq200]"<<"\r"<<endl<<"P2ITarget="<<"\r"<<endl<<"\r"<<endl;
    fout.close();
    return true;
}

//// for Event logging
//
void GSLogEvent(std::string EventContext,BYTE byEventType)
{
    //WRITE_LOG(EventContext,byEventType,false);
}

int  GSLogEvent(LPSTR strInfor, BYTE byEventType/* = LOG_EVENT_INFO*/)//available for GsExtendDll use
{
    //return WRITE_LOG(strInfor,byEventType,false);
    return 0;
}

int GSWriteToLog(LPSTR strInfor, BYTE byEventType/* = LOG_EVENT_INFO*/, bool bForce/* = false*/)
{
    //return WRITE_LOG(strInfor,byEventType,bForce);
    return 0;
}


int GetModuleFileName(PVOID handle, char *name, int size)
{
  FILE* stream = fopen("/proc/self/cmdline", "r");
  fgets(name, size, stream);
  fclose(stream);

  return strlen(name);
}
bool PathRemoveFileSpec(char *path)
{
    int i1 = findTheLastCharInStr(path, '\\');
    int i2 = findTheLastCharInStr(path, '/');
    if(i1 >=0 || i2>=0)
    {
        cutStr(path, max(i1,i2));
        return TRUE;
    }
    return FALSE;
}

int findTheLastCharInStr(char* str , char c)
{
    int ret = -1;
    for(int i = 0; i<strlen(str); i++)
    {
        if(str[i] == c)
        {
            ret = i;
        }
    }
    return ret;
}
int cutStr(char *str, char len)
{
    memset(str+len,0,strlen(str) - len);
}

void endFileRead(char* pData, FILE* pfile)
{
    if(pData)
    {
        free(pData);
        pData = NULL;
    }
    if(pfile)
    {
        //CloseHandle(pfile);
        fclose(pfile);
        //fclose(pfile);
        pfile = NULL;
    }
}

int wcsnicmp(LPCSTR s1,LPCSTR s2, int len)
{
    for(int idx = 0; idx <len; idx++)
    {
        if(!(s1[idx]==s2[idx]))
            return s1[idx] - s2[idx];
    }
    return 0;
}
bool CopyFile(LPCSTR lpExistFile, LPSTR lpNewFile,bool b_failIfExist)
{

    if(b_failIfExist == true && access(lpNewFile, 0))
    {
        return false;
    }
    FILE* fpSrc = fopen(lpExistFile, "r");
    FILE* fpDst = fopen(lpNewFile, "w+");
    struct stat statbuf;
    stat(lpExistFile,&statbuf);
    int length = statbuf.st_size;
    char* buf = new char[length];
        /*先读后先的过程*/
    fread(buf,length,1,fpSrc);
    fwrite(buf,length,1,fpDst);
    fclose(fpSrc);
    fclose(fpDst);

}
char* applyData(int size)
{
    char* p = (char*)malloc(sizeof(char)*size);
    return p;
}
int releaseData(char** ppData)
{
    if(*ppData!=NULL)
    {
        free(*ppData);
        *ppData = NULL;
        return 1;
    }
    return 0;

}



void str2upper(string& str)
{
    string::iterator it = str.begin();
    while(it!= str.end())
    {
        if(*it <='z' && *it >= 'a')
        {
            *it += 'A' - 'a';
        }
        it++;
    }
}
void str2upper(char* str)
{
    for(int idx = 0; idx< strlen(str); idx++)
    {
        if(str[idx] <= 'z' && str[idx]>='a')
        {
            str[idx] = str[idx] + ('A'-'a');
        }
    }
}

int CalID(int level, int output)
{
    if(level == 0)
        return 2;
    else
        return level*8+output;
}
