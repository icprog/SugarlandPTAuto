#include "StdAfx.h"
#include "TResultReport.h"
#include "shlwapi.h"
#include "atlstr.h"

//Maxwell 090622
extern HANDLE hWriteLogMutex;

CTResultReport::CTResultReport(void)
{
	
    ItemID=0;
    memset(UUTModelName, 0, sizeof(UUTModelName));
	memset(StationName, 0,sizeof(StationName));

}

CTResultReport::~CTResultReport(void)
{
}


long CTResultReport::InputResult(char *pItemName,const char * format,...)
{

    va_list argList;
    long     retval = 0;
    char    buffer[4096];
	memset(buffer , 0, sizeof(buffer));

    /*if have logging turned on then can also write to a file if needed */

    /* get the arguement list */
    va_start(argList, format);

    /* using vprintf to perform the printing it is the same is printf, only
     * it takes a va_list or arguments
     */
    //retval = vprintf(format, argList);

	strcpy_s(TRTemp.ItemName, sizeof(TRTemp.ItemName), pItemName);
	TRTemp.ItemID=ItemID++;	
	vsprintf_s(buffer, sizeof(buffer), format, argList);

	strcpy_s(TRTemp.TestResultDes, sizeof(TRTemp.TestResultDes), buffer);

	TRTemp.Len=(unsigned int)strlen(TRTemp.TestResultDes);
	TestResultList.push_back(TRTemp);  
	
	va_end(argList);    /* cleanup arg list */
    return(retval);
}
int CTResultReport::OutputAllResult(char *Outbuf, int Len, char *Titlebuf, int titleLen)
{
	int RetLen=0;

	if (NULL==Outbuf)
	{
		return 0;
	}
	*Outbuf=0;

	list<MA_TEST_RESULT>::iterator iter;
	for(iter=TestResultList.begin();iter!=TestResultList.end();iter++)
	{

		RetLen++;
		strcat_s(Titlebuf,titleLen,(*iter).ItemName);
		strcat_s(Titlebuf,titleLen,",");
		strcat_s(Outbuf, Len, (*iter).TestResultDes);
	}
	return RetLen;
}
int CTResultReport::OutputTitle(char *Outbuf, int Len)
{
	int RetLen=0;

	if (NULL==Outbuf)
	{
		return 0;
	}
	*Outbuf=0;

	list<MA_TEST_RESULT>::iterator iter;
	for(iter=TestResultList.begin();iter!=TestResultList.end();iter++)
	{
		RetLen++;
		strcat_s(Outbuf, Len, (*iter).ItemName);
		strcat_s(Outbuf, Len, ",");
	}
	return RetLen;

}
void CTResultReport::SetUUTModelName(char *pName)
{
	strcpy_s(UUTModelName, sizeof(UUTModelName), pName);
	return;
}
void CTResultReport::SetStationName(char *pSName)
{
	strcpy_s(StationName, sizeof(StationName), pSName);
	return;
}
int CTResultReport::OutputFormatFile(char *pFileName)
{
	char FolderName[256];
	char FileName[256];
	FILE *fp;
	errno_t err;
	char* pfoldername=FolderName;
	GetCurrentDirectory(sizeof(FolderName), pfoldername);
	sprintf_s(FileName,sizeof(FileName),"%s\\%s.txt",pfoldername,pFileName);
	err=fopen_s(&fp,FileName,"w+");
	if(!err)
	{
		fprintf(fp,"Model=%s;\n",UUTModelName);
		fprintf(fp,"Station=%s;\n",StationName);
		list<MA_TEST_RESULT>::iterator iter;
		for(iter=TestResultList.begin();iter!=TestResultList.end();iter++)
		{
			fprintf(fp,"NUMBER=%d  %s  %d;\n",(*iter).ItemID,(*iter).ItemName,(*iter).Len);
		}
		fclose(fp);
	}
	else
	{
		return 0;
	}
	return 1;
}
int CTResultReport::OutputLogFile(char *pFileName,char *pTitle,char *pData)
{

	FILE *fLog;	
	
	char PathBuf[256];
	char FileName[512];
	int iFileOpenErr = 0;
	int existFlag=0;
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\TestResult\\", PathBuf);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
        strcat_s(FileName, sizeof(FileName), pFileName);
		existFlag = PathFileExists(FileName);
		//Maxwell 090622 mutex operate log
		WaitForSingleObject(hWriteLogMutex, INFINITE); 
		Sleep(1);
		iFileOpenErr = fopen_s(&fLog, FileName, "a+");
		if(NULL == fLog)
		{
			return FALSE;
		}
		if(!existFlag)
		{
			fprintf_s(fLog,"%s\n",pTitle);
		}
		fprintf_s(fLog,"%s\n",pData);
		fclose(fLog);
		ReleaseMutex(hWriteLogMutex);
	}

	return 1;
}

//Maxwell 090702
int CTResultReport::OutputTimeLogFile(char *pFileName,char *pData)
{

	FILE *fLog;	
	
	char PathBuf[256];
	char FileName[512];
	int iFileOpenErr = 0;
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\TimeLog\\", PathBuf);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
        strcat_s(FileName, sizeof(FileName), pFileName);
		//Maxwell 090622 mutex operate log
		WaitForSingleObject(hWriteLogMutex, INFINITE); 
		Sleep(1);
		iFileOpenErr = fopen_s(&fLog, FileName, "a+");
		if(NULL == fLog)
		{
			return FALSE;
		}
		fprintf_s(fLog,"%s\n",pData);
		fclose(fLog);
		ReleaseMutex(hWriteLogMutex);
	}

	return 1;
}
//Maxwell 090702

void CTResultReport::ClearBUF()
{
	ItemID=0;
	TestResultList.clear();
	return;
}


int CTResultReport::LogDataCAL(char* sn,char *datetime,char* filename,char* pData)
{
	FILE *fLog;	
	
	char PathBuf[256]="";
	char FileName[512]="";
	char cal_log_folder[512]="";
	int iFileOpenErr = 0;
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\NF_Data\\", PathBuf,sn,datetime);

		sprintf_s(cal_log_folder, sizeof(cal_log_folder), "%s\\NF_Data\\NF_%s_%s\\", PathBuf,sn,datetime);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
		if(!PathFileExists(cal_log_folder))
		{	
			if(!CreateDirectory(cal_log_folder,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
        strcat_s(cal_log_folder, sizeof(cal_log_folder), filename);

		iFileOpenErr = fopen_s(&fLog, cal_log_folder, "a+");
		if(NULL == fLog)
		{
			return FALSE;
		}
		fprintf_s(fLog,"%s\n",pData);
		fclose(fLog);

	}

	return TRUE;
}

int CTResultReport::LogDataNF(char* sn,char *datetime,char* filename,char* pData)
{
	FILE *fLog;	
	
	char PathBuf[256];
	char FileName[512];
	char nf_log_folder[512];
	int iFileOpenErr = 0;
	
	if(!GetCurrentDirectory(sizeof(PathBuf), PathBuf))
	{
        return FALSE;
	}
	else
	{
		sprintf_s(FileName, sizeof(FileName), "%s\\NF_Data\\", PathBuf,sn,datetime);

		sprintf_s(nf_log_folder, sizeof(nf_log_folder), "%s\\NF_Data\\NF_%s_%s\\", PathBuf,sn,datetime);
		if(!PathFileExists(FileName))
		{	
			if(!CreateDirectory(FileName,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
		if(!PathFileExists(nf_log_folder))
		{	
			if(!CreateDirectory(nf_log_folder,NULL))
			{
				amprintf("Create Folder Fail!\n");
				return 0;
			}
		}
        strcat_s(nf_log_folder, sizeof(nf_log_folder), filename);
		//Maxwell 090622 mutex operate log
		//WaitForSingleObject(hWriteLogMutex, INFINITE); 
		//Sleep(1);
		iFileOpenErr = fopen_s(&fLog, nf_log_folder, "a+");
		if(NULL == fLog)
		{
			return FALSE;
		}
		fprintf_s(fLog,"%s\n",pData);
		fclose(fLog);
		//ReleaseMutex(hWriteLogMutex);
	}

	return TRUE;
}

//add start by Talen
int CTResultReport::makeItemTitle(char *outBuf, char *inPut, int rty)
{
	int i=1;
	char temp[32]="";
	CString srcBuf;
	CString dstBuf(inPut);
//	CString srcBuf(inPut);
	dstBuf+=",";
	while(i <= rty)
	{
		srcBuf=_T(inPut);
		srcBuf+=",";
		memset(temp,0,sizeof(temp));
		sprintf_s(temp,sizeof(temp),"_RTY%d,",i);
		srcBuf.Replace(",",temp);
		i++;
		if(i>rty)
		{
			srcBuf.TrimRight(",");
			//srcBuf.Delete(srcBuf.GetLength()-1,1);
		}
		dstBuf+=srcBuf;
	}
	sprintf(outBuf,"%s",dstBuf);
	return 1;
}
//add end by Talen