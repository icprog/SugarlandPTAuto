#include "stdafx.h"
#include "ConfigFile.h"

#define FILE_PASER_BUF_L 512
#define PACKET_START '{'
#define PACKET_END '}'


extern TEST_INPUT_INFO gTI;//test input information
extern list<TEST_ITEM> TItemList;





CConfigFile::CConfigFile(void)
{
	memset(StNeDFlag, -1, sizeof(STNED_FLAG)*500);
	TestItemSequence=1;
}

CConfigFile::~CConfigFile(void)
{
}

int CConfigFile::PerformParse(char * pFileName)
{

	ReadFileToMem(pFileName);
	GetSegment();

	return 1;
}
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
		//amprintf("%s;",temp_buf);
		amprintf("%s;\n",temp_buf);//Maxwell 090520
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
		amprintf("%s;\n",temp_buf);
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
		memset(FileLineBuf,0,sizeof(FileLineBuf));//Talen
		pcRtnFGet = fgets(FileLineBuf, FILE_PASER_BUF_L, fpStream);
		memset(temp_buf, 0, sizeof(temp_buf));		
		GetValidLine(FileLineBuf, temp_buf, sizeof(temp_buf));//we get the string before "//"
	    strcat_s(pContentBuf,ContentBufL,temp_buf);
	}	

    fclose(fpStream); 
    return TRUE;
}
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
	int FindFlag=9;
	char Buf[1024];
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
			if (FindFlag)
			{
				FindFlag=GetTestStaInfo(Buf, LineL);
			}
			else
			{
				GetParaContent(Buf, LineL);// get test item and limit from confia file.
			}
		}
		else
		{

		}
		iPopFlagIndex++;
	}
	return 1;
}
int CConfigFile::GetTestStaInfo(char * pBuf, int bufL)
{

	char result[512];
	int err=0;
	int i=0;
	if (!pBuf)
	{
		return 0;
	}

	if(ParserGetPara(pBuf, "POST", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Post,sizeof(gTI.TestStaInfo.Post),result);
	}	
	else
	{
		err++;
	}
	if(ParserGetPara(pBuf, "SECTOR", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Sector,sizeof(gTI.TestStaInfo.Sector),result);
	}	
	else
	{
		err++;
	}	
	if(ParserGetPara(pBuf, "LINE", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.ManufactureLine,sizeof(gTI.TestStaInfo.ManufactureLine),result);
	}	
	else
	{
		err++;
	}
	if(ParserGetPara(pBuf, "AREA", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Area,sizeof(gTI.TestStaInfo.Area),result);
	}	
	else
	{
		err++;
	}
	if(ParserGetPara(pBuf, "LOG_FILE_TYPE", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.LogFileType,sizeof(gTI.TestStaInfo.LogFileType),result);
	}	
	else
	{
		err++;
	}
	
	if(ParserGetPara(pBuf, "STATION_NAME", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.TestStaName,sizeof(gTI.TestStaInfo.TestStaName),result);
	}	
	else
	{
		err++;
	}

	if(ParserGetPara(pBuf, "STATION_ID", result, sizeof(result)))
	{
		gTI.TestStaInfo.TestStaID=atoi(result);
	}	
	else
	{
		err++;
	}

	if(ParserGetPara(pBuf, "STATION_SN", result, sizeof(result)))
	{
		gTI.TestStaInfo.TestStaSN=atoi(result);
	}	
	else
	{
		err++;
	}

	if(ParserGetPara(pBuf, "UUT_MODEL", result, sizeof(result)))
	{
		strcpy_s(gTI.UUTInfo.UUTName, sizeof(gTI.UUTInfo.UUTName),result);
	}	
	else
	{
		err++;
	}
	if(ParserGetPara(pBuf, "MEA_AUTO", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.MEAutoFlag, sizeof(gTI.TestStaInfo.MEAutoFlag),result);
	}	
	else
	{
		err++;
	}
	if(ParserGetPara(pBuf, "CCD_AUTO", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.CCDAutoFlag, sizeof(gTI.TestStaInfo.CCDAutoFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.CCDAutoFlag, sizeof(gTI.TestStaInfo.CCDAutoFlag),"0");
	}

//////////////////////////////////////
	if(ParserGetPara(pBuf, "VITEST", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.VitestFlag, sizeof(gTI.TestStaInfo.VitestFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.VitestFlag, sizeof(gTI.TestStaInfo.VitestFlag),"0");
	}

//Add start by Maxwell 2008/12/15 for open&close iperf in FT1
	if(ParserGetPara(pBuf, "IPERF_AUTO", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.IperfAutoFlag, sizeof(gTI.TestStaInfo.IperfAutoFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.IperfAutoFlag, sizeof(gTI.TestStaInfo.IperfAutoFlag),"0");
		//err++;
	}
//Add end by Maxwell 2008/12/15 for open&close iperf in FT1
//Add start by Maxwell 2009/07/28 for in FT1
	if(ParserGetPara(pBuf, "GOLDEN_MCS_SET", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.GoldenSetMCSFlag, sizeof(gTI.TestStaInfo.GoldenSetMCSFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.GoldenSetMCSFlag, sizeof(gTI.TestStaInfo.GoldenSetMCSFlag),"0");
		//err++;
	}
//Add end by Maxwell 2009/07/28 for  in FT1
//Add start by Maxwell 2009/10/06 for reboot Golden in FT1
	if(ParserGetPara(pBuf, "GOLDEN_REBOOT", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.GoldenRebootFlag, sizeof(gTI.TestStaInfo.GoldenRebootFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.GoldenRebootFlag, sizeof(gTI.TestStaInfo.GoldenRebootFlag),"0");
		//err++;
	}
//Add end by Maxwell 2009/10/06 for reboot Golden in FT1
	//Maxwell 110214
	if(ParserGetPara(pBuf, "LOG_FILE_PATH", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.TestLogPath,sizeof(gTI.TestStaInfo.TestLogPath),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.TestLogPath,sizeof(gTI.TestStaInfo.TestLogPath),"No log file path config!");
	}
	//Maxwell 110214
	//Add start by Maxwell 2008/12/16 for open&close iperf in FT1
	if(ParserGetPara(pBuf, "IPERF_BAT_1", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.IperfBat1, sizeof(gTI.TestStaInfo.IperfBat1),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 2008/12/16 for open&close iperf in FT1
	//Add start by Maxwell 2008/12/16 for open&close iperf in FT1
	if(ParserGetPara(pBuf, "IPERF_BAT_2", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.IperfBat2, sizeof(gTI.TestStaInfo.IperfBat2),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 2008/12/16 for open&close iperf in FT1
//Add start by Maxwell 2008/12/16 for open&close iperf in FT1
	if(ParserGetPara(pBuf, "GOLDEN_COM_5G", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.GoldenCmd5G, sizeof(gTI.TestStaInfo.GoldenCmd5G),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 2008/12/16 for open&close iperf in FT1
//Add start by Maxwell 2008/12/16 for open&close iperf in FT1
	if(ParserGetPara(pBuf, "GOLDEN_COM_2G", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.GoldenCmd2G, sizeof(gTI.TestStaInfo.GoldenCmd2G),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 2008/12/16 for open&close iperf in FT1
//Add start by Maxwell 2009/07/28 for set MCS in FT1
	if(ParserGetPara(pBuf, "GOLDEN_COMPORT_RATE", result, sizeof(result)))
	{
		gTI.TestStaInfo.GoldenComTransRate=atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.GoldenComTransRate=0;
		//err++;
	}
//Add end by Maxwell 2009/07/28 for set MCS in FT1

//Add start by Maxwell 2009/07/28 for set MCS in FT1
	if(ParserGetPara(pBuf, "GOLDEN_COMPORT_5G", result, sizeof(result)))
	{
		gTI.TestStaInfo.Golden5GCom=atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.Golden5GCom=0;
		//err++;
	}
//Add end by Maxwell 2009/07/28 for set MCS in FT1
//Add start by Maxwell 2009/07/28 for set MCS in FT1
	if(ParserGetPara(pBuf, "GOLDEN_COMPORT_2G", result, sizeof(result)))
	{
		gTI.TestStaInfo.Golden2GCom=atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.Golden2GCom=0;
		//err++;
	}
//Add end by Maxwell 2009/07/28 for set MCS in FT1

	//Add end by Maxwell 20110326 for open&close NARTflag
	if(ParserGetPara(pBuf, "NCArt_FLAG", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.NCArtFlag, sizeof(gTI.TestStaInfo.NCArtFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.NCArtFlag, sizeof(gTI.TestStaInfo.NCArtFlag),"0");
		//err++;
	}
	//Add end by Maxwell 20110326 for open&close NARTflag

	//Add start by Maxwell 20110326 for open&close NART_COM
	if(ParserGetPara(pBuf, "NART_COM", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.NartCmd, sizeof(gTI.TestStaInfo.NartCmd),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 20110326 for open&close NART_COM

	//Add start by Maxwell 20110326 for open&close CART_COM
	if(ParserGetPara(pBuf, "CART_COM", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.CartCmd, sizeof(gTI.TestStaInfo.CartCmd),result);
	}	
	else
	{
		//err++;
	}
//Add end by Maxwell 20110326 for open&close CART_COM
//Maxwell 20110326
	if(ParserGetPara(pBuf,"GoldenSelect",result,sizeof(result)))
	{
		gTI.TestStaInfo.GoldenSelect=atoi(result);
	}
	else
	{
		gTI.TestStaInfo.GoldenSelect=0;
	}

	if(ParserGetPara(pBuf,"GoldenSelectionMAX",result,sizeof(result)))
	{
		gTI.TestStaInfo.GoldenSelectionMAX=atof(result);
	}
	else
	{
		gTI.TestStaInfo.GoldenSelectionMAX=0;
	}

	if(ParserGetPara(pBuf,"GoldenSelectionMIN",result,sizeof(result)))
	{
		gTI.TestStaInfo.GoldenSelectionMIN=atof(result);
	}
	else
	{
		gTI.TestStaInfo.GoldenSelectionMIN=0;
	}
//Maxwell 20110326
//Maxwell 20110329
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT_ENABLE", result, sizeof(result)))
	{
		gTI.TestStaInfo.Golden_Connect_Para.connectFlag =atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.Golden_Connect_Para.connectFlag =0;
		//err++;
	}
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT_IP", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect_Para.connectIP, sizeof(gTI.TestStaInfo.Golden_Connect_Para.connectIP),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect_Para.connectIP, sizeof(gTI.TestStaInfo.Golden_Connect_Para.connectIP),"");
		//err++;
	}
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT_PORT", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect_Para.connectPORT, sizeof(gTI.TestStaInfo.Golden_Connect_Para.connectPORT),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect_Para.connectPORT, sizeof(gTI.TestStaInfo.Golden_Connect_Para.connectPORT),"");
		//err++;
	}
	if(ParserGetPara(pBuf, "CONTROL_BOARD_CONNECT_ENABLE", result, sizeof(result)))
	{
		gTI.TestStaInfo.Control_Board_Para.ConnectFlag =atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.Control_Board_Para.ConnectFlag =0;
		//err++;
	}
	if(ParserGetPara(pBuf, "CONTROL_BOARD_LED_SHOW", result, sizeof(result)))
	{
		gTI.TestStaInfo.Control_Board_Para.LedShowFlag =atoi(result);
	}
	else
	{
		gTI.TestStaInfo.Control_Board_Para.LedShowFlag =0;
		//err++;
	}
	//Maxwell 20110329

	//add start by Talen 2011/06/27
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT1_ENABLE", result, sizeof(result)))
	{
		gTI.TestStaInfo.Golden_Connect1_Para.connectFlag =atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.Golden_Connect1_Para.connectFlag =0;
		//err++;
	}
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT1_IP", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect1_Para.connectIP, sizeof(gTI.TestStaInfo.Golden_Connect1_Para.connectIP),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect1_Para.connectIP, sizeof(gTI.TestStaInfo.Golden_Connect1_Para.connectIP),"");
		//err++;
	}
	if(ParserGetPara(pBuf, "GOLDEN_SERVER_CONNECT1_PORT", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect1_Para.connectPORT, sizeof(gTI.TestStaInfo.Golden_Connect1_Para.connectPORT),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.Golden_Connect1_Para.connectPORT, sizeof(gTI.TestStaInfo.Golden_Connect1_Para.connectPORT),"");
		//err++;
	}
	//add end by Talen 2011/06/27
	//add start by Talen 2011/11/01
	for(i=0;i<200;i++)
	{
		gTI.TestStaInfo.indexSelected[i]=0;
	}
	//add end by Talen 2011/11/01
	//add start by Talen 2012/01/11
	if(ParserGetPara(pBuf, "SAMEERRO_LIMIT_TIMES", result, sizeof(result)))
	{
		gTI.TestStaInfo.errLimitTimes=atoi(result);
	}	
	else
	{
		gTI.TestStaInfo.errLimitTimes=65535;
		//err++;
	}
	//add end by Talen 2012/01/11


	if(ParserGetPara(pBuf, "PRINTLABEL_FLAG", result, sizeof(result)))
	{
		strcpy_s(gTI.TestStaInfo.PrintLabelFlag, sizeof(gTI.TestStaInfo.PrintLabelFlag),result);
	}	
	else
	{
		strcpy_s(gTI.TestStaInfo.PrintLabelFlag, sizeof(gTI.TestStaInfo.PrintLabelFlag),"0");
	}
	//add by Liu-Chen for k31 Wip Label 2012/04/11

	////add start by Talen 2012/03/13
	//if(ParserGetPara(pBuf, "RETRY_NOTICE_FLAG", result, sizeof(result)))
	//{
	//	gTI.TestStaInfo.retryNoticeFlag=atoi(result);
	//}	
	//else
	//{
	//	gTI.TestStaInfo.retryNoticeFlag=0;
	//	//err++;
	//}
	////add end by Talen 2012/03/13
	return err;
}


int CConfigFile::GetParaContent(char *pBuf, UINT Len)
{

	TEST_ITEM TestItemTemp;// use get test item information
	///090702
	char tempCmdName[512]="UUT_CMD_1";
	int uutCmdAddSum=1;
	///090702

	char result[512];
	if (!pBuf)
	{
		return 0;
	}

	memset(&TestItemTemp,0,sizeof(TestItemTemp));
	//{ITEM="Chenck UUT voltage Point1"; FLAG=1;  SPEC=5.0; LIMIT_UP=5.5; LIMIT_DN=4.5; ERR_CODE="VOL01"}
	
	TestItemTemp.selected=1;//Talen 2011/11/01

	//090613 initial sfis data use in test
	for(int i=0;i<SFISUseNum;i++)
	{
		strcpy_s(TestItemTemp.SFISDataTest[i].Item,sizeof(TestItemTemp.SFISDataTest[i].Item), "");
		strcpy_s(TestItemTemp.SFISDataTest[i].Data,sizeof(TestItemTemp.SFISDataTest[i].Data), "");
	}
	TestItemTemp.Num=TestItemSequence++;

	//090702
	for(int i=0;i<100;i++)
	{
		strcpy_s(TestItemTemp.UUTCmdAdd[i],sizeof(TestItemTemp.UUTCmdAdd[i]), "");
	}
	while(ParserGetPara(pBuf, tempCmdName, result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.UUTCmdAdd[uutCmdAddSum],sizeof(TestItemTemp.UUTCmdAdd[uutCmdAddSum]), result);
		uutCmdAddSum++;
		sprintf(tempCmdName,"UUT_CMD_%d",uutCmdAddSum);
	}
	//090702
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

//Add start by Maxwell 2009/09/28 for 6 spec in check nf_chain
	if(ParserGetPara(pBuf, "NF_SPEC_1", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[0],sizeof(TestItemTemp.MultiSpec[0]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[0],sizeof(TestItemTemp.MultiSpec[0]), "Undefine SPEC");
	}
	if(ParserGetPara(pBuf, "NF_SPEC_2", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[1],sizeof(TestItemTemp.MultiSpec[1]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[1],sizeof(TestItemTemp.MultiSpec[1]), "Undefine SPEC");
	}
	if(ParserGetPara(pBuf, "NF_SPEC_3", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[2],sizeof(TestItemTemp.MultiSpec[2]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[2],sizeof(TestItemTemp.MultiSpec[2]), "Undefine SPEC");
	}
	if(ParserGetPara(pBuf, "NF_SPEC_4", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[3],sizeof(TestItemTemp.MultiSpec[3]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[3],sizeof(TestItemTemp.MultiSpec[3]), "Undefine SPEC");
	}
	if(ParserGetPara(pBuf, "NF_SPEC_5", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[4],sizeof(TestItemTemp.MultiSpec[4]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[4],sizeof(TestItemTemp.MultiSpec[4]), "Undefine SPEC");
	}
	if(ParserGetPara(pBuf, "NF_SPEC_6", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[5],sizeof(TestItemTemp.MultiSpec[5]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[5],sizeof(TestItemTemp.MultiSpec[5]), "Undefine SPEC");
	}
	//Maxwell 20110330
	if(ParserGetPara(pBuf, "NF_SPEC_7", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[6],sizeof(TestItemTemp.MultiSpec[6]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[6],sizeof(TestItemTemp.MultiSpec[6]), "Undefine SPEC");
	}
	//Maxwell 20110330
	//Maxwell 20110330
	if(ParserGetPara(pBuf, "NF_SPEC_8", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.MultiSpec[7],sizeof(TestItemTemp.MultiSpec[7]), result);
	}
	else
	{
		strcpy_s(TestItemTemp.MultiSpec[7],sizeof(TestItemTemp.MultiSpec[7]), "Undefine SPEC");
	}
	//Maxwell 20110330
//Add end by Maxwell 2009/09/28 for 6 spec in check nf_chain

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
	////add by blucey 20100804
	//if(ParserGetPara(pBuf, "ERRO_CODE_1", result, sizeof(result)))
	//{
	//	strcpy_s(TestItemTemp.ErrorCode_1,sizeof(TestItemTemp.ErrorCode_1), result);		
	//}
	//else
	//{
	//	strcpy_s(TestItemTemp.ErrorCode_1,sizeof(TestItemTemp.ErrorCode_1), "");
	//}

	//if(ParserGetPara(pBuf, "ERRO_CODE_DES_1", result, sizeof(result)))
	//{
	//	strcpy_s(TestItemTemp.ErrorDes_1,sizeof(TestItemTemp.ErrorDes_1), result);		
	//}
	//else
	//{
	//	strcpy_s(TestItemTemp.ErrorDes_1,sizeof(TestItemTemp.ErrorDes_1), "Undefine error code descript.");
	//}///end

	if(ParserGetPara(pBuf, "RTY_VAL", result, sizeof(result)))
	{
		TestItemTemp.RtyFlag=atoi(result);
	}
	else
	{
		TestItemTemp.RtyFlag=0;
	}
//Maxwell 091005

	//Maxwell 110310
	if(ParserGetPara(pBuf, "LOOP_NUM", result, sizeof(result)))
	{
		TestItemTemp.LoopRunNum=atoi(result);
	}
	else
	{
		TestItemTemp.LoopRunNum=0;
	}
	//Maxwell 110310

	if(ParserGetPara(pBuf, "REBOOT_TIME", result, sizeof(result)))
	{
		TestItemTemp.RebootTime=atoi(result);
	}
	else
	{
		TestItemTemp.RebootTime=0;
	}
	if(ParserGetPara(pBuf, "REBOOT_COUNT", result, sizeof(result)))
	{
		TestItemTemp.RebootCount=atoi(result);
	}
	else
	{
		TestItemTemp.RebootCount=0;
	}
	
//Maxwell 091005
	//RebootTime

	if(ParserGetPara(pBuf, "UUT_CMD", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.UUTCmd,sizeof(TestItemTemp.UUTCmd), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.UUTCmd,sizeof(TestItemTemp.UUTCmd), "");
	}

	//090702

	//090702

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
	//0627
	if(ParserGetPara(pBuf, "COM_PORT", result, sizeof(result)))
	{
		TestItemTemp.ComPort=atoi(result);	
	}
	else
	{
		TestItemTemp.ComPort=0;	
	}

	//0627
	if(ParserGetPara(pBuf, "COM_PORT_2", result, sizeof(result)))
	{
		TestItemTemp.ComPort_1=atoi(result);	
	}
	else
	{
		TestItemTemp.ComPort_1=0;	
	}

	if(ParserGetPara(pBuf, "TRANS_RATE", result, sizeof(result)))
	{
		TestItemTemp.TransRate=atoi(result);	
	}
	else
	{
		TestItemTemp.TransRate=0;	
	}
	//0627
//225
	if(ParserGetPara(pBuf, "RESULT_NUM", result, sizeof(result)))
	{
		TestItemTemp.ResultNumber=atoi(result);	
	}
	else
	{
		TestItemTemp.ResultNumber=1;	
	}
//225
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

/*//Add start by Maxwell 2008/10/13
	if(ParserGetPara(pBuf, "JUMP_AFTER_INDEX", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.RetryAfterJumpID, sizeof(TestItemTemp.RetryAfterJumpID), result);
	}
	else
	{
		strcpy_s(TestItemTemp.RetryAfterJumpID, sizeof(TestItemTemp.RetryAfterJumpID), "NULL");
	}
//Add end by Maxwell 2008/10/13*/
//Add start by Maxwell 2008/10/13
	if(ParserGetPara(pBuf, "ITEMProperty", result, sizeof(result)))
	{
		TestItemTemp.ItemProperty=atoi(result);
	}
	else
	{
		TestItemTemp.ItemProperty=1;
	}
/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "CTL_PORT_1", result, sizeof(result)))
	{
		TestItemTemp.Port_1=atoi(result);	
	}
	else
	{
		TestItemTemp.Port_1=0;	
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "CTL_PORT_2", result, sizeof(result)))
	{
		TestItemTemp.Port_2=atoi(result);	
	}
	else
	{
		TestItemTemp.Port_2=0;	
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "LED_PORT_1", result, sizeof(result)))
	{
		TestItemTemp.LED_PORT_1=atoi(result);	
	}
	else
	{
		TestItemTemp.LED_PORT_1=0;	
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "LED_PORT_2", result, sizeof(result)))
	{
		TestItemTemp.LED_PORT_2=atoi(result);	
	}
	else
	{
		TestItemTemp.LED_PORT_2=0;	
	}
/*****************************************************************************************/

/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_R_SPEC_UP", result, sizeof(result)))
	{
		TestItemTemp.RGBI_R_SPEC_UP=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_R_SPEC_UP=0;	
	}
/*****************************************************************************************/
	/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_R_SPEC_DOWN", result, sizeof(result)))
	{
		TestItemTemp.RGBI_R_SPEC_DOWN=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_R_SPEC_DOWN=0;	
	}
/*****************************************************************************************/
	/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_G_SPEC_UP", result, sizeof(result)))
	{
		TestItemTemp.RGBI_G_SPEC_UP=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_G_SPEC_UP=0;	
	}
/*****************************************************************************************/
	/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_G_SPEC_DOWN", result, sizeof(result)))
	{
		TestItemTemp.RGBI_G_SPEC_DOWN=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_G_SPEC_DOWN=0;	
	}
/*****************************************************************************************/
	/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_B_SPEC_UP", result, sizeof(result)))
	{
		TestItemTemp.RGBI_B_SPEC_UP=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_B_SPEC_UP=0;	
	}
/*****************************************************************************************/
	/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_B_SPEC_DOWN", result, sizeof(result)))
	{
		TestItemTemp.RGBI_B_SPEC_DOWN=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_B_SPEC_DOWN=0;	
	}
/*****************************************************************************************/

		/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_I_SPEC_UP", result, sizeof(result)))
	{
		TestItemTemp.RGBI_I_SPEC_UP=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_I_SPEC_UP=0;	
	}
/*****************************************************************************************/


		/*****************************************************************************************/
	//Add by Sam 2011/03/28/
	if(ParserGetPara(pBuf, "RGBI_I_SPEC_DOWN", result, sizeof(result)))
	{
		TestItemTemp.RGBI_I_SPEC_DOWN=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_I_SPEC_DOWN=0;	
	}
/*****************************************************************************************/

	//Add start by haibin,li 6.15
	
	if(ParserGetPara(pBuf, "RGBI_W_SPEC_DOWN", result, sizeof(result)))
	{
		TestItemTemp.RGBI_W_SPEC_DOWN=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_W_SPEC_DOWN=0;	
	}

	if(ParserGetPara(pBuf, "RGBI_I_SPEC_UP", result, sizeof(result)))
	{
		TestItemTemp.RGBI_W_SPEC_UP=atoi(result);	
	}
	else
	{
		TestItemTemp.RGBI_W_SPEC_UP=0;	
	}
	//Add end by haibin,li 6.15


	//add start by haibin 08.01
	
	if(ParserGetPara(pBuf, "SOCKET_PORT", result, sizeof(result)))
	{
		TestItemTemp.SocketPort=atol(result);		
	}
	else
	{
		TestItemTemp.SocketPort=0;	
	}
	if(ParserGetPara(pBuf, "SOCKET_IP", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.SocketIp, sizeof(TestItemTemp.SocketIp), result);		
	}	
	else
	{
		strcpy_s(TestItemTemp.SocketIp, sizeof(TestItemTemp.SocketIp), "0.0.0.0");		
	}
	if(ParserGetPara(pBuf, "X_OFFSET_UP", result, sizeof(result)))
	{
		TestItemTemp.X_OFFSET_UP=atof(result);	
	}
	else
	{
		TestItemTemp.X_OFFSET_UP=0;	
	}
	if(ParserGetPara(pBuf, "X_OFFSET_DOWN", result, sizeof(result)))
	{
		TestItemTemp.X_OFFSET_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.X_OFFSET_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "Y_OFFSET_UP", result, sizeof(result)))
	{
		TestItemTemp.Y_OFFSET_UP=atof(result);	
	}
	else
	{
		TestItemTemp.Y_OFFSET_UP=0;	
	}
	if(ParserGetPara(pBuf, "Y_OFFSET_DOWN", result, sizeof(result)))
	{
		TestItemTemp.Y_OFFSET_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.Y_OFFSET_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "ROTATE_ANGLE_UP", result, sizeof(result)))
	{
		TestItemTemp.ROTATE_ANGLE_UP=atof(result);	
	}
	else
	{
		TestItemTemp.ROTATE_ANGLE_UP=0;	
	}
	if(ParserGetPara(pBuf, "ROTATE_ANGLE_DOWN", result, sizeof(result)))
	{
		TestItemTemp.ROTATE_ANGLE_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.ROTATE_ANGLE_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "AVG_HIST_UP", result, sizeof(result)))
	{
		TestItemTemp.AVG_HIST_UP=atof(result);	
	}
	else
	{
		TestItemTemp.AVG_HIST_UP=0;	
	}
	if(ParserGetPara(pBuf, "AVG_HIST_DOWN", result, sizeof(result)))
	{
		TestItemTemp.AVG_HIST_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.AVG_HIST_DOWN=0;	
	}		
	if(ParserGetPara(pBuf, "CCORR_UP", result, sizeof(result)))
	{
		TestItemTemp.CCORR_UP=atof(result);	
	}
	else
	{
		TestItemTemp.CCORR_UP=0;	
	}
	if(ParserGetPara(pBuf, "CCORR_DOWN", result, sizeof(result)))
	{
		TestItemTemp.CCORR_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.CCORR_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "TOTAL_AREA_UP", result, sizeof(result)))
	{
		TestItemTemp.TOTAL_AREA_UP=atof(result);	
	}
	else
	{
		TestItemTemp.TOTAL_AREA_UP=0;	
	}
	if(ParserGetPara(pBuf, "TOTAL_AREA_DOWN", result, sizeof(result)))
	{
		TestItemTemp.TOTAL_AREA_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.TOTAL_AREA_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "MAX_AREA_UP", result, sizeof(result)))
	{
		TestItemTemp.MAX_AREA_UP=atof(result);	
	}
	else
	{
		TestItemTemp.MAX_AREA_UP=0;	
	}
	if(ParserGetPara(pBuf, "MAX_AREA_DOWN", result, sizeof(result)))
	{
		TestItemTemp.MAX_AREA_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.MAX_AREA_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "MAX_PERIMETER_UP", result, sizeof(result)))
	{
		TestItemTemp.MAX_PERIMETER_UP=atof(result);	
	}
	else
	{
		TestItemTemp.MAX_PERIMETER_UP=0;	
	}
	if(ParserGetPara(pBuf, "MAX_PERIMETER_DOWN", result, sizeof(result)))
	{
		TestItemTemp.MAX_PERIMETER_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.MAX_PERIMETER_DOWN=0;	
	}	
	if(ParserGetPara(pBuf, "SQUARE_WIDTH_UP", result, sizeof(result)))
	{
		TestItemTemp.SQUARE_WIDTH_UP=atof(result);	
	}
	else
	{
		TestItemTemp.SQUARE_WIDTH_UP=0;	
	}
	if(ParserGetPara(pBuf, "SQUARE_WIDTH_DOWN", result, sizeof(result)))
	{
		TestItemTemp.SQUARE_WIDTH_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.SQUARE_WIDTH_DOWN=0;	
	}
	if(ParserGetPara(pBuf, "SQUARE_HEIGHT_UP", result, sizeof(result)))
	{
		TestItemTemp.SQUARE_HEIGHT_UP=atof(result);	
	}
	else
	{
		TestItemTemp.SQUARE_HEIGHT_UP=0;	
	}
	if(ParserGetPara(pBuf, "SQUARE_HEIGHT_DOWN", result, sizeof(result)))
	{
		TestItemTemp.SQUARE_HEIGHT_DOWN=atof(result);	
	}
	else
	{
		TestItemTemp.SQUARE_HEIGHT_DOWN=0;	
	}
	//add end by haibin 08.01


//Add end by Maxwell 2008/10/13
//Maxwell 090609
	if(ParserGetPara(pBuf, "SFIS_DATA", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.SFISData,sizeof(TestItemTemp.SFISData), result);		
	}
	else
	{
		strcpy_s(TestItemTemp.SFISData,sizeof(TestItemTemp.SFISData), "");
	}
//Maxwell 090609
	//090613 Add for STB use
	if(ParserGetPara(pBuf, "SFIS_DATA_1", result, sizeof(result)))
	{
		strcpy_s(TestItemTemp.SFISDataTest[0].Item,sizeof(TestItemTemp.SFISDataTest[0].Item), result);
	}
	else
	{
		strcpy_s(TestItemTemp.SFISDataTest[0].Item,sizeof(TestItemTemp.SFISDataTest[0].Item), "");
	}
	//090613 Add for STB use     Notice:If you have SFIS_DATA_2 you must use SFISDataTest[0] etc

	//add start by Talen 2011/07/06
	if(ParserGetPara(pBuf, "CMP_FLAG", result, sizeof(result)))
	{
		TestItemTemp.CmpFlag=atoi(result);	
	}
	else
	{
		TestItemTemp.CmpFlag=0;	
	}
	//add end by Talen 2011/07/06

	TItemList.push_back(TestItemTemp);
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
	char AnalyseBUFF[1024] = "";
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


//Maxwell 11228
int CConfigFile::ConfigFilePnNameParse(char *pfile_name)
{
	char FileLineBuf[512];
	char result[512];	

	char *pcRtnFGet;
	char delimiters[] =("=");
	FILE *fpStream = NULL;
	int err;
	int sNum = 0;	

	memset(result,0,512);
	if( (err = fopen_s( &fpStream,pfile_name, ("r") )) !=0 )
	{
		amprintf( _T("Can't open PN_NAME.ini!"));
		return FALSE;
	}

	while ( (!feof(fpStream ))&&(sNum!=3)) 
	{
		pcRtnFGet = fgets(FileLineBuf, 512, fpStream);   
		if ((*FileLineBuf=='#')||(*FileLineBuf==' '))
		{
			continue;
		}

		if(ParserGetPara(FileLineBuf, "PRODUCT_NAME", result, sizeof(result)))
		{
			strcpy_s(gTI.UUTInfo.UUTPN,sizeof(gTI.UUTInfo.UUTPN),result);
			memset(result,'\0',sizeof(result));
		}	
	}
	return 1;
}
//Maxwell 11228