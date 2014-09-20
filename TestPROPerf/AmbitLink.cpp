/********************************************************************
created:	2007/02/06
filename: 	..\TestPROPerf\TestPROPerf\AmbitLink.cpp
file path:	..\TestPROPerf\TestPROPerf
file base:	TestPROPerf
file ext:	cpp
author:		Jeffrey

purpose:	verify the network link status definition
*********************************************************************/
#include "stdafx.h"
#include "AmbitLink.h"
#include "AmbitExeCtrl.h"

int VerifyLink(char *pDesIP, int PingTime)//
{

	char  CmdBuf[128] = "";
	char OutBuf[2048] = "";
	DWORD dwExitCode = 0;
	int iIndex1 = 0;
	int iIndex2 = 0;
	int SeachCunt = 0;
	char *pToke=NULL;
	if (!pDesIP)
	{
		return 0;
	}

	//if ((5 > PingTime)||(PingTime>100))
	//{
		//return 0;
	//}
	
	//modify by jeffrey 070321
	sprintf_s(CmdBuf, sizeof(CmdBuf), "%s -n 1",pDesIP);
	for (iIndex1 = 0; iIndex1 < PingTime; iIndex1++)
	{
        memset(OutBuf, 0, sizeof(OutBuf));
		if (-1 == RunExeFileLocal(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode))
	    {
		     return 0;
	    }
		
		if (pToke = strstr(OutBuf, "replied in RTT"))
		{
			SeachCunt++;
        }
   }

   amprintf("Ping time: %d  reply: %d\n",PingTime, SeachCunt);
   if (SeachCunt)
   {
	   return 1;
   }
   return 0;
//127.0.0.1, replied in RTT:0ms
//Request timeout.
}

int WaitBootUp(char *pDesIP, int PingTime)
{
	char  CmdBuf[128] = "";
	char OutBuf[2048] = "";
	DWORD dwExitCode = 0;
	int iIndex1 = 0;
	//int iIndex2 = 0;
	int ret;
	int SeachCunt = 0;
	char *pToke=NULL;
	if (!pDesIP)
	{
		return 0;
	}

	
	
	if ((5 > PingTime)||(PingTime>100))
	{
		return 0;
	}
	
	
	
	sprintf_s(CmdBuf, sizeof(CmdBuf), "BPing.exe -s %s -n 2", pDesIP, PingTime);
	for (iIndex1 = 0; iIndex1 < PingTime; iIndex1++)
	{
        memset(OutBuf, 0, sizeof(OutBuf));
		if (-1 == RunExeFileLocal(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode))
	    {
		     ret= 0;
	    }
		if (pToke = strstr(OutBuf, "replied in RTT"))
	    {
			return 1;
        }
		else
		{
			Sleep(1000);
			if(((PingTime-1) ==iIndex1)&&(!pToke))
			{
				return 0;
			}
		}
#ifdef Wind
		for (iIndex2 = 0; iIndex2 < 2; iIndex2++)
		{
		    if (pToke = strstr(OutBuf, "replied in RTT"))
		    {
				SeachCunt++;
				if (SeachCunt > 1)
				{
					return 1;
				}
				pToke += (strlen("replied in RTT"));
            }
		}
		Sleep(1000);
#endif
		
   }

   return 0;

}
int BonjourLink(char *pCmd, int TimeOut)
{

	char  CmdBuf[128] = "";
	char OutBuf[2048] = "";
	DWORD dwExitCode = 0;
	int iIndex1 = 0;
	int iIndex2 = 0;
	int SeachCunt = 0;
	char *pToke=NULL;
	strcpy_s(CmdBuf, sizeof(CmdBuf), pCmd);
	
	if (TimeOut < 3)
	{
		TimeOut = 3;//set default time out 3 seconds
	}
	
	amprintf("Bonjour CMD: %s\n, Timeout:%d\n", CmdBuf, TimeOut);

	if (-1 == RunExeFileForBonjour(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode,TimeOut))
    {
		 amprintf("Can not open dns-sd.exe;\n");
	     return 0;
    }
	amprintf("%s;\n",OutBuf);
	pToke = strstr(OutBuf, "Domain");
	if (!pToke)
    {				
		//if (VerifyLink("BPing.exe -s 10.0.1.1", 3))
		//{
	 //      return 1;
		//}
		return 0;		
    }
	return 1;	
}

//add start by Talen 2011/04/19
int VerifyLinkUsePing(char *pDesIP, int PingTime)//
{

	char  CmdBuf[128] = "";
	char OutBuf[2048] = "";
	DWORD dwExitCode = 0;
	int iIndex1 = 0;
	int SeachCunt = 0;
	char *pToke=NULL;
	if (!pDesIP)
	{
		return 0;
	}

	//if ((5 > PingTime)||(PingTime>100))
	//{
		//return 0;
	//}
	
	//modify by jeffrey 070321
	sprintf_s(CmdBuf, sizeof(CmdBuf), "%s -n 1",pDesIP);
	for (iIndex1 = 0; iIndex1 < PingTime; iIndex1++)
	{
        memset(OutBuf, 0, sizeof(OutBuf));
		if (-1 == RunExeFileLocalForSysTool(CmdBuf, OutBuf, sizeof(OutBuf), &dwExitCode))
	    {
		     return 0;
	    }
		
		if (pToke = strstr(OutBuf, "TTL"))
		{
			//amprintf("%d",iIndex1);
			SeachCunt++;
			if(SeachCunt>=5)
			{
				return 1;
			}
        }
		//add start by Talen 2011/08/09
		else if(strstr(OutBuf, "Request timed out"))
		{
			SeachCunt=0;
		}
		//add end by Talen 2011/08/09
		else
		{
			SeachCunt=0;
			Sleep(4000);//Talen 2011/08/09
		}
   }

   return 0;
}
//add end by Talen 2011/04/19