/********************************************************************
created:	2006/11/27
filename: 	..\ambitclient\AmbitExeCtrl.h
file path:	..\ambitclient
file base:	aclientsocket
file ext:	h
author:		Jeffrey

purpose:	control the exe file
*********************************************************************/
#ifndef AMBIT_EXE_CTRL_H
#define AMBIT_EXE_CTRL_H

#include "shellapi.h"


#define PATH_BUF_SIZE 256
//#define SROM_BUF_SIZE 4096
//#define WL_CMD_BUF_SIZE 512
//#define WL_OUTPUT_BUF_SIZE 2096

//--------------------------------------------------------------
//#define DEFAULT_REMOTE_DATA_IP			    "127.0.0.1"
//#define DEFAULT_LOCAL_DATA_IP			    "127.0.0.1"
//#define DEFAULT_2GHZ_CABLE_LOSS             10  //db
//#define DEFAULT_5GHZ_CABLE_LOSS             10  //db
//#define DEFAULT_STOP_ON_FAIL                TRUE


//#define MAX_MESSAGE_LENGTH      1024

//#define PROCESS_OUTPUT_BUFFER   10240

int RunOneProcessMinisize(char *pCommandLine);//add by Liu-Chen for k31 Wip Label 2012/04/11
int RunOneProcess(char *pCommandLine);
int RunExeFile(char *pCommandLine, char *pOutputBuf, int iOutputBufSize, DWORD *pdwExitCode);
int runShellExecute(char* pFile);
int runShellExecuteAndWaiting(char* pFile, char *pCmdPara);
BOOL RunBatchFile(char *pFileName, char *pCmdPara);
int RunExeFileLocal(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileLocalBigPipe(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int KillTargetProcess(char *pTpName);
int IperfRunExeFileLocal(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode, int iTimeOut);
int RunExeFileNoOutput(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileForBonjour(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode, int PingTime);
int RunExeFileNoOutputForACP(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileNoWait(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileNoOutputShow(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
//Creates a pipe and executes a command.
int ExecuteCMD(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int HDDACPPerform(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileLocalForTP(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);//add by Talen
int RunExeFileLocalForTPPre(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);//add by Talen
int RunExeFileLocalForSysTool(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);//add by Talen
int RunExeFileLocalForDownArt(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);//add by Talen
#endif