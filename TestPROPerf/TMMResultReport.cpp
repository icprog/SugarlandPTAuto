#include "StdAfx.h"
#include "TMMResultReport.h"



CTMMResultReport::CTMMResultReport(void)
{


}

CTMMResultReport::~CTMMResultReport(void)
{


}

int CTMMResultReport::OutputTMMLogFile(char* pRtn, int BufL)
{

	int RetLen=0;
	char TempBuf[128];

	if (!pRtn)
	{
		return 0;
	}
	*pRtn=0;

	strcat_s(pRtn, BufL, TMMHeadStr);

    strcat_s(pRtn, BufL, "BEGIN\n");
	list<MA_TEST_RESULT>::iterator iter;
	for(iter=TestResultList.begin();iter!=TestResultList.end();iter++)
	{

		RetLen++;
		sprintf_s(TempBuf, sizeof(TempBuf), "$%s$\n",(*iter).ItemName);
		strcat_s(pRtn, BufL, TempBuf);
		

		sprintf_s(TempBuf, sizeof(TempBuf), "\t%s\n",(*iter).TestResultDes);
		strcat_s(pRtn, BufL, TempBuf);
	}
	strcat_s(pRtn, BufL, "END\n");

	return RetLen;
}

int CTMMResultReport::HeadContentInsert(char * pbuf)
{
	
	TMMHeadStr+=pbuf;
	TMMHeadStr+="\t\n";
	
	return 0;
}


int CTMMResultReport::FileNameInsert(char * pbuf)
{
	
	TMMFileName+=pbuf;
	TMMFileName+="_";
	
	return 0;
}

void CTMMResultReport::HeadInsertStart(void)
{
	TMMHeadStr.Empty();
}
void CTMMResultReport::FileNameInsertStart(void)
{
	TMMFileName.Empty();
}
int CTMMResultReport::WriteTMMLogFile(char *Pin)
{
#define PATH_BUF_SIZE 100
	int Lenstr;
	TMMFileName.Replace(":","");
	Lenstr = TMMFileName.GetLength();
	TMMFileName.Delete(Lenstr-1);
	TMMFileName+=".txt";


	FILE *fLog;	
	char PathBuf[256];
	char FileName[512];
	int iFileOpenErr = 0;
	
	if(!GetCurrentDirectory(PATH_BUF_SIZE, PathBuf))
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

		sprintf_s(PathBuf,sizeof(PathBuf),"%s",TMMFileName);
        strcat_s(FileName, sizeof(FileName), PathBuf);
		
		iFileOpenErr = fopen_s(&fLog, FileName, "w");
		if(NULL == fLog)
		{
			return FALSE;
		}
		fprintf_s(fLog,"%s",Pin);
		fclose(fLog);
	}

	return 1;
}