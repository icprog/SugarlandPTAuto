#ifndef VERSION_AND_LOG_H
#define VERSION_AND_LOG_H


#include "AmbitTestSysDef.h"


#define PATH_BUF_SIZE 256
#define SROM_BUF_SIZE 4096
//#define WL_CMD_BUF_SIZE 512
//#define WL_OUTPUT_BUF_SIZE 2096

//--------------------------------------------------------------
//#define DEFAULT_REMOTE_DATA_IP			    "127.0.0.1"
//#define DEFAULT_LOCAL_DATA_IP			    "127.0.0.1"

//#define DEFAULT_STOP_ON_FAIL                TRUE


#define MAX_MESSAGE_LENGTH      1024

//#define PROCESS_OUTPUT_BUFFER   10240

void PrintPassFail(BOOL bFailed);
void PrintErrorMessage(FILE *hFile, char *pMsg, bool bPrint);
void PrintTimeStamp(FILE *hFile);
void PrintTheVersion();
BOOL OpenRomFile(char *pBuf, char *pFileName);
FILE *OpenLog(char *pLogFileName, char *pOpenPara);


#endif