/********************************************************************
created:	2008/08/08
filename: 	..\TestPROPerf\TestPROPerf\TestItemScript.cpp
file path:	..\TestPROPerf\TestPROPerf
file base:	TestPROPerf
file ext:	cpp
author:		Jeffrey

purpose:	do the per item act test in process function definition

*********************************************************************/
#include "stdafx.h"
#include "TestItemScript.h"
#include "ScriptSupport.h"
#include "CenTime.h"
#include "DevOP.h"
#include "com_class.h" //627
#include "TMMResultReport.h"
#include "CommSocket.h"
#include "AGoldenSocket.h"



extern int fix_id;
extern CTMMResultReport CTReport;
extern CCommSocketClient DeviceAT100Client;

extern int AutoFixResponseFlag;


_asyn_com gComDev;//627
extern TEST_INPUT_INFO gTI;//test input information
extern list<TEST_ITEM> TItemList;//Add by maxwell 2008/10/11
extern int ParseTestItem(const char *p1, const char *p2);//Add by maxwell 2008/10/11
//Maxwell 100504
extern HANDLE hWriteLabelMutex;
extern HANDLE hOperateComMutex;//Talen 2011/09/01
extern HANDLE hOneTestingMutex;//Talen 2011/09/01
//extern int RebootFlag;//Maxwell 091005
char HDD_SN[80]="";    //Add by Tony on 091217 for send HDSN
char WIFI_MAC0[80]=""; //Add by Tony on 091223 for send WifiMac0 to SFIS in Screen
char WIFI_MAC1[80]=""; //Add by Tony on 091223 for send WifiMac1 to SFIS in Screen
char WANMAC[80]="";    //Add by Tony on 091225 for send EthernetMAC to SFIS in Screen

extern SFISSW SFIS; //Maxwell 20110326
extern GOLD_CLIENT_TYPE GoldenServer;//Maxwell 20110329
extern GOLD_CLIENT_TYPE GoldenServer1;//Talen 2011/06/27

int FileMoveFlag=1; //If move the file; //JUSTIN
extern bool IsGolden;
//extern CControlLogic CLogic;//Talen 2011/07/15

char currentSNIQlog[512]="";	//add by Talen 201105/31
//#define MULTI_ACP_MODE 

CCenTime localTempTime;//add by Talen 2012/02/20

UINT WINAPI ParaFunc(PVOID pParam)
{

	PARALLEL_PARA *pPA=(PARALLEL_PARA *)pParam;
	(*(pPA->pFunc))(pPA->pTI);

	return 1;
}



int CTestItemScript::BeginParaRun(ParallelPerfCallFunc pFunc,TEST_ITEM *pTI)
{
	HANDLE hThread=0;
	UINT dwThreadID=0;
	PARALLEL_PARA PPaa;
	PPaa.pFunc=pFunc;
    PPaa.pTI=pTI;
	hThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		ParaFunc,
		&PPaa,
		0,
		&dwThreadID 
		);

	if (hThread)
	{

	}
	else
	{
		return 0;
	}
	return 1;
}




CTestItemScript::CTestItemScript(void)
{
	//Initial parameter
	GPubBUF.Flag=0;
	memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
	SetHDSpinDownTime=0;//Initial the HD spin down timer
	SetHDSpindownFlag=0;//not set spin down HD
	SetHDSpindownFailFlag=0;//Set spin down HD function fail

}

CTestItemScript::~CTestItemScript(void)
{



}


int CTestItemScript::WaitUUTUp(TEST_ITEM *pTI)
{
	CCenTime BonjourTime;
	BonjourTime.TimeStartCount();
	char BonjourTimeBuf[64]="";

	strcpy_s(pTI->Result,sizeof(pTI->Result),"Fail");
	char OutBuf[2048]="";
	char *pToke = NULL;
	char Temp[2048]="";
	int Result=1;
	int cnt=0;
	int TimeOutSet=atoi(pTI->DiagPerfTime);
	cnt=atoi(pTI->UpLimit);
	amprintf("Wait %d seconds before bonjour\n",cnt);
	Sleep(1000*cnt);
	if (!BonjourLink(pTI->DiagCmd, TimeOutSet))
	{
		amprintf("ERROR! Can not scan UUT!;\n");
		return 1;	//change return 0 to return 1 by Talen 2011/08/29
	}
	
	BonjourTime.GetElapseTime(BonjourTimeBuf,sizeof(BonjourTimeBuf));
	strcpy_s(pTI->Result,sizeof(pTI->Result),BonjourTimeBuf);//Talen 2012/03/12

	cnt=atoi(pTI->DownLimit);
	amprintf("Wait %d seconds after bonjour\n;",cnt);
	Sleep(1000*cnt);

	return 1;
}
#ifdef MULTI_ACP_MODE
int CTestItemScript::GetSysDes()
{

	if (!GPubBUF.Flag)
	{
		if (!RunMultAcp(sIP,"syVs sySI USBi PHYS ssSK diag waCV",GPubBUF.PubBUF,sizeof(GPubBUF.PubBUF),RA_GET))
		{
			return 0;		
		}
		amprintf("RUN multi ACP syVs sySI USBi PHYS ssSK diag waCV\n;");
	}
    GPubBUF.Flag=8;//8 item to parse
	return 1;
}
#else

int CTestItemScript::GetSysDes()
{

	if (!GPubBUF.Flag)
	{
		if (!RunMultAcp(sIP,"PHYS",GPubBUF.PubBUF,sizeof(GPubBUF.PubBUF),RA_GET))
		{
			return 0;		
		}
		amprintf("PHYS\n;");
		GPubBUF.Flag=4;//4 item to parse
	}
    //GPubBUF.Flag=4;//4 item to parse
	return 1;
}

#endif


#ifdef MULTI_ACP_MODE
int CTestItemScript::CheckUUTFW(TEST_ITEM *pTI)
{

	char Temp[2048]="";
	int Result=1;

	GPubBUF.Flag=0;
    if (!GetSysDes())
	{
		return 0;
	}

    if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		//Check Version
		Result=GetsubStrInStr(Temp,sizeof(Temp),GPubBUF.PubBUF,"syVs: ","\r\n");
		if (Result)
		{
			GPubBUF.Flag--;
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT Firmware version: %s;\n", pTI->Result);
			amprintf("UUT FM in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;

}
#else
int CTestItemScript::CheckUUTFW(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char Temp[2048]="";
	int Result=1;
	DWORD dwExitCode;
	//get the system firmware version card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check Version
		amprintf("OutBuf:%s;\n",OutBuf);
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"syVs: ","\r\n");
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT Firmware version: %s;\n", pTI->Result);
			amprintf("UUT FM in Config File: %s;\n", pTI->Spec);
			amprintf("UUT Other FM in Config File: %s;\n", pTI->Spec_1);
			if ((!strAllCmp(pTI->Result,pTI->Spec))||(!strAllCmp(pTI->Result,pTI->Spec_1)))//Modified by Maxwell 2008/12/17 for double FW
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}

#endif

//Add for check UUT GM and PVT FW
int CTestItemScript::CheckUUTFWGM(TEST_ITEM *pTI)
{

	char OutBuf[2048]="";
	char Temp[2048]="";
	int Result=1;
	DWORD dwExitCode;
	//get the system firmware version card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check Version
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"syBL: ","\r\n");
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT Firmware version by syBL: %s;\n", pTI->Result);
			amprintf("UUT FM syBL in Config File: %s;\n", pTI->Spec);
			amprintf("UUT Other FM in Config File: %s;\n", pTI->Spec_1);
			if ((!strAllCmp(pTI->Result,pTI->Spec))||(!strAllCmp(pTI->Result,pTI->Spec_1)))//Modified by Maxwell 2008/12/17 for double FW
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
//Add for check UUT GM and PVT FW

//Check Rework SYS Behavior
int CTestItemScript::CheckSYSReboot(TEST_ITEM *pTI)
{
	HANDLE hRemote=NULL;
	char AnaTemp[2048]="";
	char *pToke = NULL;
	char TempBuf[2048]="";
	int TXTh=0;
	int RXTh=0;
    DWORD dwWaitResult;
	
	char OutBuf[100];
	DWORD dwExitCode;

	//amprintf("MSG;TESTITEM=%s?\n;",pTI->Name);	//Maxwell 090520
	amprintf("MSGBOX=CONTENT[Please check the system LED];\n");
	// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
	dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
	if (!ResetEvent(gTI.UiResMsg.hEvent)) 
	{ 
		printf("ResetEvent failed (%d)\n", GetLastError());
	} 
	//strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);
	//if (atoi(pTI->Spec)!=atoi(gTI.UiResMsg.MsgRes))//test fail
	//{		
	//	return 0;
	//}

	if (strlen(pTI->UUTCmd))
	{
		if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Set the acRB= command fail:%s;\n",pTI->UUTCmd);	
			return 0;
		}
	}

	Sleep(1000*atoi(pTI->CmdPerfTime));
	amprintf("MSGBOX=CONTENT[Is the behavior of the system LED all right?];\n");
	// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
	dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
	if (!ResetEvent(gTI.UiResMsg.hEvent)) 
	{ 
		printf("ResetEvent failed (%d)\n", GetLastError());
	} 
	strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);
	if (atoi(pTI->Spec)!=atoi(gTI.UiResMsg.MsgRes))//test fail
	{		
		return 0;
	}
	return 1;
}

int CTestItemScript::CheckUutLed(TEST_ITEM *pTI)
{
	HANDLE hRemote=NULL;
	char AnaTemp[2048]="";
	char *pToke = NULL;
	char TempBuf[2048]="";
	int TXTh=0;
	int RXTh=0;
    DWORD dwWaitResult;
	
	char OutBuf[100];
	DWORD dwExitCode;
	if (strlen(pTI->UUTCmd))
	{
		if (!RunExeFileNoOutput(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Set UUT LED fail CMD:%s;\n",pTI->UUTCmd);	
			return 0;
		}
	}
    amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);//add by haibin,li 07.27
	if (!strlen(pTI->InsCmd))//test led by manual
	{
		//amprintf("MSG;TESTITEM=%s?\n;",pTI->Name);	//Maxwell 090520
		amprintf("MSGBOX=LED[%s?];\n",pTI->Name);
		// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);
		if (atoi(pTI->Spec)!=atoi(gTI.UiResMsg.MsgRes))//test fail
		{		
			return 0;
		}

	}
	else//tset led by automatic
	{

		int led_spec[8];
		char result[512]="";
		int ledport;
		if(fix_id==1)
		{
			ledport=pTI->LED_PORT_1;
		}
		else if(fix_id==2)
		{
			ledport=pTI->LED_PORT_2;
		}
		else
		{
			ledport=pTI->LED_PORT_1;
		}



		int I_MIN= pTI->RGBI_I_SPEC_DOWN;
		int I_MAX= pTI->RGBI_I_SPEC_UP;
		int R_MIN= pTI->RGBI_R_SPEC_DOWN;
		int R_MAX= pTI->RGBI_R_SPEC_UP;
		int G_MIN= pTI->RGBI_G_SPEC_DOWN;
		int G_MAX= pTI->RGBI_G_SPEC_UP;
		int B_MIN= pTI->RGBI_B_SPEC_DOWN;
		int B_MAX= pTI->RGBI_B_SPEC_UP;

		led_spec[0]=I_MIN;
		led_spec[1]=I_MAX;
		led_spec[2]=R_MIN;
		led_spec[3]=R_MAX;
		led_spec[4]=G_MIN;
		led_spec[5]=G_MAX;
		led_spec[6]=B_MIN;
		led_spec[7]=B_MAX;

		//Judge Led result,retry 5 times
		for(int i=0;i<5;i++)
		{
			if(ParseLEDDetect(led_spec,ledport,result))
			{
				strcpy_s(pTI->Result,sizeof(pTI->Result),result);
				return 1;
			}
			//if(ParseLEDDetect(pAT100DevName,led_spec,ledport))
			//	return 1;
			//modified by sam 2011/04/01
			if(i==4)
			{
				strcpy_s(pTI->Result,sizeof(pTI->Result),result);
				return 0;
			}
			else
			{
				Sleep(1000);
				continue;
			}
		}
		/*
		Sleep(100*atoi(pTI->CmdPerfTime));//Add by Maxwell 2008/12/09 for sleep when test LEDs
		if (!OpenDev(&hRemote,pTI->InsName,QUERY_MODE,pTI->InsCtrlID,12638))
		{
			amprintf("Run remote throughput test tool fail;\n");
			CloseDev(hRemote);
			return 0;
		}
		amprintf("cmd:%s;\n",pTI->InsCmd);
		if (!DevQuery(hRemote,pTI->InsCmd,Result,sizeof(Result)))
		{
			amprintf("Run remote throughput test tool fail;\n");
			amprintf("%s;\n",Result);
			CloseDev(hRemote);
			return 0;
		}
		CloseDev(hRemote);	
		strcpy_s(pTI->Result,sizeof(pTI->Result),Result);
		if (strAllCmp(pTI->Spec,Result))
		{
			return 0;
		}
		*/
	
	}
	return 1;
}


int CTestItemScript::SetUUTGmtTime(TEST_ITEM *pTI)
{

	char OutBuf[2048] = "";
	char CmdBuf[2048] = "";
	char AnaTemp[2048] = "";
	char ProTemp[100] = "";

	DWORD dwExitCode;
	char *pToke = NULL;

	struct tm gmt;
	//   char tmpbuf[128];
	char timebuf[26];
	time_t ltime;
	time_t Rtnltime=0;
	time_t Cmptime=0;
	unsigned int uiTimeTempbef=0;
	unsigned int uiTimeTempaft=0;

	errno_t err;

	// Set time zone from TZ environment variable. If TZ is not set,
	// the operating system is queried to obtain the default value 
	// for the variable. 
	//
	_tzset();

	// Get UNIX-style time and display as number and string. 
	time( &ltime );
	//amprintf( "Time in seconds since UTC 1/1/70:  %.2X;\n", ltime );
	err = ctime_s(timebuf, 26, &ltime);
	if (err)
	{
		//printf("ctime_s failed due to an invalid argument.");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		return 0;
	}   
	// Display UTC. 
	err = _gmtime64_s( &gmt, &ltime );
	if (err)
	{
		//amprintf("_gmtime64_s failed due to an invalid argument.;");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		return 0;
	}
	err = asctime_s(timebuf, 26, &gmt);
	if (err)
	{
		//amprintf("asctime_s failed due to an invalid argument.;");
		///sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		return 0;
	}
	//amprintf( "GMT Time and Data:  %s;", timebuf );
    pToke = NULL;
	pToke=strchr(timebuf,0x0a);
	if (pToke)
	{
		*pToke=0;
	}
	strcpy_s(pTI->Result,sizeof(pTI->Result),timebuf);

	strcpy_s(CmdBuf, sizeof(CmdBuf),pTI->UUTCmd);
	sprintf_s(ProTemp, sizeof(ProTemp), "0x%.2X", ltime);	
	strcat_s(CmdBuf, sizeof(CmdBuf), ProTemp);	
	amprintf("Write  UUT Time %s.;\n",CmdBuf);
	if (!RunExeFileNoOutput(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Write UUT Time   Fail;\n");
		return 0;
	}

	return 1;
}
int CTestItemScript::CheckUUTGmtTime(TEST_ITEM *pTI)
{
	char OutBuf[2048] = "";
	char CmdBuf[2048] = "";
	char AnaTemp[2048] = "";
	char ProTemp[100] = "";

	DWORD dwExitCode;
	char *pToke = NULL;

	struct tm gmt;
	//   char tmpbuf[128];
	char timebuf[26];
	char UUTTimeBuf[56];
	time_t ltime;
	time_t Rtnltime=0;
	time_t Cmptime=0;
	unsigned int uiTimeTempbef=0;
	unsigned int uiTimeTempaft=0;
	strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "");
	strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes),"");
	errno_t err;

	

	// Set time zone from TZ environment variable. If TZ is not set,
	// the operating system is queried to obtain the default value 
	// for the variable. 
	//
	_tzset();

	// Get UNIX-style time and display as number and string. 
	time( &ltime );
	//amprintf( "Time in seconds since UTC 1/1/70:  %.2X;\n", ltime );
	err = ctime_s(timebuf, 26, &ltime);
	if (err)
	{
		//printf("ctime_s failed due to an invalid argument.");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),"RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}   
	// Display UTC. 
	err = _gmtime64_s( &gmt, &ltime );
	if (err)
	{
		//amprintf("_gmtime64_s failed due to an invalid argument.;");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}
	err = asctime_s(timebuf, 26, &gmt);
	if (err)
	{
		//amprintf("asctime_s failed due to an invalid argument.;");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;

	}
	//amprintf( "GMT Time and Data:  %s;", timebuf );



	memset(CmdBuf, 0, sizeof(CmdBuf));
	//amprintf("Get UUT Time......;\n");
	//strcpy_s(CmdBuf, sizeof(CmdBuf),pTI->);
	strcpy_s(CmdBuf, sizeof(CmdBuf),pTI->UUTCmd);
	if (!RunExeFileLocal(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get UUT Time   Fail;\n");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}

	pToke=NULL;
	pToke = strstr(OutBuf, "error");
	if (pToke)
	{
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT00);
		amprintf("%s;", OutBuf);
		amprintf("Get AP time Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}
	pToke=NULL;
	pToke = strstr(OutBuf, "time:");
	if (pToke)
	{
		strSTEDcpy(AnaTemp,sizeof(AnaTemp),pToke,'x',13);
		amprintf("AP Time: %s;\n", AnaTemp);
	}
	else
	{
		amprintf("Get AP Time error:%s;\n", AnaTemp);	
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	 //   strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}
	StrHEXConvert((unsigned char *)&uiTimeTempbef, AnaTemp, 8);					

	uiTimeTempaft = ACPEndianSwap32Macro( uiTimeTempbef );				
	Rtnltime = uiTimeTempaft;
	StrHEXConvert((unsigned char *)&uiTimeTempaft, AnaTemp, 8);					
	//amprintf("UUT Time = %X\n",Rtnltime);
	Cmptime = Rtnltime - ltime;//compare the time,

	ctime_s(UUTTimeBuf, 26, &Rtnltime);
	err = _gmtime64_s( &gmt, &Rtnltime);
	err = asctime_s(UUTTimeBuf, 26, &gmt);

	//Add start by Maxwell 2008/12/09 for not /n in test result
	char *pToke2 = NULL;
	pToke2=strstr(UUTTimeBuf,"\n");
	if(pToke2)
	{
		*pToke2=NULL;
	}
	//Add end by Maxwell 2008/12/09 for not /n in test result
	strcpy_s(pTI->Result,sizeof(pTI->Result),UUTTimeBuf);
	
	amprintf( "COM Time: %s;\n", timebuf);
	amprintf( "UUT  Time: %s;\n", UUTTimeBuf);
	amprintf( "different time: %d;\n", Cmptime);
	//amprintf("Computer time: %04d%02d%02d%02d%02d%02d;",gmt.tm_wday,gmt.tm_year,gmt.tm_mon,gmt.tm_mday,gmt.tm_hour,gmt.tm_min);
	//if ((Cmptime<-10)&&(Cmptime>-15180))
	if ((Cmptime<-10)&&(Cmptime>-75))
	{
		amprintf("UUT Time Test   Fail;\n");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT00);
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT01");//add error code for separate RT01<75,RT0>75 , by blucey20100806
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 10 SEC");	

		return 0;
	}
	if (Cmptime<-15180)
	{
		amprintf("UUT Date Test   Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT00");
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 75 SEC");	
		MessageBox(NULL,"RT00,the difference is more than 75 SEC,Please take to repair\nRT00,時間差異大於75秒,請轉修復.","RT00",MB_OK);
		HWND ErrBox = ::FindWindow(NULL,"RT00");
		SetWindowPos(ErrBox,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 

		return 0;
	}
	//if ((Cmptime>10)&&(Cmptime<15180))
	if ((Cmptime>10)&&(Cmptime<75))
	{
		amprintf("UUT Time Test   Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT01");
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 10 SEC");	

		return 0;
	}
	if(Cmptime>15180)
	{
		amprintf("UUT Date Test   Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT00");
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 75 SEC");	
		MessageBox(NULL,"RT00,the difference is more than 75 SEC,Please take to repair\nRT00,時間差異大於75秒,請轉修復.","RT00",MB_OK);
		HWND ErrBox = ::FindWindow(NULL,"RT00");
		SetWindowPos(ErrBox,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 

		return 0;
	}

	//add start20100805 by blucey for cover actual battery fail
		if(Cmptime>75)
	{
		amprintf("UUT Date Test   Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT00");
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 75 SEC");	
		MessageBox(NULL,"RT00,the difference is more than 75 SEC,Please take to repair\nRT00,時間差異大於75秒,請轉修復.","RT00",MB_OK);
		HWND ErrBox = ::FindWindow(NULL,"RT00");
		SetWindowPos(ErrBox,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 

		return 0;
	}
	//end
		if (Cmptime<-75)
	{
		amprintf("UUT Date Test   Fail;\n");
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT00");
	    strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "The difference is more than 75 SEC");	
		MessageBox(NULL,"RT00,the difference is more than 75 SEC,Please take to repair\nRT00,時間差異大於75秒,請轉修復.","RT00",MB_OK);
		HWND ErrBox = ::FindWindow(NULL,"RT00");
		SetWindowPos(ErrBox,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE); 

		return 0;
	}

	err = ctime_s(timebuf, 26, &Rtnltime);
	if (err)
	{
		//amprintf("ctime_s failed due to an invalid argument.;");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
		//strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode), "RT02");
	    //strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes), "Read PC Local Time Error.");
		return 0;
	}

	return 1;
}

int CTestItemScript::CheckUUTResetButton(TEST_ITEM *pTI)
{
	char Temp[500]="";
	char OutBuf[500]="";
	int Result=1;
	char *pToke=NULL;
	DWORD dwExitCode;
	char LLink[20]="";
	int iLink=0;
    DWORD dwWaitResult=0;
	
	if (!strlen(pTI->InsCmd))//test reset button by manual
	{
		
		//amprintf("MSG;TESTITEM=Please press reset button within 10 seconds.\n;");	//Maxwell 090520
		amprintf("MSGBOX=CONTENT[Please press reset button within 10 seconds.];\n");
		
		// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Get reset button information fail;\n");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
			return 0;
		}
	}
	else//test reset button by automatic
	{
		amprintf("Begin to run cmd: %s;\n",pTI->UUTCmd);
		if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Get reset button information fail;\n");
		//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
			return 0;
		}

		



	}
		
	//if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	//{
	//	amprintf("Get reset button information fail;\n");
	//	//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",RT02);
	//	return 0;
	//}

	pToke=NULL;
	pToke = strstr(OutBuf, "dirf:");
	if (!pToke)
	{
		amprintf("Get reset button information fail;\n");
		return 0;
	}
	Result=GetsubStrInStr(Temp,sizeof(Temp),pToke,"+00000000: ","|");
	if (Result)
	{
		GetXmlIndexStr(Temp, 1, ' ', LLink, sizeof(LLink));
		iLink=atoi(LLink);
		strcpy_s(pTI->Result,sizeof(pTI->Result),LLink);
		amprintf("Get reset button information %s;\n", pTI->Result);
		amprintf("Reset button information spec %s;\n", pTI->Spec);
		if (strAllCmp(pTI->Spec,pTI->Result))
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

//add start by Talen 2011/06/14
int CTestItemScript::ClickUUTResetButton(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);

	strcpy_s(pTI->Result,sizeof(pTI->Result),"Fail");
	Sleep(atoi(pTI->Spec));

	//////////////////////////////////////////////////////////

	if(fix_id==1)
	{
		if(0==PushPullResetButton(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{
			amprintf("Control Block Fail!\n");
			return 1;//change 0 to 1 by Talen 2011/09/01
		}
	}
	else if(fix_id==2)
	{
		if(0==PushPullResetButton(pTI->ComPort,pTI->TransRate,pTI->DiagCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{
			amprintf("Control Block Fail!\n");
			return 1;//change 0 to 1 by Talen 2011/09/01
		}
	}
	else
	{

		if(0==PushPullResetButton(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{	
			amprintf("Control Block Fail!\n");
			return 1;//change 0 to 1 by Talen 2011/09/01
		}
	}
	strcpy_s(pTI->Result,sizeof(pTI->Result),"Pass");
	return 1;
}
//add end by Talen 2011/06/14


//add start by Talen 2011/06/16
int CTestItemScript::ClickUUTResetButtonNoCheck(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	Sleep(atoi(pTI->Spec));
	//////////////////////////////////////////////////////////
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");

	if(fix_id==1)
	{
		if(0==PushPullResetButtonNoCheck(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{
			amprintf("Control Block Fail!\n");
			return 0;
		}
	}
	else if(fix_id==2)
	{
		if(0==PushPullResetButtonNoCheck(pTI->ComPort,pTI->TransRate,pTI->DiagCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{
			amprintf("Control Block Fail!\n");
			return 0;
		}
	}
	else
	{
		if(0==PushPullResetButtonNoCheck(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->CmdPerfTime))
		{
			amprintf("Control Block Fail!\n");
			return 0;
		}
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
//add end by Talen 2011/06/16

#ifdef MULTI_ACP_MODE
int CTestItemScript::CheckUUTUsb(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	char AnaTemp[500]="";
	char *pToke=NULL;//Add by Maxwell 2008/11/18 for USB
	int Result=1;

	if (!GetSysDes())
	{
		return 0;
	}

	if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		pToke=strstr(GPubBUF.PubBUF,"USBi");//Add by Maxwell 2008/11/18 for USB
		//Add start by Maxwell 2008/11/18 for USB
		if(pToke)
		{
			if (!ParseProperyResult(pToke, AnaTemp, sizeof(AnaTemp), "product", "<string>"))
			{
				amprintf("Parse the USB product information wrong.;\n");	
				return 0;
			}
			GetXmlIndexStr(AnaTemp, 1, ',', pTI->Result,sizeof(pTI->Result));
			if (!ParseProperyResult(pToke, AnaTemp, sizeof(AnaTemp), "speed", "<string>"))
			{
				amprintf("Parse the USB product information wrong.;\n");	
				return 0;
			}
			GetXmlIndexStr(AnaTemp, 1, ',', Temp, sizeof(Temp));	
			amprintf("Detect the USB device:%s;\n", pTI->Result);
			amprintf("Expect the USB device:%s;\n", pTI->Spec);
			amprintf("Detect the USB device speed:%s;\n", Temp);
			amprintf("Expect the USB device speed:%s;\n", pTI->UpLimit);

			if (strAllCmp(pTI->Result, pTI->Spec))
			{
				return 0;
			}
			if (strAllCmp(pTI->UpLimit, Temp))
			{
				return 0;
			}
		}
	}
	return 1;
}

#else
int CTestItemScript::CheckUUTUsb(TEST_ITEM *pTI)
{

	char AnaTemp[500]="";
	char Temp[500]="";
	char OutBuf[2048]="";
	DWORD dwExitCode;
	DWORD dwWaitResult;
	char Temp2[128]="";

	if(!strlen(pTI->InsCmd))// InsCmd =1 is test by fixture else by manual
	{
		//0628
		//get the system firmware version card MAC ID	
		if(!pTI->RetryItemCount)
		{
			amprintf("MSGBOX=CONTENT[%s];\n","Please plug in the USB Device!");
		}
		else
		{
			amprintf("MSGBOX=CONTENT[%s];\n","Please remove the USB Device and\n plug in it again!");
		}
		// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		//0628

		Sleep(3000);
	}
	else
	{
	//add start by Talen 2011/09/03
		if(0==pTI->RetryItemCount)
		{

		}
		else if(1==pTI->RetryItemCount)
		{
//			gTI.UUTInfo.retryItemCount++;//Talen 2012/03/15
		}
		else if(1<pTI->RetryItemCount)
		{
			char cmd1[32]="";
			char cmd2[32]="";
			char spec1[32]="";
			char spec2[32]="";
			if(strstr(gTI.TestStaInfo.TestStaName,"MPT1"))
			{
				if(1==gTI.TestStaInfo.fixID)
				{
					strcpy_s(cmd1,sizeof(cmd1),"@B2#");
					strcpy_s(spec1,sizeof(spec1),"@H2#");
					strcpy_s(cmd2,sizeof(cmd2),"@A2#");
					strcpy_s(spec2,sizeof(spec2),"@G2#");
				}
				else if(2==gTI.TestStaInfo.fixID)
				{
					strcpy_s(cmd1,sizeof(cmd1),"@B5#");
					strcpy_s(spec1,sizeof(spec1),"@H5#");
					strcpy_s(cmd2,sizeof(cmd2),"@A5#");
					strcpy_s(spec2,sizeof(spec2),"@G5#");
				}
			}
			else if(strstr(gTI.TestStaInfo.TestStaName,"FT2"))
			{
				strcpy_s(cmd1,sizeof(cmd1),"@B1#");
				strcpy_s(spec1,sizeof(spec1),"@H1#");
				strcpy_s(cmd2,sizeof(cmd2),"@A1#");
				strcpy_s(spec2,sizeof(spec2),"@G1#");
			}
			else
			{
				amprintf("MSGBOX=CONTENT[USB Only be checked in MPT1 and FT2!];\n");
				WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
				if (!ResetEvent(gTI.UiResMsg.hEvent)) 
				{ 
					printf("ResetEvent failed (%d)\n", GetLastError());
				}
				KillTargetProcess("AmbitProT.exe");
				int errorcode;
				_exit(errorcode);
			}

			MoveBlock(4,9600,cmd1,"10",spec1);//pull back connectors
			MoveBlock(4,9600,cmd2,"10",spec2);//push out connectors

			amprintf("ping 10.0.1.1;\n");
			if(!VerifyLinkUsePing("ping 10.0.1.1",30))
			{
				amprintf("Ping Fail;\n");
				strcpy_s(pTI->Result,sizeof(pTI->Result),"PingFail");
				return 0;
			}
			else
			{
			}			
		}
		else
		{

		}
	//add end by Talen 2011/09/03
	}
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		if (!ParseProperyResult(OutBuf, AnaTemp, sizeof(AnaTemp), "product", "<string>"))
		{
			amprintf("%s;\n",OutBuf);
			amprintf("Parse the USB product information wrong.;\n");
			return 0;
		}
        GetXmlIndexStr(AnaTemp, 1, ',', pTI->Result,sizeof(pTI->Result));
		if (!ParseProperyResult(OutBuf, AnaTemp, sizeof(AnaTemp), "speed", "<string>"))
		{
			amprintf("%s;\n",OutBuf);
			amprintf("Parse the USB product information wrong.;\n");	
			return 0;
		}
		GetXmlIndexStr(AnaTemp, 1, ',', Temp, sizeof(Temp));	
		amprintf("Detect the USB device:%s;\n", pTI->Result);
		amprintf("Expect the USB device:%s;\n", pTI->Spec);
		amprintf("Detect the USB device speed:%s;\n", Temp);
		amprintf("Expect the USB device speed:%s;\n", pTI->UpLimit);
		if (strAllCmp(pTI->Result, pTI->Spec))
		{
			return 0;
		}
		if (strAllCmp(pTI->UpLimit, Temp))
		{
			return 0;
		}
	}
	return 1;
}

#endif


//add start by Talen 2011/10/28
int CTestItemScript::CheckUUTLinkStatus(TEST_ITEM *pTI)
{
	char Temp[500]="";
	char LLink[20]="";
	int iLink=0;
	int Result=1;
	char *pToke=NULL;

	GPubBUF.Flag=0;

	//add start by Talen 2011/09/03
	if(0==pTI->RetryItemCount)
	{

	}
	else if(1==pTI->RetryItemCount)
	{
//		gTI.UUTInfo.retryItemCount++;//Talen 2012/03/15
	}
	else if(1<pTI->RetryItemCount)
	{
		char cmd1[32]="";
		char cmd2[32]="";
		char spec1[32]="";
		char spec2[32]="";
		if(strstr(gTI.TestStaInfo.TestStaName,"MPT1"))
		{
			if(2==gTI.TestStaInfo.fixID)
			{
				strcpy_s(cmd1,sizeof(cmd1),"@B5#");
				strcpy_s(spec1,sizeof(spec1),"@H5#");
				strcpy_s(cmd2,sizeof(cmd2),"@A5#");
				strcpy_s(spec2,sizeof(spec2),"@G5#");
			}
			else
			{
				strcpy_s(cmd1,sizeof(cmd1),"@B2#");
				strcpy_s(spec1,sizeof(spec1),"@H2#");
				strcpy_s(cmd2,sizeof(cmd2),"@A2#");
				strcpy_s(spec2,sizeof(spec2),"@G2#");
			}
		}
		else if(strstr(gTI.TestStaInfo.TestStaName,"FT2"))
		{
			strcpy_s(cmd1,sizeof(cmd1),"@B1#");
			strcpy_s(spec1,sizeof(spec1),"@H1#");
			strcpy_s(cmd2,sizeof(cmd2),"@A1#");
			strcpy_s(spec2,sizeof(spec2),"@G1#");
		}
		else
		{
			amprintf("MSGBOX=CONTENT[Ethernet link status only be checked in MPT1 and FT2!];\n");
			WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			if (!ResetEvent(gTI.UiResMsg.hEvent)) 
			{ 
				printf("ResetEvent failed (%d)\n", GetLastError());
			}
			KillTargetProcess("AmbitProT.exe");
			int errorcode;
			_exit(errorcode);
		}

		MoveBlock(4,9600,cmd1,"10",spec1);//pull back connectors
		MoveBlock(4,9600,cmd2,"10",spec2);//push out connectors

		amprintf("ping 10.0.1.1;\n");
		if(!VerifyLinkUsePing("ping 10.0.1.1",30))
		{
			amprintf("Ping Fail;\n");
			strcpy_s(pTI->Result,sizeof(pTI->Result),"PingFail");
			return 0;
		}
		else
		{
		}			
	}
	else
	{

	}
	//add end by Talen 2011/09/03


	if (!GetSysDes())
	{
		return 0;
	}

	if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		pToke=NULL;
		pToke = strstr(GPubBUF.PubBUF, "PHYS:");
		if (!pToke)
		{
			amprintf("Get link status error;\n");
			return 0;
		}
		Result=GetsubStrInStr(Temp,sizeof(Temp),pToke,"+00000000: ","|");
		if (Result)
		{
			GPubBUF.Flag--;
			int ethPort=0;
			if(!strAllCmp("WAN",pTI->DiagCmd))
			{
				ethPort=1;
			}
			else if(!strAllCmp("LAN1",pTI->DiagCmd))
			{
				ethPort=2;
			}
			else if(!strAllCmp("LAN2",pTI->DiagCmd))
			{
				ethPort=3;
			}
			else if(!strAllCmp("LAN3",pTI->DiagCmd))
			{
				ethPort=4;
			}
			GetXmlIndexStr(Temp, ethPort, ' ', LLink, sizeof(LLink));
			iLink=atoi(LLink);
			LinkStatusParse(pTI->DiagCmd, iLink);
			strcpy_s(pTI->Result,sizeof(pTI->Result),LLink);
			amprintf("Get %s port link status %s;\n", pTI->DiagCmd,pTI->Result);
			amprintf("%s port link status spec %s;\n", pTI->DiagCmd,pTI->Spec);
			if (iLink!=atoi(pTI->Spec))
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
//add end by Talen 2011/10/28

int CTestItemScript::LinkStatusParse(char *pPort, int LinkResult)
{
	
	int  ret =1;
	int LinkFlag=0;
	LinkFlag =LinkResult;
	switch(LinkFlag)
	{
		case 0:						
			amprintf("Port %s Link Status: NO Link!;\n",pPort);
			ret =0;				
			break;
		case 1:
			amprintf("Port %s Link Status: 10 Mbps;\n",pPort);
			ret =0;		
			break;
		case 2:
			//amprintf("Port %d Link Status: 100Mbps;\n",LinkItem);
			//amprintf("Port %d Link Status Test   Fail;\n",LinkItem);
			//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s", errlinkValue);
			ret =0;	
			break;
		case 3:
			amprintf("Port %s Link Status: 100 Mbps;\n",pPort);
			ret =0;		
			break;
		case 4:
			amprintf("Port %s Link Status: Auto Negotiation(10/100 Mbps);\n",pPort);
			ret=0;
			break;
		case 6:
			amprintf("Port %s Link Status: Auto Negotiation(1000Mbps);\n",pPort);	
			break;
		default:
			break;
	}
	return ret;
}


int CTestItemScript::SetUUTStatus(TEST_ITEM *pTI)
{
	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	char OutBuf[100];
	DWORD dwExitCode; 
	
	
	if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
	if (dwExitCode>0)
	{
       //return 0;
	}
	//if (!ExecuteCMD(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	//{
		//return 0;
	//}
	
	amprintf("%s;\n",cmd);
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}


int CTestItemScript::CheckConnection(TEST_ITEM *pTI)
{
    int PingTime=0;
	amprintf("diag cmd:%s;\n",pTI->DiagCmd);
	PingTime=atoi(pTI->UpLimit);
//Add start by Maxwell 2008/12/16 for restart golden wireless card when ping fail
	HANDLE hRemoteCard=NULL;
	char Result[2000];
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");	// add by Talen 2011/06/13
	if((1==pTI->RetryJumpCount)&&(strcmp(pTI->InsCmd,"")!=0)&&(!VerifyLink(pTI->DiagCmd, PingTime)))
	{
		if (!OpenDev(&hRemoteCard,pTI->InsName,QUERY_MODE,pTI->InsCtrlID))
		{
			amprintf("Restart Golden WiFi card fail;\n");
			//strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR1");
			CloseDev(hRemoteCard);
			return 0;
		}
		amprintf("cmd:%s;\n",pTI->InsCmd);
		if (!DevQuery(hRemoteCard,pTI->InsCmd,Result,sizeof(Result)))
		{
			amprintf("Run restart card fail;\n");
			//amprintf("%s;\n",Result);
			//strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR2");
			CloseDev(hRemoteCard);
			return 0;
		}
		//Sleep(5000);
		CloseDev(hRemoteCard);

		amprintf("Restart Golden WiFi card pass;\n");
	}
//Add end by Maxwell 2008/12/16 for restart golden wireless card when ping fail
	if(!VerifyLink(pTI->DiagCmd, PingTime))
	{
		amprintf("Ping Fail;\n");
		return 0;
	}
	else
	{
		Sleep(1000*atoi(pTI->DiagPerfTime));
		amprintf(" Ping %d times PASS;\n", PingTime);	
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");	// add by Talen 2011/06/13
	return 1;
}

int CTestItemScript::SetWifiAntenna(TEST_ITEM *pTI)
{
	char cmd[500];
	char OutBuf[100];
	DWORD dwExitCode;
	
	
	strSTEDcpy(cmd,sizeof(cmd),pTI->UUTCmd,0xff,'r');
	strcat_s(cmd,sizeof(cmd),"raST=false");
	if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
    amprintf("%s;\n",cmd);


	if (!RunExeFileNoOutput(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
	amprintf("%s;\n",pTI->UUTCmd);
	

	strSTEDcpy(cmd,sizeof(cmd),pTI->UUTCmd,0xff,'r');
	strcat_s(cmd,sizeof(cmd),"raST=true");
	if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}	
	amprintf("%s;\n",cmd);
	
	return 1;
}


#ifdef DUAL_ATN
int CTestItemScript::CheckWifiAntenna(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Check wifi antenna Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "outputs");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Check wifi antenna Fail;\n");
			return 0;
		}
		if (!ParseProperyResult(OutBuf, AnaTemp, sizeof(AnaTemp), "rxant", "<integer>"))
		{
			amprintf("Parse the USB product information wrong.;\n");	
			return 0;
		}
        GetXmlIndexStr(AnaTemp, 1, ',', pTI->Result,sizeof(pTI->Result));
	}
	amprintf("rxant value spec: %s;\n",pTI->Spec);
	amprintf("rxant value: %s;\n",pTI->Result);	
	if (atoi(pTI->Spec)!=atoi(pTI->Result))
	{
		return 0;
	}
	return 1;
}
#else
int CTestItemScript::CheckWifiAntenna(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Check wifi antenna Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "raRX");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Check UUT rx antenna Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,'0x','\r');
		amprintf(" rx antenna: %s;\n", AnaTemp);
		strcpy_s(pTI->Result, sizeof(pTI->Result), AnaTemp);
	}
	amprintf("rxant value spec: %s;\n",pTI->Spec);
	amprintf("rxant value: %s;\n",pTI->Result);	
	if (atoi(pTI->Spec)!=atoi(pTI->Result))
	{
		return 0;
	}
	return 1;
}
#endif




int CTestItemScript::CheckWifiRxThroughput(TEST_ITEM *pTI)
{

	char Result[2000];

	char AnaTemp[2048]="";
	char *pToke = NULL;
	char TempBuf[2048]="";
	double TXTh=0;
	double RXTh=0;
	

	HANDLE hRemoteIperf=NULL;
	if (!OpenDev(&hRemoteIperf,pTI->InsName,QUERY_MODE,pTI->InsCtrlID))
	{
		amprintf("Run remote throughput test tool fail;\n");
		strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR1");
		CloseDev(hRemoteIperf);
		return 0;
	}
	amprintf("cmd:%s;\n",pTI->InsCmd);
	if (!DevQuery(hRemoteIperf,pTI->InsCmd,Result,sizeof(Result)))
	{
		amprintf("Run remote throughput test tool fail;\n");
		amprintf("%s;\n",Result);
		strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR2");
		CloseDev(hRemoteIperf);
		return 0;
	}
	CloseDev(hRemoteIperf);

	amprintf("Ierf output:%s;\n",Result);

	pToke=NULL;
	pToke = strstr(Result, "connect failed:");
	if (pToke)
	{
		amprintf("iperf error:%s;\n", Result);
		strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR3");
		return 0;
	}

	pToke=NULL;
	pToke = strstr(Result, "Mbits/sec");
	if (!pToke)
	{
		amprintf("iperf error:%s;\n", Result);
		strcpy_s(pTI->Result,sizeof(pTI->Result),"IPERF_ERROR4");
		return 0;
	}

	pToke-=6;
	strSTEDcpy(AnaTemp,sizeof(AnaTemp),pToke,' ','\n');
	amprintf("RX throughput: %s;\n", AnaTemp);
	RXTh=atof(AnaTemp);
	sprintf_s(TempBuf,sizeof(TempBuf),"RX:%.2f Mbits/sec",RXTh);	

	strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);
	amprintf("Limit %s Mbits/sec;\n",pTI->DownLimit);
	
	if (RXTh<atof(pTI->DownLimit))
	{
		amprintf("Throughput %s under limit;\n",pTI->Result);	
		return 0;
	}
	return 1;
}

int CTestItemScript::CheckWifiTxThroughput(TEST_ITEM *pTI)
{

	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char TempBuf[2048]="";
	double TXTh=0;
	double RXTh=0;
	
	//get the wireless card MAC ID	
	amprintf("diag cmd:%s;\n",pTI->DiagCmd);
	if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run throughput test fail;\n");
		memset(OutBuf,0,sizeof(OutBuf));//Add by Maxwell 2008/11/16 for delete temp iperf outbuf
		return 0;		
	}
	else
	{		
		amprintf("Iperf output:%s;\n",OutBuf);
		pToke=NULL;
		pToke = strstr(OutBuf, "connect failed:");
		if (pToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			memset(OutBuf,0,sizeof(OutBuf));//Add by Maxwell 2008/11/16 for delete temp iperf outbuf
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "Mbits/sec");
		if (!pToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			memset(OutBuf,0,sizeof(OutBuf));//Add by Maxwell 2008/11/16 for delete temp iperf outbuf
			return 0;
		}

		pToke-=6;
		strSTEDcpy(AnaTemp,sizeof(AnaTemp),pToke,' ','\n');
		amprintf("TX throughput: %s;\n", AnaTemp);
		TXTh=atof(AnaTemp);
		sprintf_s(TempBuf,sizeof(TempBuf),"TX:%.2f Mbits/sec",TXTh);

		strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);

	}
	amprintf("Limit %s Mbits/sec;\n",pTI->DownLimit);
	memset(OutBuf,0,sizeof(OutBuf));//Add by Maxwell 2008/11/16 for delete temp iperf outbuf
	if (TXTh<atof(pTI->DownLimit))
	{
		amprintf("Throughput %s under limit;\n",pTI->Result);	
		return 0;
	}
	return 1;
}

int CTestItemScript::CheckEthThroughput(TEST_ITEM *pTI)
{
	
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char *pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
	char *pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Kbits/sec
	char TempBuf[2048]="";
	double TXTh=0;
	double RXTh=0;
	
	//get the wireless card MAC ID	
	amprintf("diag cmd:%s;\n",pTI->DiagCmd);
	if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run throughput test fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "connect failed:");
		if (pToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			return 0;
		}

		pToke=NULL;
		//pToke = strstr(OutBuf, "Mbits/sec");
		pToke = strstr(OutBuf, "bits/sec");//Modified by Maxwell 2009/01/09 for test Gbits/sec
		if (!pToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			return 0;
		}

		//pToke-=6;
		pToke-=7;//Modified by Maxwell 2009/01/09 for test Gbits/sec
		strSTEDcpy(AnaTemp,sizeof(AnaTemp),pToke,' ','\n');
		amprintf("TX throughput: %s;\n", AnaTemp);
		pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
		pToken = strstr(AnaTemp, "Gbits/sec");//Add by Maxwell 2009/01/09 for test Gbits/sec
		pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Gbits/sec
		pTokenK = strstr(AnaTemp, "Kbits/sec");//Add by Maxwell 2009/09/09 for test Gbits/sec
		if(pToken)
		{
			TXTh=1000*atof(AnaTemp);
		}
		else if(pTokenK)
		{
			TXTh=(atof(AnaTemp))/1000;
		}
		else
		{
			TXTh=atof(AnaTemp);
		}
		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,",TXTh);//Maxwell 20110326

		strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);

		pToke+=20;
		//pToke = strstr(pToke, "Mbits/sec");
		pToke = strstr(pToke, "bits/sec");//Modified by Maxwell 2009/01/09 for test Gbits/sec
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("RX throughput value too low;\n");
			return 0;
		}
		//pToke-=6;
		pToke-=7;//Modified by Maxwell 2009/01/09 for test Gbits/sec

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),pToke,' ','\n');
		amprintf("RX throughput: %s;\n", AnaTemp);
		//RXTh=atof(AnaTemp);
		pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
		pToken = strstr(AnaTemp, "Gbits/sec");//Add by Maxwell 2009/01/09 for test Gbits/sec
		pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Gbits/sec
		pTokenK = strstr(AnaTemp, "Kbits/sec");//Add by Maxwell 2009/09/09 for test Gbits/sec
		if(pToken)
		{
			RXTh=1000*atof(AnaTemp);
		}
		else if(pTokenK)
		{
			RXTh=(atof(AnaTemp))/1000;
		}
		else
		{
			RXTh=atof(AnaTemp);
		}
		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f",RXTh);	
		
		strcat_s(pTI->Result,sizeof(pTI->Result),TempBuf);
	}
	amprintf("Up Limit: %s Mbits/sec;\n",pTI->UpLimit);
	amprintf("Down Limit: %s Mbits/sec;\n",pTI->DownLimit);
	amprintf("Throughput %s;\n",pTI->Result);	
	if (TXTh<atof(pTI->DownLimit)||RXTh<atof(pTI->DownLimit)||TXTh>atof(pTI->UpLimit)||RXTh>atof(pTI->UpLimit))
	{
		return 0;
	}
	return 1;
}


//add start by Talen 2011/04/03
int CTestItemScript::CheckWiFiThroughPutInGolden(TEST_ITEM *pTI)
{
	//sprintf_s(pTI->Result,sizeof(pTI->Result),"-999999,-999999");//add by Talen 2011/03/31
	CTestItemScript::OpenIperfInGolden(pTI);


	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char *pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
	char *pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Kbits/sec
	char TempBuf[2048]="";
	double TXTh=0;
	double RXTh=0;
	char temp[1024]="";

	Sleep(200);
	//Pre-iperf to delete low data of iperf Maxwell 090728
	amprintf("Pre-iperf cmd:%s;\n",pTI->UUTCmdAdd[1]);
	if (!RunExeFileLocalForTPPre(pTI->UUTCmdAdd[1], OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run throughput test fail;\n");
		return 0;		
	}
	Sleep(500);
	memset(OutBuf,0,sizeof(OutBuf));
	//get the wireless card MAC ID	
	CTestItemScript::OpenIperfInGolden(pTI);	//add by Talen 2011/04/01
	amprintf("diag cmd:%s;\n",pTI->DiagCmd);
	if (!RunExeFileLocalForTP(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run throughput test fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "connect failed:");
		if (pToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			return 0;
		}

		pToke=NULL;
		//pToke = strstr(OutBuf, "Mbits/sec");
		char *ppToke=NULL;
		memset(AnaTemp,0,sizeof(AnaTemp));
		GetsubStrInStr(AnaTemp,sizeof(AnaTemp),OutBuf,"Interval","local");

		pToke=strstr(AnaTemp,"sec");
		
		while(pToke)
		{
			ppToke=pToke;
			pToke++;
			pToke=strstr(pToke,"bits/sec");
		}


		//pToke = strstr(OutBuf, "bits/sec");//Modified by Maxwell 2009/01/09 for test Gbits/sec
		if (!ppToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			return 0;
		}

		//pToke-=6;
		ppToke-=7;//Modified by Maxwell 2009/01/09 for test Gbits/sec
		memset(temp,0,sizeof(temp));
		strSTEDcpy(temp,sizeof(temp),ppToke,' ','\n');
		amprintf("TX throughput: %s;\n", temp);
		pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
		pToken = strstr(temp, "Gbits/sec");//Add by Maxwell 2009/01/09 for test Gbits/sec
		pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Gbits/sec
		pTokenK = strstr(temp, "Kbits/sec");//Add by Maxwell 2009/09/09 for test Gbits/sec
		if(pToken)
		{
			TXTh=1000*atof(temp);
		}
		else if(pTokenK)
		{
			TXTh=(atof(temp))/1000;
		}
		else
		{
			TXTh=atof(temp);
		}
		//sprintf_s(TempBuf,sizeof(TempBuf),"TX:%.2f Mbits/sec--",TXTh);
		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,",TXTh);

		strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);

		pToke=strstr(OutBuf,"sec");
		
		while(pToke)
		{
			ppToke=pToke;
			pToke++;
			pToke=strstr(pToke,"bits/sec");
		}
		//pToke = strstr(OutBuf, "bits/sec");//Modified by Maxwell 2009/01/09 for test Gbits/sec
		if (!ppToke)
		{
			amprintf("iperf error:%s;", OutBuf);
			return 0;
		}
		ppToke-=7;//Modified by Maxwell 2009/01/09 for test Gbits/sec

		memset(temp,0,sizeof(temp));
		strSTEDcpy(temp,sizeof(temp),ppToke,' ','\n');
		amprintf("RX throughput: %s;\n", temp);
		//RXTh=atof(AnaTemp);
		pToken = NULL;//Add by Maxwell 2009/01/09 for test Gbits/sec
		pToken = strstr(temp, "Gbits/sec");//Add by Maxwell 2009/01/09 for test Gbits/sec
		pTokenK = NULL;//Add by Maxwell 2009/09/09 for test Gbits/sec
		pTokenK = strstr(temp, "Kbits/sec");//Add by Maxwell 2009/09/09 for test Gbits/sec
		if(pToken)
		{
			RXTh=1000*atof(temp);
		}
		else if(pTokenK)
		{
			RXTh=(atof(temp))/1000;
		}
		else
		{
			RXTh=atof(temp);
		}
		//sprintf_s(TempBuf,sizeof(TempBuf),"RX:%.2f Mbits/sec",RXTh);	
		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f",RXTh);	
		
		strcat_s(pTI->Result,sizeof(pTI->Result),TempBuf);
	}

	amprintf("Limit %s Mbits/sec;\n",pTI->UpLimit);
	amprintf("Throughput %s;\n",pTI->Result);	
	if (TXTh<atof(pTI->UpLimit)||RXTh<atof(pTI->DownLimit))
	{
		return 0;
	}
	return 1;
}
//add end by Talen 2011/04/03

#ifdef MULTI_ACP_MODE

int CTestItemScript::CheckLocalInfo(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;

    if (!GetSysDes())
	{
		return 0;
	}

    if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		//Check Version
		Result=GetsubStrInStr(Temp,sizeof(Temp),GPubBUF.PubBUF,"ssSK: ","\r\n");
		if (Result)
		{
			GPubBUF.Flag--;
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT local information: %s;\n", pTI->Result);
			amprintf("UUT local information in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
#else
int CTestItemScript::CheckLocalInfo(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check local
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"ssSK: ","\r\n");
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT local information: %s;\n", pTI->Result);
			amprintf("UUT local information in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
#endif
#ifdef MULTI_ACP_MODE
int CTestItemScript::CheckDiagValue(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;

    if (!GetSysDes())
	{
		return 0;
	}

    if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		//Check Version
		Result=GetsubStrInStr(Temp,sizeof(Temp),GPubBUF.PubBUF,"diag: ","\r\n");
		if (Result)
		{
			GPubBUF.Flag--;
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT diag value: %s;\n", pTI->Result);
			amprintf("UUT diag value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
#else
int CTestItemScript::CheckDiagValue(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check diag
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"diag: ","\r\n");
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT diag value: %s;\n", pTI->Result);
			amprintf("UUT diag value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
#endif

#ifdef MULTI_ACP_MODE
int CTestItemScript::CheckWanPortStatus(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;

    if (!GetSysDes())
	{
		return 0;
	}

    if (!GPubBUF.Flag)
	{
		memset(GPubBUF.PubBUF,0,sizeof(GPubBUF.PubBUF));
		return 0;		
	}
	else
	{
		//Check Version
		Result=GetsubStrInStr(Temp,sizeof(Temp),GPubBUF.PubBUF,"waCV: ","\r\n");
		amprintf("UUT WAN port status value: %s;\n", Temp);
		if (Result)
		{
			GPubBUF.Flag--;
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT WAN port status value: %s;\n", pTI->Result);
			amprintf("UUT WAN port status value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
#else

int CTestItemScript::CheckWanPortStatus(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check wan port status
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"waCV: ","\r\n");
		amprintf("UUT WAN port status value: %s;\n", Temp);
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT WAN port status value: %s;\n", pTI->Result);
			amprintf("UUT WAN port status value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}

#endif

//Add start by Maxwell 2008/11/13 for Check Radio On
int CTestItemScript::CheckRadioStatus(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check Radio Status
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"raSt: ","\r\n");
		amprintf("UUT Radio status value: %s;\n", Temp);
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT Radio status value: %s;\n", pTI->Result);
			amprintf("UUT Radio status value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
//Add end by Maxwell 2008/11/13 for Check Radio On


//Add start by Tony on 091223 for K30A screen for send WIFIMAC
int CTestItemScript::CheckWIFIMAC0(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	char GetMAC[200]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";

    CString strMac;
   //get the wireless card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("ERROR! Can not find acp.exe!;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "failed:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get AP wireless card MAC ID Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,'@','\r');
		amprintf("Wirelesscard MAC: %s;\n", AnaTemp);
	

		if (strchr(AnaTemp,':'))
		{
			strSTEDcpy(GetMAC,sizeof(GetMAC),AnaTemp,' ','\r');
			strMac=GetMAC;
			strMac.Remove(':');
		}
		else
		{
			strMac=AnaTemp;
		}
	
		strMac.MakeUpper();
		strcpy_s(pTI->Result,sizeof(pTI->Result),strMac);
	}
	//amprintf("Scan WiFi MAC %s;\n",pTI->Spec);
	amprintf("Get WiFi MAC0 %s;\n",pTI->Result);
	strcpy_s(WIFI_MAC0, sizeof(WIFI_MAC0),pTI->Result);  //Send WIFIMAC0 to SFIS	
    amprintf("DAT;WIFI_MAC0=%s;\n", WIFI_MAC0);  

	/*
	if (strMac.Compare(pTI->Spec))
	{
		return 0;
	}
	*/
	return 1;
	
}

int CTestItemScript::CheckWIFIMAC1(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	char GetMAC[200]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";

    CString strMac;
   //get the wireless card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("ERROR! Can not find acp.exe!;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "failed:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get AP wireless card MAC ID Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,'@','\r');
		amprintf("Wirelesscard MAC: %s;\n", AnaTemp);
	

		if (strchr(AnaTemp,':'))
		{
			strSTEDcpy(GetMAC,sizeof(GetMAC),AnaTemp,' ','\r');
			strMac=GetMAC;
			strMac.Remove(':');
		}
		else
		{
			strMac=AnaTemp;
		}
	
		strMac.MakeUpper();
		strcpy_s(pTI->Result,sizeof(pTI->Result),strMac);
	}
	//amprintf("Scan WiFi MAC %s;\n",pTI->Spec);
	amprintf("Get WiFi MAC1 %s;\n",pTI->Result);
	strcpy_s(WIFI_MAC1, sizeof(WIFI_MAC1),pTI->Result);  //Send WIFIMAC1 to SFIS	
    amprintf("DAT;WIFI_MAC1=%s;\n", WIFI_MAC1);  

	/*
	if (strMac.Compare(pTI->Spec))
	{
		return 0;
	}
	*/
	return 1;
	
}

//Add end by Tony on 091223 for K30A screen for send WIFIMAC




//////////////////////////////////////////////////////////////////////////////////////
//For Time capsule test
int CTestItemScript::CheckMLBBattery(TEST_ITEM *pTI)
{
	
	char Result[500];
	char *pToke = NULL;
	double Voltage=0;
	double Up=0;
	double Down=0;


	HANDLE hRemote=NULL;
	if (!OpenDev(&hRemote,pTI->InsName,QUERY_MODE,pTI->InsCtrlID))
	{
		amprintf("Run remote test tool fail;\n");
		CloseDev(hRemote);
		return 0;
	}
	amprintf("cmd:%s;\n",pTI->InsCmd);
	if (!DevQuery(hRemote,pTI->InsCmd,Result,sizeof(Result)))
	{
		amprintf("Run remote test tool fail;\n");
		amprintf("%s;\n",Result);
		CloseDev(hRemote);
		return 0;
	}
	CloseDev(hRemote);


	amprintf("Instrument return:%s;\n",Result);
	Voltage=atof(Result);
	Down=atof(pTI->DownLimit);
	Up=atof(pTI->UpLimit);
	amprintf("MLB Battery voltage:%.3f;\n",Voltage);
	amprintf("MLB Battery voltage up limit:%.3f down limit:%.3f;\n",Up,Down);

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%.3f",Voltage);
	if ((Voltage<Down)||(Voltage>Up))
	{
		return 0;
	}

	return 1;
}
//Add start by Maxwell 2008/10/14
int CTestItemScript::CheckMLBBatteryUseVolt(TEST_ITEM *pTI)
{
	char OutBuf[2048] = "";
	DWORD dwExitCode = 0;
	char *pToke=NULL;
	double Up=0;
	double Down=0;
	double fVoltage=0;
    char AnaTemp[2048]="";

	if(!RunExeFileForBonjour("Volt.exe", OutBuf, sizeof(OutBuf), &dwExitCode,6))
	{
		amprintf("Can not open Volt.exe;\n");
		amprintf("MLB Battery Check   Fail;\n");
		return 0;
	}

	pToke = strstr(OutBuf, "Battery Voltage=");
	if (pToke)
	{				
		pToke = pToke +strlen("Battery Voltage=");		
		strncpy_s(AnaTemp, sizeof(AnaTemp), pToke, strlen("2.975"));	
		fVoltage =atof(AnaTemp);
		Down=atof(pTI->DownLimit);
		Up=atof(pTI->UpLimit);
		amprintf("MLB Battery voltage 1st measure:%.3f;\n",fVoltage);
		amprintf("MLB Battery voltage up limit:%.3f down limit:%.3f;\n",Up,Down);

		sprintf_s(pTI->Result,sizeof(pTI->Result),"%.3f",fVoltage);
		if ((fVoltage<Down)||(fVoltage>Up))
		{
			amprintf("MLB Battery 1st Check Fail;\n");
			return 0;

	    }
	}
//add start: prevent battery loss assembly, add 2nd battery test. both test must be in spec. Blucey 2010/08/04
	Sleep(500);
    memset(OutBuf,0,sizeof(OutBuf));
	memset(AnaTemp,0,sizeof(AnaTemp));
	if(!RunExeFileForBonjour("Volt.exe", OutBuf, sizeof(OutBuf), &dwExitCode,6))
	{
		amprintf("Can not open Volt.exe;\n");
		amprintf("MLB Battery Check   Fail;\n");
		return 0;
	}

	pToke = strstr(OutBuf, "Battery Voltage=");
	if (pToke)
	{				
		pToke = pToke +strlen("Battery Voltage=");		
		strncpy_s(AnaTemp, sizeof(AnaTemp), pToke, strlen("2.975"));	
		fVoltage =atof(AnaTemp);
		//Down=atof(pTI->DownLimit);
		//Up=atof(pTI->UpLimit);
		amprintf("MLB Battery voltage 2nd measure:%.3f;\n",fVoltage);
		amprintf("MLB Battery voltage up limit:%.3f down limit:%.3f;\n",Up,Down);

		sprintf_s(pTI->Result,sizeof(pTI->Result),"%.3f",fVoltage);
		if ((fVoltage<Down)||(fVoltage>Up))
		{
			amprintf("MLB Battery 2nd Check Fail;\n");
			return 0;
		}
	}

	//add End.

	return 1;
}
//Add end by Maxwell 2008/10/14

int CTestItemScript::CheckHDDInformation(TEST_ITEM *pTI)
{
	//char OutBuf[20000]="";//Maxwell 2009/09/17
	char OutBuf[50000]="";
	char AnaTemp[1024]="";
	char Temp[1024]="";
	
	char Model[80]="";
	char FWVersion[80]="";
	char SataVer[80]="";
	char Capacity[80]="";
	char *pTok=NULL;
	
	DWORD dwExitCode=0;

	//if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	//{
		//amprintf("Test error,Can not open the ACP tool;\n");
		//return 0;
	//}
    //int ExecuteCMD(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode)
	if (!RunExeFileLocalBigPipe(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	pTok=strstr(OutBuf,"ddSm:");
	if (pTok)
	{	
		if(strcmp(gTI.UUTInfo.UUTName,"K30") == 0)//Start, modified to auto detect K30A and K30 by blucey 20101011,
		{
		pTok=strstr(OutBuf,"wd0"); //modify by tony on 10-29 for k30
		//pTok=strstr(OutBuf,"sd0");   //for K30A Series
		}
		if(strcmp(gTI.UUTInfo.UUTName,"K30A") == 0)//add by blucey 20101011
		{
		//pTok=strstr(OutBuf,"wd0"); //modify by tony on 10-29 for k30
		pTok=strstr(OutBuf,"sd0");   //for K30A Series
		}//end20101011
		if(strcmp(gTI.UUTInfo.UUTName,"K30B") == 0)//add by blucey 20101011
		{
		//pTok=strstr(OutBuf,"wd0"); //modify by tony on 10-29 for k30
		pTok=strstr(OutBuf,"sd0");   //for K30A Series
		}//end20101011
		if (!pTok)
		{
			amprintf("Test error,wrong parse1;\n");
			return 0;
		}

		if(strcmp(gTI.UUTInfo.UUTName,"K30") == 0)//Start, modified to auto detect K30A and K30 by blucey 20101011,
		{
		//if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"sd0","SMARTStatusKey"))  //for K30A Series
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"wd0","SMARTStatusKey")) //modify by tony on 10-29 for k30
		 {
			amprintf("Test error,wrong parse2;\n");
			return 0;
		 } 
		}
		if((strcmp(gTI.UUTInfo.UUTName,"K30A") == 0)||(strcmp(gTI.UUTInfo.UUTName,"K30B") == 0));//add by blucey 20101011
		{
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"sd0","SMARTStatusKey"))  //for K30A Series
		//if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"wd0","SMARTStatusKey")) //modify by tony on 10-29 for k30
		 {
			amprintf("Test error,wrong parse3;\n");
			return 0;
		 } 
		}//end20101011

		
	
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "SMARTInfoModel", "<string>"))
		{
			amprintf("Test error,wrong parse4;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', Model,sizeof(Model));		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "SMARTInfoFirmwareVersion", "<string>"))
		{
			amprintf("Test error,wrong parse5;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', FWVersion,sizeof(FWVersion));		

		if (!ParseProperyResult(OutBuf, AnaTemp, sizeof(AnaTemp), "SMARTInfoCapacity", "<integer>"))
		{
			amprintf("Test error,wrong parse6;\n");
			return 0;
		}
		GetXmlIndexStr(AnaTemp, 1, ',', Capacity,sizeof(Capacity));		
		
		if (!ParseProperyResult(OutBuf, AnaTemp, sizeof(AnaTemp), "SMARTInfoATAVersion", "<integer>"))
		{
			amprintf("Test error,wrong parse7;\n");
			return 0;
		}
		GetXmlIndexStr(AnaTemp, 1, ',', SataVer,sizeof(SataVer));		
		
		amprintf("Detect HDD model:%s F/W:%s Capacity:%s SATA:%s;\n", Model,FWVersion,Capacity,SataVer);
		amprintf("HDD Spec model:%s Capacity up:%s down:%s;\n", pTI->Spec,pTI->UpLimit,pTI->DownLimit);
		amprintf("HDD Spec_1:%s;\n",pTI->Spec_1);
		strcpy_s(pTI->Result,sizeof(pTI->Result),Model);
		
		if ((_atoi64(Capacity)<_atoi64(pTI->DownLimit))||(_atoi64(Capacity)>_atoi64(pTI->UpLimit)))
		{
			return 0;
		}
		
		if ((!strAllCmp(pTI->Result,pTI->Spec))||(!strAllCmp(pTI->Result,pTI->Spec_1)))//,Maxwell 090923 for multi spec
		{
			return 1;
		}
		else
		{
			return 0;
		}
	
	}
	else
	{
		amprintf("Test error,wrong parse8;\n");
		return 0;
	}
	return 1;
}


/*This phase is fitted to K30A series, Mask start by Tony on 091218 */

int CTestItemScript::SetHDDFormat(TEST_ITEM *pTI)
{
	char OutBuf[500]="";
	char AnaTemp[1024]="";
	char Temp[1024]="";
	char BuildinHDD[128]="";
	char HDDSize[50]="";
	char cmd[256]="";
	char *pTok=NULL;
	char *pToken=NULL;//09/10/08
	DWORD dwExitCode=0;



	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}		
	//Modify by Maxwell 2009/10/08 to first check build in HDD name then format, prevent detect extend USB mobile HD.
	if(strcmp(gTI.UUTInfo.UUTName,"K30A") == 0||strcmp(gTI.UUTInfo.UUTName,"K30B") == 0)//Start, modified to auto detect K30A and K30 by blucey 20101011,
	{
		//
		pToken = strstr(OutBuf,"names");
		if(!pToken)
		{
			amprintf("The output of Buildin HDD wrong,;\n");
		}
		else
		{

		}
		if(!ParseProperyResult(pToken, Temp, sizeof(Temp), "<array>", "<string>"))
		{
			amprintf("Test error,no HDD name parsed %s;\n",OutBuf);
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', BuildinHDD,sizeof(BuildinHDD));		
		amprintf("Build in HDD is:%s;\n", BuildinHDD);
		//amprintf("HDD format status in config file: %s;\n", pTI->Spec);
		//sprintf_s(BuildinHDD, " %s", BuildinHDD);
		//strcat_s(pTI->DiagCmd,sizeof(pTI->DiagCmd)," ");
		strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
		strcat_s(cmd,sizeof(cmd),BuildinHDD); 

		if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Test error,Can not open the ACP tool;\n");
			return 0;
		}
	}

//end20101011
	if (!ParseProperyResult(OutBuf, Temp, sizeof(Temp), "status", "<integer>"))
	{
		amprintf("Test error,wrong parse %s;\n",OutBuf);
		return 0;
	}
	GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
	amprintf("Format HDD status:%s;\n", pTI->Result);
	amprintf("HDD format status in config file: %s;\n", pTI->Spec);

	if (!strAllCmp(pTI->Result,pTI->Spec))
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return 1;
} 


int CTestItemScript::CheckHDDFormat(TEST_ITEM *pTI)
{
	char OutBuf[3000]="";
	char AnaTemp[1024]="";
	char Temp[1024]="";
	char HDDSize[50]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}

	////DEBUG
	//amprintf("%s\n",OutBuf);
	////DEBUG

    pTok=strstr(OutBuf,"MaSt:");
	if (pTok)
	{
		pTok=strstr(OutBuf,"builtin");
		if (!pTok)
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"builtin","vendor"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "format", "<string>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect HDD format:%s;\n", pTI->Result);
		amprintf("HDD format in concfig file: %s;\n", pTI->Spec);

		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "size", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', HDDSize,sizeof(HDDSize));		
		amprintf("Detect HDD size:%s;\n", HDDSize);
		amprintf("HDD size up limit:%s down limit:%s;\n", pTI->UpLimit, pTI->DownLimit);

		if ((atoi(HDDSize)<atoi(pTI->DownLimit))||(atoi(HDDSize)>atoi(pTI->UpLimit)))
		{
			return 0;
		}
		
		if (!strAllCmp(pTI->Result,pTI->Spec))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}
int CTestItemScript::SetHDSpinDown(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char Temp[100]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
	DWORD dwWaitResult;
	int iTIMER=0;
	int iSpin=0;//Add by Maxwell 2008/12/12 for retry when fail
   
LABLE_SPIN:
	SetHDSpinDownTime=0;//Reset the HD spin down timer
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		SetHDSpindownFailFlag=1;
		amprintf("Can not open the ACP tool;\n");
		//amprintf("MSG;TESTITEM=Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!\n;");	//added by maxwell 2008/10/11  Maxwell 090520
		amprintf("MSGBOX=CONTENT[Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!];\n");	
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		for(iTIMER=0;iTIMER<30;iTIMER++)
		{
			amprintf("elapsed time is %d seconds, wait for HD spin down;\n", iTIMER);
			Sleep(1000);			
		}
		//Sleep(30*1000);//added by maxwell 2008/10/11
		return 0;
	}
    pTok=strstr(OutBuf,"outputs");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"outputs","</dict>"))
		{
			amprintf("Test error,wrong parse1;\n");
			//amprintf("MSG;TESTITEM=Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!\n;");	//added by maxwell 2008/10/11      Maxwell 090520
			amprintf("MSGBOX=CONTENT[Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!];\n");	
			SetHDSpindownFailFlag=1;
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			if (!ResetEvent(gTI.UiResMsg.hEvent)) 
			{ 
				printf("ResetEvent failed (%d)\n", GetLastError());
			} 
			for(iTIMER=0;iTIMER<30;iTIMER++)
			{
				amprintf("elapsed time is %d seconds, wait for HD spin down;\n", iTIMER);
				Sleep(1000);			
			}
			//Sleep(30*1000);//added by maxwell 2008/10/11
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "status", "<integer>"))
		{
			amprintf("wrong parse2;\n");
			//amprintf("MSG;TESTITEM=Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!\n;");	//added by maxwell 2008/10/11    //Maxwell 090520
			amprintf("MSGBOX=CONTENT[Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!];\n");	
			SetHDSpindownFailFlag=1;
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			if (!ResetEvent(gTI.UiResMsg.hEvent)) 
			{ 
				printf("ResetEvent failed (%d)\n", GetLastError());
			} 
			for(iTIMER=0;iTIMER<30;iTIMER++)
			{
				amprintf("elapsed time is %d seconds, wait for HD spin down;\n", iTIMER);
				Sleep(1000);			
			}
			//Sleep(30*1000);//added by maxwell 2008/10/11
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Set HD Spin down, status:%s;\n", pTI->Result);
	}
	else
	{
		//Add start by Maxwell 2008/12/12 for retry in Set HDD spin down
		if(iSpin<5)
		{
			Sleep(1000);
			amprintf("%d;\n",iSpin);
			iSpin++;
			goto LABLE_SPIN;	
		}
		//Add end by Maxwell 2008/12/12 for retry in Set HDD spin down
		amprintf("wrong parse3;\n");
		//amprintf("MSG;TESTITEM=Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!\n;");	//added by maxwell 2008/10/11        Maxwell 090520
		amprintf("MSGBOX=CONTENT[Set HD spin down fail!\nPlease power off UUT and wait 30s for the result!];\n");	
		SetHDSpindownFailFlag=1;
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		for(iTIMER=0;iTIMER<30;iTIMER++)
		{
			amprintf("elapsed time is %d seconds, wait for HD spin down;\n", iTIMER);
			Sleep(1000);			
		}
		//Sleep(30*1000);//added by maxwell 2008/10/11
		return 0;
	}

	SetHDSpindownFlag=1;//Set spin down flag to 1
	// Set time zone from TZ environment variable. If TZ is not set,
	// the operating system is queried to obtain the default value 
	// for the variable. 
	//
	_tzset();

	// Get UNIX-style time and display as number and string. 
	time( &SetHDSpinDownTime);
	
	amprintf("HD time: %x;\n",SetHDSpinDownTime);
	amprintf("Wait %s seconds for HD spin down;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	//SetEvent(hHDSpinDownEvent);
	return 1;
}
int CTestItemScript::CheckAndWaitHDSpinDown(TEST_ITEM *pTI)
{
    //DWORD dwWaitResult; 
	time_t HDSpinStopTime=0;//Check HD spin down time counter

	int DifferentTime=0;
	int GetTimeIndex=1;
	int StopTimeSpec=0;
	int CheckHDSpindownFlag=0;//add by maxwell 2008/10/11
	int CheckHDSpindownFailFlag=0;//add by maxwell 2008/10/15

	StopTimeSpec=atoi(pTI->Spec);//Get the stop time spec from config

_CheckHD_SpindownFlag:
	CheckHDSpindownFlag = SetHDSpindownFlag;//add by maxwell 2008/10/11
	SetHDSpindownFlag = 0;

	CheckHDSpindownFailFlag = SetHDSpindownFailFlag;//add by maxwell 2008/10/11
	SetHDSpindownFailFlag = 0;
	
	if(CheckHDSpindownFlag)//add by maxwell 2008/10/11
	{
		amprintf("Check HD spin down time;\n");
		// Set time zone from TZ environment variable. If TZ is not set,
		// the operating system is queried to obtain the default value 
		// for the variable. 
		//
		_tzset();
		for (GetTimeIndex=1;GetTimeIndex<=30;GetTimeIndex++)//get maximum time is 30 times.
		{
			// Get UNIX-style time and display as number and string. 
			time( &HDSpinStopTime);

			DifferentTime=0;
			DifferentTime = HDSpinStopTime-SetHDSpinDownTime;
			if (DifferentTime>StopTimeSpec)
			{			
				break;
			}
			else
			{
				amprintf("elapsed time is %d seconds, wait for HD spin down;\n", DifferentTime);
				Sleep(1000);
			}
		}

		SetHDSpinDownTime=0;//Reset the the timer record
		amprintf("elapsed time is %d seconds since set HD spin down;\n", DifferentTime);
		return 1;
	}//add by maxwell 2008/10/11
	else if((!CheckHDSpindownFlag)&&CheckHDSpindownFailFlag)
	{
		return 0;
	}
	else//add by maxwell 2008/10/11
	{
		SetHDSpinDownTime=0;//Reset the the timer record //add by maxwell 2008/10/11

#define maxwell	
#ifdef maxwell

		char CurrentName[100];
		TEST_ITEM *pTag=NULL;
		int FindFlag=0;

		strcpy_s(CurrentName, sizeof(CurrentName), "SET_HD_SPIN_DOWN: Set HD spin down");

		if (!ParseTestItem(CurrentName,"NULL"))
		{
			return 0;
		}

		list<TEST_ITEM>::iterator Cy;
		for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
		{			
			if (!ParseTestItem(CurrentName,(*Cy).Name))
			{
				pTag=&(*Cy);
				FindFlag=1;
				if(CTestItemScript::SetHDSpinDown(pTag))
				{
					goto _CheckHD_SpindownFlag;
				}
				else
				{
					SetHDSpindownFailFlag=0;//Add by maxwell for SetHDSpindownFailFlag if stop fail 2008/10/20
					return 0;
				}
			}
		}
		if (!FindFlag)
		{
			return 0;
		}
#endif
	}
	return 0;
}
int CTestItemScript::CheckFanSpin(TEST_ITEM *pTI)
{

	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char Temp[100]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test fan error,Can not open the ACP tool;\n");
		return 0;
	}

	//Debug use
	//amprintf("Debug use ------------------------------------------------------------------------------------------------\n");
	//amprintf("%s\n",OutBuf);
	//amprintf("Debug use ------------------------------------------------------------------------------------------------\n");
	//Debug use

    pTok=strstr(OutBuf,"sySI:");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"Fan speed","</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "value", "<integer>"))
		{
			amprintf("Test fan error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect the Fan speed:%s;\n", pTI->Result);
		amprintf("Fan speed up limit:%s down limit:%s;\n", pTI->UpLimit, pTI->DownLimit);

		if ((atoi(pTI->Result)<atoi(pTI->DownLimit))||(atoi(pTI->Result)>atoi(pTI->UpLimit)))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test fan error,wrong parse;\n");
		return 0;
	}
	return 1;
}
int CTestItemScript::CheckTemperatureSensor(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
    char Temp[100]="";


	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
    pTok=strstr(OutBuf,"sySI:");
	if (pTok)
	{
		
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"Sensor temperature","</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "value", "<integer>"))
		{
			amprintf("Test fan error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect Sensor temperature:%s;\n", pTI->Result);
		amprintf("Sensor temperature up limit:%s down limit:%s;\n", pTI->UpLimit, pTI->DownLimit);

		if ((atoi(pTI->Result)<atoi(pTI->DownLimit))||(atoi(pTI->Result)>atoi(pTI->UpLimit)))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test fan error,wrong parse;\n");
		return 0;
	}
	return 1;
}
int CTestItemScript::CheckMlbTemperatureSensor(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
    char Temp[100]="";

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
    pTok=strstr(OutBuf,"sySI:");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"MLB temperature","</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "value", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect MLB temperature:%s;\n", pTI->Result);
		amprintf("MLB temperature up limit:%s down limit:%s;\n", pTI->UpLimit, pTI->DownLimit);

		if ((atoi(pTI->Result)<atoi(pTI->DownLimit))||(atoi(pTI->Result)>atoi(pTI->UpLimit)))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}

//Add start by Maxwell 2008/12/26 for check UUT status
int CTestItemScript::CheckSystemModeStatus(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
    char Temp[100]="";

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
    pTok=strstr(OutBuf,"sySt:");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"problems","</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		//if (!strstr(pTok,"waNL"))//Modified by Maxwell 090722
		if(!strstr(pTok,"waNI"))
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),"Bridge");
			//amprintf("The parameter waNL of System Mode is %s;\n",pTI->Result);
			amprintf("The parameter waNI of System Mode is %s;\n",pTI->Result);//Modified by Maxwell 090722
		}
		else
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),"Default");
			//amprintf("The parameter waNL of System Mode is %s;\n",pTI->Result);
			amprintf("The parameter waNI of System Mode is %s;\n",pTI->Result);//Modified by Maxwell 090722
		}

		amprintf("UUT System Mode Status in Config File: %s;\n", pTI->Spec);
		if (!strAllCmp(pTI->Result,pTI->Spec))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}
//Add end by Maxwell 2008/12/26 for check UUT status


//Add start by Maxwell 2009/09/28 for check UUT radio TX_txrate
int CTestItemScript::CheckandSetWifiTXrate(TEST_ITEM *pTI)
{
	char OutBuf[10240]="";
	char PingBuf[1024]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";
	char AnaTemp2[1024]="";
	char TempBuf[200]="";
	char Temp[100]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	/*FILE *fpStream = NULL;
	int err = 0;
	int findFlag = 0;
	
	if( (err = fopen_s( &fpStream, pTI->UUTCmd, "r" )) !=0 )
	{
		amprintf("Can not load adbstb's SN file.Please check the file!\n");
		return FALSE;
	}
		
	fread(OutBuf,sizeof(char), 10240, fpStream); */

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}

    pToke=strstr(OutBuf,"raSL:");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		return 0;
	}
	else
	{

	}
	pTok=strstr(pToke,"txpower");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"txrate_local","</dict>"))
		{
			amprintf("Test error,can not find wlan0;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "txrate_remote", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));	
//
		amprintf("Detect the Golden TX_txrate:%s;\n", pTI->Result);
		amprintf("Golden TX_txrate spec in config file:%s;\n", pTI->Spec);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	if (atoi(pTI->Result)!=atoi(pTI->Spec))
	{
		if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Can not perform DiagCmd ACP command tool;\n");
			return 0;
		}
		else
		{
			if(strstr(OutBuf,"outputs"))
			{
				amprintf("The MCS set success!\n");
				Sleep(1000*atoi(pTI->CmdPerfTime));
				return 0;
			}
			else
			{
				amprintf("The MCS set fail!\n");
				return 0;
			}
		}
	}
	else
	{

	}

	return 1;
}
//Add end by Maxwell 2009/09/28 for check UUT TX_txrate

//Add start by Maxwell 2008/01/19 for check and set UUT to default mode
int CTestItemScript::CheckAndSetDefault(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check wan port status

		amprintf("Wait %s time;\n",pTI->CmdPerfTime);
		Sleep(1000*atoi(pTI->CmdPerfTime));

		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"waCV: ","\r\n");
		amprintf("UUT WAN port status value: %s;\n", Temp);
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT WAN port status value: %s;\n", pTI->Result);
			amprintf("UUT WAN port status value in Config File: %s;\n", pTI->Spec);
			if (!strAllCmp(pTI->Result,pTI->Spec))
			{
				return 1;
			}
			else
			{
				char cmd[500];
				strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
				char OutBuf[100];
				DWORD dwExitCode1;


				if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode1))
				{
					return 0;
				}
				if (dwExitCode1>0)
				{
					//return 0;
				}

				amprintf("%s;\n",cmd);
				amprintf("Wait %s time;\n",pTI->DiagPerfTime);
				Sleep(1000*atoi(pTI->DiagPerfTime));

				int PingTime=0;
				amprintf("diag cmd:BPing.exe -s 10.0.1.1;\n");
				PingTime=atoi(pTI->UpLimit);

				if(!VerifyLink("BPing.exe -s 10.0.1.1", PingTime))
				{
					amprintf("Ping Fail;\n");
					return 0;
				}
				else
				{
					amprintf(" Ping %d times PASS;\n", PingTime);
					amprintf("Sleep %s seconds after Ping successful;\n",pTI->CmdPerfTime);	
					Sleep(1000*atoi(pTI->CmdPerfTime));
				}	

				return 1;
				//return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
//Add end by Maxwell 2008/01/19 for check and set UUT to default mode

//Add start by Maxwell 2008/01/19 for check UUT radio txrate 5G
int CTestItemScript::CheckTxRate5G(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char Temp[100]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	//Sleep(2000);
    pTok=strstr(OutBuf,"wlan0");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"txpower","wlan1"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "txrate", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect the WiFi Radio TxRate 5G:%s;\n", pTI->Result);
		amprintf("WiFi Radio TxRate 5G in config file :%s;\n", pTI->Spec);

		if (atoi(pTI->Result)<atoi(pTI->Spec))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}
//Add end by Maxwell 2008/01/19 for check UUT radio txrate 5G
//Add start by Maxwell 2008/01/19 for check UUT radio txrate 2.4G
int CTestItemScript::CheckTxRate2G(TEST_ITEM *pTI)
{
	char OutBuf[4096]="";
	char AnaTemp[1024]="";
	char Temp[100]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
    pTok=strstr(OutBuf,"wlan1");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"txpower","</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "txrate", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect the WiFi Radio TxRate 2.4G:%s;\n", pTI->Result);
		amprintf("WiFi Radio TxRate 2.4G in config file :%s;\n", pTI->Spec);

		if (atoi(pTI->Result)<atoi(pTI->Spec))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}
//Add end by Maxwell 2008/01/19 for check UUT radio txrate 2.4G


//Add start by Maxwell 2009/05/08 for check sn in local file
int CTestItemScript::CheckSnInFile(TEST_ITEM *pTI)
{
	/*char OutBuf[4096]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";*/
	char SNInSFIS[100]="CKZ0359190";
	/*char Temp1[100]="";
	char *pTok=NULL;
	char *pTok1=NULL;*/
	//	char Result[512];
	DWORD dwExitCode=0;

	FILE *fpStream = NULL;
	int err = 0;
	int findFlag = 0;
	char *pcRtnFGet;
	char *pToke = NULL;
	char FileLineBuf[FILE_PASER_BUF_L];

	if( (err = fopen_s( &fpStream, pTI->UUTCmd, "r" )) !=0 )
	{
		amprintf("Can not load adbstb's SN file.Please check the file!\n");
		return FALSE;
	}

	while (!feof(fpStream))
	{
		pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 
		if(strstr(pcRtnFGet,"\n"))
		{
			pToke=strstr(pcRtnFGet,"\n");
			*pToke = '\0';
		}

		if(0==strncmp(pcRtnFGet,SNInSFIS,15))
		{		
			findFlag = 1;
			amprintf("UUT SN is in SN config file, SN is logical;\n");
			break;
		}	
		else /*if (*FileLineBuf!=' ')*/
		{
			continue;
		}
	}
	fclose(fpStream);  
	if(findFlag)
		return 1;
	else
	{
		amprintf("UUT SN is not in SN config file, SN is illogical;\n");
		return 0;
	}
}
//Add end by Maxwell 2009/05/08 for check sn in local file


//627
int CTestItemScript::SetComTrans(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char revbuf[2048]=""; 
	int ComTime=50;
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("ComPort %d TransRate at %d open fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("ComPort %d TransRate at %d open pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	strcpy_s(cmd,sizeof(cmd),"./ifconfig wlan0 up\n");
	size_t m = strlen(cmd);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Cmd write %s;\n",cmd);
		//return 0;
	}
	gComDev.read(revbuf,(int)sizeof(revbuf));
	Sleep(3000);
	strcpy_s(cmd,sizeof(cmd),"./ifconfig wlan0\n");
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Cmd write %s;\n",cmd);
		//return 0;
	}
	Sleep(1000);
	gComDev.read(revbuf,(int)sizeof(revbuf));
	//gComDev.read(revbuf,(int)sizeof(revbuf));
	//Sleep(1000);
	/////*while(ComTime&&!gComDev.read(revbuf,(int)sizeof(revbuf)))
	////{
	////	Sleep(100);
	////	ComTime--;
	////}
	////if(ComTime==0)
	////{
	////	amprintf("Com read time out!\n");
	////	gComDev.close();
	////	return 0;
	////}*/

	amprintf("Com read %s;\n",revbuf);
	if(strstr(revbuf,"status"))
	{
		amprintf("The AP is in associated status\n");
	}
	else
	{
		amprintf("The AP is not in associated status!\n");
		gComDev.close();
		return 0;
	}

	//////strcpy_s(cmd,sizeof(cmd),"./ifconfig wlan0 down\n");
	//////if(!gComDev.write(cmd, (int)strlen(cmd)))
	//////{
	//////	amprintf("Cmd write %s;\n",cmd);
	//////	//return 0;
	//////}

	/*char* cmd = new char[500];
	memcpy(cmd,pTI->InsCmd,strlen(pTI->InsCmd)+1);
	printf("1:%c\n",*cmd);*/

	//HANDLE hRemote=NULL;
	
	//amprintf("%s;\n",cmd);
	/*amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	
	
	amprintf("%s;\n",cmd);*/
	//delete cmd;
	//strcat_s(cmd,sizeof(cmd),"\0");

	///*gComDev.close();*/
	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	amprintf("Wifi Config pass;\n");
	

	return 1;
}

//072609
int CTestItemScript::OpenIperfInGolden(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0;
	int ComTime=50;

	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	//strcpy_s(cmd,sizeof(cmd),char(3));
	size_t  m = strlen(cmd);
	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write %s;\n",cStop);
		//return 0;
	}
	Sleep(100);
	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write %s;\n",cStop);
		//return 0;
	}
	Sleep(200);
	gComDev.read(revbuf,(int)sizeof(revbuf));
	strcat_s(cmd,sizeof(cmd),"\n");
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	Sleep(200);
	gComDev.read(revbuf,(int)sizeof(revbuf));

	revbufLen = (int)strlen(revbuf);


	if(revbufLen<300)
	{
		amprintf("Com read %s;\n",revbuf);
	}
	else
	{

	}
	if(strstr(revbuf,"Server listening"))
	{
		amprintf("The iperf server has been started\n");
		Sleep(300);
	}
	else
	{
		amprintf("Start iperf server fail!\n");
		gComDev.close();
		return 0;
	}

	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	amprintf("Open iperf sever pass;\n");
	gComDev.close();
	
	return 1;
}
//072609


//090901
int CTestItemScript::AutoPowerOn(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0;
	int ComTime=50;

	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	//strcpy_s(cmd,sizeof(cmd),char(3));
	size_t m = strlen(cmd);

	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(200);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd);
		gComDev.close();
		return 0;
	}
	Sleep(200);
	gComDev.read(revbuf,(int)sizeof(revbuf));

	revbufLen = (int)strlen(revbuf);
	
	

	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	amprintf("UUT has been powered on;\n");
	gComDev.close();
	
	return 1;
}
//090901

//090901
int CTestItemScript::AutoPowerOff(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0;
	int ComTime=50;

	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	//strcpy_s(cmd,sizeof(cmd),char(3));
	size_t m = strlen(cmd);
	////////if(!gComDev.write(cStop, 1))
	////////{
	////////	amprintf("Com command write %s;\n",cStop);
	////////	//return 0;
	////////}
	////////else
	////////{

	////////}
	////////Sleep(100);
	////////if(!gComDev.write(cStop, 1))
	////////{
	////////	amprintf("Com command write %s;\n",cStop);
	////////	//return 0;
	////////}
	////////else
	////////{

	////////}
	////////Sleep(200);
	////////gComDev.read(revbuf,(int)sizeof(revbuf));
	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(200);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd);
		gComDev.close();
		return 0;
	}
	Sleep(200);
	gComDev.read(revbuf,(int)sizeof(revbuf));

	revbufLen = (int)strlen(revbuf);
	
	

	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	amprintf("UUT has been powered off;\n");
	gComDev.close();
	
	return 1;
}
//////////////////091005

int CTestItemScript::AutoGoldenReboot(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char cmd2[500]="";
	char cmdReboot[500]="reboot";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0;
	//int RebootTime = 1000;

	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	strcpy_s(cmd2,sizeof(cmd2),pTI->DiagCmd);
	strcpy_s(cmdReboot,sizeof(cmdReboot),"reboot");
	strcat_s(cmdReboot,sizeof(cmdReboot),"\n");
	strcat_s(cmd,sizeof(cmd),"\n");
	strcat_s(cmd2,sizeof(cmd2),"\n");

	if((pTI->RebootCount>0)&&(pTI->RebootCount%pTI->RebootTime == 0))
	{
		//reboot part----------------------------------------------------------------------------------------------------------------------
		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(pTI->ComPort,pTI->TransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);

		size_t m = strlen(cmd);

		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write  in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);

		if(!gComDev.write(cmdReboot, (int)strlen(cmdReboot)))
		{
			amprintf("Com command write  in 2.4G Golden %s;\n",cmdReboot);
			//return 0;
		}
		else
		{
			amprintf("Com command %s write in 2.4G Golden fail;\n",cmdReboot);
			gComDev.close();
			return 0;
		}

		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(pTI->ComPort_1,pTI->TransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort_1,pTI->TransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort_1,pTI->TransRate);

		m = strlen(cmd);

		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write  in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);

		if(!gComDev.write(cmdReboot, (int)strlen(cmdReboot)))
		{
			amprintf("Com command write  in 5G Golden %s;\n",cmdReboot);
			//return 0;
		}
		else
		{
			amprintf("Com command %s write in 5G Golden fail;\n",cmdReboot);
			gComDev.close();
			return 0;
		}
		gComDev.close();
		//reboot part----------------------------------------------------------------------------------------------------------------------
		Sleep(150000);
		//set part----------------------------------------------------------------------------------------------------------------------
		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(pTI->ComPort,pTI->TransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);

		m = strlen(cmd);

		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write  in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);

		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write  in 2.4G Golden %s;\n",cmd);
			//return 0;
		}
		else
		{
			amprintf("Com command %s write in 2.4G Golden fail;\n",cmd);
			gComDev.close();
			return 0;
		}
		Sleep(500);
		gComDev.read(revbuf,(int)sizeof(revbuf));

		//amprintf("Com read %s;\n",revbuf);
		if(strstr(revbuf,"outputs"))
		{
			amprintf("The MCS of 2.4G set success!\n");
		}
		else
		{
			amprintf("The MCS of 2.4G set fail!\n");
			gComDev.close();
			return 0;
		}


		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(pTI->ComPort_1,pTI->TransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort_1,pTI->TransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort_1,pTI->TransRate);

		m = strlen(cmd);

		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write  in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);

		if(!gComDev.write(cmd2, (int)strlen(cmd2)))
		{
			amprintf("Com command write  in 5G Golden %s;\n",cmd2);
			//return 0;
		}
		else
		{
			amprintf("Com command %s write in 5G Golden fail;\n",cmd2);
			gComDev.close();
			return 0;
		}

		Sleep(500);
		gComDev.read(revbuf,(int)sizeof(revbuf));

		//amprintf("Com read %s;\n",revbuf);
		if(strstr(revbuf,"outputs"))
		{
			amprintf("The MCS of 5G set success!\n");
		}
		else
		{
			amprintf("The MCS of 5G set fail!\n");
			gComDev.close();
			return 0;
		}

		pTI->RebootCount++;
		//set part----------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		pTI->RebootCount++;
		}
		
return 1;
}

//////////////////091005

int CTestItemScript::CloseComTrans(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char revbuf[2048]=""; 
	int ComTime=50;
	
	if(gComDev.is_open())
	{
		strcpy_s(cmd,sizeof(cmd),"./ifconfig wlan0 down\n");
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Cmd write %s;\n",cmd);
			//return 0;
		}

		/*char* cmd = new char[500];
		memcpy(cmd,pTI->InsCmd,strlen(pTI->InsCmd)+1);
		printf("1:%c\n",*cmd);*/

		//HANDLE hRemote=NULL;

		//amprintf("%s;\n",cmd);
		/*amprintf("Wait %s time;\n",pTI->InsCmdTime);
		Sleep(1000*atoi(pTI->InsCmdTime));


		amprintf("%s;\n",cmd);*/
		//delete cmd;
		//strcat_s(cmd,sizeof(cmd),"\0");

		gComDev.close();
	}
	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	amprintf("Com trans close pass;\n");
	

	return 1;
}
//627

int CTestItemScript::SetInstrumentStatus(TEST_ITEM *pTI)
{
	char cmd[500]="";
	/*char* cmd = new char[500];
	memcpy(cmd,pTI->InsCmd,strlen(pTI->InsCmd)+1);
	printf("1:%c\n",*cmd);*/

	HANDLE hRemote=NULL;
	
	//amprintf("%s;\n",cmd);
	/*amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	
	
	amprintf("%s;\n",cmd);*/
	//delete cmd;
	//strcat_s(cmd,sizeof(cmd),"\0");

	amprintf("%s;\n",cmd);
	amprintf("Wait %s time;\n",pTI->InsCmdTime);
	Sleep(1000*atoi(pTI->InsCmdTime));
	
	strcpy_s(cmd,sizeof(cmd),pTI->InsCmd);

	if (!OpenDev(&hRemote,pTI->InsName,QUERY_MODE,pTI->InsCtrlID))
	{
		amprintf("Run remote tool fail;\n");
		CloseDev(hRemote);
		return 0;
	}
	amprintf("cmd:%s;\n",pTI->InsCmd);
	if (!DevWrite(hRemote,pTI->InsCmd,strlen(pTI->InsCmd)))
	{
		amprintf("Run remote tool fail;\n");
		CloseDev(hRemote);
		return 0;
	}
	CloseDev(hRemote);	


	return 1;
}
int CTestItemScript::RunDiagToolNoResult(TEST_ITEM *pTI)
{
	char cmd[500];
	HANDLE hRemote=NULL;
	char OutBuf[100];
	DWORD dwExitCode;
	
	strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
	if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
	amprintf("%s wait:%d seconds;\n",cmd,atoi(pTI->DiagPerfTime));
	Sleep(1000*atoi(pTI->DiagPerfTime));

	return 1;
}
int CTestItemScript::RunDiagToolNoResultShow(TEST_ITEM *pTI)
{
	char cmd[500];
	DWORD dwWaitResult;

	//amprintf("MSG;TESTITEM=%s?\n;",pTI->UUTCmd);	//Maxwell 090520
	amprintf("MSGBOX=CONTENT[%s?]\n;",pTI->UUTCmd);	
	// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
	dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
	if (!ResetEvent(gTI.UiResMsg.hEvent)) 
	{ 
		printf("ResetEvent failed (%d)\n", GetLastError());
	} 
	strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);

	strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
	KillTargetProcess(cmd);
	runShellExecute(cmd);
	amprintf("%s wait:%d seconds;\n",cmd,atoi(pTI->DiagPerfTime));
	Sleep(1000*atoi(pTI->DiagPerfTime));

	return 1;
}
int CTestItemScript::OperatorIOInTesting(TEST_ITEM *pTI)
{

	int WaitTime=0;
    DWORD dwWaitResult;
	
	if (strlen(pTI->DiagCmd))//test led by manual
	{
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);//haibin 08.01
		//amprintf("MSG;TESTITEM=%s?\n;",pTI->DiagCmd);	  //Maxwell 090520
		amprintf("MSGBOX=CONTENT[%s?]\n;",pTI->DiagCmd);	
		// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		} 
		strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);	
		WaitTime=atoi(pTI->DiagPerfTime);
		Sleep(1000*WaitTime);
		amprintf("Wait %s seconds;\n",pTI->DiagPerfTime);
	}
	
	return 1;
}

//Add start by Tony on 2009/12/17 for K30 series geting HDSN 
int CTestItemScript::CheckHDD_SN(TEST_ITEM *pTI)
{
	char OutBuf[10000]="";
	char AnaTemp[1024]="";
	char Temp[1024]="";

	//char HDD_SN[80]="";    //Add by Tony on 091217 for send HDSN 
	char *pTok=NULL;
	
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("STS=Test error,Can not open the ACP tool;\n");
		return 0;
	}
	pTok=strstr(OutBuf,"ddSm:");
	if (pTok)
	{	
		//pTok=strstr(OutBuf,"wd0"); //for K30 Series add by Tony on 091221
		pTok=strstr(OutBuf,"sd0");  //for K30A Series add by Tony on 091221
		if (!pTok)
		{
			amprintf("STS=Test error,wrong parse;\n");
			return 0;
		}
		
		//if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"wd0","SMARTStatusKey")) //for K30 Series add by Tony on 091221
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"sd0","SMARTStatusKey")) //for K30A Series add by Tony on 091221
		{
			amprintf("STS=Test error,wrong parse;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "SMARTInfoSerialNumber", "<string>"))
		{
			amprintf("STS=Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', HDD_SN,sizeof(HDD_SN));	

		amprintf("DAT;HDSN=%s;\n", HDD_SN);  //Modify by Tony on 2009/12/17
		//amprintf("STS=Detect HDD SN:%s;\n", HDD_SN);
		//strcpy_s(pTI->Result,sizeof(pTI->Result),HDD_SN);
		//amprintf("STS=HDD SN:%s;\n", pTI->Spec);
		////Add start by tony on 2009/05/20

		////Add end by tony on 2009/05/20
		//if (!strAllCmp(pTI->Result,pTI->Spec))
		//{
		//	return 1;
		//}
		//else
		//{
		//	return 0;
		//}
	
	}
	else
	{
		amprintf("STS=Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}

int CTestItemScript::CheckminS(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
	
	CString strSN;
	//get the wireless card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get FW min Source version Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get FW min Source version Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "minS");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get FW min Source version Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,'|','`');
		amprintf("UUT FW min Source version: %s;\n", AnaTemp);

		strSN=AnaTemp;
		//strSN.MakeUpper();
		strcpy_s(pTI->Result,sizeof(pTI->Result),strSN);
	}
	amprintf("Scan UUT FM min Source version %s;\n",pTI->Spec);
	amprintf("Get UUT FM min Source version %s;\n",pTI->Result);	
	if (strSN.Compare(pTI->Spec))
	{
		return 0;
	}
	return 1;
}
int CTestItemScript::GenerateLabelInformation(TEST_ITEM *pTI)
{

	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
	char LabelInformation[1024]="";
	
	CString strSN;
	//get the wireless card SN					
	if (!RunExeFileLocal(pTI->UUTCmdAdd[1], OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get UUT SN Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get UUT SN Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "sySN");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get UUT SN Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,' ','\r');
		amprintf("UUT SN: %s;\n", AnaTemp);

		strSN=AnaTemp;
		strSN.MakeUpper();
		strSN+=",";
		strcpy_s(pTI->Result,sizeof(pTI->Result),strSN);
	}

	//get the wireless card WAN Mac
	strcpy_s(OutBuf, sizeof(OutBuf), "");
	strcpy_s(AnaTemp, sizeof(AnaTemp), "");
	strSN="";
	if (!RunExeFileLocal(pTI->UUTCmdAdd[2], OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get UUT wan mac Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get UUT wan mac Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "waMA");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get wan mac Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,' ','\r');
		amprintf("UUT Wan MAC: %s;\n", AnaTemp);

		strSN=AnaTemp;
		strSN.MakeUpper();
		strSN.Remove(':');
		strSN+=",";
		strcat_s(pTI->Result,sizeof(pTI->Result),strSN);
	}

	//get the wireless card wifi Mac
	strcpy_s(OutBuf, sizeof(OutBuf), "");
	strcpy_s(AnaTemp, sizeof(AnaTemp), "");
	strSN="";
	if (!RunExeFileLocal(pTI->UUTCmdAdd[3], OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("ERROR! Can not find acp.exe!;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get AP wireless card MAC ID Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "raMA");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("Get wifi mac Fail;\n");
			return 0;
		}

		strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,' ','\r');
		amprintf("UUT Wifi MAC: %s;\n", AnaTemp);
		
		if (strchr(AnaTemp,':'))
		{
			char GetMAC[200]="";
			//strSTEDcpy(GetMAC,sizeof(GetMAC),AnaTemp,' ','\r');
			strSN=AnaTemp;
			strSN.Remove(':');
		}
		else
		{
			strSN=AnaTemp;
		}
	
		strSN.MakeUpper();
		strcat_s(pTI->Result,sizeof(pTI->Result),strSN);
	}
	

	//////get the wireless card wifi Mac 1
	////strcpy_s(OutBuf, sizeof(OutBuf), "");
	////strcpy_s(AnaTemp, sizeof(AnaTemp), "");
	////strSN="";
	////if (!RunExeFileLocal("acp -a 10.0.1.1 getplistvalue WiFi radios.[1].raMA", OutBuf, sizeof(OutBuf), &dwExitCode))
	////{
	////	amprintf("ERROR! Can not find acp.exe!;\n");
	////	return 0;		
	////}
	////else
	////{		
	////	pToke=NULL;
	////	pToke = strstr(OutBuf, "failed:");
	////	if (pToke)
	////	{
	////		amprintf("%s;", OutBuf);
	////		amprintf("Get AP wireless card MAC 1 ID Fail;\n");
	////		return 0;
	////	}

	////	strSTEDcpy(AnaTemp,sizeof(AnaTemp),OutBuf,'@','\r');
	////	amprintf("Wirelesscard MAC 1: %s;\n", AnaTemp);
	////

	////	if (strchr(AnaTemp,':'))
	////	{
	////		char GetMAC[200]="";
	////		strSTEDcpy(GetMAC,sizeof(GetMAC),AnaTemp,' ','\r');
	////		strSN=GetMAC;
	////		strSN.Remove(':');
	////	}
	////	else
	////	{
	////		strSN=AnaTemp;
	////	}
	////
	////	strSN.MakeUpper();
	////	strcat_s(pTI->Result,sizeof(pTI->Result),strSN);
	////}

	FILE *fLog;	
	
	char PathBuf[256]="";
	char LogName[256]="";
	char FileName[512]="";
	int iFileOpenErr = 0;
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\LabelMessage\\", PathBuf);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}

		CCenTime Time;
		char TempBuf[100];
		Time.DataFormatC(TempBuf,sizeof(TempBuf),gTI.UUTInfo.TestDate);
		strcpy_s(LogName,sizeof(LogName),TempBuf);
		strcat_s(LogName,sizeof(LogName),".csv");

        strcat_s(FileName, sizeof(FileName), LogName);
		//Maxwell 090622 mutex operate log
		WaitForSingleObject(hWriteLabelMutex, INFINITE); 
		Sleep(1);
		iFileOpenErr = fopen_s(&fLog, FileName, "a+");
		if(NULL == fLog)
		{
			return FALSE;
		}
		fprintf_s(fLog,"%s\n",pTI->Result);
		fclose(fLog);
		ReleaseMutex(hWriteLabelMutex);
	}
	
	return 1;
}

//Modify by Maxwell 20110331 for check K31 RSSI	//add offset by Talen 2011/06/13
int CTestItemScript::CheckUUTRSSI(TEST_ITEM *pTI)
{
	char OutBuf[10240]="";
	char GetBuf[5120]="";
	char AnaTemp[1024]="";
	char TempBuf[200]="";
	char Temp[100]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	amprintf("%s;\n",pTI->UUTCmd);
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}

    pToke=strstr(OutBuf,"raSL:");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		amprintf("%s;\n",OutBuf);
		return 0;
	}
	else
	{

	}

	if(!strAllCmp("2G",pTI->Spec_1))
	{
		if (!GetsubStrInStr(GetBuf, sizeof(GetBuf),pToke,"wlan0","wlan1"))
		{
			amprintf("Parser fail;\n");
			amprintf("%s;\n",OutBuf);
			return 0;
		}
		pTok=strstr(pToke,"wlan0");
	}
	else if(!strAllCmp("5G",pTI->Spec_1))
	{
		pTok=strstr(pToke,"wlan1");
	}
	else
	{
		amprintf("SPEC_1 in config is not 2G or 5G, Please check!\n");
		return 0;
	}

	amprintf("Check %s RSSI;\n",pTI->Spec_1);
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"<key>rssi_chain</key>","<key>rssi_local</key>"))
		{
			amprintf("Parser fail between rssi_chain and rssi_local;\n");
			amprintf("%s;\n",OutBuf);
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "<array>", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			amprintf("%s;\n",OutBuf);
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->MultiResult[0],sizeof(pTI->MultiResult[0]));
		sprintf_s(pTI->MultiResult[2],sizeof(pTI->MultiResult[2]),"%.2f",atof(pTI->MultiResult[0])-atof(pTI->MultiSpec[0]));//add by Talen 2011/06/13

		pToke = strstr(AnaTemp,"<integer>");	
		pToke = strstr(pToke,"</integer>");
		pToke = strstr(pToke,"<integer>");
		if (!pToke)
		{
			amprintf("Only one rssi_chain value return!;\n");
			return 0;
		}
		else
		{

		}
		strncpy_s(pTI->MultiResult[1],sizeof(pTI->MultiResult[1]),pToke+9,4);
		sprintf_s(pTI->MultiResult[3],sizeof(pTI->MultiResult[3]),"%.2f",atof(pTI->MultiResult[1])-atof(pTI->MultiSpec[1]));//add by Talen 2011/06/13


		amprintf("Detect the WiFi rssi_chain:%d,%d\n", atoi(pTI->MultiResult[0]), atoi(pTI->MultiResult[1]));
		amprintf("WiFi rssi_chain after offset:%.2f,%.2f\n", atof(pTI->MultiResult[2]), atof(pTI->MultiResult[3]));//add by Talen 2011/06/13
		
		sprintf_s(TempBuf,sizeof(TempBuf),"%d,%d,%.2f,%.2f",atoi(pTI->MultiResult[0]), atoi(pTI->MultiResult[1]),atof(pTI->MultiResult[2]), atof(pTI->MultiResult[3]));

		strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);

	}
	else
	{
		amprintf("Test error,wrong parse;\n");                             
		return 0;
	}
	//Maxwell 20110216 RSSI Test modify

	//Maxwell change 20110214
	if ((atof(pTI->MultiResult[2])>atof(pTI->UpLimit))||(atof(pTI->MultiResult[3])>atof(pTI->UpLimit)))
	{
		amprintf("Wlan rssi_chain spec is up:%s,down:%s;\n",pTI->UpLimit,pTI->DownLimit);
		return 0;
	}
	else
	{
		//amprintf("Wlan0 rssi_chain test pass!\n");
	}
	if ((atof(pTI->MultiResult[2])<atof(pTI->DownLimit))||(atof(pTI->MultiResult[3])<atof(pTI->DownLimit)))
	{
		amprintf("Wlan rssi_local spec is up:%s,down:%s;\n",pTI->UpLimit,pTI->DownLimit);
		return 0;
	}
	
	else
	{
		//amprintf("Wlan0 rssi_chain test pass!\n");
	}
	if ((fabsf(atof(pTI->MultiResult[2])-atof(pTI->MultiResult[3]))>atof(pTI->Spec)))
	{
		amprintf("The RSSI data from 2 chain not meet the difference spec:%s;\n",pTI->Spec);
		return 0;
	}

	else
	{
		//amprintf("Wlan0 rssi_chain test pass!\n");
	}
//Maxwell change 20110214

	return 1;
}
//Modify by Maxwell 20110331 for check K31 RSSI	//add offset by Talen 2011/06/13

//Add end by Maxwell 2010/02/11 for check UUT RSSI
int CTestItemScript::CheckGOLDENRSSI2G(TEST_ITEM *pTI)
{
	char OutBuf[10240]="";
	char PingBuf[1024]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";
	char AnaTemp2[1024]="";
	char TempBuf[200]="";
	char Temp[100]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}

    pToke=strstr(OutBuf,"raSL:");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		return 0;
	}
	else
	{

	}
	pTok=strstr(pToke,"wlan1");

	if (pTok)
	{
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"rssi_chain","rssi_remote"))
		{
			amprintf("No wlan1 value;\n");
			return 0;
		}

		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "rssi_local", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}

		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));	
		amprintf("Wlan1 rssi_local is %s;\n",pTI->Result);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	if ((atoi(pTI->Result)>atoi(pTI->UpLimit))||(atoi(pTI->Result)<atoi(pTI->DownLimit)))
	{
		amprintf("Wlan1 rssi_local spec is up:%s,down:%s;\n",pTI->UpLimit,pTI->DownLimit);
		return 0;
	}
	else
	{
		amprintf("Wlan1 rssi_local test pass!\n");
	}
	return 1;
}
//Add end by Maxwell 2010/02/11 for check UUT RSSI

//Add end by Maxwell 2010/02/11 for check UUT RSSI
int CTestItemScript::CheckGOLDENRSSI5G(TEST_ITEM *pTI)
{
	char OutBuf[10240]="";
	char PingBuf[1024]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";
	char AnaTemp2[1024]="";
	char TempBuf[200]="";
	char Temp[100]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}

    pToke=strstr(OutBuf,"raSL:");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		return 0;
	}
	else
	{

	}
	pTok=strstr(pToke,"wlan3");

	if (pTok)
	{
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"rssi_chain","rssi_remote"))
		{
			amprintf("No wlan3 value;\n");
			return 0;
		}

		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "rssi_local", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}

		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));	
		amprintf("Wlan3 rssi_local is %s;\n",pTI->Result);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	if ((atoi(pTI->Result)>atoi(pTI->UpLimit))||(atoi(pTI->Result)<atoi(pTI->DownLimit)))
	{
		amprintf("Wlan3 rssi_local spec is up:%s,down:%s;\n",pTI->UpLimit,pTI->DownLimit);
		return 0;
	}
	else
	{
		amprintf("Wlan3 rssi_local test pass!\n");
	}
	return 1;
}
//Add end by Maxwell 2010/02/11 for check UUT RSSI



//110212 Maxwell
int CTestItemScript::ControlGoldenStatus(TEST_ITEM *pTI)
{
	strcpy_s(pTI->Result,sizeof(pTI->Result),"Fail");//add by Talen 2011/04/19
	char cmd[512]="";
	char cmd2[512]="";
	char cmdReboot[512]="";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0; 
	
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	strcpy_s(cmd2,sizeof(cmd2),pTI->DiagCmd);
	strcpy_s(cmdReboot,sizeof(cmdReboot),"acp acRI=");
	strcat_s(cmdReboot,sizeof(cmdReboot),"\n");
	strcat_s(cmd,sizeof(cmd),"\n");
	strcat_s(cmd2,sizeof(cmd2),"\n");

	if(gComDev.is_open())
	{
		gComDev.close();
	}

	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);

	size_t m = strlen(cmd);

	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write:%s;\n",cStop);
		//return 0;
	}
	Sleep(50);
	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write:%s;\n",cStop);
		//return 0;
	}
	Sleep(50);

	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("%s Com command write: %s;\n",pTI->Name,cmd);
		//return 0;
	}
	else
	{
		amprintf("%s Com command %s write fail;\n",pTI->Name,cmd);
		gComDev.close();
		return 0;
	}

	if(!gComDev.write(cmd2, (int)strlen(cmd2)))
	{
		amprintf("%s Com command write: %s;\n",pTI->Name,cmd2);
		//return 0;
	}
	else
	{
		amprintf("%s Com command %s write fail;\n",pTI->Name,cmd2);
		gComDev.close();
		return 0;
	}

	if(!gComDev.write(cmdReboot, (int)strlen(cmdReboot)))
	{
		amprintf("Com command Reboot write: %s;\n",cmdReboot);
		//return 0;
	}
	else
	{
		amprintf("Com command Reboot %s write to Golden fail;\n",cmdReboot);
		gComDev.close();
		return 0;
	}

	//set part----------------------------------------------------------------------------------------------------------------------
	if(gComDev.is_open())
	{
		gComDev.close();
	}

	strcpy_s(pTI->Result,sizeof(pTI->Result),"Pass");

	return 1;
}
//110212 Maxwell

//110212 Maxwell
int CTestItemScript::SendGoldenMCSCMD(TEST_ITEM *pTI)
{
	strcpy_s(pTI->Result,sizeof(pTI->Result),"Fail");//add by Talen 2011/04/19
	char cmd[512]="";
	char cStop[2]={0x03};
	char revbuf[8192]=""; 
	int revbufLen = 0;
	
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	strcat_s(cmd,sizeof(cmd),"\n");

	if(gComDev.is_open())
	{
		gComDev.close();
	}

	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);

	size_t m = strlen(cmd);

	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write:%s;\n",cStop);
		//return 0;
	}
	Sleep(50);
	if(!gComDev.write(cStop, 1))
	{
		amprintf("Com command write:%s;\n",cStop);
		//return 0;
	}
	Sleep(50);

	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("%s Com command write: %s;\n",pTI->Name,cmd);
		//return 0;
	}
	else
	{
		amprintf("%s Com command %s write fail;\n",pTI->Name,cmd);
		gComDev.close();
		return 0;
	}

	Sleep(500);
	gComDev.read(revbuf,(int)sizeof(revbuf));

	amprintf("Com read %s;\n",revbuf);
	if(strstr(revbuf,"outputs"))
	{
		amprintf("The MCS set success!\n");
	}
	else
	{
		amprintf("The MCS set fail!\n");
		gComDev.close();
		return 0;
	}

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/18

	//set part----------------------------------------------------------------------------------------------------------------------
	if(gComDev.is_open())
	{
		gComDev.close();
	}

	return 1;
}
//110212 Maxwell

//Add start by Sam 2011/03/11 for log UUT cal data
int CTestItemScript::LogUUTCalData(TEST_ITEM *pTI)
{
	DWORD dwExitCode=0;
	char OutBuf[20480]="";
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}
	//strcpy(gTI.UUTInfo.TestDate,"03/10/2011");
	//strcpy(gTI.UUTInfo.TestTime,"12:21:41");
	char cal_log_filename[256]="CAL_DATA.txt";
	char datetime[256]="";
	int ret=DateTimeFormat(gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,datetime);
	if(ret)
	{
		CTReport.LogDataCAL(gTI.UUTInfo.UUTSN,datetime,cal_log_filename,OutBuf);
		return 1;
	}
	else
	{
		amprintf("Test error,can not get test start time;\n");
		return 0;
	}
	return 0;
}

//Add end by Sam 2011/03/11 for log UUT cal data

//Add start by Maxwell 2011/02/16 for check UUT radio TX_txrate
int CTestItemScript::CheckUUTTXrate(TEST_ITEM *pTI)
{
	char OutBuf[10240]="";
	char PingBuf[1024]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";
	char AnaTemp2[1024]="";
	char TempBuf[200]="";
	char Temp[100]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	/*FILE *fpStream = NULL;
	int err = 0;
	int findFlag = 0;
	
	if( (err = fopen_s( &fpStream, pTI->UUTCmd, "r" )) !=0 )
	{
		amprintf("Can not load adbstb's SN file.Please check the file!\n");
		return FALSE;
	}
		
	fread(OutBuf,sizeof(char), 10240, fpStream); */

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Can not perform ACP command tool;\n");
		return 0;
	}

    pToke=strstr(OutBuf,"raSL:");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		return 0;
	}
	else
	{

	}
	pTok=strstr(pToke,"txpower");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"txrate_local","</dict>"))
		{
			amprintf("Test error,can not find wlan0;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "txrate_remote", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));	
//
		amprintf("Detect the Golden TX_txrate:%s;\n", pTI->Result);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	return 1;
}
//Add end by Maxwell 2011/02/16 for check UUT radio TX_txrate


//Maxwell 20110330 for check UUT radio nf_chain K31
int CTestItemScript::CheckWifiNoiseFloor(TEST_ITEM *pTI)
{
	char OutBuf[20480]="";
	char PingBuf[1024]="";
	char AnaTemp[1024]="";
	char AnaTemp1[1024]="";
	char AnaTemp2[1024]="";
	char TempBuf[256]="";
	char Temp[128]="";
	char *pTok=NULL;
	char *pToke=NULL;
	char *pcRtnFGet=NULL;
	DWORD dwExitCode=0;

	FILE *fpStream = NULL;
	int err = 0;
	int findFlag = 0;
	//add start by Talen 2011/08/06
	if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}

	if(GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"ssSK: ","\r\n"))
	{
		if(strstr(Temp,"error"))
		{
			amprintf("%s;\n",OutBuf);
			return 0;
		}
		else if(!strlen(Temp))
		{
		
		}
		else if(strstr(pTI->Spec,Temp))
		{
		
		}
		else if(strstr(pTI->Spec,"Undefine SPEC"))
		{
		
		}
		else
		{
			amprintf("The DUT SKU Can not support the channel;\n");
			return 1;
		}
	}
	else
	{
		amprintf("%s;\n",OutBuf);
		return 0;
	}
	//add end by Talen 2011/08/06
	memset(OutBuf,0,sizeof(OutBuf));
	dwExitCode=0;
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	char nf_log_filename[512]="";
	int nCh=0;
	int wifimode=-1;
	if (strstr(pTI->UUTCmd,"index:i:"))
	{
		char *pos=strstr(pTI->UUTCmd,"index:i:")+strlen("index:i:");
		if(pos)
		{
			wifimode=atoi(pos);
		}
		else
		{
			amprintf("Test CMD ERROR!;\n");
			return 0;
		}
	}
	else
	{
		amprintf("Test CMD ERROR!;\n");
		return 0;
	}

	int nCh2G=0;
	int nCh5G=0;
	char WiFiBuf[20480]="";
	char WiFiCMD[256]="acp.exe -a 10.0.1.1 -p public WiFi";
	if (!RunExeFileLocal(WiFiCMD, WiFiBuf, sizeof(WiFiBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	int ret=GetCurrentCh(WiFiBuf,nCh2G,nCh5G);
	if(ret==0)
	{
		amprintf("Can not get Channel Info\n");
		return 0;
	}
	if(wifimode==0)
	{
		sprintf(nf_log_filename,"NF_%dG_CH_%03d.txt",2,nCh2G);	
	}
	else if(wifimode==1)
	{
		sprintf(nf_log_filename,"NF_%dG_CH_%03d.txt",5,nCh5G);
	}
	
    char datetime[256]="";

	 ret=DateTimeFormat(gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,datetime);
	 if(ret)
	 {
		 CTReport.LogDataNF(gTI.UUTInfo.UUTSN,datetime,nf_log_filename,OutBuf);
	 }
	 else
	 {
		 amprintf("Test error,can not get test start time;\n");
		 return 0;
	 }
	
////debug use
//	if( (err = fopen_s( &fpStream, pTI->DiagCmd, "r" )) !=0 )
//	{
//		amprintf("Can not load adbstb's SN file.Please check the file!\n");
//		return FALSE;
//	}
//		
//	fread(OutBuf,sizeof(char), 10240, fpStream); 
//
////debug use

	//Sleep(2000);
    pToke=strstr(OutBuf,"plist version=");
	if (!pToke)
	{
		amprintf("Run acp raSL fail,no value return!;\n");
		return 0;
	}
	else
	{

	}
	pTok=strstr(pToke,"outputs");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"data","<key>factory-cal-nf-ext</key>"))
		{
			amprintf("Test error,can not find cal-nf-ctl test data;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "<key>factory-cal-nf-ctl</key>", "<string>"))
		{
			amprintf("Test error,wrong parse, no chain A cal-nf-ctl;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->MultiResult[0],sizeof(pTI->MultiResult[0]));	

		pToke = strstr(AnaTemp,"<string>");	
		pToke = strstr(pToke,"</string>");
		pToke = strstr(pToke,"<string>");
		if (!pToke)
		{
			amprintf("Only one nf_chain value of cal-nf-ctl return!;\n");
			return 0;
		}
		else
		{

		}
		strncpy_s(pTI->MultiResult[1],sizeof(pTI->MultiResult[1]),pToke+8,7);

		amprintf("Detect the WiFi nf_chain cal-nf-ctl:chain A: %.2f,chain B: %.2f;\n", atof(pTI->MultiResult[0]), atof(pTI->MultiResult[1]));
		//amprintf("WiFi Radio Status cal-nf nf_chain in config file :%s,%s,%s;\n", pTI->MultiSpec[0],pTI->MultiSpec[1],pTI->MultiSpec[2]);
		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,%.2f,",atof(pTI->MultiResult[0]), atof(pTI->MultiResult[1]));

		strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);

	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	//
	pTok=strstr(OutBuf,"<key>factory-cal-nf-ctl</key>");
	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"<key>factory-cal-nf-ext</key>","<key>median-pwr-ext</key>"))
		{
			amprintf("Test error,can not find median-pwr-ctl;\n");
			return 0;
		}
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "<key>median-pwr-ctl</key>", "<string>"))
		{
			amprintf("Test error,wrong parse,no chain A cal-nf-ext;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->MultiResult[2],sizeof(pTI->MultiResult[2]));	

		pToke = strstr(AnaTemp,"<key>median-pwr-ctl</key>");
		pToke = strstr(pToke,"<string>");
		pToke = strstr(pToke,"</string>");
		pToke = strstr(pToke,"<string>");
		if (!pToke)
		{
			amprintf("Only one nf_chain value of median-pwr-ctl return!;\n");
			return 0;
		}
		else
		{

		}
		strncpy_s(pTI->MultiResult[3],sizeof(pTI->MultiResult[3]),pToke+8,7);
		
		amprintf("Detect the WiFi nf_chain median-pwr-ctl:chain A: %.2f,chain B: %.2f;\n", atof(pTI->MultiResult[2]), atof(pTI->MultiResult[3]));

		sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,%.2f",atof(pTI->MultiResult[2]), atof(pTI->MultiResult[3]));//add a "," by Talen 2011/04/03

		strcat_s(pTI->Result,sizeof(pTI->Result),TempBuf);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	if(!wifimode)
	{

	}
	else
	{
		strcat_s(pTI->Result,sizeof(pTI->Result),",");
		amprintf("Test in 5G Noise floor!;\n");

		pTok=strstr(OutBuf,"outputs");
		if (pTok)
		{

			if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"<key>factory-cal-nf-ctl</key>","<key>median-pwr-ctl</key>"))
			{
				amprintf("Test error,can not find cal-nf-ext test data;\n");
				strcat_s(pTI->Result,sizeof(pTI->Result),",,,");//add by Talen 2011/04/12
				return 0;
			}

			if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "<key>factory-cal-nf-ext</key>", "<string>"))
			{
				amprintf("Test error,wrong parse, no chain A cal-nf-ext;\n");
				strcat_s(pTI->Result,sizeof(pTI->Result),",,,");//add by Talen 2011/04/12
				return 0;
			}
			GetXmlIndexStr(Temp, 1, ',', pTI->MultiResult[4],sizeof(pTI->MultiResult[4]));	

			pToke = strstr(AnaTemp,"<key>factory-cal-nf-ext</key>");
			pToke = strstr(pToke,"<string>");	
			pToke = strstr(pToke,"</string>");
			pToke = strstr(pToke,"<string>");
			if (!pToke)
			{
				amprintf("Only one nf_chain value of cal-nf-ext return!;\n");
				return 0;
			}
			else
			{

			}
			strncpy_s(pTI->MultiResult[5],sizeof(pTI->MultiResult[5]),pToke+8,7);

			amprintf("Detect the WiFi nf_chain cal-nf-ext:chain A: %.2f,chain B: %.2f;\n", atof(pTI->MultiResult[4]), atof(pTI->MultiResult[5]));
			//amprintf("WiFi Radio Status cal-nf nf_chain in config file :%s,%s,%s;\n", pTI->MultiSpec[0],pTI->MultiSpec[1],pTI->MultiSpec[2]);
			sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,%.2f,",atof(pTI->MultiResult[4]), atof(pTI->MultiResult[5]));

			strcat_s(pTI->Result,sizeof(pTI->Result),TempBuf);

		}
		else
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}

		//
		pTok=strstr(OutBuf,"<key>median-pwr-ctl</key>");
		if (pTok)
		{

			if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,"<key>median-pwr-ctl</key>","uncal-nf-ctl"))
			{
				amprintf("Test error,can not find median-pwr-ext;\n");
				return 0;
			}

			if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "<key>median-pwr-ext</key>", "<string>"))
			{
				amprintf("Test error,wrong parse,no chain A median-pwr-ext;\n");
				return 0;
			}
			GetXmlIndexStr(Temp, 1, ',', pTI->MultiResult[6],sizeof(pTI->MultiResult[6]));	

			pToke = strstr(AnaTemp,"<key>median-pwr-ext</key>");
			pToke = strstr(pToke,"<string>");
			pToke = strstr(pToke,"</string>");
			pToke = strstr(pToke,"<string>");
			if (!pToke)
			{
				amprintf("Only one nf_chain value of median-pwr-ext return!;\n");
				return 0;
			}
			else
			{

			}
			strncpy_s(pTI->MultiResult[7],sizeof(pTI->MultiResult[7]),pToke+8,7);

			amprintf("Detect the WiFi nf_chain median-pwr-ext:chain A: %.2f,chain B: %.2f;\n", atof(pTI->MultiResult[6]), atof(pTI->MultiResult[7]));

			sprintf_s(TempBuf,sizeof(TempBuf),"%.2f,%.2f",atof(pTI->MultiResult[6]), atof(pTI->MultiResult[7]));

			strcat_s(pTI->Result,sizeof(pTI->Result),TempBuf);
		}
		else
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
	}

	if (atof(pTI->MultiResult[0])>atof(pTI->MultiSpec[0]))
	{
		amprintf("Chain A cal-nf-ctl fail, spec is  %.2f;\n",atof(pTI->MultiSpec[0]));
		return 0;
	}
	else
	{

	}
	if (atof(pTI->MultiResult[1])>atof(pTI->MultiSpec[1]))
	{
		amprintf("Chain B cal-nf-ctl fail, spec is %.2f;\n",atof(pTI->MultiSpec[1]));
		return 0;
	}
	else
	{

	}
	if (atof(pTI->MultiResult[2])>atof(pTI->MultiSpec[2]))
	{
		amprintf("Chain A median-pwr-ctl fail, spec is  %.2f;\n",atof(pTI->MultiSpec[2]));
		return 0;
	}
	else
	{

	}
	if (atof(pTI->MultiResult[3])>atof(pTI->MultiSpec[3]))
	{
		amprintf("Chain B median-pwr-ctl fail, spec is %.2f;\n",atof(pTI->MultiSpec[3]));
		return 0;
	}
	else
	{

	}

	if(!wifimode)
	{
		
	}
	else
	{
		amprintf("Test in 5G Noise floor!;\n");

		if (atof(pTI->MultiResult[4])>atof(pTI->MultiSpec[4]))
		{
			amprintf("Chain A cal-nf-ext fail, spec is  %.2f;\n",atof(pTI->MultiSpec[4]));
			return 0;
		}
		else
		{

		}
		if (atof(pTI->MultiResult[5])>atof(pTI->MultiSpec[5]))
		{
			amprintf("Chain B cal-nf-ext fail, spec is %.2f;\n",atof(pTI->MultiSpec[5]));
			return 0;
		}
		else
		{

		}
		if (atof(pTI->MultiResult[6])>atof(pTI->MultiSpec[6]))
		{
			amprintf("Chain A median-pwr-ext fail, spec is  %.2f;\n",atof(pTI->MultiSpec[6]));
			return 0;
		}
		else
		{

		}
		if (atof(pTI->MultiResult[7])>atof(pTI->MultiSpec[7]))
		{
			amprintf("Chain B median-pwr-ext fail, spec is %.2f;\n",atof(pTI->MultiSpec[7]));
			return 0;
		}
		else
		{

		}
	}

	return 1;
}
//Maxwell 20110330 for check UUT radio nf_chain K31

//Maxwell 20110326
int CTestItemScript::CheckK31TempSensor(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[1024]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
    char Temp[128]="";
	char tempTSResult[256]="";
	double DeltaTemp=0;
	double initialTemp=0;
	double cal=0;

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}

    pTok=strstr(OutBuf,"sySI:");
	if (pTok)
	{
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,pTI->DiagCmd/*Used for items to parser*/,"</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "value", "<integer>"))
		{
			amprintf("Test error,decimal wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect %s decimal value:%s;\n", pTI->DiagCmd, pTI->Result);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
	}
		
	pTok=strstr(OutBuf,"sySI:");
	if (pTok)
	{
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,pTI->DiagCmd,"</dict>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}


		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), "thousandths", "<integer>"))
		{
			amprintf("Test error,thousandths wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', tempTSResult,sizeof(tempTSResult));		
		amprintf("Detect %s thousandths value:%s;\n", pTI->DiagCmd, tempTSResult);
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	strcat_s(pTI->Result,sizeof(pTI->Result),".");
	strcat_s(pTI->Result,sizeof(pTI->Result),tempTSResult);

	//add start by Talen 2011/07/06
	if(!pTI->CmpFlag)
	{
		if(strstr(pTI->DiagCmd,"Local Temp 1"))
		{
			strcpy_s(gTI.UUTInfo.LocalTemp1,sizeof(gTI.UUTInfo.LocalTemp1),pTI->Result);
			localTempTime.TimeStartCount();//add by Talen 2012/02/20
		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 1"))
		{
			strcpy_s(gTI.UUTInfo.RemoteTemp1,sizeof(gTI.UUTInfo.RemoteTemp1),pTI->Result);
		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 2"))
		{
			strcpy_s(gTI.UUTInfo.RemoteTemp2,sizeof(gTI.UUTInfo.RemoteTemp2),pTI->Result);
		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 3"))
		{
			strcpy_s(gTI.UUTInfo.RemoteTemp3,sizeof(gTI.UUTInfo.RemoteTemp3),pTI->Result);
		}
		else
		{
		
		}
	}
	else
	{
		if(strstr(pTI->DiagCmd,"Local Temp 1"))
		{
			float curLoaclTemp=atof(pTI->Result);//add by Talen 2012/02/20
			
			//add start by Talen 2011/08/10
			initialTemp=atof(gTI.UUTInfo.LocalTemp1);
			DeltaTemp=atof(pTI->Result)-atof(gTI.UUTInfo.LocalTemp1);
			//cal=0.2628*initialTemp+DeltaTemp;
			cal=0.4581*initialTemp+DeltaTemp;
			char ppTemp[124]="";
			amprintf("Delta_Temp + 0.4581*initial_temp;\n");
			sprintf_s(ppTemp,sizeof(ppTemp),"%f\0",cal);
			strcat_s(pTI->Result,sizeof(pTI->Result),",");
			strcat_s(pTI->Result,sizeof(pTI->Result),ppTemp);

			char deltaTime[64]="";
			localTempTime.GetElapseTime(deltaTime,sizeof(deltaTime));
			strcat_s(pTI->Result,sizeof(pTI->Result),",");
			strcat_s(pTI->Result,sizeof(pTI->Result),deltaTime);


			if(fabs(curLoaclTemp-atof(gTI.UUTInfo.LocalTemp1))>atof(pTI->Spec))
			{
				amprintf("%s privious value: %s;\n",pTI->DiagCmd,gTI.UUTInfo.LocalTemp1);
				amprintf("%s current value: %f;\n",pTI->DiagCmd,curLoaclTemp);
				amprintf("Current and Previous %s value can not meet the difference spec %s;\n",pTI->DiagCmd,pTI->Spec);
				return 0;
			}
			else
			{
			}

			if((cal>=atof(pTI->MultiSpec[1]))||(cal<=atof(pTI->MultiSpec[0])))
			{
				amprintf("calculated result is %f, not in (%s,%s);\n",cal,pTI->MultiSpec[0],pTI->MultiSpec[1]);
				return 0;
			}
			else
			{
				amprintf("calculated result is %f, in (%s,%s);\n",cal,pTI->MultiSpec[0],pTI->MultiSpec[1]);
			}
			//add end by Talen 2011/08/10

		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 1"))
		{
			if(fabs(atof(pTI->Result)-atof(gTI.UUTInfo.RemoteTemp1))>atof(pTI->Spec))
			{
				amprintf("%s privious value: %s;\n",pTI->DiagCmd,gTI.UUTInfo.RemoteTemp1);
				amprintf("%s current value: %s;\n",pTI->DiagCmd,pTI->Result);
				amprintf("Current and Previous %s value can not meet the difference spec %s;\n",pTI->DiagCmd,pTI->Spec);
				return 0;
			}
			else
			{
			}
		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 2"))
		{
			if(fabs(atof(pTI->Result)-atof(gTI.UUTInfo.RemoteTemp2))>atof(pTI->Spec))
			{
				amprintf("%s privious value: %s;\n",pTI->DiagCmd,gTI.UUTInfo.RemoteTemp2);
				amprintf("%s current value: %s;\n",pTI->DiagCmd,pTI->Result);
				amprintf("Current and Previous %s value can not meet the difference spec %s;\n",pTI->DiagCmd,pTI->Spec);
				return 0;
			}
			else
			{
			}
		}
		else if(strstr(pTI->DiagCmd,"Remote Temp 3"))
		{
			if(fabs(atof(pTI->Result)-atof(gTI.UUTInfo.RemoteTemp3))>atof(pTI->Spec))
			{
				amprintf("%s privious value: %s;\n",pTI->DiagCmd,gTI.UUTInfo.RemoteTemp3);
				amprintf("%s current value: %s;\n",pTI->DiagCmd,pTI->Result);
				amprintf("Current and Previous %s value can not meet the difference spec %s;\n",pTI->DiagCmd,pTI->Spec);
				return 0;
			}
			else
			{
			}
		}
		else
		{
		
		}
	}
	//add end by Talen 2011/07/06

	amprintf("%s up limit:%s down limit:%s;\n", pTI->DiagCmd, pTI->UpLimit, pTI->DownLimit);

	if ((atof(pTI->Result)<atof(pTI->DownLimit))||(atof(pTI->Result)>atof(pTI->UpLimit)))
	{
		return 0;
	}

	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//Maxwell 20110326

//Talen 2011/05/11
int CTestItemScript::RunIQProgram(TEST_ITEM *pTI)
{
	HWND ToolWin=NULL;
	HWND ToolWin1=NULL;

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20
	
	ToolWin = ::FindWindow(NULL,pTI->Spec);//Sepc define for find window
	if (!ToolWin)
	{

		//if(!RunExeFileLocalShowNoWait(cmd,OutBuf,sizeof(OutBuf),&dwExitCode))
		//{
		//	amprintf("Can not open the IQ.bat;\n");
		//	return 0;
		//}

		if (runShellExecute(pTI->UUTCmd))
		{
			amprintf("Can not open the IQ.bat;\n");
			return 0;		
		}
		int waittime=20;
		while(waittime)
		{
			if(ToolWin1=::FindWindow(NULL,pTI->Spec))
			{
				break;
			}
			Sleep(100);
			waittime--;
		}
		if(!ToolWin1)
		{
			amprintf("No cmd line found, test fail!");
			return 0;
		}
		else
		{
			::SendMessage(ToolWin1,WM_CHAR,'y',1);//change 'r' to 'y' by Talen 2011/07/09
		}
	}
	else
	{
			::SendMessage(ToolWin,WM_CHAR,'y',1);//change 'r' to 'y' by Talen 2011/07/09
	}

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//Talen 2011/05/11


//Maxwell 20110326
int CTestItemScript::DeletePathFile(TEST_ITEM *pTI)
{
	char cmdBuf[512]="";
	int revbufLen = 0;
	char cmd[512]="";
	//int ComTime=50;
//	errno_t err;

	//strcpy_s(cmdBuf,sizeof(cmdBuf),pTI->UUTCmd); //Comment by Justin 20101022
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20

	sprintf_s(cmdBuf,sizeof(cmdBuf),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->UUTCmd);

	//FILE *fLog;	
	
	char PathBuf[512]="";
	char FileName[512]="";
	int iFileOpenErr = 0;
	//E:\wxpeng\M35F_ART_PT\art\bin\558-PT.log
	//PathBuf=E:\wxpeng\M35F_ART_PT\art\bin\
	//cmd=558-PT.log

	//GetCurrentDirectory(sizeof(PathBuf), PathBuf);
	char TokeCh[512]="";
	char *pToke=NULL;
	//strcpy_s(TokeCh,sizeof(TokeCh),cmdBuf);
	strcpy_s(FileName,sizeof(FileName),cmdBuf);	

	if(!PathFileExists(FileName))
	{
		amprintf("No file %s exist!\n",FileName);
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
		return 1;
	}
	else
	{

	}

	int delCount=1;
	while(delCount<=10 && !DeleteFile(FileName))
	{
		Sleep(100);
		delCount++;

	}

	if(delCount>1)
	{
		amprintf("Delete File %s %d times;\n",FileName,delCount);
	}

	if(delCount>10)
	{
		amprintf("Delete %s fail\n",FileName);
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20
		return 0;
	}
	else
	{
		amprintf("Delete %s successfully\n",FileName);
		Sleep(100);
	}

	if(!PathFileExists(FileName))
	{
		amprintf("Now no file %s exist!\n",FileName);
	}
	else
	{
		amprintf("Now the file %s also exist!Delete function fail\n",FileName);
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20

		return 0;
	}

	Sleep(1000*atoi(pTI->CmdPerfTime));
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
	return 1;
}
//Maxwell 20110326

//Maxwell 20110326
int CTestItemScript::CheckIQTestFinish(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char FileName[512]="";
	//errno_t err;
	char OutBuf[5000]="";
	DWORD dwExitCode=0;

	long lastline=0;
	long timeout=0;

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	//strcpy_s(FileName,sizeof(FileName),pTI->DiagCmd);
	sprintf_s(FileName,sizeof(FileName),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->DiagCmd);

	//runShellExecute(cmd);

	HWND ToolWin=NULL;
	int f=1;
	int index=0;

	while (f)
	{
		ToolWin=NULL;
		ToolWin = ::FindWindow(NULL,"IQ");
		int i = 0;
		if (ToolWin)
		{
			while(cmd[i]!='\0')
			{
				::SendMessage(ToolWin,WM_CHAR,cmd[i],1);
				Sleep(100);
				i++;
			}
			//::SendMessage(ToolWin,WM_CHAR,'\r',1);
			f=0;
		}
		else
		{
			amprintf("IQ is not running!\n");
			return 0;
		}
	}
	index=0;
	while(!PathFileExists(FileName))
	{
		Sleep(100);
		index++;
		if(index>atoi(pTI->DiagPerfTime))
		{
			amprintf("IQ test out!\n");
			::SendMessage(ToolWin,WM_CLOSE,NULL,NULL);
			return 0;
		}
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//Maxwell 20110326

//Maxwell 20110326
int CTestItemScript::GenerateIQTestResultGRR(TEST_ITEM *pTI)
{
	char cmd[512]="";
	char TimeBuf[512]="";
	errno_t err;
	char strtime[128]="";
	char strtimeHMS[128]="";
	char tempdir[512]="";
	char desfile[512]="";

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);	
	//::GetSystemTime(&SystemTime);	
	sprintf_s(strtime,sizeof(strtime),"%d%02d%02d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay);
	//sprintf_s(ServerFolder,sizeof(ServerFolder),"%d%d",SystemTime.wYear,SystemTime.wMonth);
	sprintf_s(strtimeHMS,sizeof(strtimeHMS),"%02d%02d%02d",SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	sprintf_s(cmd,sizeof(cmd),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->UUTCmd);//Justin

	CCenTime Time;
	Time.DataFormatC(TimeBuf,sizeof(TimeBuf),gTI.UUTInfo.TestDate);
	//strcat_s(cmd,sizeof(cmd),".txt");
	amprintf("The IQ Text Result is %s\n",cmd);

	char ResultBuf1 [256000]="";

	size_t  PrvSize=0;
	size_t  CurrentSize=0;

	FILE *fpStream=NULL;
	FILE *fpWriteStream=NULL;

	char FilePath[512]="";

	char *pcRtnFGet=NULL;
	strcpy_s(FilePath,sizeof(FilePath),pTI->DiagCmd);
	if(!PathFileExists(FilePath))
	{	
		if(!CreateDirectory(FilePath,NULL))
		{
			amprintf("CreateDirectory %s failed;\n",FilePath);
			return 0;
		}
	}
	else
	{

	}


	strcat_s(FilePath,sizeof(FilePath),gTI.TestStaInfo.TesterPCName);
	strcat_s(FilePath,sizeof(FilePath),"_");
	strcat_s(FilePath,sizeof(FilePath),TimeBuf);

	char FileName[512]="";
	int Index=30;
	int FailFlag=0;
	char *pToke=NULL;
	int iFileLength = 0;

	while(Index--)
	{
		err = fopen_s( &fpStream, cmd, "rb" );
		Sleep(200);
		if (err)
		{
			Sleep(200);
			continue;
		}

		fseek(fpStream,0, SEEK_END);
		iFileLength = ftell(fpStream);
		amprintf("File length %d!\n",iFileLength);
		fseek(fpStream,0, SEEK_SET);

		fread(ResultBuf1, sizeof(char), 256000, fpStream);

		fseek(fpStream,0, SEEK_SET);

		while (!feof(fpStream))
		{
			char szString[16]="";
			char itemNumber[16]="";
			char ItemString[512]="";
			char FileLineBuf[512]="";

			pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

			if(pToke=strstr(FileLineBuf, "*  F A I L  *"))
			{
				amprintf("Test Result Fail!\n");
				break;
			}
			else if(pToke=strstr(FileLineBuf, "*  P A S S  *"))
			{
				amprintf("Test Result Pass!\n");
				FailFlag=1;
				break;
			}
			else
			{

			}
		}
		fclose(fpStream);  
		break;
	}

	if(FailFlag)
	{
		strcat_s(FilePath,sizeof(FilePath),"_PASS");
	}
	else
	{
		strcat_s(FilePath,sizeof(FilePath),"_FAIL");
	}

	sprintf_s(FileName,sizeof(FileName),"%s.txt",FilePath);
	amprintf("before open %s;\n",FileName);
	int iFileOpenErr=fopen_s( &fpWriteStream, FileName, "a+" );
	amprintf("iFileOpenErr=%d;\n",iFileOpenErr);
	if(0!=iFileOpenErr)
	{
		amprintf("fopen_s error;\n");
	}
	if(NULL==fpWriteStream)
	{
		amprintf("fpWriteStream is NULL;\n");
	}
	amprintf("after open %s;\n",FileName);
	int yy;
	yy=fprintf_s(fpWriteStream,"\nUUT_SN=%s;\n",gTI.UUTInfo.UUTSN);
	amprintf("yy=%d;\n",yy);
	size_t tt;
	amprintf("strlen(ResultBuf1)=%d;\n",strlen(ResultBuf1));
	amprintf("iFileLength=%d;\n",iFileLength);
	tt=fwrite(ResultBuf1, sizeof(char), iFileLength, fpWriteStream);
	amprintf("tt=%d;\n",tt);
	amprintf("after write to %s;\n",FileName);
	fclose(fpWriteStream);
	amprintf("after close %s;\n",FileName);


	//Generate detail file path;
	//ADD Justin 2011/01/08;

	if(!strstr(gTI.TestStaInfo.TestLogPath,"No log file path config")/*&&(!strncmp(SFIS.SSFISMode,"SFIS_ON",7) || !strncmp(pTI->Spec,"UPLOAD",6))*/)
	{
		//sprintf_s(tempdir,sizeof(tempdir),"\\\\10.117.2.5\\mfg\\mfg\\data\\%s_%s",gTI.UUTInfo.UUTPN,gTI.TestStaInfo.TestStaName);
		sprintf_s(tempdir,sizeof(tempdir),"%s\\%s_%s",gTI.TestStaInfo.TestLogPath,gTI.UUTInfo.UUTPN,gTI.TestStaInfo.TestStaName);

		if(!PathFileExists(tempdir))
		{	
			if(!CreateDirectory(tempdir,NULL))
			{
				amprintf("CreateDirectory %s failed\n",tempdir);
				return 0;
			}
		}
		else
		{

		}

		strcat_s(tempdir,sizeof(tempdir),"\\");
		strcat_s(tempdir,sizeof(tempdir),strtime);


		if(!PathFileExists(tempdir))
		{	
			if(!CreateDirectory(tempdir,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",tempdir);
				return 0;
			}
		}
		else
		{

		}

		strcat_s(tempdir,sizeof(tempdir),"\\");
		strcat_s(tempdir,sizeof(tempdir),gTI.TestStaInfo.TesterPCName);

		if(!PathFileExists(tempdir))
		{	
			if(!CreateDirectory(tempdir,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",tempdir);
				return 0;
			}
		}
		else
		{

		}
		//add start by Talen 2011/08/20
		strcat_s(tempdir,sizeof(tempdir),"\\IQlog");

		if(!PathFileExists(tempdir))
		{	
			if(!CreateDirectory(tempdir,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",tempdir);
				return 0;
			}
		}
		else
		{

		}
		//add end by Talen 2011/08/20

		sprintf_s(desfile,sizeof(desfile),"%s\\%s_%s.txt",tempdir,gTI.UUTInfo.UUTSN,strtimeHMS);

		//strcpy_s(gTI.TestStaInfo.TestLogPath,sizeof(gTI.TestStaInfo.TestLogPath),desfile);

		if(!CopyFile(cmd,desfile,FALSE))
		{
			amprintf("STS=Failed to copy IQ detail log file to server!;");
			return 0;

		}

		char dir[1024]="";
		char FileTemp[512]="";
		char ttempdir[512]="";
		char Resultdir[512]="";
		char ttempfile[512]="";

		strcpy(Resultdir,cmd);
		char *pptemp=strstr(Resultdir,"Log");
		*pptemp='\0';
		strcat(Resultdir,"Result");

		strcpy(ttempfile,cmd);
		pptemp=strstr(ttempfile,"log_all.txt");
		*pptemp='\0';
		strcat(ttempfile,"RXCalDataLogFile.txt");
		 
		sprintf_s(ttempdir,sizeof(ttempdir),"%s\\Result\\\0",tempdir);
		////sprintf_s(dir,sizeof(dir),"\"%s\\\"",tempdir);
		////sprintf_s(FileTemp,sizeof(FileTemp),"%s\\IQReportLog.csv",tempdir); //Justin 
		sprintf_s(dir,sizeof(dir),"\"%s\" \"%s\" \"%s\" \"%s\"",Resultdir,ttempdir,ttempfile,tempdir);
		
		if((SystemTime.wHour==8)&&(SystemTime.wMinute>1&&SystemTime.wMinute<30)&& FileMoveFlag==1)
		{
			if(!PathFileExists(ttempdir))
			{
				if(!RunBatchFile("MOVE.bat",dir))
				{
					amprintf("Failed to Move result file to server;\n");
					return 0;
				}
				else
				{
					amprintf("Move to server sucessfully;\n");
				}
				FileMoveFlag=0;
			}
			else
			{
				amprintf("Have already moved the file;\n");
			}

		}


	}

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20

	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//Maxwell 20110326

//Maxwell 20110326
int CTestItemScript::GetIQTestResult(TEST_ITEM *pTI)//Maxwell 10/09/16
{
	char cmd[500]="";
	errno_t err;
	//strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd); //Justin 20101222
	sprintf_s(cmd,sizeof(cmd),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->UUTCmd); //Justin 20101222

	amprintf("The Text Result is %s\n",cmd);

	char datetime[256]="";
	FILE *fLog=NULL;	
	char PathBuf[256]="";
	char IQFilePath[512]="";
	char IQFilePath2[512]="";
	char iq_fail_filename[512]="";
	int iFileOpenErr = 0;
	 int ret=DateTimeFormat(gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,datetime);
	 if(ret)
	 {
		 if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
		 {
			 return FALSE;
		 }
		 else
		 {
			 if(strlen(pTI->InsCmd))
			{
				strcpy(PathBuf,pTI->InsCmd);
			}
			 sprintf_s(IQFilePath, sizeof(IQFilePath), "%s\\IQ_FAILS\\", PathBuf);
			  sprintf_s(IQFilePath2, sizeof(IQFilePath2), "%s\\IQ_FAILS\\%s\\", PathBuf,gTI.TestStaInfo.TesterPCName);

			 sprintf_s(iq_fail_filename, sizeof(iq_fail_filename), "%s\\IQ_FAILS\\%s\\IQ_%s_%s.txt", PathBuf,gTI.TestStaInfo.TesterPCName,gTI.UUTInfo.UUTSN,datetime);
			 if(!PathFileExists(IQFilePath))
			 {	
				 if(!CreateDirectory(IQFilePath,NULL))
				 {
					 amprintf("Create Folder Fail!\n");
					 return 0;
				 }
			 }

			 if(!PathFileExists(IQFilePath2))
			 {	
				 if(!CreateDirectory(IQFilePath2,NULL))
				 {
					 amprintf("Create Folder Fail!\n");
					 return 0;
				 }
			 }
			 //Maxwell 090622 mutex operate log
			 //WaitForSingleObject(hWriteLogMutex, INFINITE); 
			 //Sleep(1);
			 //fprintf_s(fLog,"%s\n",pData);
			// fclose(fLog);
			 //ReleaseMutex(hWriteLogMutex);
		 }
		 //CTReport.LogDataNF(gTI.UUTInfo.UUTSN,datetime,nf_log_filename,OutBuf);
	 }
	 else
	 {
		 amprintf("Test error,can not get test start time;\n");
		 return 0;
	 }


	char ResultBuf1 [256000]="";

	size_t  PrvSize=0;
	size_t  CurrentSize=0;

	FILE *fpStream=NULL;
	FILE *fpWriteStream=NULL;
	FILE *fpServerStream=NULL;

	int SumCounter = 0;

	char FilePath[512]="";
	char ServerFilePath[512]="";

	char *pcRtnFGet=NULL;
	strcpy_s(FilePath,sizeof(FilePath),pTI->DiagCmd);
	strcpy_s(ServerFilePath,sizeof(ServerFilePath),gTI.TestStaInfo.TestLogPath);

	char FileName[512]="";
	char ServerFileName[512]="";
	int Index=30;
	int FailFlag=0;
	char *pToke=NULL;

	int iFileLength = 0;

	while(Index--)
	{
		err = fopen_s( &fpStream, cmd, "rb" );
		Sleep(200);
		if (err)
		{
			Sleep(200);
			continue;
		}

		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),"");

		fseek(fpStream,0, SEEK_END);
		iFileLength = ftell(fpStream);
		amprintf("File length %d!\n",iFileLength);
		fseek(fpStream,0, SEEK_SET);

		fread(ResultBuf1, sizeof(char), 256000, fpStream);

		fseek(fpStream,0, SEEK_SET);

		int ItemNumber = 1;

		while (!feof(fpStream))
		{
			char szString[16]="";
			char itemNumber[16]="";
			char ItemString[512]="";//Maxwell 100301
			char FileLineBuf[512]="";

			sprintf_s(szString, "%d.",ItemNumber);

			if(ItemNumber>0&&ItemNumber<10)
			{
				sprintf_s(itemNumber, "F00%d",ItemNumber);
			}
			else if(ItemNumber>=10&&ItemNumber<100)
			{
				sprintf_s(itemNumber, "F0%d",ItemNumber);
			}
			else
			{
				sprintf_s(itemNumber, "F%d",ItemNumber);
			}

			sprintf_s(szString, "%d.",ItemNumber);

			pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

			if((pToke=strstr(FileLineBuf,szString))&&strstr(FileLineBuf,"__________________"))//Modify By Maxwell 100302
			{
				ItemNumber++;
				strcpy_s(ItemString,sizeof(ItemString),FileLineBuf);

				if(strstr(FileLineBuf,"TX_VERIFY_POWER"))
				{
					double PowerValue = 0;
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "POWER_AVERAGE_DBM        :"))
						{ 
							
							char POWERString[128] = "";
							PowerValue = atof(pToke+strlen("POWER_AVERAGE_DBM        :"));

							sprintf_s(POWERString,sizeof(POWERString),",%.2f",PowerValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),POWERString);
							SumCounter++;
						}
						//////////else if(pToke=strstr(FileLineBuf,"TX_POWER_DBM             :")) //Add for Golden selection;
						//////////{
						//////////	double TargetPowerLevel = 0;
						//////////	char TargetPower[128] ="";
						//////////	TargetPowerLevel = atof(pToke+strlen("TX_POWER_DBM             :"));
						//////////	if((PowerValue-TargetPowerLevel)>gTI.TestStaInfo.GoldenSelectionMAX || (PowerValue-TargetPowerLevel)<gTI.TestStaInfo.GoldenSelectionMIN)
						//////////	{
						//////////		amprintf("Golden select fail :%f-->Item %s",(PowerValue-TargetPowerLevel),FileLineBuf);
						//////////		IsGolden=false;
						//////////	}
						//////////	else
						//////////	{
						//////////		amprintf("Golden select data :%f-->Item %s",(PowerValue-TargetPowerLevel),FileLineBuf);
						//////////	}


						//////////}
						else
						{

						}
					}
				}
				else if(strstr(FileLineBuf,"THROUGHPUT_TEST"))
				{
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "THROUGHPUT               :"))
						{ 
							double EVMValue = 0;
							char EVMString[128] = "";
							EVMValue = atof(pToke+strlen("THROUGHPUT               :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",EVMValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else
						{

						}
					}
				}
				else if(strstr(FileLineBuf,"TX_VERIFY_EVM"))
				{

					int FreqErrorVerifyFlag = 0;
					double PowerValue = 0;

					//if(strstr(FileLineBuf,"TX_VERIFY_EVM 2442 MCS7 HT20"))
					//{
						FreqErrorVerifyFlag = 1;
					//}
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "EVM_AVG_DB               :"))
						{ 
							double EVMValue = 0;
							char EVMString[128] = "";
							EVMValue = atof(pToke+strlen("EVM_AVG_DB               :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",EVMValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else if(pToke=strstr(FileLineBuf, "POWER_AVG_DBM            :"))
						{
							
							char EVMString[128] = "";
							PowerValue = atof(pToke+strlen("POWER_AVG_DBM            :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",PowerValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else if((1==FreqErrorVerifyFlag)&&(pToke=strstr(FileLineBuf, "FREQ_ERROR_AVG           :")))
						{
							double FreqValue = 0;
							char FreqString[128] = "";
							FreqValue = atof(pToke+strlen("FREQ_ERROR_AVG           :"));

							sprintf_s(FreqString,sizeof(FreqString),",%.2f",FreqValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),FreqString);
							SumCounter++;
						}
						//////////////else if(pToke=strstr(FileLineBuf,"TX_POWER_DBM             :")) //Add for Golden selection;
						//////////////{
						//////////////	double TargetPowerLevel = 0;
						//////////////	char TargetPower[128] ="";
						//////////////	TargetPowerLevel = atof(pToke+strlen("TX_POWER_DBM             :"));
						//////////////	if((PowerValue-TargetPowerLevel)>gTI.TestStaInfo.GoldenSelectionMAX || (PowerValue-TargetPowerLevel)<gTI.TestStaInfo.GoldenSelectionMIN)
						//////////////	{
						//////////////		amprintf("Golden select fail :%f-->Item %s",(PowerValue-TargetPowerLevel),FileLineBuf);
						//////////////		IsGolden=false;
						//////////////	}
						//////////////	else
						//////////////	{
						//////////////		amprintf("Golden select data :%f-->Item %s",(PowerValue-TargetPowerLevel),FileLineBuf);
						//////////////	}


						//////////////}
						else
						{

						}
					}
				}
				else if(strstr(FileLineBuf,"RX_VERIFY_PER"))
				{
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "PER                      :"))
						{ 
							double EVMValue = 0;
							char EVMString[128] = "";
							EVMValue = atof(pToke+strlen("PER                      :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",EVMValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else
						{

						}
					}
				}
				else if(strstr(FileLineBuf,"RX_SWEEP_PER"))
				{
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "SENS_POWER_LEVEL_DBM     :"))
						{ 
							double SenPowerValue = 0;
							char SenPowerString[128] = "";
							SenPowerValue = atof(pToke+strlen("SENS_POWER_LEVEL_DBM     :"));

							sprintf_s(SenPowerString,sizeof(SenPowerString),",%.2f",SenPowerValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),SenPowerString);
							SumCounter++;
						}
						else
						{

						}
					}
				}
				else if(strstr(FileLineBuf,"TX_VERIFY_MASK"))
				{
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "POWER_AVERAGE_DBM        :"))
						{ 
							double EVMValue = 0;
							char EVMString[128] = "";
							EVMValue = atof(pToke+strlen("POWER_AVERAGE_DBM        :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",EVMValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else if(pToke=strstr(FileLineBuf, "VIOLATION_PERCENT        :"))
						{
							double EVMValue = 0;
							char EVMString[128] = "";
							EVMValue = atof(pToke+strlen("VIOLATION_PERCENT        :"));

							sprintf_s(EVMString,sizeof(EVMString),",%.2f",EVMValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),EVMString);
							SumCounter++;
						}
						else
						{

						}
					}
				}
				///////add start by Talen 2011/06/07/07
				else if(strstr(FileLineBuf,"OTA_VERIFY"))
				{
					while(!strstr(FileLineBuf, "ERROR_MESSAGE"))
					{
						fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

						if(pToke=strstr(FileLineBuf, "SENS_POWER_LEVEL_DBM     :"))
						{ 
							double SenPowerValue = 0;
							char SenPowerString[128] = "";
							SenPowerValue = atof(pToke+strlen("SENS_POWER_LEVEL_DBM     :"));

							sprintf_s(SenPowerString,sizeof(SenPowerString),",%.2f",SenPowerValue);
							strcat_s(pTI->Result,sizeof(pTI->Result),SenPowerString);
							SumCounter++;
						}
						else
						{

						}
					}
				}
				///////add end by Talen 2011/07/07
				else if(strstr(FileLineBuf,"TX_VERIFY_SPECTRUM"))
				{

				}
				else
				{

				}
			}

			else if(pToke=strstr(FileLineBuf, "*  F A I L  *"))
			{
				iFileOpenErr = fopen_s(&fLog, iq_fail_filename, "a+");
				if(NULL == fLog)
				{
					return FALSE;
				}

				char FileLineBuf[512]="";
				int FailSum=0;
				fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 

				while(!strstr(FileLineBuf, "Total Run(s)"))
				{
					if(pToke=strstr(FileLineBuf,"<--Failed"))
					{
						if(fLog)
						{
							fprintf_s(fLog,"%s",FileLineBuf);
						}
						int FailNumber = atoi(FileLineBuf);
						char FailErrorCode[32] = ""; 
						sprintf_s(FailErrorCode, "F%d",FailNumber);
						if((FailSum<20)&&FailNumber)
						{
							strcat_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),FailErrorCode);
						}
						FailSum++;
					}
					fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream); 
				}

				if(0==strlen(pTI->ErrorCode))
				{
					strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),"SY00");
				}
				else
				{

				}

				amprintf("Test Result Fail!\n");
				break;
			}
			else if(pToke=strstr(FileLineBuf, "*  P A S S  *"))
			{
				amprintf("Test Result Pass!\n");
				FailFlag=1;
				break;
			}
			else
			{
				
			}
		}
		fclose(fpStream);  
		if(fLog)
		{
			fclose(fLog);
		}
		break;
	}
	
	amprintf("The Sum of result item is %d;\n",SumCounter);
	if(Index==0)
	{
		amprintf("The Text Result %s can not be opened\n",cmd);
		strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),"GT00");
		return 0;
	}

	if(!strlen(gTI.UUTInfo.UUTMO))
	{
		strcpy_s(gTI.UUTInfo.UUTMO,sizeof(gTI.UUTInfo.UUTMO),"K31 Test log");
	}
	strcat_s(FilePath,sizeof(FilePath),gTI.UUTInfo.UUTMO);
	if(!PathFileExists(FilePath))
	{	
		if(!CreateDirectory(FilePath,NULL))
		{
			amprintf("CreateDirectory %s failed;\n",FilePath);
			return 0;
		}
	}

	strcat_s(FilePath,sizeof(FilePath),"\\");
	strcat_s(FilePath,sizeof(FilePath),gTI.TestStaInfo.TestStaName);
	if(!PathFileExists(FilePath))
	{	
		if(!CreateDirectory(FilePath,NULL))
		{
			amprintf("CreateDirectory %s failed;\n",FilePath);
			return 0;
		}
	}


	strcat_s(FilePath,sizeof(FilePath),"\\");
	strcat_s(FilePath,sizeof(FilePath),gTI.TestStaInfo.TesterPCName);
	if(!PathFileExists(FilePath))
	{	
		if(!CreateDirectory(FilePath,NULL))
		{
			amprintf("CreateDirectory %s failed;\n",FilePath);
			return 0;
		}
	}


	if(FailFlag)
	{
		strcat_s(FilePath,sizeof(FilePath),"\\PASS");
		if(!PathFileExists(FilePath))
		{	
			if(!CreateDirectory(FilePath,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",FilePath);
				return 0;
			}
		}

		CCenTime t;
		char timeBuf[128]="";
		char timeTempBuf[128]="";
		t.GetDate(timeBuf,sizeof(timeBuf));
		int k=0;
		for(int index=0;index<128;index++)
		{
			if(timeBuf[index]!='/')
			{
				timeTempBuf[k]=timeBuf[index];
				k++;
			}
			else
			{
			}
			if(timeBuf[index]==0)
			{
				timeTempBuf[k]=0;
				break;
			}
		}
		strcat_s(FilePath,sizeof(FilePath),"\\");
		strcat_s(FilePath,sizeof(FilePath),timeTempBuf);
		//sprintf_s(FileName, sizeof(FileName), _T("%s\\LogFile\\PASS\\%s\\"), FilePath,timeTempBuf);
		if(!PathFileExists(FilePath))
		{	
			if(!CreateDirectory(FilePath,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",FilePath);
				return FailFlag;
			}
		}
		else
		{
		}

		
		if(!strlen(gTI.UUTInfo.UUTSN))
		{
			strcpy_s(gTI.UUTInfo.UUTSN,sizeof(gTI.UUTInfo.UUTSN),"6F0000000000");
		}
		//strcat_s(FilePath,sizeof(FilePath),"\\");
		sprintf_s(FileName,sizeof(FileName),"%s\\%s.txt",FilePath,gTI.UUTInfo.UUTSN);
		memset(currentSNIQlog,0,sizeof(currentSNIQlog));//add by Talen 2011/05/31
		strcpy(currentSNIQlog,FileName);	//add by Talen 2011/05/31
		int iFileOpenErr=fopen_s( &fpWriteStream, FileName, "a+" );
		fwrite(ResultBuf1, sizeof(char), iFileLength, fpWriteStream);
		//fprintf_s(fpWriteStream,"%s\n",ResultBuf1);
		fclose(fpWriteStream);
	
	}
	else
	{
		strcat_s(FilePath,sizeof(FilePath),"\\FAIL");
		if(!PathFileExists(FilePath))
		{	
			if(!CreateDirectory(FilePath,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",FilePath);
				return FailFlag;
			}
		}

		CCenTime t;
		char timeBuf[128]="";
		char timeTempBuf[128]="";
		t.GetDate(timeBuf,sizeof(timeBuf));
		int k=0;
		for(int index=0;index<128;index++)
		{
			if(timeBuf[index]!='/')
			{
				timeTempBuf[k]=timeBuf[index];
				k++;
			}
			else
			{
			}
			if(timeBuf[index]==0)
			{
				timeTempBuf[k]=0;
				break;
			}
		}
		strcat_s(FilePath,sizeof(FilePath),"\\");
		strcat_s(FilePath,sizeof(FilePath),timeTempBuf);
		//sprintf_s(FileName, sizeof(FileName), _T("%s\\LogFile\\PASS\\%s\\"), FilePath,timeTempBuf);
		if(!PathFileExists(FilePath))
		{	
			if(!CreateDirectory(FilePath,NULL))
			{
				amprintf("CreateDirectory %s failed;\n",FilePath);
				return FailFlag;
			}
		}
		else
		{
		}

		if(!strlen(gTI.UUTInfo.UUTSN))
		{
			strcpy_s(gTI.UUTInfo.UUTSN,sizeof(gTI.UUTInfo.UUTSN), "6F0000000000");
		}
		sprintf_s(FileName,sizeof(FileName),"%s\\%s.txt",FilePath,gTI.UUTInfo.UUTSN);
		memset(currentSNIQlog,0,sizeof(currentSNIQlog));//add by Talen 2011/05/31
		strcpy(currentSNIQlog,FileName);	//add by Talen 2011/05/31
		int iFileOpenErr=fopen_s( &fpWriteStream, FileName, "a+" );
		fwrite(ResultBuf1, sizeof(char), iFileLength, fpWriteStream);
		fclose(fpWriteStream);
	}
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return FailFlag;
}
//Maxwell 20110326

//add start by Talen 2011/05/31
int CTestItemScript::GetArtVersion(TEST_ITEM *pTI)
{
	errno_t err;
	FILE *fpStream=NULL;
	char FileLineBuf[512]="";
	char *Toke=NULL;
	char tempbuf[50]="";
	int result=0;

	char logFile[512]="";
	memset(logFile,0,sizeof(logFile));
	sprintf_s(logFile,sizeof(logFile),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->UUTCmd);

	amprintf("Get Art Version from file: %s;\n",logFile);

	if(!PathFileExists(logFile))
	{	
		amprintf("The log file \"%s\" not exit;\n",logFile);
		return 0;
	}
	err = fopen_s( &fpStream, logFile, "rb" );
	if(err)
	{
		amprintf("Can not open the file: %s;\n",logFile);
		return 0;
	}
	
	while(!feof(fpStream))
	{
		fgets(FileLineBuf,sizeof(FileLineBuf),fpStream);

		if(Toke=strstr(FileLineBuf,"DUT_DRIVER_INFO"))
		{
			result=strSTEDcpy(tempbuf,sizeof(tempbuf),Toke,':',' ');
			break;//add by Talen 2012/03/12
		}

	}

	err=fclose(fpStream);
	if(!err)
	{
		amprintf("Close file \"%s\" fail;\n",logFile);
	}

	if(!result)
	{
		amprintf("Can not find DUT_DRIVER_INFO;\n");
		return 0;
	}

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s",tempbuf);
	amprintf("Detect art version : %s;\n",pTI->Result);
	if(strAllCmp(pTI->Result,pTI->Spec))
	{
		return 0;
	}
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/05/31


int CTestItemScript::SwitchEthernet(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	if(fix_id==1)
	{
		char cmd[512]="";
		sprintf(cmd,"%s;\r\n",pTI->DiagCmd);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		int wait_time=atoi(pTI->DiagPerfTime);
		Sleep(wait_time);
	}
	else if(fix_id==2)
	{
		char cmd[512]="";
		sprintf(cmd,"%s;\r\n",pTI->UUTCmd);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		int wait_time=atoi(pTI->CmdPerfTime);
		Sleep(wait_time);
	}
	else
	{
		char cmd[512]="";
		sprintf(cmd,"%s;\r\n",pTI->DiagCmd);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		int wait_time=atoi(pTI->DiagPerfTime);
		Sleep(wait_time);
	}
	return 1;
}

int CTestItemScript::BlockPushFront(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	if(fix_id==1)
	{
		PushFrontBlock1(pTI->Port_1);
	}
	else if(fix_id==2)
	{
		PushFrontBlock1(pTI->Port_2);
	}
	else
	{
		PushFrontBlock1(pTI->Port_1);
	}
	int wait_time=atoi(pTI->CmdPerfTime);
	Sleep(wait_time);
	return 1;
}



int CTestItemScript::BlockPullBack(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	if(fix_id==1)
	{
		PullBackBlock1(pTI->Port_1);
	}
	else if(fix_id==2)
	{
		PullBackBlock1(pTI->Port_2);
	}
	else
	{
		PullBackBlock1(pTI->Port_1);
	}
	int wait_time=atoi(pTI->CmdPerfTime);
	Sleep(wait_time);
	return 1;
}




int CTestItemScript::PanelPushDown(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	if(fix_id==1)
	{
		PushDownPanel1(pTI->Port_1);
	}
	else if(fix_id==2)
	{
		PushDownPanel1(pTI->Port_2);
	}
	else
	{
		PushDownPanel1(pTI->Port_1);
	}
	int wait_time=atoi(pTI->CmdPerfTime);
	Sleep(wait_time);
	return 1;
}

int CTestItemScript::PanelPullUp(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	if(fix_id==1)
	{
		PullUpPanel1(pTI->Port_1);
	}
	else if(fix_id==2)
	{
		PullUpPanel1(pTI->Port_2);
	}
	else
	{
		PullUpPanel1(pTI->Port_1);
	}
	int wait_time=atoi(pTI->CmdPerfTime);
	Sleep(wait_time);
	return 1;
}


int CTestItemScript::LEDCOLORTEST(TEST_ITEM *pTI)
{
	amprintf("FIXTURE ID:%d\n",fix_id);
	int led_spec[8];
	char result[512]="";
	int ledport;
	if(fix_id==1)
	{
		ledport=pTI->LED_PORT_1;
	}
	else if(fix_id==2)
	{
		ledport=pTI->LED_PORT_2;
	}
	else
	{
		ledport=pTI->LED_PORT_1;
	}
	

	int I_MIN= pTI->RGBI_I_SPEC_DOWN;
	int I_MAX= pTI->RGBI_I_SPEC_UP;
	int R_MIN= pTI->RGBI_R_SPEC_DOWN;
	int R_MAX= pTI->RGBI_R_SPEC_UP;
	int G_MIN= pTI->RGBI_G_SPEC_DOWN;
	int G_MAX= pTI->RGBI_G_SPEC_UP;
	int B_MIN= pTI->RGBI_B_SPEC_DOWN;
	int B_MAX= pTI->RGBI_B_SPEC_UP;

	led_spec[0]=I_MIN;
	led_spec[1]=I_MAX;
	led_spec[2]=R_MIN;
	led_spec[3]=R_MAX;
	led_spec[4]=G_MIN;
	led_spec[5]=G_MAX;
	led_spec[6]=B_MIN;
	led_spec[7]=B_MAX;
	
	
	//Judge Led result,retry 5 times
	for(int i=0;i<5;i++)
	{
		if(ParseLEDDetect(led_spec,ledport,result))
		{
			return 1;
		}
		//if(ParseLEDDetect(pAT100DevName,led_spec,ledport))
		//	return 1;
		else if(i>=5)
		{
			return 0;
		}
		else
		{
			Sleep(1000);
			continue;
		}
	}
	

	//strcpy_s(pTI->Result,sizeof(pTI->Result),TempBuf);
	
	
	return 1;
}

//Talen 
int CTestItemScript::DownArtandCheck(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	char exitcmd[10]="exit\r";


	KillTargetProcess("WinSCP.com");
	KillTargetProcess("WinSCP.exe");
	
	if(pTI->CmpFlag)
	{
		amprintf("%s;\n",pTI->UUTCmd);
		if (runShellExecute(pTI->UUTCmd))
		{
			amprintf("Run %s Fail;\n",pTI->UUTCmd);
			KillTargetProcess("WinSCP.com");
			KillTargetProcess("WinSCP.exe");
			return 0;
		}
	}
	else
	{
		amprintf("%s;\n",pTI->DiagCmd);
		if (runShellExecute(pTI->DiagCmd))
		{
			amprintf("Run %s Fail;\n",pTI->DiagCmd);
			KillTargetProcess("WinSCP.com");
			KillTargetProcess("WinSCP.exe");
			return 0;
		}
	}

	HWND ToolWin=NULL;
	int i=0;
	int count=400;

	while(!ToolWin&&count--)
	{
		ToolWin=NULL;
		//amprintf("%d",count);
		ToolWin = ::FindWindow(NULL,"WinSCP");
		//Sleep(1000);
		//amprintf("::FindWindow after;\n");
		if(ToolWin)
		{
			
			amprintf("find WinSCP window;\n");
			//SendMessageCallback(ToolWin,WM_CHAR,'U',1,NULL,NULL);
			
			::SendMessage(ToolWin,WM_CHAR,'U',1);
			Sleep(50);
			//SendMessageCallback(ToolWin,WM_CHAR,'\r',1,NULL,NULL);
			::SendMessage(ToolWin,WM_CHAR,'\r',1);
			Sleep(50);
			::SendMessage(ToolWin,WM_CHAR,'U',1);
			Sleep(50);
			//SendMessageCallback(ToolWin,WM_CHAR,'\r',1,NULL,NULL);
			::SendMessage(ToolWin,WM_CHAR,'\r',1);
			Sleep(50);
			//amprintf("send \"U\";\n");
			::SendMessage(ToolWin,WM_CHAR,'Y',1);
			Sleep(50);
			::SendMessage(ToolWin,WM_CHAR,'\r',1);
			Sleep(50);
			while(exitcmd[i]!='\0')
			{
				//SendMessageCallback(ToolWin,WM_CHAR,exitcmd[i],1,NULL,NULL);
				//amprintf("%c\n",exitcmd[i]);
				::SendMessage(ToolWin,WM_CHAR,exitcmd[i],1);
				i++;
				Sleep(50);
			}
			//amprintf("send exit;\n");
			break;
		}
		else
		{
			//amprintf("No found %d;\n",count);
		}
		Sleep(100);
	}


	HANDLE scpWin=NULL;
	int findCount=200;
	while(FindWindow(NULL,"WinSCP")&&findCount)
	{
		//amprintf("%d;\n",findCount);
		findCount--;
		Sleep(200);
	}

	if(0 == findCount)
	{
		KillTargetProcess("WinSCP.exe");
		KillTargetProcess("WinSCP.com");	
		//amprintf("Kill Process;\n");
	}

	FILE *stream = NULL;
	errno_t Errcode;
	char fileName[512]="";
	sprintf_s(fileName,sizeof(fileName),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->Spec);

	if(PathFileExists(fileName))
	{	
	}
	else
	{
		amprintf("%s not exit;\n",fileName);
	}


	int openCount=1;
	while(openCount<=10 && (Errcode=fopen_s(&stream,fileName,"r")))
	{
		openCount++;
		Sleep(100);
	}
	if(openCount>1)
	{
		amprintf("Open File %s %d times;\n",fileName,openCount);
	}
	if(Errcode)
	{
		amprintf("Open %s Fail!\n",fileName);
		return 0;
	}
	fread_s(OutBuf,sizeof(OutBuf),1,sizeof(OutBuf),stream);

	if(fclose(stream))
	{
		amprintf("Close %s Fail!\n",fileName);
		return 0;
	}
	amprintf("\n%s\n;",OutBuf);
	
	if(pTI->CmpFlag)
	{
		if(strstr(OutBuf,"radio-cal-ath1=")||strstr(OutBuf,"radio-cal-ath0="))
		{
			amprintf("erase cal data fail;\n");
			return 0;
		}
		else
		{
		}
	}
	else
	{
	}

	if(!strstr(OutBuf,"2012  art"))
	{
		amprintf("Can not find \"2012  art\". Download ART Fail;\n");
		return 0;
	}
	else
	{
	}

	amprintf("Download ART Pass;\n");
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
	Sleep(1000*atoi(pTI->DiagPerfTime));
	return 1;
}
//Talen 


//Maxwell 20110329
int CTestItemScript::SetGoldenConnect(TEST_ITEM *pTI)
{
	char Temp[128]="";

	char cmd[128]="";
	sprintf_s(Temp,sizeof(Temp),"%s;\r\n",pTI->UUTCmd);
	
	amprintf("Cmd send to Golden: %s;\n",Temp);

	if(!ResetEvent(GoldenServer.hRevEvent)) 
	{
		return 0;
	}
	else
	{

	}

	if (!GoldenAmbitSend(&GoldenServer,Temp,(int)(sizeof(Temp))))
	{
		amprintf("Failed to send Cmd to Golden:%s\n",Temp);
		return 0;
	}
	else
	{
		amprintf("Cmd send to Golden:%s successfully!\n",Temp);
	}

	if(WAIT_TIMEOUT == WaitForSingleObject(GoldenServer.hRevEvent, 2000))
	{
		amprintf("No recv from Golden output.\n");
		return 0;
	}
	else
	{
		strcpy_s(pTI->Result,sizeof(pTI->Result),GoldenServer.RecvInfo);
		amprintf("Recv Golden output:%s.\n",pTI->Result);
	}

	return 1;
}
//Maxwell 20110329

//Maxwell 20110329
int CTestItemScript::SetAudioTest(TEST_ITEM *pTI)
{
	amprintf("fix_id:%d;\n",fix_id);
	char Temp[128]="";
	char cmd[128]="";
	memset(Temp,0,sizeof(Temp));

	if(strstr(pTI->UUTCmd,"[ANALOG]"))
	{
		strcpy_s(pTI->Result,sizeof(pTI->Result),"0.0,0.0,0.0,0.0,FAIL");
	}
	else if(strstr(pTI->UUTCmd,"[DIGITAL]"))
	{
		strcpy_s(pTI->Result,sizeof(pTI->Result),"FAIL,FAIL");
	}
	else
	{

	}

	//add start by Talen 2011/09/03
	if(0==pTI->RetryItemCount)
	{

	}
	else if(1==pTI->RetryItemCount)
	{
//		gTI.UUTInfo.retryItemCount++;//Talen 2012/03/15
	}
	else if(1<pTI->RetryItemCount)
	{
		char cmd1[32]="";
		char cmd2[32]="";
		char spec1[32]="";
		char spec2[32]="";
		if(strstr(gTI.TestStaInfo.TestStaName,"MPT1") || strstr(gTI.TestStaInfo.TestStaName,"MPT3"))
		{
			if(2==gTI.TestStaInfo.fixID)
			{
				strcpy_s(cmd1,sizeof(cmd1),"@B5#");
				strcpy_s(spec1,sizeof(spec1),"@H5#");
				strcpy_s(cmd2,sizeof(cmd2),"@A5#");
				strcpy_s(spec2,sizeof(spec2),"@G5#");
			}
			else
			{
				strcpy_s(cmd1,sizeof(cmd1),"@B2#");
				strcpy_s(spec1,sizeof(spec1),"@H2#");
				strcpy_s(cmd2,sizeof(cmd2),"@A2#");
				strcpy_s(spec2,sizeof(spec2),"@G2#");
			}
		}
		else if(strstr(gTI.TestStaInfo.TestStaName,"FT2"))
		{
			strcpy_s(cmd1,sizeof(cmd1),"@B1#");
			strcpy_s(spec1,sizeof(spec1),"@H1#");
			strcpy_s(cmd2,sizeof(cmd2),"@A1#");
			strcpy_s(spec2,sizeof(spec2),"@G1#");
		}
		else
		{
			amprintf("MSGBOX=CONTENT[Audio only be tested in MPT1 MPT3 and FT2!];\n");
			WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			if (!ResetEvent(gTI.UiResMsg.hEvent)) 
			{ 
				printf("ResetEvent failed (%d)\n", GetLastError());
			}
			KillTargetProcess("AmbitProT.exe");
			int errorcode;
			_exit(errorcode);
		}

		MoveBlock(4,9600,cmd1,"10",spec1);//pull back connectors
		MoveBlock(4,9600,cmd2,"10",spec2);//push out connectors

		amprintf("ping 10.0.1.1;\n");
		if(!VerifyLinkUsePing("ping 10.0.1.1",30))
		{
			amprintf("Ping Fail;\n");
			strcpy_s(pTI->Result,sizeof(pTI->Result),"PingFail,FAIL");
			return 0;
		}
		else
		{
		}			
	}
	else
	{

	}
	//add end by Talen 2011/09/03


	//Talen 2012/03/10
	GOLD_CLIENT_TYPE *pGoldenServer=NULL;
	if(1==fix_id)
	{
		pGoldenServer=&GoldenServer;
	}
	else if(2==fix_id)
	{
		pGoldenServer=&GoldenServer1;
	}
	else
	{
		amprintf("Fix_id = %d, not 1 or 2!\n",fix_id);
		return 0;
	}
	//Talen 2012/03/10

	if((!pTI->ResultFlag)&&(pTI->RtyFlag>0))
	{
		amprintf("Sleep 10s time;\n");
		Sleep(10000);
	}

//add start by Talen 2011/08/30
	memset(pGoldenServer->RecvInfo,0,sizeof(pGoldenServer->RecvInfo));
	sprintf_s(Temp,sizeof(Temp),"%sSN[%s];\r\n",pTI->UUTCmd,gTI.UUTInfo.UUTSN);//add SN info by Talen 2012/02/21
	amprintf("Cmd send to audio test: %s;\n",Temp);

	if(!ResetEvent(pGoldenServer->hRevEvent)) 
	{
		return 0;
	}
	else
	{

	}

	if (!GoldenAmbitSend(pGoldenServer,Temp,(int)(strlen(Temp))))
	{
		amprintf("Failed to send Cmd to audio:%s\n",Temp);
		//add start by Talen 2011/07/27
		if(!ReStartGoldenServerConnect(pGoldenServer,gTI.TestStaInfo.Golden_Connect_Para.connectIP,gTI.TestStaInfo.Golden_Connect_Para.connectPORT,"connect with Server"))
		{
			amprintf("ERROR! The Connect relinked fail.\n");
			return 0;
		}

		if(!ResetEvent(pGoldenServer->hRevEvent)) 
		{
			return 0;
		}
		else
		{

		}

		if (!GoldenAmbitSend(pGoldenServer,Temp,(int)(strlen(Temp))))
		{
			amprintf("Failed to send Cmd to audio:%s\n",Temp);
			return 0;
		}
		else
		{
			amprintf("Cmd send to audio:%s successfully!\n",Temp);
		}
		//add end by Talen 2011/07/27
		//return 0;
	}
	else
	{
		amprintf("Cmd send to audio:%s successfully!\n",Temp);
	}

	if(WAIT_TIMEOUT == WaitForSingleObject(pGoldenServer->hRevEvent, 50000))
	{
		amprintf("No recv from audio output.\n");

		//add start by Talen 2011/07/15

		if(!ReStartGoldenServerConnect(pGoldenServer,gTI.TestStaInfo.Golden_Connect_Para.connectIP,gTI.TestStaInfo.Golden_Connect_Para.connectPORT,"connect with Server"))
		{
			amprintf("ERROR! The Connect relinked fail.\n");
			return 0;
		}

		if(!ResetEvent(pGoldenServer->hRevEvent)) 
		{
			return 0;
		}
		else
		{

		}

		if (!GoldenAmbitSend(pGoldenServer,Temp,(int)(strlen(Temp))))
		{
			amprintf("Failed to send Cmd to audio:%s\n",Temp);
			return 0;
		}
		else
		{
			amprintf("Cmd send to audio:%s successfully!\n",Temp);
		}

		if(WAIT_TIMEOUT == WaitForSingleObject(pGoldenServer->hRevEvent, 50000))
		{
			amprintf("No recv from audio output.\n");
			if(pTI->RetryItemCount==pTI->RtyFlag)//add by Talen 2012/02/21
			{
				strcpy_s(pTI->ErrorCode,sizeof(pTI->ErrorCode),"TO99");
				strcpy_s(pTI->ErrorDes,sizeof(pTI->ErrorDes),"Audio test failed, can not recv from Audio test server");
			}
			return 0;
		}

		//add end by Talen 2011/07/15
		//return 0;//remove by Talen 2011/07/27
	}

	amprintf("Recv audio test diag output:%s.\n",pGoldenServer->RecvInfo);
	char SoundLoopBackXVersion[32]="";
	memset(SoundLoopBackXVersion,0,sizeof(SoundLoopBackXVersion));
	int digiResult=1;

	if(strstr(pGoldenServer->RecvInfo,"[ANALOG]"))
	{
		amprintf("[ANALOG] test finish!.\n");

		char *pToke = strstr(pGoldenServer->RecvInfo,"[ANALOG]:");

		if(pToke)
		{
			pToke+=9;
			amprintf("Separation L value:%.3f.\n",atof(pToke));
			sprintf_s(pTI->MultiResult[0], sizeof(pTI->MultiResult[0]), "%.3f,",atof(pToke));
			strcpy_s(pTI->Result,sizeof(pTI->Result),pTI->MultiResult[0]);

			if(pToke = strstr(pToke,","))
			{
				pToke+=1;
				amprintf("Separation R value:%.3f.\n",atof(pToke));
				sprintf_s(pTI->MultiResult[1], sizeof(pTI->MultiResult[1]), "%.3f,",atof(pToke));
				strcat_s(pTI->Result,sizeof(pTI->Result),pTI->MultiResult[1]);
			}
			else
			{
				amprintf("NO separation R value return.\n");
				return 0;
			}

			if(pToke = strstr(pToke,","))
			{
				pToke+=1;
				amprintf("THD+N L value:%.3f.\n",atof(pToke));
				sprintf_s(pTI->MultiResult[2], sizeof(pTI->MultiResult[2]), "%.3f,",atof(pToke));
				strcat_s(pTI->Result,sizeof(pTI->Result),pTI->MultiResult[2]);
			}
			else
			{
				amprintf("NO THD+N L value return.\n");
				return 0;
			}

			if(pToke = strstr(pToke,","))
			{
				pToke+=1;
				amprintf("THD+N R value:%.3f.\n",atof(pToke));
				sprintf_s(pTI->MultiResult[3], sizeof(pTI->MultiResult[3]), "%.3f",atof(pToke));
				strcat_s(pTI->Result,sizeof(pTI->Result),pTI->MultiResult[3]);
			}
			else
			{
				amprintf("NO THD+N R value return.\n");
				return 0;
			}

			//Talen 2012/03/22
			if(GetsubStrInStr(SoundLoopBackXVersion,sizeof(SoundLoopBackXVersion),pGoldenServer->RecvInfo,"[VERSION:","]"))
			{
				strcat_s(pTI->Result,sizeof(pTI->Result),",");
				strcat_s(pTI->Result,sizeof(pTI->Result),SoundLoopBackXVersion);
			}
			else
			{
				amprintf("(GetsubStrInStr Fail;\n");
				strcat_s(pTI->Result,sizeof(pTI->Result),",FAIL");
				return 0;
			}

			if(strAllCmp(pTI->DiagCmd,SoundLoopBackXVersion))
			{
				amprintf("SoundLoopBackX Version can not match;\n");
				amprintf("The current version of SoundLoopBackX is %s;\n",SoundLoopBackXVersion);
				amprintf("Expect version is %s;\n",pTI->DiagCmd);
				return 0;
			}
			//Talen 2012/03/22

			if (atof(pTI->MultiResult[0])<atof(pTI->Spec)||atof(pTI->MultiResult[0])>atof(pTI->Spec_1))
			{
				amprintf("Separation L fail, spec is  DOWN: %f, UP: %f;\n",atof(pTI->Spec),atof(pTI->Spec_1));
				return 0;
			}
			else
			{

			}

			if (atof(pTI->MultiResult[1])<atof(pTI->Spec)||atof(pTI->MultiResult[1])>atof(pTI->Spec_1))
			{
				amprintf("Separation R fail, spec is  DOWN: %f, UP: %f;\n",atof(pTI->Spec),atof(pTI->Spec_1));
				return 0;
			}
			else
			{

			}

			if (atof(pTI->MultiResult[2])<atof(pTI->DownLimit)||atof(pTI->MultiResult[2])>atof(pTI->UpLimit))
			{
				amprintf("THD+N L fail, spec is  DOWN: %f, UP: %f;\n",atof(pTI->DownLimit),atof(pTI->UpLimit));
				return 0;
			}
			else
			{

			}

			if (atof(pTI->MultiResult[3])<atof(pTI->DownLimit)||atof(pTI->MultiResult[3])>atof(pTI->UpLimit))
			{
				amprintf("THD+N R fail, spec is  DOWN: %f, UP: %f;\n",atof(pTI->DownLimit),atof(pTI->UpLimit));
				return 0;
			}
			else
			{

			}
		}
		else
		{
			amprintf("[ANALOG] value return fail.\n");
			return 0;
		}
	}
	else if(strstr(pGoldenServer->RecvInfo,"[DIGITAL]"))
	{
		amprintf("[DIGITAL] test finish!.\n");
		if(strstr(pGoldenServer->RecvInfo,"PASS"))
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),"PASS");
			amprintf("[DIGITAL] result pass!.\n");
		}
		else if(strstr(pGoldenServer->RecvInfo,"FAIL"))
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),"FAIL");
			amprintf("[DIGITAL] result fail!.\n");
			digiResult=0;
		}
		else
		{
			amprintf("[DIGITAL] test return error!.\n");
			digiResult=0;
		}

		//Talen 2012/03/22
		if(GetsubStrInStr(SoundLoopBackXVersion,sizeof(SoundLoopBackXVersion),pGoldenServer->RecvInfo,"[VERSION:","]"))
		{
			strcat_s(pTI->Result,sizeof(pTI->Result),",");
			strcat_s(pTI->Result,sizeof(pTI->Result),SoundLoopBackXVersion);
		}
		else
		{
			amprintf("(GetsubStrInStr Fail;\n");
			strcat_s(pTI->Result,sizeof(pTI->Result),",FAIL");
			return 0;
		}

		if(strAllCmp(pTI->DiagCmd,SoundLoopBackXVersion))
		{
			amprintf("The current version of SoundLoopBackX is %s;\n",SoundLoopBackXVersion);
			amprintf("Expect version is %s;\n",pTI->DiagCmd);
			return 0;
		}
		//Talen 2012/03/22
		if(digiResult==0)
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test output error.\n");
		return 0;
	}

	return 1;
}
//Maxwell 20110329

//Maxwell 20110330
int CTestItemScript::WriteSNMACFile(TEST_ITEM *pTI)
{
	FILE *fLog;	
	
	char cmd[500]="";
	char PathBuf[256];
	char FileName[512];
	int iFileOpenErr = 0;
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/04/20
	
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\%s", PathBuf,cmd);
		iFileOpenErr = fopen_s(&fLog, FileName, "a+");
		if(!iFileOpenErr)
		{
			fclose(fLog);
		}
		///*if(!PathFileExists(FileName))
		//{	
		//	if(!CreateDirectory(FileName,NULL))
		//	{
		//		amprintf("Create WC.art Fail!\n");
		//		return 0;
		//	}
		//}*/
        //strcat_s(FileName, sizeof(FileName), pFileName);
		//Maxwell 090622 mutex operate log
		//WaitForSingleObject(hWriteLogMutex, INFINITE); 
		Sleep(1);
		iFileOpenErr = fopen_s(&fLog, FileName, "w+");
		if(!iFileOpenErr)
		{
			if(NULL == fLog)
			{
				return FALSE;
			}
			char MACAddress[32]="";

			if(strlen(gTI.UUTInfo.UUTSN)&&/*strlen(gTI.UUTInfo.UUTMAC1)&&*/strlen(gTI.UUTInfo.UUTMAC2)&&strlen(gTI.UUTInfo.UUTMAC3))	//remove UUTMAC1 by Talen 2011/045/02
			{
				//if(!AddquteToMACAddress(MACAddress,sizeof(MACAddress),gTI.UUTInfo.UUTMAC1))
				//{
				//	amprintf("(D)Check AddquteToMACAddress Fail and quit;");
				//	fclose(fLog);//Show message for debug add by Wind 20080121
				//	return 0;
				//}
				//else
				//{

				//}

				/*
				if(12!=strlen(gTI.UUTInfo.UUTMAC1))
				{
					amprintf("MAC address length is not 12;\n");
					fclose(fLog);
					return 0;
				}
				else
				{

				}
				*/

				if(12!=strlen(gTI.UUTInfo.UUTMAC2))
				{
					amprintf("Wifi MAC0 address length is not 12;\n");
					fclose(fLog);
					return 0;
				}
				else
				{

				}

				if(12!=strlen(gTI.UUTInfo.UUTMAC3))
				{
					amprintf("Wifi MAC1 address length is not 12;\n");
					fclose(fLog);
					return 0;
				}
				else
				{

				}

				char MacFront[32] = "";

				fprintf_s(fLog,"MAC_OPTION          = 3\n");
				fprintf_s(fLog,"SERIAL_OPTION       = 3\n");
				fprintf_s(fLog,"STATIC_MAC_ADDRESS_1           =             %s\n",gTI.UUTInfo.UUTMAC2);
				fprintf_s(fLog,"STATIC_MAC_ADDRESS_2           =             %s\n",gTI.UUTInfo.UUTMAC3);
				
				fprintf_s(fLog,"\n");
				fprintf_s(fLog,"STATIC_SERIAL_NUMBER	=	%s\n",gTI.UUTInfo.UUTSN);

				fclose(fLog);
			}
			else
			{
				fprintf_s(fLog," ");
				fclose(fLog);
				return 0;
			}
			
		}
		else
		{
			amprintf("No Serial_No file opened!\n");
			return 0;
		}
		//ReleaseMutex(hWriteLogMutex);
	}

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/04/20
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//Maxwell 20110330

//add start by Talen 2011/04/19
int CTestItemScript::CheckConnectionUsePing(TEST_ITEM *pTI)
{
    int PingTimes=0;
	CCenTime PingTime;
	char PingTimeBuf[50];
	PingTime.TimeStartCount();
	strcpy_s(pTI->Result,sizeof(pTI->Result),"Fail");
	amprintf("diag cmd:%s;\n",pTI->DiagCmd);
	PingTimes=atoi(pTI->UpLimit);

	if(!VerifyLinkUsePing(pTI->DiagCmd, PingTimes))
	{
		amprintf("Ping Fail;\n");
		return 0;
	}
	else
	{
		amprintf(" Ping Test PASS;\n");	
	}

	PingTime.GetElapseTime(PingTimeBuf,sizeof(PingTimeBuf));
	strcpy_s(pTI->Result,sizeof(pTI->Result),PingTimeBuf);
	Sleep(1000*atoi(pTI->DiagPerfTime));
	return 1;
}
//add end by Talen 2011/04/19


//add end by Talen 2011/05/11
int CTestItemScript::CtrlArtRun(TEST_ITEM *pTI)
{
 	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	char OutBuf[2048];
	DWORD dwExitCode;
	
	
	amprintf("%s;\n",cmd);
	
	if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run UUT CMD Fail;\n");
		return 0;
	}
	
	if(!strlen(OutBuf))
	{
		amprintf("No Output;\n");
		return 0;
	}

	if(!strstr(OutBuf,"outputs"))
	{
		amprintf("%s;\n",OutBuf);
		amprintf("Can not find \"outputs\" from output;\n");
		return 0;
	}

	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/05/11


//add start by Talen 2011/05/12
int CTestItemScript::AutoCOMReboot(TEST_ITEM *pTI)
{
	if((pTI->RebootCount>=0)&&(pTI->RebootCount%pTI->RebootTime == 0))
	{
		char cmd[512]="";
		char cmdFind[512]="";
		char OutBuf[2048]="";
		char Temp[1024]="";
		DWORD dwExitCode;

		strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
		strcpy_s(cmdFind,sizeof(cmdFind),pTI->DiagCmd);

		if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Run disable device command fail;\n");
			return 0;		
		}
		else
		{
			if(strstr(OutBuf,"device(s) disabled")&&(!strstr(OutBuf,"Disable failed")))
			{
				amprintf("Device disable Pass;\n");
				Sleep(1000);
			}
			else
			{
				amprintf("Device disable Fail;\n");
				return 0;
			}
		}
		memset(OutBuf,0,sizeof(OutBuf));

		if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Run enable command fail;\n");
			return 0;		
		}
		else
		{
			if(strstr(OutBuf,"device(s) enabled")&&(!strstr(OutBuf,"Enable failed")))
			{
				amprintf("Device enable Pass;\n");
				Sleep(1000);
			}
			else
			{
				amprintf("Device enable Fail;\n");
				return 0;
			}
		}
			
		amprintf("Device reboot Pass;\n");
		pTI->RebootCount++;
		Sleep(1000*atoi(pTI->DiagPerfTime));
	}
	else
	{
		pTI->RebootCount++;
	}

	return 1;
}
//add end by Talen 2011/05/12


//add end by Talen 2011/05/18
int CTestItemScript::CommitCaldataFlash(TEST_ITEM *pTI)
{
 	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	char OutBuf[2048];
	DWORD dwExitCode;
	
	
	amprintf("%s;\n",cmd);
	
	if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run UUT CMD Fail;\n");
		return 0;
	}
	
	if(!strlen(OutBuf))
	{
		amprintf("No Output;\n");
		return 0;
	}

	if(!strstr(OutBuf,"outputs"))
	{
		amprintf("Can not find \"outputs\" from output;\n");
		return 0;
	}

	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/05/18


//091127
int CTestItemScript::AutoFixtureControl(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char revbuf[8192]=""; 
	int revbufLen = 0;
	int ComTime=50;
	char spec[124]="";
	int errcode;
	DWORD dwWaitResult;

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/06/13
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	AutoFixResponseFlag = 0;

	//ResetEvent(hFixtureResposeEvent);
	WaitForSingleObject(hOperateComMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	//strcpy_s(cmd,sizeof(cmd),char(3));
	size_t m = strlen(cmd);

	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd);
		gComDev.close();
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	Sleep(200);
	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	revbufLen = 0;
	while(revbufLen==0&&i<atoi(pTI->DiagPerfTime))
	{
		gComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(i==atoi(pTI->DiagPerfTime))
	{
		amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(pTI->DiagPerfTime));
		gComDev.close();
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}

	amprintf("Auto fixture reply: %s!\n",revbuf);
	
	memset(spec,0,sizeof(spec));
	sprintf_s(spec,sizeof(spec),"%s\r\n",pTI->Spec);
	
	if (strstr(spec,revbuf))
	{
		amprintf("Right Command Reply !\n");
	}
	else
	{
		amprintf("Wrong Command Reply !\n");
		gComDev.close();
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}

	/*PostThreadMessage(AutoFixtureThreadID,WM_PROPERF_FIXTURE,(UINT)cmd,(UINT)strlen(cmd)*sizeof(unsigned char));

	if(WAIT_TIMEOUT == WaitForSingleObject(hFixtureResposeEvent, 5000))
	{
		amprintf("AutoFixture control timeout;\n");
		return 0;
	}

	if(AutoFixResponseFlag)
	{
		amprintf("Auto Fixture control fail;\n");
		return 0;
	}*/

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/06/13
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	//amprintf("UUT has been powered on;\n");
	ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01

	return 1;
}
//091127

//add start by Talen 2011/06/16
int CTestItemScript::AutoFixtureControlNoCheck(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char revbuf[8192]=""; 
	int revbufLen = 0;
	int ComTime=50;

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/06/13
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	AutoFixResponseFlag = 0;

	//ResetEvent(hFixtureResposeEvent);
	WaitForSingleObject(hOperateComMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",pTI->ComPort,pTI->TransRate);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
	//len = com.read(strtem, sizeof(strtem)); 

	//strcpy_s(cmd,sizeof(cmd),char(3));
	size_t m = strlen(cmd);
	
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd);
		gComDev.close();
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}
	
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/06/13
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	//amprintf("UUT has been powered on;\n");
	ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
	return 1;
}
//add end by Talen 2011/06/16

//add start by Talen 2011/05/25
int CTestItemScript::SetUUTChannel(TEST_ITEM *pTI)
{
 	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	char OutBuf[2048]="";
	char Temp[50]="";
	char setresult[10]="";
	DWORD dwExitCode;
	
	//add start by Talen 2011/08/06
	if (!RunExeFileLocal(pTI->DiagCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}

	if(GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"ssSK: ","\r\n"))
	{
		if(strstr(Temp,"error"))
		{
			amprintf("%s;\n",OutBuf);
			return 0;
		}
		else if(!strlen(Temp))
		{
		
		}
		else if(strstr(pTI->Spec,Temp))
		{
		
		}
		else if(strstr(pTI->Spec,"Undefine SPEC"))
		{
		
		}
		else
		{
			amprintf("The DUT SKU Can not support the channel;\n");
			return 1;
		}
	}
	else
	{
		amprintf("%s;\n",OutBuf);
		return 0;
	}
	//add end by Talen 2011/08/06
	memset(OutBuf,0,sizeof(OutBuf));
	dwExitCode=0;	

	amprintf("%s;\n",cmd);
	
	if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Run UUT CMD Fail;\n");
		return 0;
	}
	
	if(!strlen(OutBuf))
	{
		amprintf("No Output;\n");
		return 0;
	}

	if(!strstr(OutBuf,"outputs"))
	{
		amprintf("Can not find \"outputs\" from output;\n");
		return 0;
	}

	if (!ParseProperyResult(OutBuf, Temp, sizeof(Temp), "status", "<integer>"))
	{
		amprintf("%s;\n",OutBuf);
		amprintf("Test error,wrong parse;\n");
		return 0;
	}

	GetXmlIndexStr(Temp, 1, ',', setresult,sizeof(setresult));

	if(strAllCmp(setresult,"0"))
	{
		amprintf("%s;\n",OutBuf);
		amprintf("Set Channel fail;\n");
		return 0;
	}
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/05/05

//add start by Talen 2011/06/14
int CTestItemScript::BlockControl(TEST_ITEM *pTI)
{
	HANDLE hFixture = CreateMutex(NULL,FALSE,"DoubleFixture");//haibin.li 2013/8/26

	for (int i=0;i<60;i++)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hFixture);
			hFixture=NULL;
			Sleep(1000);
		}else{
			break;
		}
	}	

	amprintf("FIXTURE ID:%d\n",fix_id);

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");//add by Talen 2011/06/14
	if(fix_id==1)
	{
		if(0==MoveBlock(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->Spec))
		{
			amprintf("Control Block Fail!\n");
			ReleaseMutex(hFixture);//haibin.li 2013/8/26
			return 1;//change 0-->1 by Talen 2011/09/01
		}
	}
	else if(fix_id==2)
	{
		if(0==MoveBlock(pTI->ComPort,pTI->TransRate,pTI->DiagCmd,pTI->DiagPerfTime,pTI->Spec_1))
		{
			amprintf("Control Block Fail!\n");
			ReleaseMutex(hFixture);//haibin.li 2013/8/26
			return 1;//change 0-->1 by Talen 2011/09/01
		}
	}
	else
	{
		if(0==MoveBlock(pTI->ComPort,pTI->TransRate,pTI->UUTCmd,pTI->DiagPerfTime,pTI->Spec))
		{
			amprintf("Control Block Fail!\n");
			ReleaseMutex(hFixture);//haibin.li 2013/8/26
			return 1;//change 0-->1 by Talen 2011/09/01
		}
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");//add by Talen 2011/06/14
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	ReleaseMutex(hFixture);//haibin.li 2013/8/26
	return 1;
}
//add end by Talen 2011/06/14


//add start by haibin,li 06.13
//int CTestItemScript::CheckLedCmd(TEST_ITEM *pTI)
//{	
//	char OutBuf[2048]="";
//	DWORD dwExitCode;
//	char CmdBuf[2048]="";
//	char cmd[2048];
//	char analyse[2048];
//
//	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
//	/*if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
//	{
//		amprintf("OutBuf:%s", OutBuf);
//		return 0;		
//	}
//	else
//	{	
//		amprintf("OutBuf:%s", OutBuf);
//	}  */ 
//
//	strcpy_s(OutBuf,sizeof(OutBuf),"01234");
//
//	strcpy_s(analyse,sizeof(analyse),OutBuf);
//
//	if(strstr(OutBuf,"Failed")) 
//	{
//		amprintf("Error!Result of Command:%s", OutBuf);
//		return 0;
//	}
//	if(strstr(OutBuf,"error")) 
//	{
//		amprintf("Error!Result of Command:%s", OutBuf);
//		return 0;
//	}
//
//	char *posr=strstr(cmd,"-r");
//	char *posi=strstr(cmd,"-i");
//	char *posw=strstr(cmd,"-w");
//	char *pToke;
//	
//	char p1[2048]="",temp[200];
//	int r=0,g=0,b=0,i=0,j=0;
//	double w;
//	if(posr&&posi&&posw)
//	{
//		strSTEDcpy(temp,200,analyse,0xff,',');		
//		r=atoi(temp);
//		
//		pToke=strchr(analyse,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		g=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		b=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		i=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;	
//		//strSTEDcpy(temp,200,pToke,0xff,',');
//		w=atof(pToke);
//
//		sprintf_s(p1,sizeof(p1),"%d,%d,%d,%d,%.2f",r,g,b,i,w);		
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posr&&posw)
//	{		
//		strSTEDcpy(temp,200,analyse,0xff,',');		
//		r=atoi(temp);
//		
//		pToke=strchr(analyse,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		g=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		b=atoi(temp);
//		
//		pToke=strchr(pToke,',');
//	    pToke+=1;	    
//		w=atof(pToke);
//		
//		sprintf_s(p1,sizeof(p1),"%d,%d,%d,,%.2f",r,g,b,w);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posi&&posw)
//	{		
//		strSTEDcpy(temp,200,analyse,0xff,',');		
//		i=atoi(temp);
//		
//		pToke=strchr(analyse,',');
//	    pToke+=1;	    
//		w=atoi(pToke);		
//		
//		sprintf_s(p1,sizeof(p1),",,,%d,%.2f",i,w);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posw)
//	{			
//		w=atof(analyse);
//		
//		sprintf_s(p1,sizeof(p1),",,,,%.2f",w);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posr&&posi)
//	{	
//		strSTEDcpy(temp,200,analyse,0xff,',');		
//		r=atoi(temp);
//		
//		pToke=strchr(analyse,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		g=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		b=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;	    
//		i=atoi(pToke);
//		
//		sprintf_s(p1,sizeof(p1),"%d,%d,%d,%d,,",r,g,b,i);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posr)
//	{		
//		strSTEDcpy(temp,200,analyse,0xff,',');		
//		r=atoi(temp);
//		
//		pToke=strchr(analyse,',');
//	    pToke+=1;
//	    strSTEDcpy(temp,200,pToke,0xff,',');
//		g=atoi(temp);
//
//		pToke=strchr(pToke,',');
//	    pToke+=1;	   
//		b=atoi(pToke);		
//		
//		sprintf_s(p1,sizeof(p1),"%d,%d,%d,,",r,g,b);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else if(posi)
//	{			
//		i=atoi(analyse);		
//		
//		sprintf_s(p1,sizeof(p1),",,,%d,",i);
//		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
//	}
//	else 
//	{		
//		amprintf("UUT_CMD parameter error.");
//		strcpy_s(pTI->Result,sizeof(pTI->Result),"Command error!");
//		return 0;
//	}
//	if(r<pTI->RGBI_R_SPEC_DOWN||r>pTI->RGBI_R_SPEC_UP)
//	{
//		amprintf("R_SPEC_DOWN:%d\n",pTI->RGBI_R_SPEC_DOWN);
//		amprintf("R_SPEC_UP:%d\n",pTI->RGBI_R_SPEC_UP);
//		amprintf("Current R_SPEC:%d",r);
//		return 0;
//	}
//	if(g<pTI->RGBI_G_SPEC_DOWN||g>pTI->RGBI_G_SPEC_UP)
//	{
//		amprintf("G_SPEC_DOWN:%d\n",pTI->RGBI_G_SPEC_DOWN);
//		amprintf("G_SPEC_UP:%d\n",pTI->RGBI_G_SPEC_UP);
//		amprintf("Current G_SPEC:%d",g);
//		return 0;
//	}
//	if(b<pTI->RGBI_B_SPEC_DOWN||b>pTI->RGBI_B_SPEC_UP)
//	{
//		amprintf("B_SPEC_DOWN:%d\n",pTI->RGBI_B_SPEC_DOWN);
//		amprintf("B_SPEC_UP:%d\n",pTI->RGBI_B_SPEC_UP);
//		amprintf("Current B_SPEC:%d",b);
//		return 0;
//	}
//	if(i<pTI->RGBI_I_SPEC_DOWN||i>pTI->RGBI_I_SPEC_UP)
//	{
//		amprintf("I_SPEC_DOWN:%d\n",pTI->RGBI_I_SPEC_DOWN);
//		amprintf("I_SPEC_UP:%d\n",pTI->RGBI_I_SPEC_UP);
//		amprintf("Current I_SPEC:%d",i);
//		return 0;
//	}
//	if(w<pTI->RGBI_W_SPEC_DOWN||w>pTI->RGBI_W_SPEC_UP)
//	{
//		amprintf("W_SPEC_DOWN:%d\n",pTI->RGBI_W_SPEC_DOWN);
//		amprintf("W_SPEC_UP:%d\n",pTI->RGBI_W_SPEC_UP);
//		amprintf("Current W_SPEC:%f",w);
//		return 0;
//	}
//
//	return 1;	
//
//}

int CTestItemScript::CheckLedCmd(TEST_ITEM *pTI)
{		
	char OutBuf[2048]="";
	DWORD dwExitCode;
	char CmdBuf[2048]="";
	char cmd[2048];
	char analyse[2048];

	if(1==fix_id)
	{
		strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	}
	else if(2==fix_id)
	{
		strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
	}
	else
	{
		strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	}	
	if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("OutBuf:%s;", OutBuf);
		return 0;		
	}
	else
	{	
		amprintf("OutBuf:%s;", OutBuf);
	}   

	//strcpy_s(OutBuf,sizeof(OutBuf),"160,152,069,012345,456.00");

	strcpy_s(analyse,sizeof(analyse),OutBuf);

	if(strstr(OutBuf,"Failed")) 
	{
		amprintf("Error!Result of Command:%s;", OutBuf);
		return 0;
	}
	if(strstr(OutBuf,"error")) 
	{
		amprintf("Error!Result of Command:%s;", OutBuf);
		return 0;
	}

	char *posr=strstr(cmd,"-r");
	char *posi=strstr(cmd,"-i");
	char *posw=strstr(cmd,"-w");	
	
	char p1[2048]="",p2[2048]="",temp[100];
	int r=0,g=0,b=0,i=0,j=0;
	double w;
	if(posr&&posi&&posw)
	{
		for(j=0;j<3;j++)
			temp[j]=analyse[j];		
		r=atoi(temp);
		for(j=4;j<7;j++)
			temp[j-4]=analyse[j];
		g=atoi(temp);
		for(j=8;j<11;j++)
			temp[j-8]=analyse[j];
		b=atoi(temp);
		for(j=12;j<18;j++)
			temp[j-12]=analyse[j];
		i=atoi(temp);
		for(j=19;j<25;j++)
			temp[j-19]=analyse[j];
		w=atof(temp);
		sprintf_s(p1,sizeof(p1),"%d,%d,%d,%d,%.2f",r,g,b,i,w);		
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posr&&posw)
	{		
		for(j=0;j<3;j++)
			temp[j]=analyse[j];		
		r=atoi(temp);
		for(j=4;j<7;j++)
			temp[j-4]=analyse[j];
		g=atoi(temp);
		for(j=8;j<11;j++)
			temp[j-8]=analyse[j];
		b=atoi(temp);		
		for(j=12;j<18;j++)
			temp[j-12]=analyse[j];
		w=atof(temp);

		/*for(int pp=0;pp<12;pp++)
			p1[pp]=analyse[pp];		
		for(int pi=12;pi<18;pi++)
			p2[pi-12]=analyse[pi];
		strcat_s(p1,sizeof(p1),",");
		strcat_s(p1,sizeof(p1),p2);*/
		sprintf_s(p1,sizeof(p1),"%d,%d,%d,,%.2f",r,g,b,w);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posi&&posw)
	{		
		for(j=0;j<5;j++)
			temp[j]=analyse[j];
		i=atoi(temp);
		for(j=6;j<12;j++)
			temp[j-6]=analyse[j];
		w=atof(temp);

		/*strcpy_s(p1,sizeof(p1),",,,");
		strcat_s(p1,sizeof(p1),analyse);*/
		sprintf_s(p1,sizeof(p1),",,,%d,%.2f",i,w);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posw)
	{	
		for(j=0;j<6;j++)
			temp[j]=analyse[j];
		w=atof(temp);

		/*strcpy_s(p1,sizeof(p1),",,,,");
		strcat_s(p1,sizeof(p1),analyse);*/
		sprintf_s(p1,sizeof(p1),",,,,%.2f",w);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posr&&posi)
	{	
		for(j=0;j<3;j++)
			temp[j]=analyse[j];		
		r=atoi(temp);
		for(j=4;j<7;j++)
			temp[j-4]=analyse[j];
		g=atoi(temp);
		for(j=8;j<11;j++)
			temp[j-8]=analyse[j];
		b=atoi(temp);
		for(j=12;j<17;j++)
			temp[j-12]=analyse[j];
		i=atoi(temp);

		/*strcpy_s(p1,sizeof(p1),analyse);
		strcat_s(p1,sizeof(p1),",");*/
		sprintf_s(p1,sizeof(p1),"%d,%d,%d,%d,,",r,g,b,i);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posr)
	{		
		for(j=0;j<3;j++)
			temp[j]=analyse[j];		
		r=atoi(temp);
		for(j=4;j<7;j++)
			temp[j-4]=analyse[j];
		g=atoi(temp);
		for(j=8;j<11;j++)
			temp[j-8]=analyse[j];
		b=atoi(temp);

		/*strcpy_s(p1,sizeof(p1),analyse);
		strcat_s(p1,sizeof(p1),",,");*/
		sprintf_s(p1,sizeof(p1),"%d,%d,%d,,",r,g,b);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else if(posi)
	{		
		for(j=0;j<5;j++)
			temp[j]=analyse[j];
		i=atoi(temp);

		/*strcpy_s(p1,sizeof(p1),",,,");
		strcat_s(p1,sizeof(p1),analyse);
		strcat_s(p1,sizeof(p1),",");*/
		sprintf_s(p1,sizeof(p1),",,,%d,",i);
		strcpy_s(pTI->Result,sizeof(pTI->Result),p1);
	}
	else 
	{		
		amprintf("UUT_CMD parameter error.");
		strcpy_s(pTI->Result,sizeof(pTI->Result),"Command error!");
		return 0;
	}
	if(r<pTI->RGBI_R_SPEC_DOWN||r>pTI->RGBI_R_SPEC_UP)
	{
		amprintf("R_SPEC_DOWN:%d\n",pTI->RGBI_R_SPEC_DOWN);
		amprintf("R_SPEC_UP:%d\n",pTI->RGBI_R_SPEC_UP);
		amprintf("Current R_SPEC:%d",r);
		return 0;
	}
	if(g<pTI->RGBI_G_SPEC_DOWN||g>pTI->RGBI_G_SPEC_UP)
	{
		amprintf("G_SPEC_DOWN:%d\n",pTI->RGBI_G_SPEC_DOWN);
		amprintf("G_SPEC_UP:%d\n",pTI->RGBI_G_SPEC_UP);
		amprintf("Current G_SPEC:%d",g);
		return 0;
	}
	if(b<pTI->RGBI_B_SPEC_DOWN||b>pTI->RGBI_B_SPEC_UP)
	{
		amprintf("B_SPEC_DOWN:%d\n",pTI->RGBI_B_SPEC_DOWN);
		amprintf("B_SPEC_UP:%d\n",pTI->RGBI_B_SPEC_UP);
		amprintf("Current B_SPEC:%d",b);
		return 0;
	}
	if(i<pTI->RGBI_I_SPEC_DOWN||i>pTI->RGBI_I_SPEC_UP)
	{
		amprintf("I_SPEC_DOWN:%d\n",pTI->RGBI_I_SPEC_DOWN);
		amprintf("I_SPEC_UP:%d\n",pTI->RGBI_I_SPEC_UP);
		amprintf("Current I_SPEC:%d",i);
		return 0;
	}
	if(w<pTI->RGBI_W_SPEC_DOWN||w>pTI->RGBI_W_SPEC_UP)
	{
		amprintf("W_SPEC_DOWN:%d\n",pTI->RGBI_W_SPEC_DOWN);
		amprintf("W_SPEC_UP:%d\n",pTI->RGBI_W_SPEC_UP);
		amprintf("Current W_SPEC:%f",w);
		return 0;
	}

	return 1;	

}
//add end by haibin,li 06.13

//add start by haibin,li 07.21
int CTestItemScript::HiPotTest(TEST_ITEM *pTI)
{
	char cmd[500]="";
	char revbuf[500]=""; 
	int revbufLen = 0;	

	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");
	
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(pTI->ComPort,pTI->TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail;\n",pTI->ComPort,pTI->TransRate);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);


	sprintf_s(pTI->UUTCmd,sizeof(pTI->UUTCmd),"%s","SYSTem:LOCK:REQuest?");//Remote control by PC
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);	
	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s success;\n",pTI->UUTCmd);		
	}
	else
	{
		amprintf("Com command %s write fail;\n",pTI->UUTCmd);
		gComDev.close();
		return 0;
	}	

	sprintf_s(pTI->UUTCmd,sizeof(pTI->UUTCmd),"%s","SAFE:RES:AREP ON");//Auto reply
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);	
	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s success;\n",pTI->UUTCmd);		
	}
	else
	{
		amprintf("Com command %s write fail;\n",pTI->UUTCmd);
		gComDev.close();
		return 0;
	}


	sprintf_s(pTI->UUTCmd,sizeof(pTI->UUTCmd),"%s","SAFE:STARt");//start test
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);	
	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s success;\n",pTI->UUTCmd);		
	}
	else
	{
		amprintf("Com command %s write fail;\n",pTI->UUTCmd);
		gComDev.close();
		return 0;
	}
	Sleep(4000);	

	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	revbufLen = 0;
	while(revbufLen==0&&i<atoi(pTI->DiagPerfTime))
	{
		gComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(i==atoi(pTI->DiagPerfTime))
	{
		amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(pTI->DiagPerfTime));
		gComDev.close();
		return 0;
	}
	amprintf("Auto fixture reply: %s\n",revbuf);		
	if (strstr(revbuf,"PASS"))
	{
		amprintf("Test successful!\n");
	}	
	else
	{	
		sprintf_s(pTI->UUTCmd,sizeof(pTI->UUTCmd),"%s","SAFE:STOP");//Set to standby mode
		strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
		strcat_s(cmd,sizeof(cmd),"\r\n");
		Sleep(50);
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write %s success;\n",pTI->UUTCmd);
		}
		else
		{
			amprintf("Com command %s write fail;\n",pTI->UUTCmd);
			gComDev.close();
			return 0;
		}
		amprintf("Test fail!\n");
		gComDev.close();
		return 0;		
	}	
	sprintf_s(pTI->UUTCmd,sizeof(pTI->UUTCmd),"%s","SAFE:STOP");//Set to standby mode
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);   
	strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s success;\n",pTI->UUTCmd);		
	}
	else
	{
		amprintf("Com command %s write fail;\n",pTI->UUTCmd);
		gComDev.close();
		return 0;
	}
	
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));	

	return 1;
}
//add end by haibin,li 07.21

//add start by haibin,li 08.01
int CTestItemScript::VisualInspection(TEST_ITEM *pTI)
{ 
	char OutBuf[2048]="";
	DWORD dwExitCode;
	char analyse[2048];
	char temp[500];

	char SocIp[100];
	u_short port;
	double d[10];

	SOCKET     soc,sckComm;
    struct     sockaddr_in   client;
    char       szSendBuffer[128]="";
    int        iRetval,iLength;
    char       szBuffer[20];
	
	strcpy_s(gTI.LOGOInfo.OutBuf,sizeof(gTI.LOGOInfo.OutBuf),"");
	strcpy_s(gTI.LOGOInfo.LogoSN,sizeof(gTI.LOGOInfo.LogoSN),"");
	gTI.LOGOInfo.X_OFFSET=0.00;
	gTI.LOGOInfo.Y_OFFSET=0.00;
	gTI.LOGOInfo.ROTATE_ANGLE=0.00;
	gTI.LOGOInfo.AVG_HIST=0.00;
	gTI.LOGOInfo.CCORR=0.00;
	gTI.LOGOInfo.TOTAL_AREA=0.00;
	gTI.LOGOInfo.MAX_AREA=0.00;
	gTI.LOGOInfo.MAX_PERIMETER=0.00;
	gTI.LOGOInfo.SQUARE_WIDTH=0.00;
	gTI.LOGOInfo.SQUARE_HEIGHT=0.00;
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s"
		,",0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");
	strcpy_s(szSendBuffer,sizeof(szSendBuffer),pTI->UUTCmd);
	port=(u_short)(pTI->SocketPort);
	strcpy_s(SocIp,sizeof(SocIp),pTI->SocketIp);
    WSADATA    wsaData;
    iRetval=WSAStartup(0x202,&wsaData);
    if(iRetval!=0)
    {
        amprintf( "Load Winsock Fail! ");
        return 0;
    }
    soc=socket(AF_INET,SOCK_STREAM,0);
    if(soc==INVALID_SOCKET)
    {
        return 0;
    }
    client.sin_port=htons(port);
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr( SocIp);
    if(connect(soc,(struct sockaddr *)&client,sizeof(client))==-1)
    {
        amprintf( "Connect error!\n ");
	    return 0;
    }
	if((send(soc,szSendBuffer,sizeof(szSendBuffer),0)<0))
	{
		amprintf("can't send data to server!");
		return 0;
	}
	else
	{
		amprintf( "Send command:%s",szSendBuffer);
		iRetval=recv(soc,OutBuf,sizeof(OutBuf),0);
		if(iRetval==0) 
		{ 
			amprintf( "receive error.\n "); 
			return 0; 
		} 
		else 
		{
			amprintf( "OutBuf:%s.\n",OutBuf); 	
		}		
	}		
	strcpy_s(analyse,sizeof(analyse),OutBuf);	
	
	char *pToke;	
	
	pToke=strstr(analyse,"SN:");
	strSTEDcpy(gTI.LOGOInfo.LogoSN,sizeof(gTI.LOGOInfo.LogoSN),pToke,':',']');	
	pToke=strstr(analyse,"X_OFFSET:");	
	
	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.X_OFFSET=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.Y_OFFSET=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.ROTATE_ANGLE=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.AVG_HIST=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.CCORR=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.TOTAL_AREA=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.MAX_AREA=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.MAX_PERIMETER=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.SQUARE_WIDTH=atof(temp);
	pToke+=1;

	pToke=strchr(pToke,':');
	strSTEDcpy(temp,200,pToke,':',']');
	gTI.LOGOInfo.SQUARE_HEIGHT=atof(temp);
	pToke+=1;
	
	sprintf_s(gTI.LOGOInfo.OutBuf,sizeof(gTI.LOGOInfo.OutBuf),"%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
		,gTI.LOGOInfo.LogoSN,gTI.LOGOInfo.X_OFFSET,gTI.LOGOInfo.Y_OFFSET,gTI.LOGOInfo.ROTATE_ANGLE,gTI.LOGOInfo.AVG_HIST,gTI.LOGOInfo.CCORR,
		gTI.LOGOInfo.TOTAL_AREA,gTI.LOGOInfo.MAX_AREA,gTI.LOGOInfo.MAX_PERIMETER,gTI.LOGOInfo.SQUARE_WIDTH,gTI.LOGOInfo.SQUARE_HEIGHT);
	amprintf(gTI.LOGOInfo.OutBuf);
	strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.LOGOInfo.OutBuf);	
	closesocket(soc);
    WSACleanup();
	return 1;	
}
int CTestItemScript::CheckLogoSN(TEST_ITEM *pTI)
{	
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");	
	amprintf("Scan SN:%s\n",gTI.UUTInfo.UUTSN);
    amprintf("Laser SN:%s\n",gTI.LOGOInfo.LogoSN);
	if(strAllCmp(gTI.UUTInfo.UUTSN,gTI.LOGOInfo.LogoSN))
	{
		amprintf("SN not match!");
		return 0;
	}
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
int CTestItemScript::CheckLogoPosition(TEST_ITEM *pTI)
{
    sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");
	bool b[3];
	if(b[0]=(gTI.LOGOInfo.X_OFFSET<pTI->X_OFFSET_DOWN||gTI.LOGOInfo.X_OFFSET>pTI->X_OFFSET_UP))
	{
		amprintf("X_OFFSET_DOWN:%f\n",pTI->X_OFFSET_DOWN);
		amprintf("X_OFFSET_UP:%f\n",pTI->X_OFFSET_UP);
		amprintf("Current X_OFFSET:%f",gTI.LOGOInfo.X_OFFSET);
		//return 0;
	}
	if(b[1]=(gTI.LOGOInfo.Y_OFFSET<pTI->Y_OFFSET_DOWN||gTI.LOGOInfo.Y_OFFSET>pTI->Y_OFFSET_UP))
	{
		amprintf("Y_OFFSET_DOWN:%f\n",pTI->Y_OFFSET_DOWN);
		amprintf("Y_OFFSET_UP:%f\n",pTI->Y_OFFSET_UP);
		amprintf("Current Y_OFFSET:%f",gTI.LOGOInfo.Y_OFFSET);
		//return 0;
	}
	if(b[2]=(gTI.LOGOInfo.ROTATE_ANGLE<pTI->ROTATE_ANGLE_DOWN||gTI.LOGOInfo.ROTATE_ANGLE>pTI->ROTATE_ANGLE_UP))
	{
		amprintf("ROTATE_ANGLE_DOWN:%f\n",pTI->ROTATE_ANGLE_DOWN);
		amprintf("ROTATE_ANGLE_UP:%f\n",pTI->ROTATE_ANGLE_UP);
		amprintf("Current ROTATE_ANGLE:%f",gTI.LOGOInfo.ROTATE_ANGLE);
		//return 0;
	}
	if(b[0]||b[1]||b[2]) return 0;
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
int CTestItemScript::CheckLogoDepth(TEST_ITEM *pTI)
{
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");	
	if(gTI.LOGOInfo.AVG_HIST<pTI->AVG_HIST_DOWN||gTI.LOGOInfo.AVG_HIST>pTI->AVG_HIST_UP)
	{
		amprintf("AVG_HIST_DOWN:%f\n",pTI->AVG_HIST_DOWN);
		amprintf("AVG_HIST_UP:%f\n",pTI->AVG_HIST_UP);
		amprintf("Current AVG_HIST:%f",gTI.LOGOInfo.AVG_HIST);
		return 0;
	}		
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
int CTestItemScript::CheckLogoSkuMark(TEST_ITEM *pTI)
{
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");
	bool b[4];
	if(b[0]=(gTI.LOGOInfo.CCORR<pTI->CCORR_DOWN||gTI.LOGOInfo.CCORR>pTI->CCORR_UP))
	{
		amprintf("CCORR_DOWN:%f\n",pTI->CCORR_DOWN);
		amprintf("CCORR_UP:%f\n",pTI->CCORR_UP);
		amprintf("Current CCORR:%f",gTI.LOGOInfo.CCORR);
		//return 0;
	}
	if(b[1]=(gTI.LOGOInfo.TOTAL_AREA<pTI->TOTAL_AREA_DOWN||gTI.LOGOInfo.TOTAL_AREA>pTI->TOTAL_AREA_UP))
	{
		amprintf("TOTAL_AREA_DOWN:%f\n",pTI->TOTAL_AREA_DOWN);
		amprintf("TOTAL_AREA_UP:%f\n",pTI->TOTAL_AREA_UP);
		amprintf("Current TOTAL_AREA:%f",gTI.LOGOInfo.TOTAL_AREA);
		//return 0;
	}
	if(b[2]=(gTI.LOGOInfo.MAX_AREA<pTI->MAX_AREA_DOWN||gTI.LOGOInfo.MAX_AREA>pTI->MAX_AREA_UP))
	{
		amprintf("MAX_AREA_DOWN:%f\n",pTI->MAX_AREA_DOWN);
		amprintf("MAX_AREA_UP:%f\n",pTI->MAX_AREA_UP);
		amprintf("Current MAX_AREA:%f",gTI.LOGOInfo.MAX_AREA);
		//return 0;
	}
	if(b[3]=(gTI.LOGOInfo.MAX_PERIMETER<pTI->MAX_PERIMETER_DOWN||gTI.LOGOInfo.MAX_AREA>pTI->MAX_PERIMETER_UP))
	{
		amprintf("MAX_PERIMETER_DOWN:%f\n",pTI->MAX_PERIMETER_DOWN);
		amprintf("MAX_PERIMETER_UP:%f\n",pTI->MAX_PERIMETER_UP);
		amprintf("Current MAX_PERIMETER:%f",gTI.LOGOInfo.MAX_AREA);
		//return 0;
	}	
	if(b[0]||b[1]||b[2]||b[3]) return 0;
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
int CTestItemScript::CheckLogoSquareSize(TEST_ITEM *pTI)
{
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Fail");
	bool b[2];
	if(b[0]=(gTI.LOGOInfo.SQUARE_WIDTH<pTI->SQUARE_WIDTH_DOWN||gTI.LOGOInfo.SQUARE_WIDTH>pTI->SQUARE_WIDTH_UP))
	{
		amprintf("SQUARE_WIDTH_DOWN:%f\n",pTI->SQUARE_WIDTH_DOWN);
		amprintf("SQUARE_WIDTH_UP:%f\n",pTI->SQUARE_WIDTH_UP);
		amprintf("Current SQUARE_WIDTH:%f",gTI.LOGOInfo.SQUARE_WIDTH);
		//return 0;
	}
	if(b[1]=(gTI.LOGOInfo.SQUARE_HEIGHT<pTI->SQUARE_HEIGHT_DOWN||gTI.LOGOInfo.SQUARE_HEIGHT>pTI->SQUARE_HEIGHT_UP))
	{
		amprintf("SQUARE_HEIGHT_DOWN:%f\n",pTI->SQUARE_HEIGHT_DOWN);
		amprintf("SQUARE_HEIGHT_UP:%f\n",pTI->SQUARE_HEIGHT_UP);
		amprintf("Current SQUARE_HEIGHT:%f",gTI.LOGOInfo.SQUARE_HEIGHT);
		//return 0;
	}
	if(b[0]||b[1]) return 0;
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
	return 1;
}
//add end by haibin,li 08.01

//add start by haibin,li 08.23
int CTestItemScript::CheckSensorPosition(TEST_ITEM *pTI)
{
	int WaitTime=0;
    DWORD dwWaitResult;	
	char cmd[500]="";
	char revbuf[500]=""; 
	int revbufLen = 0;		
	
	if(!strcmp(gTI.TestStaInfo.TestStaName,"HIPOT"))
	{
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","Pass");
		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(pTI->ComPort,pTI->TransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail;\n",pTI->ComPort,pTI->TransRate);
			//return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",pTI->ComPort,pTI->TransRate);
		strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
		strcat_s(cmd,sizeof(cmd),"\r\n");
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write %s;\n",cmd);
		}
		Sleep(500);
		memset(revbuf,'\0',sizeof(revbuf));
		int j = 0;
		revbufLen = 0;
		while(revbufLen==0&&j<atoi(pTI->DiagPerfTime))
		{
			gComDev.read(revbuf,(int)sizeof(revbuf));
			revbufLen = (int)strlen(revbuf);
			Sleep(100);
			j++;
		}
		amprintf("Auto fixture reply: %s!\n",revbuf);
		if(strstr(revbuf,pTI->Spec))
		{
			return 1;
		}
		else
		{
			amprintf("MSGBOX=CONTENT[Is the Fixture LED GREEN?]\n;");	
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			if (!ResetEvent(gTI.UiResMsg.hEvent)) 
			{ 
				printf("ResetEvent failed (%d)\n", GetLastError());
			} 
			strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);	
			WaitTime=atoi(pTI->DiagPerfTime);
			Sleep(1000*WaitTime);
			amprintf("Wait %s seconds;\n",pTI->DiagPerfTime);
		}
	}
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atoi(pTI->CmdPerfTime));	

	return 1;
	
}
//add end by haibin,li 08.23

//add start by Talen 2011/06/23
int CTestItemScript::CheckSKUTested(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	DWORD dwExitCode;
	DWORD dwWaitResult;
	int errcode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//Check local
		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"ssSK: ","\r\n");
		if (Result)
		{
			if(strstr(Temp,"error"))
			{
				amprintf("%s;",Temp);
				return 0;
			}
			amprintf("UUT local information: %s;\n", Temp);
			amprintf("UUT local information in Config File: %s;", pTI->Spec);

			if(!strlen(Temp))
			{
				sprintf_s(pTI->Result,sizeof(pTI->Result),"%s","");
				return 1;
			}
			else
			{
				sprintf_s(pTI->Result,sizeof(pTI->Result),"%s",Temp);
				if(strstr(pTI->Spec,Temp))
				{
					return 1;
				}
				else
				{
					amprintf("MSGBOX=CONTENT[%s%s%s];\n","Please put the DUT in the chamber for ",Temp," !");
					dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
					if (!ResetEvent(gTI.UiResMsg.hEvent)) 
					{ 
						printf("ResetEvent failed (%d)\n", GetLastError());
					}
					KillTargetProcess("AmbitProT.exe");
					_exit(errcode);
					return 0;
				}
			}

		}
		else
		{
			return 0;
		}
	}
	return 1;
}

//add end by Talen 2011/06/23

//add start by Talen 2011/06/27
int CTestItemScript::CheckUUTDbugOff(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char *Toke=NULL;
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		amprintf("%s;\n",OutBuf);
		if(!strstr(OutBuf,"dbug:"))
		{
			amprintf("Can not find \"dbug\"!\n");
			return 0;
		}
		if(strstr(OutBuf,"kConnectionErr"))
		{
			return 0;
		}
		if(strstr(OutBuf,"kACPUnknownPropertyErr"))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
//add end by Talen 2011/06/27

//add start by Talen 2011/06/29
int CTestItemScript::CheckUUTDbugOn(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char OutBuf[2048]="";
	char *Toke=NULL;
	DWORD dwExitCode;
				
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		amprintf("%s;\n",OutBuf);
		if(!strstr(OutBuf,"dbug:"))
		{
			amprintf("Can not find \"dbug\"!\n");
			return 0;
		}

		if(strstr(OutBuf,"kConnectionErr"))
		{
			return 0;
		}

		Result=GetsubStrInStr(Temp,sizeof(Temp),OutBuf,"dbug: ","\r\n");
		//amprintf("UUT dbug mode value: %s;\n", Temp);

		
		if (Result)
		{
			strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
			amprintf("UUT dbug mode value: %s;\n", pTI->Result);
			amprintf("UUT dbug mode value in Config File SPEC: %s;\n", pTI->Spec);
			amprintf("UUT dbug mode value in Config File SPEC_1: %s;\n", pTI->Spec_1);
			if (!strAllCmp(pTI->Result,pTI->Spec)||!strAllCmp(pTI->Result,pTI->Spec_1))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
    return 1;
}
//add end by Talen 2011/06/29


//add start by Talen 2011/06/27
int CTestItemScript::CheckAndSetDbugOn(TEST_ITEM *pTI)
{
	char Temp[2048]="";
	int Result=1;
	char cmd[512]="";
	char OutBuf[2048]="";
	DWORD dwExitCode;

	amprintf("%s;\n",pTI->UUTCmd);
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Perform ACP property Fail;\n");
		return 0;		
	}
	else
	{
		//amprintf("%s;\n",OutBuf);
		if(!strstr(OutBuf,"dbug:"))
		{
			amprintf("Can not find \"dbug:\"!\n");
			return 0;
		}

		if(strstr(OutBuf,"kConnectionErr"))
		{
			return 0;
		}

		if(strstr(OutBuf,"0x00000001") || strstr(OutBuf,"0x00000000"))
		{
			return 1;
		}

		else if(strstr(OutBuf,"kACPUnknownPropertyErr"))
		{
			strcpy_s(cmd,sizeof(cmd),pTI->DiagCmd);
			amprintf("%s;\n",cmd);
			if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
			{
				return 0;
			}
			if (dwExitCode>0)
			{
				//return 0;
			}

			amprintf("Wait %s time;\n",pTI->DiagPerfTime);
			Sleep(1000*atoi(pTI->DiagPerfTime));

			int PingTime=0;
			amprintf("diag cmd:BPing.exe -s 10.0.1.1;\n");
			PingTime=atoi(pTI->UpLimit);

			if(!VerifyLink("BPing.exe -s 10.0.1.1", PingTime))
			{
				amprintf("Ping Fail;\n");
				return 0;
			}
			else
			{
				amprintf(" Ping %d times PASS;\n", PingTime);
				amprintf("Sleep %s seconds after Ping successful;\n",pTI->CmdPerfTime);
				Sleep(1000*atoi(pTI->CmdPerfTime));	
			}	
			return 1;
		}
		
		else
		{
			return 0;
		}
	}
	return 1;
}
//add end by Talen 2011/06/27

//liu-chen 2011/06/27
int CTestItemScript::CheckAndSetRadioOn(TEST_ITEM *pTI)
{
	char cmd[512]="";
	char OutBuf[4096]="";
	char OutBuf1[4096]="";
	char AnaTemp2[1024]="";
	char AnaTemp5[1024]="";
	char Temp2[100]="";
	char Temp5[100]="";
	char *pTok2=NULL;
	char *pTok5=NULL;
	DWORD dwExitCode=0;
	int PingTime=0;
	PingTime=atoi(pTI->UpLimit);

	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);

	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf1, sizeof(OutBuf1), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	//Sleep(2000);
    pTok2=strstr(OutBuf1,"WiFi:");
	if (pTok2)
	{

		if (!GetsubStrInStr(AnaTemp2, sizeof(AnaTemp2),pTok2,"raSk","raT2"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}


		if (!ParseProperyResult(AnaTemp2, Temp2, sizeof(Temp2), "raSt", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		if ((atoi(Temp2)!=0))
		{
			sprintf_s(cmd,sizeof(cmd),"acp.exe -a 10.0.1.1 -p public setplistvalue WiFi radios.[0].raSt 0");
			if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
			{
				amprintf("Test error,Can not open the ACP tool;\n");
				return 0;
			}
		}
	}
	pTok5=strstr(OutBuf1,"WiFi:");
	pTok5=strstr(OutBuf1,"</dict>");
	if (pTok5)
	{

		if (!GetsubStrInStr(AnaTemp5, sizeof(AnaTemp5),pTok5,"raSk","raT2"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}


		if (!ParseProperyResult(AnaTemp5, Temp5, sizeof(Temp5), "raSt", "<integer>"))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}		
		if (atoi(Temp5)!=0)
		{
			sprintf_s(cmd,sizeof(cmd),"acp.exe -a 10.0.1.1 -p public setplistvalue WiFi radios.[1].raSt 0");
			if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
			{
				amprintf("Test error,Can not open the ACP tool;\n");
				return 0;
			}
		}
	}
	if(strstr(cmd,"acp.exe -a 10.0.1.1 -p public setplistvalue WiFi radios")!=NULL)
	{
		amprintf("Radio off and turn Radio on\n");
		sprintf_s(cmd,sizeof(cmd),"acp.exe -a 10.0.1.1 -p public acFN= acRI=");
		if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("Test error,Can not open the ACP tool;\n");
			return 0;
		}
		
		amprintf("Wait %s seconds time after reboot;\n",pTI->DiagPerfTime);
		Sleep(1000*atoi(pTI->DiagPerfTime));

		amprintf("diag cmd:BPing.exe -s 10.0.1.1;\n");

		if(!VerifyLink("BPing.exe -s 10.0.1.1", PingTime))
		{
			amprintf("Ping Fail;\n");
			return 0;
		}
		else
		{
			amprintf(" Ping %d times PASS;\n", PingTime);
			amprintf("Sleep %s seconds after Ping successful;\n",pTI->CmdPerfTime);
			Sleep(1000*atoi(pTI->CmdPerfTime));
			
		}
		return 1;
	}
	return 1;
}
//liu-chen 2011/06/27

//add start by Talen 2011/07/04
int CTestItemScript::WriteSNPNtoTxt(TEST_ITEM *pTI)
{
	FILE *snStream=NULL;
	FILE *okStream=NULL;
	int flag=0;
	char temp[128]="";
	int count = atoi(pTI->DiagPerfTime);

	amprintf("%s;\n",gTI.UUTInfo.UUTSN);

	if(PathFileExists(pTI->DiagCmd))
	{	
		if(!DeleteFile(pTI->DiagCmd))
		{
			amprintf("Delete File %s Fail;\n",pTI->DiagCmd);
			return 0;
		}
		amprintf("Delete File %s Pass;\n",pTI->DiagCmd);
	}

	snStream=fopen(pTI->UUTCmd,"w");
	if(!snStream)
	{
		amprintf("Open File %s Fail;\n",pTI->UUTCmd);
		return 0;
	}

	memset(temp,0,sizeof(temp));
	sprintf_s(temp,sizeof(temp),"%s",gTI.UUTInfo.UUTSN);
	strcat_s(temp,sizeof(temp),"\r\n");
	strcat_s(temp,sizeof(temp),gTI.UUTInfo.UUTPN);

	fwrite(temp,1,strlen(temp),snStream);

	if(fclose(snStream))
	{
		amprintf("Close File %s Fail;\n",pTI->UUTCmd);
		return 0;
	}

	amprintf("Write SN to File %s Pass;\n",pTI->UUTCmd);

	okStream=fopen(pTI->DiagCmd,"w+");
	if(!okStream)
	{
		amprintf("Open File %s Fail;\n",pTI->DiagCmd);
		return 0;
	}

	if(fclose(okStream))
	{
		amprintf("Close File %s Fail;\n",pTI->DiagCmd);
		return 0;
	}

	amprintf("Create File %s Pass;\n",pTI->DiagCmd);

	while(count--)
	{
		if(!PathFileExists(pTI->DiagCmd))
		{
			flag=1;
			break;
		}
		Sleep(1000);
	}

	if(flag)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
//add end by Talen 2011/07/04


//add start by Talen 2011/07/04
int CTestItemScript::CheckUUTch(TEST_ITEM *pTI)
{
	int nCh2G=0;
	int nCh5G=0;
	char WiFiBuf[20480]="";
	char WiFiCMD[256]="acp.exe -a 10.0.1.1 -p public WiFi";
	DWORD dwExitCode=0;
	if (!RunExeFileLocal(WiFiCMD, WiFiBuf, sizeof(WiFiBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
	int ret=GetCurrentCh(WiFiBuf,nCh2G,nCh5G);
	if(ret==0)
	{
		amprintf("Can not get Channel Info\n");
		return 0;
	}
	if(!strcmp(pTI->Spec,"2G"))
	{
		amprintf("Channel In Config File :%s\n",pTI->Spec_1);
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%d",nCh2G);
		amprintf("Get Current Channel:%s\n",pTI->Result);
	
		if(atoi(pTI->Spec_1)==atoi(pTI->Result))
		{
			amprintf("Get Current Channel:%s,Test PASS\n",pTI->Result);
			
		}
		else
		{
			amprintf("Get Current Channel:%s,Test FAIL\n",pTI->Result);
			return 0;
		}
	}
	else if(!strcmp(pTI->Spec,"5G"))
	{
		amprintf("Channel In Config File :%s\n",pTI->Spec_1);
		sprintf_s(pTI->Result,sizeof(pTI->Result),"%d",nCh5G);
		amprintf("Get Current Channel:%s\n",pTI->Result);
		
		if(atoi(pTI->Spec_1)==atoi(pTI->Result))
		{
			amprintf("Get Current Channel:%s,Test PASS\n",pTI->Result);
		}
		else
		{
			amprintf("Get Current Channel:%s,Test FAIL\n",pTI->Result);
			return 0;
		}
	}
	else
	{
		amprintf("Please Check Your SPEC!\n");
		return 0;
	}

	return 1;
	
}
//add end by Talen 2011/07/04

//add start by Talen 2011/07/07
int CTestItemScript::SetUUTCheckReturn(TEST_ITEM *pTI)
{
	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	char OutBuf[512]="";
	DWORD dwExitCode;
	
	memset(OutBuf,0,sizeof(OutBuf));
	
	amprintf("%s;\n",cmd);
	if (!RunExeFileLocal(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
	if (dwExitCode>0)
	{
       //return 0;
	}
	
	if(!strstr(OutBuf,pTI->Spec))
	{
		amprintf("Return Value: %s;\n",OutBuf);
		amprintf("Spec: %s;\n",pTI->Spec);
		amprintf("The return value can not match spec;\n");
		return 0;
	}
	
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/07/07

//add start by Talen 2011/07/08
int CTestItemScript::RunProcessNoWait(TEST_ITEM *pTI)
{
	char cmd[500];
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	

	
	amprintf("%s;\n",cmd);
	if (!RunOneProcess(cmd))
	{
		return 0;
	}
	
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/07/08


//add start by joyce 2011/07/08
int CTestItemScript::CheckProductWeight(TEST_ITEM *pTI)
{

	 char Recv[2048];
	 char Temp[2048]="";
	 double Weight=0;
     int Result=0;
	 int TestResult=0;
	 
	//Open com
     _thread_com Com;
    if (!Com.is_open()) 
	 {
		 if (!Com.open(pTI->ComPort, 9600))
		 {		
			 amprintf("Com cann't be open");
			  	
			 return 0;
         }
	 }
	 else 
	 {
		  amprintf("com may been opened");
		 	
		 return 0;
	 }
 //start by joyce 2011/07/29 
    //judge spec
	double uplimit=atof(pTI->UpLimit);
	double downlimit=atof(pTI->DownLimit);

	 amprintf("LIMIT_UP:%s;\n",pTI->UpLimit);
	 amprintf("LIMIT_DW:%s;\n",pTI->DownLimit);
	 //end by joyce 2011/07/29
	 for(int i=0;i<20;i++)
	 {
		memset(Recv,0,2000); 
		Com.read(Recv,2048);
		//amprintf("recv:%s\n",Recv);
	    Result=GetsubStrInStr(Temp,sizeof(Temp),Recv,"+","g");
		if (Result)
		{
			char   *   p   =  remove(Temp,Temp+2048,' ');   
            *p   =   0;
            Weight=atof(Temp);
			 //start by joyce 2011/07/29 
			 if (Weight>=downlimit &&Weight<=uplimit) 
			// if (Weight>=atof(pTI->DownLimit)&&Weight<=atof(pTI->UpLimit)) 
			 //end by joyce 2011/07/29
			 {
				TestResult=1;
				 break;
			}
		}
		
		 Sleep(500);
	 }
	 
	Com.close();
	//start by joyce 2011/07/29
	sprintf_s(pTI->Result,sizeof(pTI->Result),"%s",Temp);
	amprintf("UUT Product Weight: %s;\n",Temp);
	 
   // strcpy_s(pTI->Result,sizeof(pTI->Result),Temp);
	//amprintf("UUT Product Weight: %s;\n", Temp);
   //end by joyce 2011/07/29
	return TestResult;
}

//add end by joyce 2011/07/08

//add start by Talen 2011/08/15
int CTestItemScript::CheckWanPortConnection(TEST_ITEM *pTI)
{
	char OutBuf[512]="";
	DWORD dwExitCode;
	char wanIP[128]="";
	char cmd[512]="";
	int timeout=atoi(pTI->Spec);
	
	
	amprintf("%s;\n",pTI->UUTCmd);
	do
	{
		memset(OutBuf,0,sizeof(OutBuf));
		if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
		{
			amprintf("run cmd fail;\n");
			return 0;
		}
		if (dwExitCode>0)
		{
		   //return 0;
		}
		if(strstr(OutBuf,"0.0.0.0"))
		{
			Sleep(1000);
		}
		else
		{
			break;
		}
	}while(timeout--);

	amprintf("%s;\n",OutBuf);

	char *pToke=strstr(OutBuf,"waIP: ");

	if(!pToke)
	{
		return 0;
	}
	
	GetsubStrInStr(wanIP,sizeof(wanIP),OutBuf,"waIP: ","\r\n");

	memset(cmd,0,sizeof(cmd));
	
	sprintf_s(cmd,sizeof(cmd),"BPing.exe -s %s",wanIP);

	amprintf(cmd);

	if(!VerifyLink(cmd, atoi(pTI->UpLimit)))
	{
		amprintf("Ping Fail;\n");
		return 0;
	}
	
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/08/15


//add start by Talen 2011/08/30
int CTestItemScript::CheckLedBehavior(TEST_ITEM *pTI)
{
	DWORD dwWaitResult;
	if(0==gTI.UUTInfo.TotalResultFlag)
	{
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);//add by haibin,li 07.27
		amprintf("MSGBOX=CONTENT[Is system led blink normally?];\n");
		// Reset gTI.UiResMsg.hEvent to nonsignaled, to block readers.
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		{ 
			printf("ResetEvent failed (%d)\n", GetLastError());
		}
		if(2==atoi(gTI.UiResMsg.MsgRes))
		{
			gTI.UUTInfo.LedBlink=0;//Talen 2011/09/09
			//strcpy_s(gTI.UUTInfo.LedBlink,sizeof(pTI->Result),"0");
		}
		else
		{
			gTI.UUTInfo.LedBlink=1;//Talen 2011/09/09
			//strcpy_s(pTI->Result,sizeof(pTI->Result),"1");
		}
		//strcpy_s(pTI->Result,sizeof(pTI->Result),gTI.UiResMsg.MsgRes);
		amprintf("Wait %s time;\n",pTI->CmdPerfTime);
		Sleep(1000*atof(pTI->CmdPerfTime));
		return 1;
	}
	gTI.UUTInfo.LedBlink=1;
	//strcpy_s(pTI->Result,sizeof(pTI->Result),"1");
	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/08/30


//add start by Talen 2011/08/31
int CTestItemScript::WaitForTest(TEST_ITEM *pTI)
{
	char FileName[512]="";
	int count=0;
	int timeout=10*atoi(pTI->UpLimit);

	memset(FileName,0,sizeof(FileName));

	sprintf_s(FileName,sizeof(FileName),"%s\\%s",gTI.TestStaInfo.LocalFilePath,pTI->Spec);

	WaitForSingleObject(hOneTestingMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);
	while(count<=timeout)
	{
		if(!PathFileExists(FileName))
		{
			amprintf("No file %s exist!\n",FileName);
			break;
		}
		Sleep(100);
	}

	if(count>timeout)
	{
		amprintf("%s still exit!\n",FileName);
		amprintf("%ss timeout!\n",pTI->UpLimit);
		return 0;
	}
	FILE *stream=NULL;

	if(fopen_s(&stream,FileName,"a+"))
	{
		amprintf("CreateFile %s Fail;\n",FileName);
		ReleaseMutex(hOneTestingMutex);//add by Talen 2011/09/01
		return 0;
	}
	if(fclose(stream))
	{
		amprintf("Close %s Fail;\n",FileName);
		ReleaseMutex(hOneTestingMutex);//add by Talen 2011/09/01
		return 0;
	}
	ReleaseMutex(hOneTestingMutex);//add by Talen 2011/09/01

	amprintf("CreateFile %s Pass;\n",FileName);

	amprintf("Wait %s time;\n",pTI->CmdPerfTime);
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/08/31


//Talen
int CTestItemScript::PerformPCLI(TEST_ITEM *pTI)
{
	char OutBuf[2048];
	memset(OutBuf,0,sizeof(OutBuf));
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";

	//get the wireless card MAC ID					
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		//amprintf("Get UUT SN Fail;\n");
		return 0;		
	}
	else
	{		
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;", OutBuf);
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, "0xbb000158 = ");
		if (!pToke)
		{
			amprintf("%s;", OutBuf);
			amprintf("can not find 0xbb000158 = ");
			return 0;
		}

		GetsubStrInStr(AnaTemp,sizeof(AnaTemp),OutBuf," = "," ");
		amprintf("PCLI: %s;\n", AnaTemp);

		strcpy_s(pTI->Result,sizeof(pTI->Result),AnaTemp);
	}
	amprintf("SPEC in configfile: %s;\n",pTI->Spec);
	amprintf("Get from UUT %s;\n",pTI->Result);	
	if (strAllCmp(pTI->Spec,pTI->Result))
	{
		return 0;
	}
	return 1;
}

//add start by Talen 2011/10/28
int CTestItemScript::CheckUUTInfo(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
	
	CString uutInfo;
	//get the wireless card MAC ID
	amprintf("Cmd :%s;\n",pTI->UUTCmd);
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get UUT Information Fail;\n");
		amprintf("%s;\n",OutBuf);
		return 0;		
	}
	else
	{	
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;\n", OutBuf);
			amprintf("Get UUT Information Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, pTI->DiagCmd);
		if (!pToke)
		{
			amprintf("%s;\n", OutBuf);
			amprintf("Get UUT Information Fail;\n");
			return 0;
		}
		if(strlen(pTI->DiagCmd))
		{
			GetsubStrInStr(AnaTemp,sizeof(AnaTemp),OutBuf,": ","\r");
			uutInfo=AnaTemp;
		}
		else
		{
			uutInfo=OutBuf;
			uutInfo.TrimRight("\r\n");
		}
		amprintf("UUT Information: %s;\n", AnaTemp);
		strcpy_s(pTI->Result,sizeof(pTI->Result),uutInfo);		
	}
	amprintf("Scan UUT Information: %s;\n",pTI->Spec);
	amprintf("Get UUT Information: %s;\n",pTI->Result);
	
	if(!strAllCmp(pTI->DiagCmd,"waMA"))
	{
		uutInfo.Remove(':');
	}

	uutInfo.MakeUpper();

	CString spec(pTI->Spec);
	spec.MakeUpper();
	if (uutInfo.Compare(spec))
	{
		return 0;
	}
	amprintf("Wait %.f time;\n",atof(pTI->CmdPerfTime));
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/10/28

//add start by Talen 2011/10/28
int CTestItemScript::ReadUUTInfo(TEST_ITEM *pTI)
{
	char OutBuf[2048]="";
	char AnaTemp[2048]="";
	DWORD dwExitCode;
	char *pToke = NULL;
	char CmdBuf[2048]="";
	
	CString uutInfo;
	//get the wireless card MAC ID
	amprintf("Cmd :%s;\n",pTI->UUTCmd);
	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Get UUT Information Fail;\n");
		amprintf("%s;\n",OutBuf);
		return 0;		
	}
	else
	{	
		pToke=NULL;
		pToke = strstr(OutBuf, "error:");
		if (pToke)
		{
			amprintf("%s;\n", OutBuf);
			amprintf("Get UUT Information Fail;\n");
			return 0;
		}

		pToke=NULL;
		pToke = strstr(OutBuf, pTI->DiagCmd);
		if (!pToke)
		{
			amprintf("%s;\n", OutBuf);
			amprintf("Get UUT Information Fail;\n");
			return 0;
		}
		if(strlen(pTI->DiagCmd))
		{
			GetsubStrInStr(AnaTemp,sizeof(AnaTemp),OutBuf,": ","\r");
			uutInfo=AnaTemp;
		}
		else
		{
			uutInfo=OutBuf;
			uutInfo.TrimRight("\r\n");
		}
		amprintf("UUT Information: %s;\n", AnaTemp);
		strcpy_s(pTI->Result,sizeof(pTI->Result),uutInfo);		
	}
	amprintf("Scan UUT Information: %s;\n",pTI->Spec);
	amprintf("Get UUT Information: %s;\n",pTI->Result);
	
	if(!strAllCmp(pTI->DiagCmd,"waMA"))
	{
		uutInfo.Remove(':');
	}

	uutInfo.MakeUpper();

	CString spec(pTI->Spec);
	spec.MakeUpper();
	if (uutInfo.Compare(spec))
	{
//		return 0;
	}
	amprintf("Wait %.f time;\n",atof(pTI->CmdPerfTime));
	Sleep(1000*atof(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2011/10/28

//add start by Talen 2011/10/28
int CTestItemScript::SetUUTInfo(TEST_ITEM *pTI)
{
	char cmd[500];
	char MACAddress[32]="";
	char uutInfo[256]="";
	
	
	strcpy_s(cmd,sizeof(cmd),pTI->UUTCmd);
	strcat_s(cmd,sizeof(cmd)," ");
	strcat_s(cmd,sizeof(cmd),pTI->DiagCmd);
	strcat_s(cmd,sizeof(cmd),"=");
	amprintf("Scan UUT Information : %s;\n",pTI->Spec);
	
	memset(uutInfo,0,sizeof(uutInfo));
	if(!strAllCmp(pTI->DiagCmd,"waMA"))
	{
		if(!AddquteToMACAddress(uutInfo,sizeof(uutInfo),pTI->Spec))
		{
			amprintf("(D)Check AddquteToMACAddress Fail and quit;\n");   //Show message for debug add by Wind 20080121
			return 0;
		}
	}
	else
	{
		strcpy_s(uutInfo,sizeof(uutInfo),pTI->Spec);
	}
	strcat_s(cmd,sizeof(cmd),uutInfo);
	
	char OutBuf[100];
	DWORD dwExitCode;
	amprintf("%s;\n",cmd);
	if (!RunExeFileNoOutput(cmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		return 0;
	}
	amprintf("Written UUT Information %s: %s;\n",pTI->DiagCmd,uutInfo);	
	strcpy_s(pTI->Result,sizeof(pTI->Result),uutInfo);
	return 1;
}
//add end by Talen 2011/10/28


//add start by Talen 2011/10/31
int CTestItemScript::CheckWifiRadioStatus(TEST_ITEM *pTI)
{
	char OutBuf[4096]="";
	char AnaTemp[1024]="";
	char Temp[100]="";
	char *pTok=NULL;
	DWORD dwExitCode=0;
	char front[16]="";
	char back[16]="";
	char library[40][16]={"acEn","country","dWDS","dwFl","iso_cc","legacywds","phymodes","rTSN","ra1C","raAu","raCA","raCh","raCi","raCl","raDe","raDt","raEA","raEV","raF2","raFl","raKT","raMA","raMd","raMu","raNm","raPo","raR2","raRe","raRo","raSk","raSt","raT2","raTm","raU2","raWC","raWM","sku","vaps","wdFl"};

	int keyIndex=0;
	int i=0;
	for(i=0;i<40;i++)
	{
		keyIndex=i;
		if(!strAllCmp(pTI->DiagCmd,library[i]))
		{
			break;
		}
	}

	if(keyIndex>=39)
	{
		amprintf("DIAG_CMD can not match one in program,Please check DIAG_CMD or program;\n");
		return 0;
	}

	if(keyIndex==0)
	{
		strcpy_s(front,sizeof(front),"<dict>");
		strcpy_s(back,sizeof(back),library[keyIndex+1]);
	}
	else if(keyIndex==38)
	{
		strcpy_s(front,sizeof(front),library[keyIndex-1]);
		strcpy_s(back,sizeof(back),"</dict>");
	}
	else
	{
		strcpy_s(front,sizeof(front),library[keyIndex-1]);
		strcpy_s(back,sizeof(back),library[keyIndex+1]);
	}


	if (!RunExeFileLocal(pTI->UUTCmd, OutBuf, sizeof(OutBuf), &dwExitCode))
	{
		amprintf("Test error,Can not open the ACP tool;\n");
		return 0;
	}
    pTok=strstr(OutBuf,"WiFi:");
	
	if(!strAllCmp("5G",pTI->Spec_1))
	{
		pTok=strstr(OutBuf,"</dict>");
	}
	else if(!strAllCmp("2G",pTI->Spec_1))
	{
	}
	else
	{
		amprintf("Please check SPEC_1 in config;\n");
		return 0;
	}

	if (pTok)
	{
		
		if (!GetsubStrInStr(AnaTemp, sizeof(AnaTemp),pTok,front,back))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		
		
		if (!ParseProperyResult(AnaTemp, Temp, sizeof(Temp), pTI->DiagCmd,pTI->UUTCmdAdd[1]))
		{
			amprintf("Test error,wrong parse;\n");
			return 0;
		}
		GetXmlIndexStr(Temp, 1, ',', pTI->Result,sizeof(pTI->Result));		
		amprintf("Detect the WiFi Radio Status %s:%s;\n", pTI->Spec_1,pTI->Result);
		amprintf("WiFi Radio Status %s in config file :%s;\n", pTI->Spec_1,pTI->Spec);

		if (atoi(pTI->Result)!=atoi(pTI->Spec))
		{
			return 0;
		}
	}
	else
	{
		amprintf("Test error,wrong parse;\n");
		return 0;
	}
	return 1;
}
//add end by Talen 2011/10/31


//add start by Talen 2012/01/13
int CTestItemScript::KillProcess(TEST_ITEM *pTI)
{
	KillTargetProcess(pTI->UUTCmd);
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2012/01/13



//add start by Talen 2012/01/18
int CTestItemScript::RunBatchFileAndWait(TEST_ITEM *pTI)
{
	if(!RunBatchFile(pTI->UUTCmd,pTI->DiagCmd))
	{
		amprintf("Failed to run the bat file: %s;\n",pTI->UUTCmd);
		return 0;
	}
	Sleep(1000*atoi(pTI->CmdPerfTime));
	return 1;
}
//add end by Talen 2012/01/18


//int CTestItemScript::PrintWipLabel(TEST_ITEM *pTI)
//{
	/*char cmd[500]="";
	char FileName[512]="";
	char OutBuf[5000]="";
	DWORD dwExitCode=0;

	long lastline=0;
	long timeout=0;
	int i = 0;

	HWND ToolWin=NULL;
	HWND WIP=NULL;
	ToolWin = ::FindWindow(NULL,"Print_wip");
	amprintf("ToolWin=%d;\n",ToolWin);
	SetWindowPos(ToolWin,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	::ShowWindow(ToolWin,SW_RESTORE);
	strcpy_s(cmd,sizeof(cmd),gTI.UUTInfo.UUTSN);
	WIP=::FindWindow(NULL,_T("Print_wip"));
	strcat_s(cmd,sizeof(cmd),"\r");
	while(cmd[i]!='\0')
	{
		SendMessageCallback(WIP,WM_CHAR,cmd[i],1,NULL,NULL);
		Sleep(50);
		i++;
	}*/
//	int cnt=0;
//	int j=0;
//	long id;
//	HWND bhd,bhds,bhde;
//	CWnd *pChildWnd,*pChildWnds,*pChildWnde;
//	HWND hWnd;
//	HINSTANCE OpRtn;
//	CString FileName,str;
//	hWnd=::FindWindow(NULL,_T("Bonjour")); 
//	j=0;
//	while(!hWnd&&j<120)
//	{
//		Sleep(1000);
//		j++;
//		hWnd=::FindWindow(NULL,_T("Bonjour")); 
//	}
//	if(!hWnd)
//	{
//		//MessageBox(_T("Not Found Window 'Bonjour'!"),_T("提示"),MB_OK|MB_ICONWARNING);
//		AfxMessageBox(_T("Not Found Window 'Bonjour'!"),0,MB_OK|MB_ICONWARNING);
//		//return 0;
//		goto EXIT_RETURN_ZERO;
//	}
//
//	pChildWnd=FindWindowEx(hWnd,NULL,NULL,_T("下一步(&N) >"));
//	pChildWnds=FindWindowEx(hWnd,NULL,NULL,_T("下一步(&N)>"));
//	pChildWnde=FindWindowEx(hWnd,NULL,NULL,_T("&Next >"));
//	j=0;
//	while(!(pChildWnd||pChildWnds||pChildWnde)&&j<10)
//	{
//		Sleep(1000);
//		j++;
//		pChildWnd=FindWindowEx(hWnd,NULL,NULL,_T("下一步(&N) >"));
//		pChildWnds=FindWindowEx(hWnd,NULL,NULL,_T("下一步(&N)>"));
//		pChildWnde=FindWindowEx(hWnd,NULL,NULL,_T("&Next >"));
//	}	
//	if(pChildWnd)
//	{    
//		bhd=pChildWnd->GetSafeHwnd(); 
//		id=::GetDlgCtrlID(bhd);
//		::SendMessage(hWnd,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhd); 
//	}
//	else if(pChildWnds)
//	{    
//		bhds=pChildWnds->GetSafeHwnd(); 
//		id=::GetDlgCtrlID(bhds);
//		::SendMessage(hWnd,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhds); 
//	}
//	else if(pChildWnde)
//	{    
//		bhde=pChildWnde->GetSafeHwnd(); 
//		id=::GetDlgCtrlID(bhde);
//		::SendMessage(hWnd,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhde); 
//	}
//	else
//	{
//		//MessageBox(_T("Not Found Button '下一步(&N) >'!"),_T("提示"),MB_OK|MB_ICONWARNING);
//		AfxMessageBox(_T("Not Found Button '下一步(&N) >'!"),0,MB_OK|MB_ICONWARNING);
//		return 0;
//	}
//	return 1;
//}
//add by Liu-Chen for k31 Wip Label 2012/04/11
int CTestItemScript::PrintWipLabel(TEST_ITEM *pTI)
{	
		int cnt=0;
		int j=0;
		long id;
		HWND bhd,bhds,bhde;    
		HWND hWnd,hWnds,hWnde;
		HINSTANCE OpRtn;
		hWnd=::FindWindow(NULL,_T("Java 安裝程式 – 歡迎使用")); 	
		hWnds=::FindWindow(NULL,_T("Java 安装程序 – 欢迎使用")); 
		hWnde=::FindWindow(NULL,_T("Java Setup - Welcome"));
		while(!(hWnd||hWnds||hWnde)&&j<100)
		{
			Sleep(1000);
			j++;
			hWnd=::FindWindow(NULL,_T("Java 安裝程式 – 歡迎使用")); 
			hWnds=::FindWindow(NULL,_T("Java 安装程序 – 欢迎使用")); 
			hWnde=::FindWindow(NULL,_T("Java Setup - Welcome"));
		}
		if(!(hWnd||hWnds||hWnde))
		{
			MessageBox(NULL,_T("Not Found Window 'Java 安裝程式'!"),_T("提示"),MB_OK|MB_ICONWARNING);		
			return 0;
		}
		//-------------------------------------------------------------------------------------------------------
		bhd=FindWindowEx(hWnd,NULL,NULL,_T("安裝(&I) >"));
		bhds=FindWindowEx(hWnds,NULL,NULL,_T("安装(&I) >"));
		bhde=FindWindowEx(hWnde,NULL,NULL,_T("&Install >"));
		j=0;
		while(!(bhd||bhds||bhde)&&j<10)
		{
			Sleep(1000);
			j++;
			bhd=FindWindowEx(hWnd,NULL,NULL,_T("安裝(&I) >"));
			bhds=FindWindowEx(hWnds,NULL,NULL,_T("安装(&I) >"));
			bhde=FindWindowEx(hWnde,NULL,NULL,_T("&Install >"));
		}	
		if(bhd)
		{    
			id=::GetDlgCtrlID(bhd);
			//::SendMessage(hWnd,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhd); 
			::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)(gTI.UUTInfo.UUTSN));
		}
		else if(bhds)
		{    
			id=::GetDlgCtrlID(bhds);
			::SendMessage(hWnds,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhds); 
		}
		else if(bhde)
		{    
			id=::GetDlgCtrlID(bhde);
			::SendMessage(hWnde,WM_COMMAND, MAKELONG(id,BN_CLICKED),(LPARAM)bhde); 
		}
		else
		{
			MessageBox(NULL,_T("Not Found Button '安裝(&I) >'!"),_T("提示"),MB_OK|MB_ICONWARNING);		
			return 0;
		}	
	return 1;
}
//add by Liu-Chen for k31 Wip Label 2012/04/11