#include "stdafx.h"
#include "Config.h"
#include <winsock2.h>
#include "SFISDefine.h"
#include "AmParser.h"
//#include "list"
//using namespace std;

//#include "MainControlPanel.h"
extern AM_PROT_INFO gStationInfo;
SFIS_VARIABLE s_variable;
list <SFIS_TRANSMIT_ITEM> ReadList;
list <SFIS_TRANSMIT_ITEM> SendList;


bool OpenFileDlg(int itab)
{

	if(2==itab)
	{
		char tabname[40]="";
		char str[128]="";
		sprintf_s(str,sizeof(str),"%s","sfis.ini");
		ConfigFileParse(str);
		memset(str,0,sizeof(str));
		sprintf_s(str,sizeof(str),"%s","PN_NAME.ini");
		ConfigFilePnNameParse(str);
	}
	return true;
}


int AmbitLoadConfig() 
{
	if(OpenFileDlg(2))
	{
		return true;
	}
	return false;

}
int ConfigFilePnNameParse(char *pfile_name)
{
	wchar_t FileLineBuf[512];
	wchar_t result[512];	

	wchar_t *pcRtnFGet;
	wchar_t delimiters[] =_T("=");
	FILE *fpStream = NULL;
	int err;
	int sNum = 0;	

	memset(result,0,512);
	if( (err = fopen_s( &fpStream,pfile_name, ("r") )) !=0 )
	{
		AfxMessageBox( _T("Can't open PN_NAME.ini!"));
		return FALSE;
	}

	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		CAmParser parse;
		pcRtnFGet = fgetws(FileLineBuf, 512, fpStream);   
		if ((*FileLineBuf==L'#')||(*FileLineBuf==L' '))
		{
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		if(parse.ParserGetPara(FileLineBuf, _T("PRODUCT_NAME"), result, sizeof(result)/2))
		{
			wcscpy_s(s_variable.PN,sizeof(s_variable.PN)/2,result);
			//090618
			wcscpy_s(gStationInfo.uut.UUTPN, sizeof(gStationInfo.uut.UUTPN)/2,result);
			wmemset(result,L'\0',sizeof(result)/2);
		}
	}
	return 1;
}
int ConfigFileParse(char *pfile_name)
{	
	wchar_t FileLineBuf[512];
	wchar_t result[512];
	wchar_t temp_buf[512];

	wchar_t *pcRtnFGet;
	wchar_t delimiters[] =_T("=");
	FILE *fpStream = NULL;
	int err;
	int sNum = 0;	

	wmemset(result,0,512);
	if( (err = fopen_s( &fpStream,pfile_name, ("r") )) !=0 )
	{
		AfxMessageBox( _T("Can't open sfis.ini!"));
		return FALSE;
	}

	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		CAmParser parse;
		pcRtnFGet = fgetws(FileLineBuf, 512, fpStream); 

		if ((*FileLineBuf==L'#')||(*FileLineBuf==L' ')||(*FileLineBuf==L'/'))
		{
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		wmemset(temp_buf, 0, sizeof(temp_buf)/2);		
		GetValidLine(FileLineBuf, temp_buf, sizeof(temp_buf)/2);//we get the string before "//"
		if(parse.ParserGetPara(temp_buf, _T("SFIS->TE"), result, sizeof(result)/2))
		{
			//SN(25)+MAC(12)+WIFIMAC0(12)+WIFIMAC1(12)+PN_NAME(10)+PASS
			s_variable.ReadItemNum=0;
			SFIS_TRANSMIT_ITEM tempList;
			wchar_t strTemp[512]=_T("");
			wchar_t DesBuf[512]=_T("");
			wchar_t strVale[10]=_T("");
			wchar_t *pTemp=NULL;
			wchar_t *podd=NULL;
			int readLength=0;
			wcscpy_s(strTemp,sizeof(strTemp)/2,result);

			while(GetsubStrFromStr(DesBuf,sizeof(DesBuf)/2,strTemp,_T(")")))
			{
				memset(&tempList,0,sizeof(tempList));
				tempList.readStartNum=readLength;
				GetsubStrToStr(tempList.Item,sizeof(tempList.Item)/2,strTemp,_T("("));
				GetsubStrInStr(strVale,sizeof(strVale)/2,DesBuf,_T("("),_T(")"));
				tempList.readLength=_wtoi(strVale);
				s_variable.READ_LENGTH_VALID+=_wtoi(strVale);
				readLength+=_wtoi(strVale);
				tempList.readFlag=1;
				ReadList.push_back(tempList);
				s_variable.ReadItemNum++;
				podd=wcsstr(strTemp,_T("+"));
				if(!podd)
				{
					break;
				}
				podd+=1;		
				wcscpy_s(strTemp,sizeof(strTemp)/2,podd);
			}

		}
		//SN(25)+MAC(12)+WIFIMAC0(12)+WIFIMAC1(12)+PC_NAME(12)
		if(parse.ParserGetPara(temp_buf,_T("TE->SFIS"), result, sizeof(result)/2))
		{
			SFIS_TRANSMIT_ITEM tempList;
			wchar_t strTemp[512]=_T("");
			wchar_t DesBuf[512]=_T("");
			wchar_t strVale[10]=_T("");
			wchar_t *pTemp=NULL;
			wchar_t *podd=NULL;
			int sendLength=0;
			wcscpy_s(strTemp,sizeof(strTemp)/2,result);

			while(GetsubStrFromStr(DesBuf,sizeof(DesBuf)/2,strTemp,_T(")")))
			{
				memset(&tempList,0,sizeof(tempList));
				tempList.sendStartNum=sendLength;
				GetsubStrToStr(tempList.Item,sizeof(tempList.Item)/2,strTemp,_T("("));
				GetsubStrInStr(strVale,sizeof(strVale)/2,DesBuf,_T("("),_T(")"));
				tempList.sendLength=_wtoi(strVale);
				s_variable.SEND_LENGTH_VALID+=_wtoi(strVale);
				sendLength+=_wtoi(strVale);
				tempList.sendFlag=1;
				SendList.push_back(tempList);
				podd=wcsstr(strTemp,_T("+"));
				if(!podd)
				{
					break;
				}
				podd+=1;		
				wcscpy_s(strTemp,sizeof(strTemp)/2,podd);
			}			
		}
		if(parse.ParserGetPara(temp_buf,_T("BAR_CODE"), result, sizeof(result)/2))
		{
        
			wcscpy_s(s_variable.BarCode, sizeof(s_variable.BarCode)/2, result);
		
		}
		if (parse.ParserGetPara(temp_buf, _T("MODE_OPTION"), result, sizeof(result)/2))
		{         
			s_variable.ModeOptionFlag=_wtoi(result);
		}
		if (parse.ParserGetPara(temp_buf, _T("VIRT_SMO_FLAG"), result, sizeof(result)/2))
		{         
			s_variable.VirtSmoFlag=_wtoi(result);
		}
	}		
	fclose(fpStream); 
	return TRUE;
}

int ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen)
{

	char TagBUFF[128] = "";
	char AnalyseBUFF[512] = "";
	char *pToken = NULL;
	char *pConStart = NULL;
	char *PConEnd = NULL;

	if((NULL==pDesBUFF)||(NULL==pTagBuff)||(NULL==pRtnConBUFF))
	{
		return 0;
	}
	if ((RtnConBUFFLen <= 0) || (RtnConBUFFLen > 512))
	{
		return 0;
	}
	strcpy_s(AnalyseBUFF, sizeof(AnalyseBUFF), pDesBUFF);
	strcpy_s(TagBUFF, sizeof(TagBUFF), pTagBuff);

	pToken = strstr(AnalyseBUFF, TagBUFF); 
	if (pToken==NULL) 
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}
	pToken = strstr(pToken, "="); 
	if (pToken)
	{
		pConStart = pToken;
		pConStart += 1;  
	}
	else
	{
		return 0;
	}
	pToken = strstr(pToken, ";"); 
	if (pToken)
	{
		PConEnd = pToken;
		*PConEnd = NULL;
	}
	else
	{
		return 0;
	}
	if (pConStart && PConEnd)
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, pConStart);
	}	
	return 1;

}

bool ParesePathName( char* pPathName,char* desPathName,int deslen)
{
	if((NULL==pPathName)||(NULL==desPathName))
	{
		return false;
	}
	char path[512]="";
	char* pstart=NULL;
	char* pend=NULL;
	pstart=pPathName;
	pend=strstr(pstart,"\\");	
	while(pend)
	{
		strncat_s(desPathName,deslen,pstart,pend-pstart);
		strcat_s(desPathName,deslen,"\\\\");
		pstart=pend+1;		
		pend=strstr(pstart,"\\");	
	}
	strcat_s(desPathName,deslen,pstart);
	return true;
}
bool GetDirFromFileName(char* pFileName,char* pDirName,int iDirNameLen)
{
	if(NULL==pFileName||NULL==pDirName)
	{
		return false;
	}
	char *pstart=NULL;
	char *pend=NULL;
	pstart=pFileName;	
	pend=strstr(pstart,"\\");
	if(!pend)
	{
		return false;
	}
	while(pend)
	{

		pstart=pend;
		pend=strstr(pstart+1,"\\");
	}
	strncpy_s(pDirName,iDirNameLen,pFileName,pstart-pFileName);

	return true;

}

void MyMultiByteToWideChar(char* schars,CString &wstr) //////////////char to CString
{
	// TODO: Add your specialized code here and/or call the base class
	int widecharlen=MultiByteToWideChar( //计算从Ansi转换到Unicode后需要的字节数
		CP_ACP,
		MB_COMPOSITE,
		(char*)schars, //要转换的Ansi字符串
		-1, //自动计算长度
		0,
		0
		);
	CString tmpstr;
	LPWSTR p=tmpstr.GetBuffer(widecharlen); //为转换后保存Unicode字符串分配内存
	MultiByteToWideChar( //从Ansi转换到Unicode字符
		CP_ACP,
		MB_COMPOSITE,
		(char*)schars,
		-1,
		p, //转换到tmpstr
		widecharlen //最多转换widecharlen个Unicode字符
		);
	tmpstr.ReleaseBuffer();
	wstr = tmpstr;

}

BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)////////////Wchar to char
  {
   DWORD dwMinSize;
   dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
   if(dwSize < dwMinSize)
   {
    return FALSE;
   }
   WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
   return TRUE;
}


int GetsubStrFromStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pEndStr)
{
	wchar_t *pStart=NULL;
	wchar_t *pEnd=NULL;
	if ((!DesBuf)||(!SourceBuf))
	{
		return 0;
	}
	pEndStr=wcsstr(SourceBuf,pEndStr);
	if(!pEndStr)
	{
        return 0;
	}
	if((pEndStr-SourceBuf)>=BufLen)
	{
		return 0;
	}
	wcsncpy_s(DesBuf,BufLen,SourceBuf,pEndStr-SourceBuf+1);
	return 1;	
}

int GetsubStrToStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pEndStr)
{
	wchar_t *pStart=NULL;
	wchar_t *pEnd=NULL;
	if ((!DesBuf)||(!SourceBuf))
	{
		return 0;
	}
	pEndStr=wcsstr(SourceBuf,pEndStr);
	if(!pEndStr)
	{
        return 0;
	}
	if((pEndStr-SourceBuf)>=BufLen)
	{
		return 0;
	}
	wcsncpy_s(DesBuf,BufLen,SourceBuf,pEndStr-SourceBuf);
	return 1;	
}


int GetValidLine(wchar_t *pDesBUFF,wchar_t *pRtnConBUFF, int RtnSize)
{
	wchar_t TagBuf[512]=_T("");
    wchar_t AnalyseBUFF[512]=_T("");
	
	wchar_t *pTok=NULL;
	if ((!pDesBUFF)||(!pRtnConBUFF))
	{
		return 0;
	}
	wcscpy_s(AnalyseBUFF,sizeof(AnalyseBUFF)/2, pDesBUFF); 
	pTok = wcsstr(AnalyseBUFF,_T("//"));
	if (pTok)
	{
		*pTok=0;
	}

	wcscpy_s(pRtnConBUFF, RtnSize, AnalyseBUFF);
	return 1;
}


int GetsubStrInStr(wchar_t *DesBuf,int BufLen,wchar_t *SourceBuf,wchar_t *pStartStr,wchar_t *pEndStr)
{
	wchar_t *pStart=NULL;
	wchar_t *pEnd=NULL;
	if ((!DesBuf)||(!SourceBuf))
	{
		return 0;
	}
	pStart=wcsstr(SourceBuf,pStartStr);
	if(!pStart)
	{
		return 0;
	}

	pEndStr=wcsstr(pStart+wcslen(pStartStr),pEndStr);
	if(!pStart||!pEndStr)
	{
        return 0;
	}
	if((pEndStr-pStart-wcslen(pStartStr))>=(unsigned)BufLen)
	{
		return 0;
	}
	wcsncpy_s(DesBuf,BufLen,pStart+wcslen(pStartStr),pEndStr-pStart-wcslen(pStartStr));
	return 1;	
}

int GetLocalHostName(char* sHostName)	
{
	WSADATA				wsd;
	char				szString[255];
	if (WSAStartup(0x202,&wsd) != 0)
	{
		sprintf_s(szString,"Failed to load Winsock!");
		return 1;
	}
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//产生错误
		strcpy_s(sHostName,sizeof("没有取得"),"没有取得");
		//strcpy(sHostName,"没有取得");
		return GetLastError();
	}
	strcpy_s(sHostName,strlen(szHostName)+1,szHostName);
	WSACleanup();
	return 0;
}

int GetIpAddress(char *sHostName, char *sIpAddress)
{
	WSADATA				wsd;
	char				szString[255];
	if (WSAStartup(0x202,&wsd) != 0)
	{
		sprintf_s(szString,"Failed to load Winsock!");
		return 1;
	}
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName);
	if(lpHostEnt==NULL)
	{
		//产生错误
		sIpAddress=NULL;
		return GetLastError();
	}
	//获取IP
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		//转换为标准格式
		strcpy_s(sIpAddress,strlen(inet_ntoa(inAddr))+1,inet_ntoa(inAddr));
		//strcpy(sIpAddress,inet_ntoa(inAddr));
	}
	WSACleanup();
	return 0;
}