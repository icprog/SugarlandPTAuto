#include "StdAfx.h"
#include "ProcessControl.h"
#include <process.h> 
#include "tlhelp32.h"
#include "shellapi.h"
//////////////////////////////////////////////////////////////////////////
// para about test informations

#ifdef _UNICODE


CProcessControl::CProcessControl(void)
{




}
CProcessControl::~CProcessControl(void)
{




}

int CProcessControl::StartProcessInLocal(CString CmdLine)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	wchar_t LocalPathBuf[2048];

	CString strTemp;
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;

	
    //if(pOutputBuf)
    //{
        //SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		///
        //::CreatePipe(&hRead, &hWrite, &sa, 0);
        //si.hStdOutput = hWrite;
        //si.hStdError = hWrite;
    //}
	
	
	GetCurrentDirectory(sizeof(LocalPathBuf)/2, LocalPathBuf);
	strTemp+=LocalPathBuf;
	strTemp+=_T("\\");
	strTemp+=CmdLine;
	wcscpy_s(LocalPathBuf, sizeof(LocalPathBuf)/2, strTemp);

    ZeroMemory( &pi, sizeof(pi) );
	Sleep(20);//Maxwell 101011
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				   // Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        goto _Exit;
	}
	
		
	// Wait until child process exits.
   // WaitForSingleObject( pi.hProcess, INFINITE);
	//Sleep(100);
    //GetExitCodeProcess( pi.hProcess, pdwExitCode);	
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
    if(pi.hThread)
        CloseHandle( pi.hThread );
    if(hRead)
        CloseHandle(hRead);
    if(hWrite)
        CloseHandle(hWrite);
	
	return iRet;
}

int CProcessControl::KillTargetProcess(CString ModuleName)
{
	SHFILEINFO shSmall;
	PROCESSENTRY32 ProcessInfo;
	int m_nProcess=0;
	HANDLE hKillProcess;


	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{
		return 0;
	}

	ProcessInfo.dwSize=sizeof(ProcessInfo);
	BOOL Status=Process32First(SnapShot,&ProcessInfo);
#ifdef _DEBUG
	//printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
	while(Status)
	{
		m_nProcess++;
		ZeroMemory(&shSmall,sizeof(shSmall));
		SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall,
			sizeof(shSmall),SHGFI_ICON|SHGFI_SMALLICON);
#ifdef _DEBUG
		//	 printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
		//pTok = strstr(ProcessInfo.szExeFile, pTpName);
		//ModuleName.Compare(ProcessInfo.szExeFile);
		if (!ModuleName.Compare(ProcessInfo.szExeFile))		
		{
			hKillProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE,ProcessInfo.th32ProcessID);
			if(::TerminateProcess(hKillProcess,1))
			{
#ifdef _DEBUG
				//printf("kill %s process\n", ProcessInfo.szExeFile);
#endif   
				return 1;
			}
		}
		Status=Process32Next(SnapShot,&ProcessInfo);
	}
	return 0;
}


#else


int GetProcessNameFromCmdLine(char *pInput, char *pOutput, int sizeRtn)
{
	return (strSTEDcpy(pOutput, sizeRtn, pInput,0xFF,' '));
}


int runShellExecute(char* pFile)
{
    HINSTANCE OpRtn;
    //HANDLE hProcess;
    //DWORD ExitCode;
    char* pOption = NULL;
    
    OpRtn = ShellExecute(NULL,
	             TEXT("open"),
			      TEXT(pFile),
			      pOption, // option here SW_HIDE SW_SHOWNORMAL
			      NULL,
			      SW_SHOWNORMAL ); //SW_SHOWNORMAL );

    if (OpRtn <= (void *)32)
    {
        return -1;
    }
    else
    {
	    return 0;
    }
}
//////////////////////////////////////////////////////////////////////////
//
int runShellExecuteAndWaiting(char* pFile, char *pCmdPara)
{
	
    SHELLEXECUTEINFO aShellExecStruct;
    
	if (NULL == pFile) 
	{
		return 0;
	}
	if (NULL == pCmdPara) 
	{
		return 0;
	}
	
	aShellExecStruct.cbSize = sizeof(SHELLEXECUTEINFO);
    aShellExecStruct.fMask = SEE_MASK_NOCLOSEPROCESS;
    aShellExecStruct.hwnd = ::GetDesktopWindow();
    aShellExecStruct.lpVerb = "open";
    aShellExecStruct.lpFile = pFile;
    aShellExecStruct.lpParameters = pCmdPara; //" www.google.com";//lpszPathName;
    aShellExecStruct.lpDirectory = "";//"c:\\temp";
    aShellExecStruct.nShow = SW_HIDE; //1:SW_SHOWNORMAL;//2:SW_MINIMIZE; //3:SW_HIDE;
    
     
    BOOL aResult = ::ShellExecuteEx(&aShellExecStruct);   
    if (aResult)
    {
         HANDLE aProcHandle = aShellExecStruct.hProcess;
         if (aProcHandle)
         {
              ::WaitForSingleObject(aProcHandle, INFINITE);
			  ::CloseHandle(aProcHandle);
         }
		 return 0;
    }
    else
    {
		return -1;
    }
}
//////////////////////////////////////////////////////////////////////////
//
BOOL RunBatchFile(char *pFileName, char *pCmdPara)
{
	
	char PathBuf[PATH_BUF_SIZE];
	
	if (NULL == pFileName) 
	{
		return FALSE;
	}
	if (NULL == pCmdPara) 
	{
		return FALSE;
	}
	
	GetCurrentDirectory(PATH_BUF_SIZE, PathBuf);
	strncat_s(PathBuf, sizeof(PathBuf),"\\", strlen("\\"));
    strncat_s(PathBuf, sizeof(PathBuf), pFileName, strlen(pFileName));
	runShellExecuteAndWaiting(PathBuf, pCmdPara);
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//
int RunExeFile(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	
    iRet = 0;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        pCommandLine,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = -1;
        goto _Exit;
	}
	
	// Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
        ::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
        pOutputBuf[dwBytesRead] = 0;
    }
	
	
_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
    if(pi.hThread)
        CloseHandle( pi.hThread );
    if(hRead)
        CloseHandle(hRead);
    if(hWrite)
        CloseHandle(hWrite);
	
	return iRet;
}
//////////////////////////////////////////////////////////////////////////
//run the exe in local path
int RunExeFileLocal(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));


    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 10000);//SET pipe buffer size to 10K for recive the data from ACP tool
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,20000))
	{		
		KillTargetProcess(ProcessName);		
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		pOutputBuf[dwBytesRead] = 0;
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
    if(pi.hThread)
        CloseHandle( pi.hThread );
    if(hRead)
        CloseHandle(hRead);
    if(hWrite)
        CloseHandle(hWrite);
	
	return iRet;
}

int RunOneProcess(char *pCommandLine)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	si.wShowWindow =SW_SHOWNORMAL;
	
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        
	}	
	
  
	return iRet;
}
//Creates a pipe and executes a command.
int ExecuteCMD(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{

	char LocalPathBuf[1024];
	char cmdTempBuf[1024];


	memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));


	char   psBuffer[128];
	FILE   *pPipe;


	/* Run DIR so that it writes its output to a pipe. Open this
	* pipe with read text attribute so that we can read it 
	* like a text file. */

	if( (pPipe = _popen(LocalPathBuf, "rt")) == NULL )
	{
		exit(1 );
	}
	/* Read pipe until end of file, or an error occurs. */
	while(fgets(pOutputBuf, iOutputBufSize, pPipe))
	{
		printf(pOutputBuf);
	}
	/* Close pipe and print return value of pPipe. */
	if (feof( pPipe))
	{
		printf( "\nProcess returned %d\n", _pclose( pPipe ) );
	}
	else
	{
		printf( "Error: Failed to read the pipe to the end.\n");
	}

	return 1;
}
int RunExeFileNoOutput(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        
	}	
		
	// Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE);//Modified by Maxwell 2008/12/29 for not wait INFINITE
	//WaitForSingleObject( pi.hProcess, 20000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);	
   
	return iRet;
}


int RunExeFileNoOutputShow(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        
	}	
		
	// Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);	
   
	return iRet;
}


int RunExeFileForBonjour(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode,int PingTime)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	

	char ProcessName[300]="";
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));


    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.
	if(WAIT_TIMEOUT == WaitForSingleObject( pi.hProcess, PingTime*1000))
	{
		KillTargetProcess(ProcessName);			
		return 0;
	}
	
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
        ::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
        pOutputBuf[dwBytesRead] = 0;
    }
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
    if(pi.hThread)
        CloseHandle( pi.hThread );
    if(hRead)
        CloseHandle(hRead);
    if(hWrite)
        CloseHandle(hWrite);
	
	return iRet;
}



int KillTargetProcess(char *pTpName)
{
	SHFILEINFO shSmall;
	PROCESSENTRY32 ProcessInfo;
	int m_nProcess=0;
    HANDLE hKillProcess;
			
	char *pTok = 0;

	if (!pTpName)
	{
		return 0;
	}

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{
		return 0;
	}

	ProcessInfo.dwSize=sizeof(ProcessInfo);
   	BOOL Status=Process32First(SnapShot,&ProcessInfo);
#ifdef _DEBUG
	//printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
	while(Status)
	{
		 m_nProcess++;
		 ZeroMemory(&shSmall,sizeof(shSmall));
		 SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall,
					sizeof(shSmall),SHGFI_ICON|SHGFI_SMALLICON);
#ifdef _DEBUG
	//	 printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
	     pTok = strstr(ProcessInfo.szExeFile, pTpName);
		 if (pTok)		
		 {
			    hKillProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE,ProcessInfo.th32ProcessID);
			    if(::TerminateProcess(hKillProcess,1))
			    {
#ifdef _DEBUG
		         printf("kill %s process\n", ProcessInfo.szExeFile);
#endif   
					return 1;
			    }
		 }
		 Status=Process32Next(SnapShot,&ProcessInfo);
	}
    return 0;
}
//added by Wyatt
int CheckProcessExit(char *pTpName)
{
	SHFILEINFO shSmall;
	PROCESSENTRY32 ProcessInfo;
	int m_nProcess=0;
    HANDLE hKillProcess;
			
	char *pTok = 0;

	if (!pTpName)
	{
		return 0;
	}

	HANDLE SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot==NULL)
	{
		return 0;
	}

	ProcessInfo.dwSize=sizeof(ProcessInfo);
   	BOOL Status=Process32First(SnapShot,&ProcessInfo);
#ifdef _DEBUG
	//printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
	while(Status)
	{
		 m_nProcess++;
		 ZeroMemory(&shSmall,sizeof(shSmall));
		 SHGetFileInfo(ProcessInfo.szExeFile,0,&shSmall,
					sizeof(shSmall),SHGFI_ICON|SHGFI_SMALLICON);
#ifdef _DEBUG
	//	 printf("process name:%s\n", ProcessInfo.szExeFile);
#endif
	     pTok = strstr(ProcessInfo.szExeFile, pTpName);
		 if (pTok)		
		 {
			   /* hKillProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE,ProcessInfo.th32ProcessID);
			    if(::TerminateProcess(hKillProcess,1))
			    {
#ifdef _DEBUG
		         printf("kill %s process\n", ProcessInfo.szExeFile);
#endif   
					return 1;
			    }*/
			 return 1;
		 }
		 Status=Process32Next(SnapShot,&ProcessInfo);
	}
    return 0;
}


int RunExeFileNoOutputForACP(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
//    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;        
	}
	int p=GetLastError();
	return iRet;
}

int RunExeFileNoWait(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
//    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	//si.wShowWindow = SW_HIDE;
	si.wShowWindow =SW_SHOWNORMAL;
	
    if(pOutputBuf)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
        ::CreatePipe(&hRead, &hWrite, &sa, 0);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.
   // WaitForSingleObject( pi.hProcess, INFINITE);
	Sleep(100);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);	
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
    if(pi.hThread)
        CloseHandle( pi.hThread );
    if(hRead)
        CloseHandle(hRead);
    if(hWrite)
        CloseHandle(hWrite);
	
	return iRet;
}





#endif






