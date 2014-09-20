#include "stdafx.h"
#include "AmbitExeCtrl.h"
//#include <time.h>
//#include <windows.h>
//#include <process.h>    /* _beginthread, _endthread */
//#include "conio.h"
#include <process.h> 
#include "tlhelp32.h"
#include "ScriptSupport.h"
//////////////////////////////////////////////////////////////////////////
// para about test informations
//int iPwrInitialFlag = 0;

//char    *g_pDataIP = DEFAULT_REMOTE_DATA_IP;
//char    *g_pHostIP = DEFAULT_LOCAL_DATA_IP;
//char    *g_pLogFile = NULL;
//char	  g_pCurFullFile[128]="mytest.txt"; 
//BOOL    g_bStopOnFail = DEFAULT_STOP_ON_FAIL;
//BOOL    g_bFailed = false;
//char    g_pCurLogFile[32] = "currentlog.txt";

//////////////////////////////////////////////////////////////////////////
//
extern CLIENT_TYPE ImageProSoc;


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
	if(!PathFileExists(PathBuf))
	{
		amprintf("No file %s exist!\n",PathBuf);
		return FALSE;
	}
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
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;/\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 20000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

    
    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
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
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,150000))//20000->150000 modity by blucey 2010/01/28 for Apple 60sec each channel.
	{		
		KillTargetProcess(ProcessName);
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	
	return iRet;
}


//////////////////////////////////////////////////////////////////////////
//run the exe in local path with big pipe
int RunExeFileLocalBigPipe(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 40000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

    
    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
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
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,20000))
	{		
		KillTargetProcess(ProcessName);		
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	
	return iRet;
}

int HDDACPPerform(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
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

		::CreatePipe(&hRead, &hWrite, &sa, 10000);
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
		printf("======================%d\n", dwBytesRead);
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


	//char   psBuffer[128];
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

	char ProcessName[300]="";
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	
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
		goto _ExitNoOutput;
        
	}	
		
	// Wait until child process exits.
  //  WaitForSingleObject( pi.hProcess, INFINITE);//Modified by Maxwell 2008/12/29 for not wait INFINITE
	//WaitForSingleObject( pi.hProcess, 20000);
	//Maxwell 090629
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,20000))
	{		
		KillTargetProcess(ProcessName);	
		iRet = 0;
		goto _ExitNoOutput;
	}
	//Maxwell 090629
    GetExitCodeProcess( pi.hProcess, pdwExitCode);	

_ExitNoOutput:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
   
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
		//Maxwell 090629
		iRet=0;
		goto _Exit;
		//return 0;
		//Maxwell 090629
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


//add start by Talen 2011/04/03
int RunExeFileLocalForTP(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 20000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

	//KillTargetProcess(ProcessName); //add by Talen 2011/04/02

    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
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
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,20000))//change to 20000 by Talen 2011/03/31
	{		
		//KillTargetProcess("iperf.exe");
		//KillTargetProcess("iperf.exe");
		//KillTargetProcess(ProcessName);
		KillTargetProcess(ProcessName);		
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	
	return iRet;
}
//add end by Talen 2011/04/03


//add start by Talen 2011/04/03
int RunExeFileLocalForTPPre(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 20000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

	//KillTargetProcess(ProcessName); //add by Talen 2011/04/02

    
    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
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
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,20000))//change to 20000 by Talen 2011/03/31
	{		
		//KillTargetProcess("iperf.exe");
		//KillTargetProcess("iperf.exe");
		//KillTargetProcess(ProcessName);
		KillTargetProcess(ProcessName);		
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	
	return iRet;
}
//add end by Talen 2011/04/03

//add start by Talen 2011/04/21
int RunExeFileLocalForSysTool(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 20000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

    
    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
	if( !CreateProcess( NULL,		// No module name (use command line). 
		pCommandLine,
        //LocalPathBuf,				// Command line. 
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
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,150000))//20000->150000 modity by blucey 2010/01/28 for Apple 60sec each channel.
	{		
		KillTargetProcess(ProcessName);		
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	

_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	
	return iRet;
}
//add end by Talen 2011/04/21

//add start by Talen 2011/04/23
int RunExeFileLocalForDownArt(char *pCommandLine, char *pOutputBuf, int iOutputBufSize,  DWORD *pdwExitCode)
{
	STARTUPINFO si;
	//amprintf("STARTUPINFO si;\n");//Debug use
    PROCESS_INFORMATION pi;
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
	int iRet;
    DWORD   dwBytesRead;
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[1024];
	char cmdTempBuf[1024];
	int errorflag=0;
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
	//amprintf("PROCESS_INFORMATION pi;\n");//Debug use
    si.cb = sizeof(si);
	//amprintf("si.cb = sizeof(si);\n");//Debug use
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//amprintf("si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;/\n");//Debug use
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = SW_HIDE;
	//si.wShowWindow =SW_SHOWNORMAL;
	

	char ProcessName[300]="";
	//amprintf("GetProcessNameFromCmdLine;\n");//Debug use
	GetProcessNameFromCmdLine(pCommandLine, ProcessName, sizeof(ProcessName));
	//amprintf("pCommandLine: %s;\n",pCommandLine);//Debug use
	//amprintf("ProcessName: %s;\n",ProcessName);//Debug use


    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE }; // inheritable handle
		
		//amprintf("CreatePipe start!\n");//Debug use
        //::CreatePipe(&hRead, &hWrite, &sa, 20000);//Maxwell 090917
		 ::CreatePipe(&hRead, &hWrite, &sa, 20000);//SET pipe buffer size to 10K for recive the data from ACP tool
		//amprintf("CreatePipe success!\n");//Debug use
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
    }
	
    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	strcpy_s(cmdTempBuf, pCommandLine);
	//amprintf("cmdTempBuf %s!\n",cmdTempBuf);//Debug use
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));
	//amprintf("LocalPathBuf %s!\n",LocalPathBuf);//Debug use

    
    ZeroMemory( &pi, sizeof(pi) );
	//amprintf("ZeroMemory !\n");//Debug use
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        CREATE_NEW_CONSOLE,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		) 
		
	{
		iRet = 0;
		//amprintf("CreateProcess fail!\n");//Debug use
        goto _Exit;
	}
	
	
		
	// Wait until child process exits.

	//if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,INFINITE))//Modified by Maxwell 2008/12/24 for not to wait if fail
	//amprintf("Start to wait!\n");//Debug use
	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,50000))//20000->150000 modity by blucey 2010/01/28 for Apple 60sec each channel.
	{		
		KillTargetProcess(ProcessName);		
		//amprintf("KillTargetProcess success!\n");//Debug use
		iRet=0;
		//amprintf("goto _Exit start!\n");//Debug use
		goto _Exit;
		//amprintf("Start to wait!\n");//Debug use
	}
	//WaitForSingleObject( pi.hProcess, 5000);
	//amprintf("wait process sucess;\n");
    GetExitCodeProcess( pi.hProcess, pdwExitCode);
	//amprintf("GetExitCodeProcess;\n");
	
    if(pOutputBuf)
    {
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		//amprintf("ReadFile start!\n");//Debug use
		::ReadFile(hRead, pOutputBuf, iOutputBufSize - 1, &dwBytesRead, NULL);
		//amprintf("ReadFile pass!\n");//Debug use
		//amprintf("pOutputBuf: %s;\n",pOutputBuf);//Debug use
		pOutputBuf[dwBytesRead] = 0;
		//amprintf("pOutputBuf clear pass!\n");//Debug use
	}
	
	//amprintf("before _Exit;\n");
_Exit:
    if(pi.hProcess)
        CloseHandle( pi.hProcess );
		//amprintf("CloseHandle: hProcess;\n");//Debug use
    if(pi.hThread)
        CloseHandle( pi.hThread );
		//amprintf("CloseHandle: hThread;\n");//Debug use
    if(hRead)
        CloseHandle(hRead);
		//amprintf("CloseHandle: hRead;\n");//Debug use
    if(hWrite)
        CloseHandle(hWrite);
		//amprintf("CloseHandle: hWrite;\n");//Debug use
	//amprintf("before return from RunExeFileLocalForDownArt;\n");
	return iRet;
}
//add end by Talen 2011/04/23

int RunOneProcessMinisize(char *pCommandLine)
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
	si.wShowWindow =SW_SHOWMINNOACTIVE;
	
	
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
//add by Liu-Chen for k31 Wip Label 2012/04/11