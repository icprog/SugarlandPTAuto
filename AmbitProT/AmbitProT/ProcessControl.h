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

#define PATH_BUF_SIZE 256



class CProcessControl
{

public:
	CProcessControl(void);
    ~CProcessControl(void);

public:
	int StartProcessInLocal(CString CmdLine);

    int KillTargetProcess(CString ModuleName);


#ifdef _UNICODE









#else

int RunOneProcess(char *pCommandLine);
int RunExeFile(char *pCommandLine, char *pOutputBuf, int iOutputBufSize, DWORD *pdwExitCode);
int runShellExecute(char* pFile);
int runShellExecuteAndWaiting(char* pFile, char *pCmdPara);
BOOL RunBatchFile(char *pFileName, char *pCmdPara);
int RunExeFileLocal(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int KillTargetProcess(char *pTpName);
int CheckProcessExit(char *pTpName);
int IperfRunExeFileLocal(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode, int iTimeOut);
int RunExeFileNoOutput(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileForBonjour(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode, int PingTime);
int RunExeFileNoOutputForACP(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileNoWait(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
int RunExeFileNoOutputShow(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
//Creates a pipe and executes a command.
int ExecuteCMD(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode);
#endif





};
#endif