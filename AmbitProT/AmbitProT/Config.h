
#include "stdafx.h"





int AmbitLoadConfig();
bool OpenFileDlg(int itab);
int ConfigFilePnNameParse(char *pfile_name);
int ConfigFileParse(char *pfile_name);
int ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen);
bool ParesePathName( char* pPathName,char* desPathName,int deslen);
bool GetDirFromFileName(char* pFileName,char* pDirName,int iDirNameLen);
int GetsubStrToStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pEndStr);
int GetsubStrFromStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pEndStr);
int GetValidLine(wchar_t *pDesBUFF,wchar_t *pRtnConBUFF, int RtnSize);

int GetsubStrInStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pStartStr,wchar_t *pEndStr);
int GetLocalHostName(char* sHostName);
int GetIpAddress(char *sHostName, char *sIpAddress);

void MyMultiByteToWideChar(char* schars,CString &wstr);
BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);


