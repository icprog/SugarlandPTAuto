#include "StdAfx.h"
#include "AmLogRecord.h"
#include "shlwapi.h"


//HANDLE hLogRecordMutex;

CAmLogRecord::CAmLogRecord(void)
{
	//hLogRecordMutex = CreateMutex(NULL,FALSE,_T("LogRecordTXT"));
}

CAmLogRecord::~CAmLogRecord(void)
{
	//if (hLogRecordMutex)
	//{
	//	CloseHandle(hLogRecordMutex);
	//}
}

long CAmLogRecord::amprintf(const char * format,...)
{

    va_list argList;
    long     retval = 0;
	char    buffer[2048];

	char dataSend[2048];
	memset(buffer , 0, sizeof(buffer));
	memset(dataSend, 0, sizeof(dataSend));

    va_start(argList, format);
    retval = vprintf(format, argList);
    fflush(stdout);

	Sleep(1);
	vsprintf_s(dataSend, sizeof(dataSend), format, argList);
	/////////////////////////////////////////////////////////
	FILE *fp;	
	int iFileOpenErr = 0;
	wchar_t PathBuf[256];
	wchar_t FileName[512];
	if(!GetCurrentDirectory(sizeof(PathBuf)/2, PathBuf))
	{
		return FALSE;
	}
	else
	{
		swprintf_s(FileName, sizeof(FileName)/2, _T("%s\\LogFile\\"), PathBuf);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				return 0;
			}
		}
		CTime t=CTime::GetCurrentTime();
		CString strTime = t.Format( "%Y%m%d" );
		wchar_t wchTempBuf[256]=_T("");
		wcscpy_s(wchTempBuf,sizeof(wchTempBuf)/2,strTime);
		wcscat_s(FileName, sizeof(FileName)/2,wchTempBuf);
		wcscat_s(FileName, sizeof(FileName)/2, _T("-Log.txt"));
		//WaitForSingleObject(hLogRecordMutex, INFINITE); 
		Sleep(1);
		iFileOpenErr = _wfopen_s(&fp, FileName, _T("a+"));
		if(NULL == fp)
		{
			return FALSE;
		}
		size_t cLen;
		wchar_t wData[2048];
		mbstowcs_s(&cLen,wData,sizeof(wData)/2,dataSend,strlen(dataSend));
		fwprintf_s(fp,_T("%s\n"),wData);
		fclose(fp);
		//ReleaseMutex(hLogRecordMutex);
	}
	/////////////////////////////////////////////////////
	return 1;
}