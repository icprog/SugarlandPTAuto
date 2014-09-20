#ifndef CONTROL_LOGIC_H
#define CONTROL_LOGIC_H



#include "AClientSockets.h"
#include "AmbitExeCtrl.h"
#include "VersionAndLog.h"
#include "AAPTest.h"
#include "TResultReport.h"
#include "TestTask.h"
#include "ConfigFile.h"
#include "AGoldenSocket.h"//Talen 2011/07/27

#define ErrorCode
#ifdef ErrorCode
#include "ErrorCode.h"
#endif
#define CMD_DELI "CMD_"

//char IPAddrTag[20];
//UINT IPPortTag;

class CControlLogic
{
public:
	CControlLogic();
public:
	~CControlLogic();
public:
    int Init();
public:
	int ReadConfigFile(void);
	int ReadPNConfig(void);
	int StartParseCammandTask(void);
    int GetDataFromGUI(char *pBuf);
public:
	// thread add by jeffrey 070226
	unsigned dwParsePktThreadID;
	HANDLE hParsePktThread;
public:
	void Clear(void);
private:
	int TestTaskPrepare();
    int TestTaskClear();
public:
    int RunTest();
public:
	int InformationCross(void);
public:
	int ParseCommand(char * pCMD,UINT DataLen);
	int StartGoldenServerConnect(GOLD_CLIENT_TYPE *Server,char *IP,char *PORT,char *connectInfo);
};


#endif