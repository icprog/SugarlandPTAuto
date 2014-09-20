#include "stdafx.h"


#include "ControlLogic.h"
#include "ControlLogicDefine.h"
#include "AGoldenSocket.h"

int AutoFixStatusFlag=0;//Added by Maxwell 1225    0 for status OK, 1 for fail
int AutoFixResponseFlag=0;//Added by Maxwell 1225    0 for Response OK, 1 for fail

char IPAddrTag[20];
UINT IPPortTag;
//Maxwell 090610

HANDLE hMsgSendMutex;
HANDLE hWriteLogMutex;
HANDLE hWriteLabelMutex;//MAXWLL 100504
HANDLE hWriteServerLogMutex;//Talen 2011/09/01
HANDLE hOperateComMutex;//Talen 2011/09/01
HANDLE hOneTestingMutex;//Talen 2011/09/01

GOLD_CLIENT_TYPE GoldenServer;//Maxwell 20110329
GOLD_CLIENT_TYPE GoldenServer1;//Talen 2011/06/27
extern TEST_INPUT_INFO gTI;//test input information //Maxwell 20110329

extern int fix_id;//Talen 2011/06/27

unsigned int _stdcall ParsePktProcess(LPVOID lpPara);

CControlLogic::CControlLogic(void)
{
}
CControlLogic::~CControlLogic(void)
{
}
int CControlLogic::Init()
{
	// start socket
	if (!InitSocket())
	{
		return 0;
	}
	else
	{
        //----------------------------------------------------------------------------------------------------------
		//initilization the client socket type informations
		sprintf_s(AmPROUI.IPAddr, sizeof(AmPROUI.IPAddr), "%s", IPAddrTag);
		AmPROUI.uPort = IPPortTag;
		sprintf_s(AmPROUI.AmSkcInfo, sizeof(AmPROUI.AmSkcInfo), "%s", "socket comm with UI");
		AmPROUI.hRevEvent = NULL;
		AmPROUI.pgRevBufType = NULL;
		AmPROUI.iRevBufTypeLen = 0;
		AmPROUI.dwRevThreadID = NULL;
		AmPROUI.iConnectSerFlag = 0;
		
		// now start load the socket and connect the server
		if (LoadWinsock(&AmPROUI))
		{
			return 0;
		}
	}

	if (!StartParseCammandTask())
	{
		return 0;
	}
	
	if (!ReadConfigFile())
	{
		return 0;
	}

	if (!TestTaskPrepare())
	{
		return 0;
	}

	//Maxwell 11228
	if (!ReadPNConfig())
	{
		return 0;
	}
	//Maxwell 11228

	if(1==fix_id)
	{
		//Maxwell 20110329
		if(!gTI.TestStaInfo.Golden_Connect_Para.connectFlag)
		{

		}
		else
		{
			if(StartGoldenServerConnect(&GoldenServer,gTI.TestStaInfo.Golden_Connect_Para.connectIP,gTI.TestStaInfo.Golden_Connect_Para.connectPORT,"connect with Server"))
			{
				printf("Golden Server connect pass!\r\n");
				//amprintf("Golden Server connect pass!\n");

			}
			else
			{
				amprintf("Golden Server connect fail, testperf exit!\n");
				return 0;
			}
		}
		//Maxwell 20110329
	}

	//add start by Talen 2011/06/27
	else if(2==fix_id)
	{
		if(!gTI.TestStaInfo.Golden_Connect1_Para.connectFlag)
		{

		}
		else
		{
			if(StartGoldenServerConnect(&GoldenServer1,gTI.TestStaInfo.Golden_Connect1_Para.connectIP,gTI.TestStaInfo.Golden_Connect1_Para.connectPORT,"connect with Server"))
			{
				printf("Golden Server2 connect pass!\r\n");
				//amprintf("Golden Server2 connect pass!\n");
			}
			else
			{
				amprintf("Golden Server2 connect fail, testperf exit!\n");
				return 0;
			}
		}
	}

	else
	{
		if(!gTI.TestStaInfo.Golden_Connect_Para.connectFlag)
		{

		}
		else
		{
			if(StartGoldenServerConnect(&GoldenServer,gTI.TestStaInfo.Golden_Connect_Para.connectIP,gTI.TestStaInfo.Golden_Connect_Para.connectPORT,"connect with Server"))
			{
				printf("Golden Server connect pass!\r\n");
				//amprintf("Golden Server connect pass!\n");
			}
			else
			{
				amprintf("Golden Server connect fail, testperf exit!\n");
				return 0;
			}
		}
	}
	//add end by Talen 2011/06/27

	Sleep(1000);
    InformationCross();
	return 1;
}
int CControlLogic::ReadConfigFile(void)
{
   CConfigFile ConfigFile;
   return (ConfigFile.PerformParse("ambitconfig.txt"));

}

int CControlLogic::ReadPNConfig(void)
{
   CConfigFile ConfigFile;
   return (ConfigFile.ConfigFilePnNameParse("PN_NAME.ini"));

}


int CControlLogic::StartParseCammandTask(void)
{
 
   	hParsePktThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		ParsePktProcess,
		this,
		0,
		&dwParsePktThreadID 
		);

    if (hParsePktThread)
	{
      // CloseHandle(hParsePktThread);

		AmPROUI.dwRevThreadID=(DWORD)dwParsePktThreadID;
	}
	else
	{
		return 0;
	}
	
	return 1;
}


unsigned int _stdcall ParsePktProcess(LPVOID lpPara)
{

	MSG msg;
	char cCmdBuf[512] = "";
	char RevBuf[3004];
	UINT DataLen=0;
	CControlLogic *pCL=(CControlLogic*)lpPara;
	//debug use 090520
	//pCL->ParseCommand("SFIS=SN[6F8821345]WIFIMAC[001122334455]ETHMAC[0011223344455]HDCKEY[1234567]RESULT[PASS];",strlen("SFIS=SN[6F8821345]WIFIMAC[001122334455]ETHMAC[0011223344455]HDCKEY[1234567]RESULT[PASS];"));

	while (1)
	{
		if (!WaitMessage()) 
		{
			return 1;
		}	
		memset(RevBuf,0,sizeof(RevBuf));
		while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
		{    
			DataLen=0;
			DataLen=(UINT)msg.lParam;

			if (DataLen>3004)
			{
			    if ((char *)msg.wParam)
			    {
				    free((char *)msg.wParam);
			    }
				continue;
			}
			else
			{	
				memcpy_s(RevBuf, sizeof(RevBuf),(char *)msg.wParam,DataLen);
				if ((char *)msg.wParam)
			    {
				    free((char *)msg.wParam);
			    }
			}
			pCL->ParseCommand(RevBuf,DataLen);
			//debug 090520
			}	  	
	}
	return 1;
}

int CControlLogic::GetDataFromGUI(char *pBuf)
{
    return 1;
}


int CControlLogic::TestTaskPrepare()
{
    return (TestInitial());
}


int CControlLogic::TestTaskClear()
{
	TestUninitial();
	return 1;
}

int CControlLogic::RunTest()
{

	return 1;
}

void CControlLogic::Clear(void)
{
	AmbitCloseClient();
	TestTaskClear();
}

int CControlLogic::InformationCross(void)
{

	SendInfRequrementToUI();

	SendIDInfoToUI();
	Sleep(100);
	SendLogTileToUI();
	return 0;
}

int CControlLogic::ParseCommand(char *pCMD,UINT DataLen)
{	
  return (AcceptCMD(pCMD,DataLen));
}

//Maxwell 20110329
int CControlLogic::StartGoldenServerConnect(GOLD_CLIENT_TYPE *Server,char *IP,char *PORT,char *connectInfo)
{
	printf("StartGoldenServerConnect;\r\n");//Talen 2011/07/28
	char buflog[256]="";
	if (!GoldenInitSocket())
	{
		return 0;
	}
	else
	{
        //----------------------------------------------------------------------------------------------------------
		//initilization the client socket type informations
		size_t ConvertedChars=0;
		//wcstombs_s(&ConvertedChars, buflog, sizeof(buflog), gTI.TestStaInfo.Golden_Connect_Para.connectIP,wcslen(gTI.TestStaInfo.Golden_Connect_Para.connectIP));
		sprintf_s(Server->IPAddr, sizeof(Server->IPAddr), "%s",IP);
		Server->uPort = atoi(PORT);
		sprintf_s(Server->AmSkcInfo, sizeof(Server->AmSkcInfo), "%s", connectInfo);
		Server->hRevEvent = NULL;
		Server->pgRevBufType = NULL;
		Server->iRevBufTypeLen = 0;
		Server->dwRevThreadID = NULL;
		Server->iConnectSerFlag = 0;
		
		// now start load the socket and connect the server
		if (GoldenLoadWinsock(Server))
		{

			return 0;
		}
	}
	printf("StartGoldenServerConnect pass;\r\n");//Talen 2011/07/28
	return 1;
}
//Maxwell 20110329