#pragma once


#include "AmbitLink.h"
#include "AmbitExeCtrl.h"
#include "AmbitParser.h"
#include "math.h"
#include "atlstr.h"

#define ErrorCode_others
#ifdef ErrorCode_others

#include "ErrorCode.h"

#endif

#define sIP "10.0.1.1"
#define ACPWAITTIME 1000


#define	ACPEndianSwap32Macro( VALUE )		\
	( ( ( VALUE >> 24 ) & 0x000000FF ) | 	\
	  ( ( VALUE >>  8 ) & 0x0000FF00 ) | 	\
	  ( ( VALUE <<  8 ) & 0x00FF0000 ) | 	\
	  ( ( VALUE << 24 ) & 0xFF000000 ) )

#define	ACPEndianSwap16Macro( VALUE )  \
	( ( ( VALUE >>  8 ) & 0x00FF ) | 	\
	  ( ( VALUE <<  8 ) & 0xFF00 ) )


typedef struct APTestPara
{
	char APIP[20];
	char SN[20];
	char MAC[20];
}AP_TEST_PARA;

enum OP_MODE
{
	WR,
	W,
	R
};



enum
{
	RA_GET,
	RA_SET,
	RA_NWAIT,
};


// use parse the xml file 
//add by jeffrey 09/21 
int ParseProperyResult(char *SeachStr, char *Rtn, int RtnLenth, char *KeyWord, char *InType);
int GetXmlIndexValue(char *pBuf, int iIndex, int *pVal);
int GetXmlIndexStr(char *pBuf, int iIndex, char Tag, char *pVal, int RtnSize);
int HconverD(char* pHStr,int hstrlength);
int SinglecharConver(char* pch);
unsigned char HEXConvert (unsigned char ucHEX);
int StrHEXConvert(unsigned char *pDes, char *psrc, int iCovertNum);
int RunMultAcp(char* IP,char* pCommand,char* DesBuf,int BufLen,int Run_Type/*1Get 2 Set 3 no wait*/);
int GetsubStrInStr(char*DesBuf,int BufLen,char*SourceBuf,char*pStartStr,char*pEndStr);
int strSTEDcpy(char *pDes, int DesSize, const char *pSrc,const char start, const char end);
int strAllCmp(const char *p1, const char *p2);
bool AddquteToMACAddress(char* newMACAddress,int lenMACAddress,char* pMAC);
void DateFormatC(char *buf, char chr,char aftchar);
int GetCurrentCh(char* Buf,int &nCh2G,int &nCh5G);
int DateTimeFormat(char *date, char* time,char *datetime);

int PullUpPanel1(int port);
int PullUpPanel2(int port);
int PushDownPanel1(int port);
int PushDownPanel2(int port);
int SetResetButton1(int m_time,int port);
int SetResetButton2(int m_time,int port);
int PushFrontBlock1(int port);
int PushFrontBlock2(int port);
int PullBackBlock1(int port);
int PullBackBlock2(int port);

int ParseLEDDetect(int *spec,int led_port,char *result);

int LEDShowPassStatus(void);
int LEDShowFailStatus(void);
int LEDShowTestingStatus(void);
unsigned int _stdcall ThreadForDownArt(LPVOID lpPara);//add by Talen 2011/04/22

int MoveBlock(int ComPort,int TransRate,char *Cmd,char *Timeout,char *Spec);//add by Talen 2011/06/14
int PushPullResetButton(int ComPort,int TransRate,char *Cmd,char *Timeout,char *PauseTime);//add by Talen 2011/06/15
int PushPullResetButtonNoCheck(int ComPort,int TransRate,char *Cmd,char *Timeout,char *PauseTime);//add by Talen 2011/06/16