#include "stdafx.h"

#include "AmbitProT.h"
#include "ConfigFile.h"
#include "AmParser.h"


#define FILE_PASER_BUF_L 512
#define PACKET_START '{'
#define PACKET_END '}'


CConfigFile::CConfigFile(void)
{

#ifdef _UNICODE

#else
	memset(StNeDFlag, -1, sizeof(STNED_FLAG)*500);
	TestItemSequence=1;
#endif
}

CConfigFile::~CConfigFile(void)
{




}

#ifdef _UNICODE
int CConfigFile::PerformParse(CString FileName)
{

	if (!ReadFileToMem(FileName))
	{
		return 0;
	}
	GetSector();

	return 1;
}
#else
int CConfigFile::PerformParse(char * pFileName)
{

	ReadFileToMem(pFileName);
	GetSegment();

	return 1;
}
#endif


#ifdef _UNICODE
int CConfigFile::ReadFileToMem(CString FileName)
{	
	wchar_t FilePath[500];

	CString FullFileName;
	CString strTemp;
	
	
	wchar_t FileLineBuf[1024];
	wchar_t *pcRtnFGet;

	FILE *fpStream = NULL;
	int err;
	int sNum = 0;	

	int MarkPosition=0;
	//initialize variable
	GetCurrentDirectory(sizeof(FilePath)/2, FilePath);

    FullFileName+=FilePath;
	FullFileName+=_T("\\");
	FullFileName+=FileName;

	// Open for read 
    if( (err = _wfopen_s( &fpStream, FullFileName, _T("r"))) !=0 )
    {
		//strTemp.Format(_T("Can not open file '%s'\n"),FullFileName);
		//AfxMessageBox(strTemp);
	    return FALSE;
    }
	
	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		pcRtnFGet = fgetws(FileLineBuf, sizeof(FileLineBuf)/2, fpStream);
		strTemp=FileLineBuf;
		
		if (!strTemp.IsEmpty())
		{
			MarkPosition=strTemp.Find(_T("//"));
			if (-1==MarkPosition)
			{
				FileContent+=strTemp;
			}
			else if (MarkPosition>0)
			{
				strTemp.Delete(MarkPosition, 1000);
				FileContent+=strTemp;
			}
			else
			{


			}
		}
	}	

    fclose(fpStream); 
    return TRUE;
}



int CConfigFile::GetSector()
{
	wchar_t  tempBuf[1000];

	CString resToken;
	int curPos= 0;

	resToken= FileContent.Tokenize(_T("{}"),curPos);
	wcscpy_s(tempBuf, sizeof(tempBuf)/2, resToken);
    GetSystemConfigInfo(tempBuf, sizeof(tempBuf)/2);
	while (resToken != "")
	{
		resToken= FileContent.Tokenize(_T("{}"),curPos);
		wcscpy_s(tempBuf, sizeof(tempBuf)/2, resToken);
		GetSystemConfigInfo(tempBuf, sizeof(tempBuf)/2);
	};

	return 1;
}



int CConfigFile::GetSystemConfigInfo(wchar_t *pBuf, int bufL)
{
	CAmParser Parser;

	wchar_t result[512];
	int err=0;
	if (!pBuf)
	{
		return 0;
	}

	if(Parser.ParserGetPara(pBuf, _T("CONFIG"), result, sizeof(result)/2))
	{
		if (!wcscmp(result, _T("PTS")))
		{
			if (Parser.ParserGetPara(pBuf, _T("ENABLE_FLAG"), result, sizeof(result)/2))
			{
				gStationInfo.PtsInfo.PtsSwitch=_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("TITLE_VERSION"), result, sizeof(result)/2))
			{         
				wcscpy_s(gStationInfo.PtsInfo.PtsTitleVer, sizeof(gStationInfo.PtsInfo.PtsTitleVer)/2,result);
			}
			if (Parser.ParserGetPara(pBuf, _T("IP"), result, sizeof(result)/2))
			{         
				wcscpy_s(gStationInfo.PtsInfo.PtsServerIP, sizeof(gStationInfo.PtsInfo.PtsServerIP)/2,result);
			}

		}
		
			
		if (!wcscmp(result, _T("EQUIPMENT")))
		{
			//Maxwell 090610
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_SERVER_IP"), result, sizeof(result)/2))
			{
				wcscpy_s(gStationInfo.EquipInfo.EqipServerIP, sizeof(gStationInfo.EquipInfo.EqipServerIP)/2, result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_LINE"), result, sizeof(result)/2))
			{
				wcscpy_s(gStationInfo.EquipInfo.EqipLine, sizeof(gStationInfo.EquipInfo.EqipLine)/2, result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_STATION"), result, sizeof(result)/2))
			{
				wcscpy_s(gStationInfo.EquipInfo.EqipStation, sizeof(gStationInfo.EquipInfo.EqipStation)/2, result);
			}
			
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_XML_FLAG"), result, sizeof(result)/2))
			{
				gStationInfo.EquipInfo.EqipXmlFlag=_wtof(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_LOG_PATH"), result, sizeof(result)/2))
			{
				wcscpy_s(gStationInfo.EquipInfo.EqipLogPath, sizeof(gStationInfo.EquipInfo.EqipLogPath)/2, result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_OPER_LOG_PATH"), result, sizeof(result)/2))
			{
				wcscpy_s(gStationInfo.EquipInfo.EqipOperPath, sizeof(gStationInfo.EquipInfo.EqipOperPath)/2, result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_ALARM_RATE"), result, sizeof(result)/2))
			{
				gStationInfo.EquipInfo.EqipAlarmRate=_wtof(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_CONTROL_FLAG"), result, sizeof(result)/2))
			{
				gStationInfo.EquipInfo.EqipCtlFlag=_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("EQUIP_ENABLE_FLAG"), result, sizeof(result)/2))
			{
				gStationInfo.EquipInfo.EqipEnableFlag=_wtoi(result);
			}
			/*EQUIP_LOG_PATH=F:\mfg\Counter\AP\W209\FT2\;
			EQUIP_OPER_LOG_PATH=F:\mfg\Counter\AP\W209\FT2\;
			EQUIP_ALARM_RATE=0.8;
			EQUIP_CONTROL_FLAG=1;*/
		}

		if (!wcscmp(result, _T("UIDISPLAY")))
		{
			if (Parser.ParserGetPara(pBuf, _T("STANDBY_TIME"), result, sizeof(result)/2))
			{
				gStationInfo.UiDisplay.StandByTime=_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("TP_TIMEOUT_TIME"), result, sizeof(result)/2))
			{         
				gStationInfo.UiDisplay.TPtimeout=_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("SFIS_TIMEOUT"), result, sizeof(result)/2))
			{         
				gStationInfo.UiDisplay.SfisTimeout=_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("REPEAT_TIME"), result, sizeof(result)/2))
			{         
				gStationInfo.UiDisplay.RepeatTime=_wtoi(result);
				if(gStationInfo.UiDisplay.RepeatTime>1)
				{
					gVariable.Repeat_Time_Flag=1;
				}
				else
				{
					gVariable.Repeat_Time_Flag=0;
				}
			}
			
		}
	

		if (!wcscmp(result, _T("MULTITASK")))
		{
			if (Parser.ParserGetPara(pBuf, _T("NUMBER"), result, sizeof(result)/2))
			{
				gStationInfo.MultiTaskSetting.Number =_wtoi(result);				
			}
			if (Parser.ParserGetPara(pBuf, _T("CONTROL_TYPE"), result, sizeof(result)/2))
			{         
				wcscpy_s(gStationInfo.MultiTaskSetting.ControlMethod, sizeof(gStationInfo.MultiTaskSetting.ControlMethod)/2, result);
			}
		}


		if (!wcscmp(result, _T("SERVER1")))
		{
			if (Parser.ParserGetPara(pBuf, _T("SERVER1_CONNECT"), result, sizeof(result)/2))
			{         
				gStationInfo.Server1Client.Enable =_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("SERVER1_IP"), result, sizeof(result)/2))
			{         
				wcscpy_s(gStationInfo.Server1Client.IP, sizeof(gStationInfo.Server1Client.IP)/2, result);
			}
			if (Parser.ParserGetPara(pBuf, _T("SERVER1_PORT"), result, sizeof(result)/2))
			{
				gStationInfo.Server1Client.Port =_wtoi(result);
			}
			if (Parser.ParserGetPara(pBuf, _T("SERVER1_CLIENT_NUM"), result, sizeof(result)/2))
			{         
				wcscpy_s(gStationInfo.Server1Client.ClientNumber, sizeof(gStationInfo.Server1Client.ClientNumber)/2, result);
			}
		}
	
	}	
	else
	{
		err++;
	}
	return err;

}

#else
int CConfigFile::ReadFileToMem(char *pfile_name)
{	
	char file_name[256];
	char temp_buf[FILE_PASER_BUF_L];
	char FileLineBuf[FILE_PASER_BUF_L];


	char *pcRtnFGet;
	//char delimiters[] = "=";
	FILE *fpStream = NULL;
	int err;
	int sNum = 0;	
	size_t TotalCnt=0;
	size_t ContentBufL;
	
	
	//initialize variable
	GetCurrentDirectory(sizeof(file_name),file_name);
	sprintf_s(temp_buf, sizeof(temp_buf), "\\%s", pfile_name);
	strcat_s(file_name, 256, temp_buf);	
 

	// Open for read 
    if( (err = fopen_s( &fpStream, file_name, "r" )) !=0 )
    {
		sprintf_s(temp_buf,sizeof(temp_buf),"The file '%s' was not opened\n",  pfile_name);
		//amprintf("STS=%s;",temp_buf);
	    return FALSE;
    }

	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream);
		memset(temp_buf, 0, sizeof(temp_buf));
		GetValidLine(FileLineBuf,temp_buf, sizeof(temp_buf));//we get the string before "//"	
		TotalCnt+=strlen(temp_buf);
	}	
    fclose(fpStream);  


	// Open for read 
    if( (err = fopen_s( &fpStream, file_name, "r" )) !=0 )
    {
		sprintf_s(temp_buf,sizeof(temp_buf),"The file '%s' was not opened\n",  pfile_name);
		//amprintf("STS=%s;",temp_buf);
	    return FALSE;
    }

    ContentBufL=TotalCnt*sizeof(char)+100;
    pContentBuf=(char *)malloc(ContentBufL);
	if (!pContentBuf)
	{
		return FALSE;
	}
    *pContentBuf=0;
	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream);
		memset(temp_buf, 0, sizeof(temp_buf));		
		GetValidLine(FileLineBuf, temp_buf, sizeof(temp_buf));//we get the string before "//"
	    strcat_s(pContentBuf,ContentBufL,temp_buf);
	}	

    fclose(fpStream); 
    return TRUE;


int CConfigFile::GetSegment()
{

	FindPaketSEflag(pContentBuf, strlen(pContentBuf));
	if (!LegalJudge())
	{
		return 0;
	}
	GetItem();
	if (pContentBuf)
	{
		free(pContentBuf);
		pContentBuf=NULL;
	}
	return 1;
}
int CConfigFile::LegalJudge()
{

	int iPopFlagIndex=0;

	while(!((StNeDFlag[iPopFlagIndex].StFlagIndex ==-2)&&(StNeDFlag[iPopFlagIndex].EdFlagIndex ==-2)))
	{


		if(((StNeDFlag[iPopFlagIndex].StFlagIndex ==-3)&&(StNeDFlag[iPopFlagIndex].EdFlagIndex ==-3)))// we find nothing
		{	
			return 0;
		}
		if ((StNeDFlag[iPopFlagIndex].StFlagIndex !=-1)&&(StNeDFlag[iPopFlagIndex].EdFlagIndex !=-1))// we find start and end
		{		

		}
		else if (StNeDFlag[iPopFlagIndex].StFlagIndex !=-1)// we find start only
		{
			return 0;				
		}
		else if (StNeDFlag[iPopFlagIndex].EdFlagIndex !=-1)// we find end only
		{
			return 0;
		}
		else
		{
            return 0;
		}
		iPopFlagIndex++;	

	}
	return 1;
}

int CConfigFile::GetItem()
{
	int iPopFlagIndex=0;
	char Buf[512];
	int STFlag;
	int ENDFlag;
	int LineL;

	while(!((StNeDFlag[iPopFlagIndex].StFlagIndex ==-2)&&(StNeDFlag[iPopFlagIndex].EdFlagIndex ==-2)))
	{
		if ((StNeDFlag[iPopFlagIndex].StFlagIndex !=-1)&&(StNeDFlag[iPopFlagIndex].EdFlagIndex !=-1))// we find start and end
		{		
			STFlag=StNeDFlag[iPopFlagIndex].StFlagIndex;
			ENDFlag=StNeDFlag[iPopFlagIndex].EdFlagIndex;
			LineL=ENDFlag-STFlag;
			memcpy_s(Buf, sizeof(Buf), pContentBuf+STFlag+1,LineL);		
			*(Buf+LineL-1)=0;// set string end flag
			
			GetSystemConfigInfo(Buf, LineL);
		
		}
		else
		{

		}
		iPopFlagIndex++;
	}
	return 1;
}
int CConfigFile::GetParaContent(char *pBuf, UINT Len)
{

	/*TEST_ITEM TestItemTemp;// use get test item information
	char result[512];
	if (!pBuf)
	{
		return 0;
	}

	memset(&TestItemTemp,0,sizeof(TestItemTemp));
	//{ITEM="Chenck UUT voltage Point1"; FLAG=1;  SPEC=5.0; LIMIT_UP=5.5; LIMIT_DN=4.5; ERR_CODE="VOL01"}
	

	TestItemTemp.Num=TestItemSequence++;

	if(ParserGetPara(pBuf, "ITEM", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.Name,sizeof(TestItemTemp.Name), result);
		if (!CopyCaseStr(TestItemTemp.ID, sizeof(TestItemTemp.ID),result,':'))
		{
			strcpy_s(TestItemTemp.ID, sizeof(TestItemTemp.ID), "NOID");
		}
	}
	else
	{
		strcpy_s(TestItemTemp.Name,sizeof(TestItemTemp.Name), "Undefine item name.");
		strcpy_s(TestItemTemp.ID, sizeof(TestItemTemp.ID), "NOID");
	}

	if(ParserGetPara(pBuf, "FLAG", result, sizeof(result)))
	{
		TestItemTemp.Flag=atoi(result);
	}
	else
	{
		TestItemTemp.Flag=1;
	}

	if(ParserGetPara(pBuf, "SPEC", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.Spec,sizeof(TestItemTemp.Spec), result);
	}
	else
	{
		strcpy_s(TestItemTemp.Spec,sizeof(TestItemTemp.Spec), "Undefine SPEC.");
	}

//Add start by Maxwell 2008/12/17 for double FW config in ambitconfig
	if(ParserGetPara(pBuf, "SPEC_1", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.Spec_1,sizeof(TestItemTemp.Spec_1), result);
	}
	else
	{
		strcpy_s(TestItemTemp.Spec_1,sizeof(TestItemTemp.Spec_1), "Undefine SPEC_1.");
	}
//Add end by Maxwell 2008/12/17 for double FW config in ambitconfig

	if(ParserGetPara(pBuf, "LIMIT_UP", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.UpLimit,sizeof(TestItemTemp.UpLimit), result);
	}
	else
	{
		strcpy_s(TestItemTemp.UpLimit,sizeof(TestItemTemp.UpLimit), "Undefine up limit.");
	}

	if(ParserGetPara(pBuf, "LIMIT_DN", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.DownLimit,sizeof(TestItemTemp.DownLimit), result);
	}
	else
	{
		strcpy_s(TestItemTemp.DownLimit,sizeof(TestItemTemp.DownLimit), "Undefine down limit.");
	}

	if(ParserGetPara(pBuf, "RESULT_TYPE", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.ResultType,sizeof(TestItemTemp.ResultType), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.ResultType,sizeof(TestItemTemp.ResultType), "");
	}

	if(ParserGetPara(pBuf, "ERR_CODE", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.ErrorCode,sizeof(TestItemTemp.ErrorCode), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.ErrorCode,sizeof(TestItemTemp.ErrorCode), "");
	}

	if(ParserGetPara(pBuf, "ERR_CODE_DES", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.ErrorDes,sizeof(TestItemTemp.ErrorDes), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.ErrorDes,sizeof(TestItemTemp.ErrorDes), "Undefine error code descript.");
	}

	if(ParserGetPara(pBuf, "RTY_VAL", result, sizeof(result)))
	{
		TestItemTemp.RtyFlag=atoi(result);
	}
	else
	{
		TestItemTemp.RtyFlag=0;
	}

	if(ParserGetPara(pBuf, "UUT_CMD", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.UUTCmd,sizeof(TestItemTemp.UUTCmd), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.UUTCmd,sizeof(TestItemTemp.UUTCmd), "");
	}

	if(ParserGetPara(pBuf, "DIAG_CMD", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.DiagCmd,sizeof(TestItemTemp.DiagCmd), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.DiagCmd,sizeof(TestItemTemp.DiagCmd), "");
	}

	if(ParserGetPara(pBuf, "INS_CMD", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.InsCmd,sizeof(TestItemTemp.InsCmd), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.InsCmd,sizeof(TestItemTemp.InsCmd), "");
	}


	if(ParserGetPara(pBuf, "UUT_CMD_TIME", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.CmdPerfTime,sizeof(TestItemTemp.CmdPerfTime), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.CmdPerfTime,sizeof(TestItemTemp.CmdPerfTime), "0");
	}

	if(ParserGetPara(pBuf, "DIAG_CMD_TIME", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.DiagPerfTime,sizeof(TestItemTemp.DiagPerfTime), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.DiagPerfTime,sizeof(TestItemTemp.DiagPerfTime), "0");
	}

	if(ParserGetPara(pBuf, "PARALLEL_RUN", result, sizeof(result)))
	{
		TestItemTemp.Parallel=atoi(result);	
	}
	else
	{
		TestItemTemp.Parallel=0;	
	}
	
	if(ParserGetPara(pBuf, "INS_CTRL_ID", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.InsCtrlID,sizeof(TestItemTemp.InsCtrlID), result);	
	}
	else
	{
		strcpy_s(TestItemTemp.InsCtrlID,sizeof(TestItemTemp.InsCtrlID), "127.0.0.1");	
	}
	
	if(ParserGetPara(pBuf, "INS_NAME", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.InsName,sizeof(TestItemTemp.InsName), result);
	}
	else
	{
		strcpy_s(TestItemTemp.InsName,sizeof(TestItemTemp.InsName), "Device");
	}

	if(ParserGetPara(pBuf, "INS_CMD_TIME", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.InsCmdTime, sizeof(TestItemTemp.InsCmdTime), result);
	}
	else
	{
		strcpy_s(TestItemTemp.InsCmdTime, sizeof(TestItemTemp.InsCmdTime), "0");
	}

	if(ParserGetPara(pBuf, "JUMP_INDEX", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.RetryJumpID, sizeof(TestItemTemp.RetryJumpID), result);
	}
	else
	{
		strcpy_s(TestItemTemp.RetryJumpID, sizeof(TestItemTemp.RetryJumpID), "NULL");
	}

/*Add start by Maxwell 2008/10/13
	if(ParserGetPara(pBuf, "JUMP_AFTER_INDEX", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.RetryAfterJumpID, sizeof(TestItemTemp.RetryAfterJumpID), result);
	}
	else
	{
		strcpy_s(TestItemTemp.RetryAfterJumpID, sizeof(TestItemTemp.RetryAfterJumpID), "NULL");
	}
//Add end by Maxwell 2008/10/13
//Add start by Maxwell 2008/10/13
	if(ParserGetPara(pBuf, "ITEMProperty", result, sizeof(result)))
	{
		TestItemTemp.ItemProperty=atoi(result);
	}
	else
	{
		TestItemTemp.ItemProperty=1;
	}
//Add end by Maxwell 2008/10/13

	TItemList.push_back(TestItemTemp);*/
	return 1;
}

int CConfigFile::JudgeStr(char *pBuf)
{
	if (!pBuf)
	{
		return 0;
	}

	if (strchr(pBuf,'\"'))
	{
		if (strchr(pBuf+1,'\"'))
		{
			return 1;
		}
	}
	return 0;
}

int CConfigFile::FindPaketSEflag(char *PBuf, size_t DataLen)
{

	UINT Index=0;
	int FindTarget=0;
	int insertIndex=0;
	
	for (Index=0;Index<DataLen;Index++)
	{
		if (PACKET_START == *(PBuf+Index))
		{
			StNeDFlag[insertIndex].StFlagIndex=Index;
			FindTarget=1;

			Index+=7;// if we found start flag, we must jump the data packet lenth flag segment
			if (!(Index<DataLen))// if the index large than datalen, we break
			{
				break;
			}
		}
		
		if(PACKET_END == *(PBuf+Index))
		{

			StNeDFlag[insertIndex++].EdFlagIndex=Index;
			
			FindTarget=2;
		}
	}   
	if (1==FindTarget)
	{
        insertIndex++;
	}
	else if (0==FindTarget)
	{
        StNeDFlag[insertIndex].EdFlagIndex=-3;
		StNeDFlag[insertIndex++].StFlagIndex=-3;
	}
	StNeDFlag[insertIndex].EdFlagIndex=StNeDFlag[insertIndex].StFlagIndex=-2;
	return FindTarget;
}


int CConfigFile::GetValidLine(char *pDesBUFF,char *pRtnConBUFF, int RtnSize)
{
	char TagBuf[512]="";
    char AnalyseBUFF[512]="";
	
	char *pTok=NULL;
	if ((!pDesBUFF)||(!pRtnConBUFF))
	{
		return 0;
	}
	strcpy_s(AnalyseBUFF,sizeof(AnalyseBUFF), pDesBUFF); 
    //we fetch the string before "//"
	pTok = strstr(AnalyseBUFF,"//");
	if (pTok)
	{
		*pTok=0;
	}

	strcpy_s(pRtnConBUFF, RtnSize, AnalyseBUFF);
	return 1;
}
int CConfigFile::ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen)
{

	char TagBUFF[128] = "";
	char AnalyseBUFF[512] = "";
	char ItemID[100]="";
	char *pToken = NULL;
	char *pFlagToken = NULL;
	char *pConStart = NULL;
	char *PConEnd = NULL;

	if((NULL==pDesBUFF)||(NULL==pTagBuff)||(NULL==pRtnConBUFF))
	{
		return 0;
	}
	if ((RtnConBUFFLen <= 0) || (RtnConBUFFLen > 512))
	{
		return 0;
	}
	
	
	strcpy_s(AnalyseBUFF, sizeof(AnalyseBUFF), pDesBUFF);
	strcpy_s(TagBUFF, sizeof(TagBUFF), pTagBuff);

	pFlagToken = NULL;
	pFlagToken=strstr(AnalyseBUFF,TagBUFF);
	if (!pFlagToken)
	{
        return 0;
	}




	if (!CopyCaseStr(ItemID, sizeof(ItemID),pFlagToken,'='))
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}

	if (strlen(ItemID)!=strlen(TagBUFF))
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}



	if (!strncmp(ItemID, TagBUFF, strlen(TagBUFF)))
	{
		pToken = strstr(pFlagToken, "="); 
		if (pToken)
		{
			pConStart = pToken;
			pConStart += 1;  
		}
		else
		{
			return 0;
		}
		pToken = strstr(pToken, ";"); 
		if (pToken)
		{
			PConEnd = pToken;
			*PConEnd = NULL;
		}
		else
		{
			return 0;
		}
		if (pConStart && PConEnd)
		{
			strcpy_s(pRtnConBUFF, RtnConBUFFLen, pConStart);
		}	
		return 1;
	}
	else
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}

}
/*this function is copy some character from string to other case one spec character was happen*/
int CConfigFile::CopyCaseStr(char *pDst, int DstLen, const char *pSrc, char caseCH)
{

	const char *pTok=0;

	if ((!pDst)||(!pSrc))
	{
		return 0;
	}

	pTok=strchr(pSrc, caseCH);
	if (!pTok)
	{
		*pDst=0;
		return 0;
	}

	return(!(strncpy_s(pDst, DstLen,pSrc, (pTok-pSrc))));
}

unsigned char CConfigFile::HEXConvert (unsigned char ucHEX)
{
	unsigned char A = 'A', a='a',zero = '0';
	
	if (ucHEX >= a) return (ucHEX-a+10);
	if (ucHEX >= A) return (ucHEX-A+10);
	if (ucHEX >= zero) return (ucHEX-zero);
	
	return 0;
}
int CConfigFile::StrHEXConvert(char *psrc, int iCovertNum,DWORD *pRtn)
{

	int i = 0;
	int j = 0;
	DWORD dwResult=0;
	BYTE ByteTemp1 = 0;
	BYTE ByteTemp2 = 0;
	BYTE ByteTemp = 0;
	char GetBuf[100] = "";
	char BufTemp[100] = "";

	char *pTok=NULL;

	if (!psrc)
	{
		return 0;
	}

	strcpy_s(BufTemp, sizeof(BufTemp), psrc);

	pTok=strchr(BufTemp, 'x');
	if (pTok)
	{
       strcpy_s(GetBuf, sizeof(GetBuf), (pTok+1));
	}
	else
	{
		return 0;
	}

	for (i = 0,j = 0; i < iCovertNum; i+=2, j++)
	{
		ByteTemp1 = HEXConvert (GetBuf[i]);
		ByteTemp2 = HEXConvert (GetBuf[i+1]);
		ByteTemp = ByteTemp1 << 4 | ByteTemp2;
		dwResult|=ByteTemp;
		if (i>=(iCovertNum-2))
		{
			break;
		}
		dwResult <<= 8;
	}
	*pRtn=dwResult;
	return 1;
}

char *CConfigFile::HexToStrConvert(char *PtrRtn, unsigned int  uiHex)//convert a ui to char
{

	int iIndex = 0;
	int iMaskIndex = 0;
	int iMask = 0xFF;
	
	if (!PtrRtn)
	{
		return 0;
	}

	for (iIndex = 0,  iMaskIndex = 3; iIndex < 4; iIndex++, iMaskIndex--)
	{
		*(PtrRtn + iIndex) = (uiHex & (iMask<<(8 * iMaskIndex)))>>(8 * iMaskIndex);
	}
    *(PtrRtn + iIndex) = NULL;
    return PtrRtn;

}
#endif 
