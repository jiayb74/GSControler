#ifndef GSAPI
#define GSAPI

#include <string>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "GSDefine.h"
#include "win2linuxtypdef.h"

#define BOOL bool
using namespace std;
//Int to byte array
 void ntob(int nDataIn,BYTE* pByteOut);
//Byte array to int
 int bton(BYTE* pbData);
 double GsString2Double(std::string strValue);

//////////////////////////////////////////////////////////////////////////
// º¯ÊýÉùÃ÷
 int  ReadIniFile(LPCSTR lpFileName, char **ppData);
 DWORD  GetLine(LPSTR pLine, DWORD dwOffset, DWORD dwSize, char **ppData);
 BOOL IsComment(LPCSTR pLine);
 BOOL IsSection(LPCSTR pLine);
 BOOL IsSectionName(LPCSTR pLine, LPCSTR pSection);
 BOOL IsKey(LPCSTR pLine , LPCSTR pKeyName, LPSTR* pValue, DWORD* dwValLen );
 void WriteValue(FILE* pFile , LPCSTR pAppName, LPCSTR pKeyName, LPCSTR pString);
 DWORD  GetString(LPCSTR lpAppName, LPCSTR lpKeyName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName , char **pData);

 BOOL IsPrivateProfileSectionName(LPCSTR lpAppName,
                                            LPCSTR lpFileName );
 BOOL IsPrivateProfileKeyName(LPCSTR pKeyName,
                                            LPCSTR lpAppName,
                                            LPCSTR lpFileName );

 DWORD GetPrivateProfileString(LPCSTR lpAppName,
                                            LPCSTR lpKeyName,
                                            LPCSTR lpDefault,
                                            LPSTR  lpReturnedString,
                                            DWORD   Size,
                                            LPCSTR lpFileName );

 UINT GetPrivateProfileInt(LPCSTR lpAppName,
                                          LPCSTR lpKeyName,
                                          INT nDefault,
                                          LPCSTR lpFileName );

 BOOL WritePrivateProfileString(LPCSTR lpAppName,
                                              LPCSTR lpKeyName,
                                              LPCSTR lpString,
                                              LPCSTR lpFileName);

 BOOL WritePrivateProfileInt(LPCSTR lpAppName,
                                                    LPCSTR lpKeyName,
                                                    INT     Value,
                                                    LPCSTR lpFileName);
 double GetPrivateProfileDouble(LPCSTR lpAppName,
                                            LPCSTR lpKeyName,
                                            double  dDefault,
                                            LPCSTR lpFileName );
 BOOL WritePrivateProfileDouble(LPCSTR lpAppName,
                                            LPCSTR lpKeyName,
                                            double  dDefault,
                                            LPCSTR lpFileName );



 int GetModuleFileName(PVOID Handle, char *name, int size);

 bool PathRemoveFileSpec(char* path);
 int findTheLastCharInStr(char *str , char c);
 int cutStr(char* str, char len);


 void GSLogEvent(std::string EventContext,BYTE byEventType);

 int wcsnicmp(LPCSTR s1, LPCSTR s2, int len);
 void endFileRead(char* pData, FILE* pfile);

 bool CopyFile(LPCSTR lpExistFile, LPSTR lpNewFile,bool b_failIfExist);
 BOOL InitHandle();
 int releaseData(char **ppData);

 bool SetAutoRun(bool bIsAutorun);
 bool ClrXCIni();
 void printdastr();
 void str2upper(string& str);
 void str2upper(char* str);
 int CalID(int level, int output);
#endif // GSAPI

