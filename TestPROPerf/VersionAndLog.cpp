#include "stdafx.h"
#include "VersionAndLog.h"


//////////////////////////////////////////////////////////////////////////
//
void PrintPassFail(BOOL bFailed)
{
    if(bFailed)
    {
		printf("\r\n");
		printf("========================================================================\n");
        printf("FFFFFFFFFF     AA        IIIIII    LL          EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("FFFFFFFFFF   AA  AA      IIIIII    LL          EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("FF          AA    AA       II      LL          EE          DD   DD    !!\r\n");
		printf("FF          AA    AA       II      LL          EE          DD    DD   !!\r\n");
		printf("FFFFFF      AAAAAAAA       II      LL          EEEEEE      DD    DDD  !!\r\n");
		printf("FFFFFF      AAAAAAAA       II      LL          EEEEEE      DD    DDD  !!\r\n");
		printf("FF          AA    AA       II      LL          EE          DD    DD   !!\r\n");
		printf("FF          AA    AA     IIIIII    LLLLLLLLLL  EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("FF          AA    AA     IIIIII    LLLLLLLLLL  EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("========================================================================\n");
		printf("Board is not functioning properly.\r\n");
    }
    else
    {
        printf("\r\n");
		printf("========================================================================\n");
        printf("PPPPPPPPP      AA      SSSSSSSSSS   SSSSSSSSS  EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("PPPPPPPPPP   AA  AA   SSSSSSSSSSS  SSSSSSSSSS  EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("PP      PP  AA    AA  SS           SS          EE          DD   DD    !!\r\n");
		printf("PP      PP  AA    AA  SS           SS          EE          DD    DD   !!\r\n");
		printf("PPPPPPPPP   AAAAAAAA   SSSSSSSSS    SSSSSSSS   EEEEEE      DD    DDD  !!\r\n");
		printf("PPPPPPPP    AAAAAAAA   SSSSSSSSS    SSSSSSSS   EEEEEE      DD    DDD  !!\r\n");
		printf("PP          AA    AA           SS          SS  EE          DD    DD   !!\r\n");
		printf("PP          AA    AA           SS          SS  EE          DD   DD    !!\r\n");
		printf("PP          AA    AA  SSSSSSSSSSS  SSSSSSSSSS  EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("PP          AA    AA  SSSSSSSSSS   SSSSSSSSS   EEEEEEEEEE  DDDDDD     !!\r\n");
		printf("========================================================================\n");
		printf("### Board passed all tests!!! ###\r\n");
    }

}
//////////////////////////////////////////////////////////////////////////
//
void PrintErrorMessage(FILE *hFile, char *pMsg, bool bPrint = TRUE)
{

	if(hFile)
    {
        fprintf(hFile, "#\n%s\t", pMsg);
		//fprintf(fCurLog, "%s\r\n", pMsg);
    }
    if(bPrint)
    {
        printf("%s\r\n", pMsg);
    }
}
//////////////////////////////////////////////////////////////////////////
//
void PrintTimeStamp(FILE *hFile)
{
    SYSTEMTIME	time;
    char szMsg[MAX_MESSAGE_LENGTH];

    GetLocalTime(&time);
    sprintf_s(szMsg, sizeof(szMsg),  "Time:\t%04d/%02d/%02d %02d:%02d:%02d::%03d",   time.wYear, time.wMonth,
	    			                						time.wDay, time.wHour, time.wMinute, 
								                	    time.wSecond, time.wMilliseconds);
    PrintErrorMessage(hFile, szMsg, TRUE);
}
//////////////////////////////////////////////////////////////////////////
//
void PrintTheVersion()
{

   printf("*******************************************************************************\n");
   printf("\n");
   printf("               Test program, version:%s\n", SOFTWARE_VERSION);
   printf("               Copyright(C) Ambit Micro system.   \n");
   printf("               CPE II RD Software Team     \n");
   printf("\n");
   printf("*******************************************************************************\n");

}
//////////////////////////////////////////////////////////////////////////
//
BOOL OpenRomFile(char *pBuf, char *pFileName)
{
	char PathBuf[256];
	char RomFileName[512];
	FILE *pRomFile;
	int iFileOpenErr = 0;
	
	if (NULL == pBuf) 
	{
		return FALSE;
	}
	if (NULL == pFileName) 
	{
		return FALSE;
	}
	memset(pBuf,0, SROM_BUF_SIZE);
  	if(!GetCurrentDirectory(PATH_BUF_SIZE, PathBuf))
	{
        return FALSE;
	}
	else
	{
        sprintf_s(RomFileName, sizeof(RomFileName),  "%s\\%s", PathBuf, pFileName);
	    iFileOpenErr = fopen_s(&pRomFile, RomFileName, "r");
		if(NULL == pRomFile)
		{
			return FALSE;
		}

		fseek(pRomFile,0, SEEK_END);
		int iFileLength = ftell(pRomFile);
		fseek(pRomFile,0, SEEK_SET);

		if (!fread(pBuf, sizeof(char), iFileLength, pRomFile)) 
		{
			return FALSE;
		}
		if (pRomFile) 
		{
			fclose(pRomFile);
		}		
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// 
FILE *OpenLog(char *pLogFileName, char *pOpenPara)
{
    FILE *fLog;	
	char PathBuf[256];
	char FileName[512];
	int iFileOpenErr = 0;
	
	if (NULL == pLogFileName) 
	{
		return FALSE;
	}
	if(NULL == pOpenPara)
	{
		return FALSE;
	}
	if(!GetCurrentDirectory(PATH_BUF_SIZE, PathBuf))
	{
        return FALSE;
	}
	else
	{
        sprintf_s(FileName, sizeof(FileName), "%s\\%s", PathBuf, pLogFileName);
		iFileOpenErr = fopen_s(&fLog, FileName, pOpenPara);
		if(NULL == fLog)
		{
			return FALSE;
		}				
	}
	return fLog;
}