// TestPROPerf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VersionAndLog.h"
#include "ControlLogic.h"

#include "TestItemScript.h"
#include "AmbitTestSysDef.h"

#include "CommSocket.h"

extern char IPAddrTag[20];
extern UINT IPPortTag;
extern int fix_id;
extern TEST_INPUT_INFO gTI;//test input information //Maxwell 20110329
//char IPAddrTag[20];
//UINT IPPortTag;

CControlLogic CLogic;
int gEntireExitFlag=1;

CCommSocketClient DeviceAT100Client;

//Maxwell 090610
extern HANDLE hMsgSendMutex;
//Maxwell 090622
extern HANDLE hWriteLogMutex;
//Maxwell 100504
extern HANDLE hWriteLabelMutex;
extern HANDLE hWriteServerLogMutex;//Talen 2011/09/01
extern HANDLE hOperateComMutex;//Talen 2011/09/01
extern HANDLE hOneTestingMutex;//Talen 2011/09/01

int TestperfInitial()
{
	return (CLogic.Init());
}
void TestPerfClear()
{
	CLogic.Clear();
//Maxwell 090610
	if (hMsgSendMutex)
	{
		CloseHandle(hMsgSendMutex);
	}
	//Maxwell 090622
	if (hWriteLogMutex)
	{
		CloseHandle(hWriteLogMutex);
	}
		//Maxwell 100504
	if (hWriteLabelMutex)
	{
		CloseHandle(hWriteLabelMutex);
	}
	if(hWriteServerLogMutex)//Talen 2011/09/01
	{
		CloseHandle(hWriteServerLogMutex);
	}
	if(hOperateComMutex)//Talen 2011/09/01
	{
		CloseHandle(hOperateComMutex);
	}
	if(hOneTestingMutex)//Talen 2011/09/01
	{
		CloseHandle(hOneTestingMutex);
	}

}

//define global variable for config file 
int _tmain(int argc, _TCHAR* argv[])
{	
	hMsgSendMutex = CreateMutex(NULL,FALSE, NULL);
	//Maxwell 090622
	hWriteLogMutex = CreateMutex(NULL,FALSE,"WriteLogTXT");
	hWriteLabelMutex = CreateMutex(NULL,FALSE,"WriteLabelTXT");//Maxwell 100504
	hWriteServerLogMutex = CreateMutex(NULL,FALSE,"WriteServerLogTXT");//Talen 2011/09/01
	hOperateComMutex = CreateMutex(NULL,FALSE,"OperateComTXT");//Talen 2011/09/01
	hOneTestingMutex = CreateMutex(NULL,FALSE,"OneTestingTXT");//Talen 2011/09/01

	//Initialization

	//Maxwell 090525 

	if ( (argc == 5)&&(!strcmp(argv[1],"-i"))&&(!strcmp(argv[3],"-p")))
	{
		strcpy_s(IPAddrTag,sizeof(IPAddrTag),argv[2]);
		IPPortTag = atoi(argv[4]);
	}
	else
	{
		if((argc == 2)&&(!strcmp(argv[1],"-v") || !strcmp(argv[1],"-V")))//Talen 2012/03/30
		{
			printf("Version : %s",SOFTWARE_VERSION);
			return 0;
		}
		else
		{
			printf("Args format wrong!\n");
			return 0;
		}
	}

	fix_id=IPPortTag-10000+1;

	gTI.TestStaInfo.fixID=fix_id;
	//strcpy_s(IPAddrTag,sizeof(IPAddrTag),"127.0.0.1");
	//IPPortTag = 10000;

	PrintTheVersion();	

	if(!TestperfInitial())
	{
		printf("ERROR Can not connect server.exit\n");
        goto LABLE_TEST_EXIT;
	}

	if(!gTI.TestStaInfo.Control_Board_Para.ConnectFlag)
	{

	}
	else
	{
		int ret=DeviceAT100Client.CommConnect(2,"192.168.1.200",30000);
		if(!ret)
		{
			printf("ERROR Can not connect at100 server.exit\n");
			goto LABLE_TEST_EXIT;
		}
	}

	while (gEntireExitFlag)
	{
		Sleep(500);		
	}
    
LABLE_TEST_EXIT:	
	Sleep(500);
	//--------------------------------------------------------------
	//clear and terminate
	DeviceAT100Client.CommDisconnect();
	TestPerfClear();
	printf("TestPerf Exit.\n");
	//--------------------------------------------------------------
	return 0;
}

