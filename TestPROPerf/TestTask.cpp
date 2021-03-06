/********************************************************************
created:	2008/04/29
filename: 	TestTask.cpp
file path:	..\TestPROPerf\TestPROPerf
file base:	TestTask.cpp
file ext:	cpp
author:		Jeffrey

purpose:	do the act test in process function definition and script description

*********************************************************************/
#include "stdafx.h"
#include "TestTask.h"
#include "TMMResultReport.h"
#include "AmbitParser.h"
#include "CenTime.h"
#include "AAPTest.h"
#include "TestItemScript.h"//Added by Maxwell 2008/10/14
#include "AmParser.h"//maxwell 090520
#include "ScriptSupport.h"



int fix_id=0;
bool gThreadExitFlag =true;
bool IsGolden = true;
int FreezeFlag=0;//Added by Maxwell 2008/11/6
int ItemTestID =1;//Maxwell 090521
int sfisStartFlag;//add by Talen 2011/10/26 for distinguish SFIS start or CLICK start
//int RebootTime = 1;//Maxwell 091005
HANDLE hTestThread;
extern char HDD_SN[80];    //Add by Tony on 091217 for send HDSN
extern char WIFI_MAC0[80]; //Add by Tony on 091223 for send WifiMac0 to SFIS in Screen
extern char WIFI_MAC1[80]; //Add by Tony on 091223 for send WifiMac1 to SFIS in Screen
extern char WANMAC[80];	   //Add by Tony on 091225 for send EthernetMAC to SFIS in Screen
extern TEST_INPUT_INFO gTI;//test input information

extern HANDLE hWriteServerLogMutex;//Talen 2011/09/01
extern int gEntireExitFlag;
extern CTestItemScript tiScript;//Added by Maxwell 2008/10/14

extern char DetailLogBuffer[128000];//Maxwell 11228

list<TEST_ITEM> TItemList;
list<SFIS_ITEM> SItemList;//Maxwell 090520

RUN_TEST_PARA gRunTestPara;// add by jeffrey 070225
TEST_INPUT_INFO gTI;//test input information
SFISSW SFIS;
CTMMResultReport CTReport;

int RunTest()
{	
	/**********************************************************************/
		//Add by Sam 2011/03/30
		if(gTI.TestStaInfo.Control_Board_Para.ConnectFlag&&gTI.TestStaInfo.Control_Board_Para.LedShowFlag)
		{
			LEDShowTestingStatus();
		}
		/**********************************************************************/
	unsigned TestThreadID;
	//HANDLE hTestThread;//Modified by Maxwell 2008/11/6

	hTestThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		MainTestThread,
		NULL,
		0,
		&TestThreadID 
		);

	if (hTestThread)
	{
	}

//Add start by Maxwell 2008/11/6
	//if(FreezeFlag)
	//{
	//	SuspendThread(hTestThread);
	//}
//Add end by Maxwell 2008/11/6
	return 1;
}

int strMaxLenCmp(const char *p1, const char *p2)
{
	size_t len;
	len=strlen(p1);
	if (strlen(p2)>len)
	{
		len=strlen(p2);
	}
	return (strncmp(p1,p2,len));
}

int AcceptCMD(char *pCmd,UINT DataLen)
{
	//Maxwell 090520
	CAmParser cParser;
	cParser.SetStartStopTag("[","]");

	char DDBuf[3004]="";
	char cCmdBuf[512]="";
	char cUseBuf[512]="";
	char *pTok=NULL;
	char *pToken=NULL;
	char *pTokFlag=NULL;
	int iCovResult = 0;

	if (!pCmd)
	{
		return 0;
	}

	memcpy_s(DDBuf,sizeof(DDBuf),pCmd,DataLen);

	/*
	UI->TEST:CONTROL=CMD[START];
	TEST->UI:CONTROL=ECHOCMD[START];
	TEST->UI:ITEM=NUMER[12];


	UI->TEST:SFIS=SWITCH[ON/OFF];SFIS=SN[6F8821345]WIFIMAC[001122334455]ETHMAC[0011223344455]HDCKEY[1234567]RESULT[PASS];


	UI->TEST:CONTROL=CMD[PAUSE]; 
	TEST->UI:CONTROL=ECHOCMD[PAUSE];
	UI->TEST:CONTROL=CMD[CONTINUE];
	TEST->UI:CONTROL=ECHOCMD[CONTINUE];
	UI->TEST:CONTROL=CMD[STOP]; 
	TEST->UI:CONTROL=ECHOCMD[STOP];


	TEST->UI:ITEM=ID[1]NAME[Check video quality];
	TEST->UI:ITEM=ID[1]RESULT[PASS];



	TEST->UI:MSGBOX=CONTENT[content];
	UI->TEST:MSGBOX=RESPONSE[Input/Result];

	TEST->UI:SFIS=SN[6F8821345]WIFIMAC[001122334455]ETHMAC[0011223344455]HDCKEY[1234567]GETITEM[1122334455];CONTROL=RESULT[PASS/FAIL];
	TEST->UI:LOG=CONTENT[test log data];
	*/

//Maxwell 090525 modify for parser more than 1 words in TP
	pToken = DDBuf;
	amprintf("%s;\n",pToken);
	while(strstr(pToken,";"))
	{
		if (pTok=strstr(pToken, "FIXTURE="))
		{
			if (!cParser.ParserGetPara(pTok, "FIXID", cCmdBuf, sizeof(cCmdBuf)))
			{
				amprintf("Illogical Command Input!;\n");	
			}	
			//fix_id = atoi(cCmdBuf);
		}

		//add start by Talen 2011/11/01
		else if(pTok=strstr(pToken, "CONFIG"))
		{
			if(strstr(pTok,"RESPONSE["))
			{
				if (!cParser.ParserGetPara(pTok, "RESPONSE", cCmdBuf, sizeof(cCmdBuf)))
				{
					amprintf("RESPONSE: Illogical Command Input!;\n");
					return 1;
				}	
				else
				{
					ParserConfigItem(cCmdBuf);
					Setambitconfig();
				}
			}

			if(strstr(pTok,"OPID["))
			{
				if (!cParser.ParserGetPara(pTok, "OPID", cCmdBuf, sizeof(cCmdBuf)))
				{
					amprintf("OPID: Illogical Command Input!;\n");
					return 1;
				}	
				else
				{
					strcpy_s(gTI.TestStaInfo.opID,sizeof(gTI.TestStaInfo.opID),cCmdBuf);
				}
			}
			
		}
		//add end by Talen 2011/11/01
		else if (pTok=strstr(pToken, "CONTROL="))
		{		
			if (!cParser.ParserGetPara(pTok, "CMD", cCmdBuf, sizeof(cCmdBuf)))
			{
				amprintf("Illogical Command Input!;\n");	
			}	
			else
			{
				//iCovResult = atoi(cCmdBuf);
				//gRunTestPara.CMDCode = iCovResult;
			}	

			if (!strncmp("STOP", cCmdBuf, strlen("STOP")))
			{
				gEntireExitFlag = FALSE;
				return 1;
			}

			if (!strncmp("EXIT", cCmdBuf, strlen("EXIT")))
			{
				gEntireExitFlag = FALSE;
				return 1;
			}
			//add start by Talen 2011/10/26
			else if (!strncmp("SFIS_START", cCmdBuf, strlen("SFIS_START")))
			{
				amprintf("CONTROL=ECHOCMD[SFIS_START];");
				sfisStartFlag=1;
				if(gThreadExitFlag)
				{
					RunTest();
				}					
				else
				{
					amprintf("Illogical Command Input!code!=START;\n");	
				}
			}
			//add end by Talen 2011/10/26
			else if (!strncmp("START", cCmdBuf, strlen("START")))
			{
				amprintf("CONTROL=ECHOCMD[START];");	
				sfisStartFlag=0;//add by Talen 2011/10/26
				if(gThreadExitFlag)
				{
					RunTest();
				}					
				else
				{
					amprintf("Illogical Command Input!code!=START;\n");	
				}
			}
		}

		else if (pTok=strstr(pToken, "SFIS"))
		{
			if (!ParserGetPara(pToken, "SFIS", cCmdBuf, sizeof(cCmdBuf)))
			{
				amprintf("SFIS DDBuf:%s;\n",DDBuf);
				amprintf("Illogical SFIS Command Fromat Input!;\n");	
				return 1;
			}	
			if (cParser.ParserGetPara(cCmdBuf, "SWITCH", cUseBuf, sizeof(cUseBuf)))
			{
				//amprintf("Illogical Command Input!;\n");	
				if (!strncmp("ON", cUseBuf, strlen(cUseBuf)))
				{
					sprintf_s(SFIS.SSFISMode,sizeof(SFIS.SSFISMode),"SFIS_ON");
					amprintf("Test is in SFIS on mode!;\n");
				}
				//add start by Talen 2011/03/22
				else if(!strncmp("VISUAL_ON", cUseBuf, strlen(cUseBuf)))
				{
					sprintf_s(SFIS.SSFISMode,sizeof(SFIS.SSFISMode),"SFIS_OFF");
					amprintf("Test is in VISUAL_SFIS on mode!;\n");
				}
				//add end by Talen 2011/03/22
				else if(!strncmp("OFF", cUseBuf, strlen(cUseBuf)))
				{
					sprintf_s(SFIS.SSFISMode,sizeof(SFIS.SSFISMode),"SFIS_OFF");
					amprintf("Test is in SFIS off mode!;\n");
				}
			}
			else
			{
				char TempBuf[512] ="";
				char *pToke=NULL;
				char OutBuf[512]="";
				SFIS_ITEM SfisItemTemp;
				pTokFlag = strstr(pToken, "[");
				if (!pTokFlag)
				{
					amprintf("SFIS Data is empty;\n");	
					return 1;
				}
				pTok += strlen("SFIS=");
				int mn = pTokFlag-pTok;
				strncpy_s(SfisItemTemp.Item,sizeof(SfisItemTemp.Item),pTok,(int)(pTokFlag-pTok));

				//SN=%s;MACID1=%s;WIFIMAC=%s;ITEM1=%s;
				if (cParser.ParserGetPara(pTok, SfisItemTemp.Item, TempBuf, sizeof(TempBuf)))
				{
					strcpy_s(SfisItemTemp.Data, sizeof(SfisItemTemp.Data), TempBuf);
				}
				SItemList.push_back(SfisItemTemp);
				pTok = strstr(pTok,"]");
				pTok+=1;
				while(*pTok!=';')
				{
					pTokFlag = strstr(pTok, "[");
					strncpy_s(SfisItemTemp.Item,sizeof(SfisItemTemp.Item),pTok,pTokFlag-pTok);

					//SN=%s;MACID1=%s;WIFIMAC=%s;ITEM1=%s;
					if (cParser.ParserGetPara(pTok, SfisItemTemp.Item, TempBuf, sizeof(TempBuf)))
					{
						strcpy_s(SfisItemTemp.Data, sizeof(SfisItemTemp.Data), TempBuf);
					}
					SItemList.push_back(SfisItemTemp);
					pTok = strstr(pTok,"]");
					pTok+=1;
				}

				//Maxwell 090609
				list<SFIS_ITEM>::iterator Csy;
				list<TEST_ITEM>::iterator Cy;


				for (Csy=SItemList.begin(); Csy!=SItemList.end();Csy++)
				{
					amprintf("SFIS ID:%s Data:%s;\n",(*Csy).Item,(*Csy).Data);
					if (!strMaxLenCmp("SN",(*Csy).Item))
					{
						strcpy_s(gTI.UUTInfo.UUTSN,sizeof(gTI.UUTInfo.UUTSN),(*Csy).Data);	
					}
					else
					{
					}
					if (!strMaxLenCmp("ETHERNETMAC",(*Csy).Item))
					{
						strcpy_s(gTI.UUTInfo.UUTMAC1, sizeof(gTI.UUTInfo.UUTMAC1),(*Csy).Data);	
					}
					else
					{
					}
					if (!strMaxLenCmp("WIFIMAC0",(*Csy).Item))
					{
						strcpy_s(gTI.UUTInfo.UUTMAC2, sizeof(gTI.UUTInfo.UUTMAC2),(*Csy).Data);	
					}
					else
					{
					}
					if (!strMaxLenCmp("WIFIMAC1",(*Csy).Item))
					{
						strcpy_s(gTI.UUTInfo.UUTMAC3, sizeof(gTI.UUTInfo.UUTMAC3),(*Csy).Data);	
					}
					else
					{
					}
				}

				for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
				{
					for (Csy=SItemList.begin(); Csy!=SItemList.end();Csy++)
					{
						//amprintf("SFIS ID:%s Data:%s;\n",(*Csy).Item,(*Csy).Data);
						if (!strMaxLenCmp((*Cy).SFISData,(*Csy).Item))
						{	
							strcpy_s((*Cy).Spec,sizeof((*Cy).Spec),(*Csy).Data);
						}
						else
						{

						}
						
						//090613
						for(int i = 0;i<SFISUseNum;i++)
						{
							if (!strMaxLenCmp((*Cy).SFISDataTest[i].Item,(*Csy).Item))
							{	
								strcpy_s((*Cy).SFISDataTest[i].Data,sizeof((*Cy).SFISDataTest[i].Data),(*Csy).Data);
								amprintf("SFIS ID:%s Data:%s;\n",(*Csy).Item,(*Csy).Data);
							}
							else
							{

							}
						}
						//090613
					}
				}

				//Maxwell 090609

				//if (ParserGetPara(pTok, "WIFIMAC", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.UUTInfo.UUTMAC2, sizeof(gTI.UUTInfo.UUTMAC1), TempBuf);	
				//	SetSFISData("CHECK_WL_CARD",TempBuf,0);
				//}
				//if (ParserGetPara(pTok, "ITEM1", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.UUTInfo.UUTMAC3, sizeof(gTI.UUTInfo.UUTMAC1), TempBuf);	
				//	SetSFISData("CHECK_WL_CARD",TempBuf,1);

				//}
				//if (ParserGetPara(pTok, "SN", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.UUTInfo.UUTSN, sizeof(gTI.UUTInfo.UUTSN), TempBuf);
				//	SetSFISData("SET_UUT_SN",TempBuf);
				//	SetSFISData("CHECK_UUT_SN",TempBuf);
				//}
				//if (ParserGetPara(pTok, "ITEM5", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.TestStaInfo.Operator, sizeof(gTI.TestStaInfo.Operator), TempBuf);
				//}
				//if (ParserGetPara(pTok, "MO", TempBuf, sizeof(TempBuf)))
				//{
				//	//strcpy_s(gUUTInfo.UUTMO, sizeof(gUUTInfo.UUTMO), TempBuf);
				//}
				//if (ParserGetPara(pTok, "NAME", TempBuf, sizeof(TempBuf)))
				//{
				//	//strcpy_s(gUUTInfo.UUTName, sizeof(gUUTInfo.UUTName), TempBuf);
				//}
				//if (ParserGetPara(pTok, "SFISSW", TempBuf, sizeof(TempBuf)))
				//{
				//	SFIS.SFISMode =atoi(TempBuf);
				//}
				//if (ParserGetPara(pTok, "TESTRES", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.UiResMsg.MsgRes, sizeof(gTI.UiResMsg.MsgRes), TempBuf);
				//	SetEvent(gTI.UiResMsg.hEvent);
				//}
				//if (ParserGetPara(pTok, "VR", TempBuf, sizeof(TempBuf)))
				//{
				//	strcpy_s(gTI.TestStaInfo.UIVersion, sizeof(gTI.TestStaInfo.UIVersion), TempBuf);		
				//}
			}
		}

		else if (pTok=strstr(pToken, "MSGBOX"))
		{
			if (!ParserGetPara(pToken, "MSGBOX", cCmdBuf, sizeof(cCmdBuf)))
			{
				amprintf("Illogical MSGBOX Command Fromat Input!;\n");	
				return 1;
			}	
			if (cParser.ParserGetPara(cCmdBuf, "RESPONSE", cUseBuf, sizeof(cUseBuf)))
			{
				//amprintf("Illogical Command Input!;");	
				//if (!strncmp("Input", cUseBuf, strlen(cUseBuf)))
				//{
				////090619 MSGBOX
				//}
				/*else if(!strncmp("ResultPass", cUseBuf, strlen(cUseBuf)))
				{*/
					strcpy_s(gTI.UiResMsg.MsgRes, sizeof(gTI.UiResMsg.MsgRes), cUseBuf);
					SetEvent(gTI.UiResMsg.hEvent);
			/*	}*/
			}
			else
			{
				
			}
		}
		/*else if (pTok=strstr(DDBuf, "DAT"))
		{
			pTok+=strlen("DAT;");
			if (!AnaTheData(pTok))
			{
			}
		}
		else if(pTok=strstr(DDBuf, "INI"))
		{
			pTok+=strlen("INI;");
			if (!AnaTheData(pTok))
			{

			}
		}
		else if(pTok=strstr(DDBuf, "MSG"))
		{
			pTok+=strlen("MSG;");
			if (!AnaTheData(pTok))
			{

			}
		}*/
		else
		{
			printf("The other data.\n");
		} 
		pToken=strstr(pToken,";");
		pToken+=1;
	} //Maxwell  090525

	return 1;
}

int AnaTheData(char *pBuf)
{
	char TempBuf[512] ="";
	char *pToke=NULL;
	char OutBuf[512]="";
	if (!pBuf)
	{
		return 0;
	}
    //SN=%s;MACID1=%s;WIFIMAC=%s;ITEM1=%s;
	if (ParserGetPara(pBuf, "MACID1", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.UUTInfo.UUTMAC1, sizeof(gTI.UUTInfo.UUTMAC1), TempBuf);
		SetSFISData("SET_UUT_WMAC",TempBuf);
		SetSFISData("CHECK_UUT_WMAC",TempBuf);
	}
	if (ParserGetPara(pBuf, "WIFIMAC", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.UUTInfo.UUTMAC2, sizeof(gTI.UUTInfo.UUTMAC1), TempBuf);	
		SetSFISData("CHECK_WL_CARD",TempBuf,0);
	}
	if (ParserGetPara(pBuf, "ITEM1", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.UUTInfo.UUTMAC3, sizeof(gTI.UUTInfo.UUTMAC1), TempBuf);	
		SetSFISData("CHECK_WL_CARD",TempBuf,1);

	}
	if (ParserGetPara(pBuf, "SN", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.UUTInfo.UUTSN, sizeof(gTI.UUTInfo.UUTSN), TempBuf);
		SetSFISData("SET_UUT_SN",TempBuf);
		SetSFISData("CHECK_UUT_SN",TempBuf);
	}
	if (ParserGetPara(pBuf, "ITEM5", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.TestStaInfo.Operator, sizeof(gTI.TestStaInfo.Operator), TempBuf);
	}
    if (ParserGetPara(pBuf, "MO", TempBuf, sizeof(TempBuf)))
	{
		//strcpy_s(gUUTInfo.UUTMO, sizeof(gUUTInfo.UUTMO), TempBuf);
	}
    if (ParserGetPara(pBuf, "NAME", TempBuf, sizeof(TempBuf)))
	{
		//strcpy_s(gUUTInfo.UUTName, sizeof(gUUTInfo.UUTName), TempBuf);
	}
	if (ParserGetPara(pBuf, "SFISSW", TempBuf, sizeof(TempBuf)))
	{
		SFIS.SFISMode =atoi(TempBuf);
		amprintf("SFIS MODE:%d\n",SFIS.SFISMode);
	}
	if (ParserGetPara(pBuf, "TESTRES", TempBuf, sizeof(TempBuf)))
	{
		strcpy_s(gTI.UiResMsg.MsgRes, sizeof(gTI.UiResMsg.MsgRes), TempBuf);
		SetEvent(gTI.UiResMsg.hEvent);
	}
	if (ParserGetPara(pBuf, "VR", TempBuf, sizeof(TempBuf)))
	{
	    strcpy_s(gTI.TestStaInfo.UIVersion, sizeof(gTI.TestStaInfo.UIVersion), TempBuf);		
	}
	return 1;
}

void ClearSFISBufSpec()
{
	////Maxwell 090609
	//	list<TEST_ITEM>::iterator Cy;

	//			for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	//			{
	//				strcpy_s((*Cy).Spec, sizeof((*Cy).Spec), "");
	//				for(int i = 0;i<SFISUseNum;i++)
	//				{
	//					strcpy_s((*Cy).SFISDataTest[i].Data,sizeof((*Cy).SFISDataTest[i].Data),"");
	//				}
	//			}

	////Maxwell 090609

	ClearSFISData("SET_UUT_WMAC");
	ClearSFISData("CHECK_UUT_WMAC");
	ClearSFISData("CHECK_WL_CARD");
	ClearSFISData("SET_UUT_SN");
	ClearSFISData("CHECK_UUT_SN");


	//Talen 2012/02/09
	list<TEST_ITEM>::iterator Cy;

	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if(!strMaxLenCmp((*Cy).SFISData,"SN"))
		{
		}
		else if(!strMaxLenCmp((*Cy).SFISData,"ETHERNETMAC"))
		{
		}
		else if(!strMaxLenCmp((*Cy).SFISData,"WIFIMAC0"))
		{
		}
		else if(!strMaxLenCmp((*Cy).SFISData,"WIFIMAC1"))
		{
		}
		else
		{
			continue;
		}

		strcpy_s((*Cy).Spec,sizeof((*Cy).Spec),"");
	}

	//Talen 2012/02/09

	return;
}

int SetSFISData(char *pITEMID,char *pData,int Index)
{
	list<TEST_ITEM>::iterator Cy;
	if ((!pITEMID)|| (!pData))
	{
		amprintf("Set SFIS data wrong;\n");
		return 0;
	}
	if (!strlen(pData))
	{
		amprintf("SFIS pData empty;\n");
		return 0;
	}
	if (!strlen(pITEMID))
	{
		amprintf("SFIS pITEMID empty;\n");
		return 0;
	}

	if (0==SFIS.SFISMode)
	{
		amprintf("Test in SFIS OFF;\n");
		return 0;
	}
	if (2==SFIS.SFISMode)
	{
		amprintf("Test in SFIS simulator;\n");
		return 0;
	}
	
	
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{

		if (!strMaxLenCmp((*Cy).ID,pITEMID))
		{	
			if (-1!=Index)
			{
				if (Index==atoi((*Cy).UpLimit))
				{
                    strcpy_s((*Cy).Spec,sizeof((*Cy).Spec),pData);
					amprintf("SFIS ID:%s Data:%s;\n",(*Cy).ID,(*Cy).Spec);
				}
			}
			else
			{
                strcpy_s((*Cy).Spec,sizeof((*Cy).Spec),pData);
				amprintf("SFIS ID:%s Data:%s;\n",(*Cy).ID,(*Cy).Spec);
			}
		}
	}
	return 1;
}

void ClearSFISData(char *pITEMID)
{
	list<TEST_ITEM>::iterator Cy;
	if ((!pITEMID))
	{
		amprintf("Clear SFIS data wrong;\n");
		return;
	}

	if (!strlen(pITEMID))
	{
		amprintf("Clear SFIS pITEMID empty;\n");
		return;
	}

	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if (!strMaxLenCmp((*Cy).ID,pITEMID))
		{	
			strcpy_s((*Cy).Spec,sizeof((*Cy).Spec),"");
          //  amprintf("Clear %s SFIS spec;\n",(*Cy).ID);
		}
	}
	return;
}
unsigned int _stdcall MainTestThread(LPVOID lpParameter)
{
	list<TEST_ITEM>::iterator Cy;
	PerfTest(&Cy);
	ReportTestResult(Cy);
	AfterTestWork();
	return 1;
}

int SendInfRequrementToUI()
{
	//amprintf("INI;GETVER=;");      //Maxwell 090521
	return 1;
}
int SendLogTileToUI()
{
	char Tile[5000];
	CTReport.OutputTitle(Tile, sizeof(Tile));
	//amprintf("INI;LOGTITLE=%s;\n",Tile);     //Maxwell 090521
	return 1;
}

int SendIDInfoToUI(void)
{
	//prepare informations

	strcpy_s(gTI.TestStaInfo.TestProgramVersion,
		    sizeof(gTI.TestStaInfo.TestProgramVersion),
			SOFTWARE_VERSION);
	
//Maxwell 090521	
	amprintf("CONTROL=TSNAME[%s]TSMODEL[%s]TSVER[%s];", 
		gTI.TestStaInfo.TestStaName,
		gTI.UUTInfo.UUTName,
		gTI.TestStaInfo.TestProgramVersion
		//gTI.TestStaInfo.TestStaSN,
		//gTI.TestStaInfo.TestStaID,
		//gTI.UUTInfo.UUTName
		); //Add send UUT model to UI for database use
	
	
	
	    // Create a manual-reset event object. The master thread sets 
    // this to nonsignaled when it writes to the shared buffer. 

	gTI.UiResMsg.hEvent=NULL;
	gTI.UiResMsg.hEvent = CreateEvent( 
        NULL,         // default security attributes
        TRUE,         // manual-reset event
        FALSE,         // initial state is signaled
        "ResEvent"  // object name
        ); 
    if (gTI.UiResMsg.hEvent == NULL) 
    { 
        printf("CreateEvent failed (%d)\n", GetLastError());
    }

	return 1;
}

int MultiEventCreate()
{
	HANDLE hEvent; 
	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		(*Cy).hEndEvent=NULL;
		if (((*Cy).Parallel>0)&&((*Cy).Flag>0))
		{
			// Create event objects.
			hEvent = CreateEvent( 
				NULL,   // default security attributes
				TRUE,  // auto-reset event object
				FALSE,  // initial state is nonsignaled
				NULL);  // unnamed object

			if (hEvent == NULL) 
			{ 
				printf("CreateEvent error: %d\n", GetLastError()); 
				return 0;
			} 
			(*Cy).hEndEvent=hEvent;
		}
	}
	return 1;
}
int WaitMultiEvent(list<TEST_ITEM>::iterator CyStop)
{
	DWORD dwWaitResult; 
	list<TEST_ITEM>::iterator Cy;
	
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if (CyStop==Cy)
		{
			break;
		}
		if ((*Cy).hEndEvent)
		{
			// Request ownership of mutex.
			dwWaitResult = WaitForSingleObject( 
				(*Cy).hEndEvent,   // handle to mutex
				60000L);   // five-second time-out interval
            SetEvent((*Cy).hEndEvent);
		}
	}
	return 1;
}

int JudgeMultiTestResult(list<TEST_ITEM>::iterator CyStop)
{
	list<TEST_ITEM>::iterator Cy;
	
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if (CyStop==Cy)
		{
			break;
		}
		if ((*Cy).hEndEvent)
		{
			if (TEST_FAIL==(*Cy).ResultFlag)// test result process
			{
				return (*Cy).Flag;
			}
		}
	}
	return 0;
}


void ResetMultiEvent()
{
	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if ((*Cy).hEndEvent)
		{
			ResetEvent((*Cy).hEndEvent);
		}
	}
	return;
}

void MultiEventClear()
{
	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if ((*Cy).hEndEvent)
		{
			CloseHandle((*Cy).hEndEvent);
		}
	}
}

int TestInitial()
{ 	
	//Maxwell 20110326
	char PathBuf[256]="";
	GetCurrentDirectory(sizeof(PathBuf),PathBuf);
	strcpy_s(gTI.TestStaInfo.LocalFilePath,sizeof(gTI.TestStaInfo.LocalFilePath),PathBuf); 
	//Maxwell 20110326
	if(!MultiInitial())
	{
		return 0;
	}
	UUTIOInitial();// communication IO parameter set and initial
    TestReportReset();
	MultiEventCreate();
	if (!InstrumentInitial())//Instrument Initial
	{
		amprintf("Can not open the MeasurementAuto.exe,testperf exit;\n");
		return 0;
	}
	if (!CCDInitial())//Instrument Initial
	{
		amprintf("Can not open the iImageT.exe,testperf exit;\n");
		return 0;
	}	
//Add start by Maxwell 2008/12/15 for open iperf in FT1
	if (!IperfInitial())//Instrument Initial
	{
		amprintf("Can not open the Iperf,testperf exit;\n");
		return 0;
	}
	//Add end by Maxwell 2008/12/15 for open iperf in FT1

	//Add start by Maxwell 2009/10/05 for Reboot Golden in FT1
	if (!THGoldenReboot())//Instrument Initial
	{
		amprintf("Can not reboot Golden,testperf exit;\n");
		//amprintf("RESULT[FAIL];CONTROL=RESULT[FAIL];");
		return 0;
	}
	if (!K31printInitial())//Instrument Initial
	{
		amprintf("Can not open the M48_CUST_SN_8.0.0.9.exe,testperf exit;\n");
		return 0;
	}
	//add by Liu-Chen for k31 Wip Label 2012/04/11

	else
	{
		//amprintf("RESULT[PASS];CONTROL=RESULT[PASS];");
	}
	//Sleep(150000);
	//Add end by Maxwell 2009/10/05 for Reboot Golden in FT1
//Add start by Maxwell 2009/07/28 for open iperf in FT1
	if (!THGoldenInitial())//Instrument Initial
	{
		amprintf("Can not set the Golden's MCS index,testperf exit;\n");
		/*amprintf("Can not reboot Golden,testperf exit;\n");
		amprintf("RESULT[FAIL];CONTROL=RESULT[FAIL];");*/
		return 0;
	}
	else
	{
		/*THGoldenInitial();
		amprintf("RESULT[PASS];CONTROL=RESULT[PASS];");*/
	}
	//Add end by Maxwell 2009/07/28 for open iperf in FT1
	if (!RunNCart())//Instrument Initial
	{
		amprintf("Can not call up Nart&Cart,testperf exit;\n");
		//amprintf("RESULT[FAIL];CONTROL=RESULT[FAIL];");
		return 0;
	}
	else
	{
		//amprintf("RESULT[PASS];CONTROL=RESULT[PASS];");
	}
	//haibin 2011/08/01
	if (!VitestInitial())
	{
		amprintf("Can not open the vitest.exe,testperf exit;\n");
		return 0;
	}	
	//haibin 2011/08/01

	if (!strcmp(gTI.TestStaInfo.TestStaName,"MPT2"))	 //add by liu-chen
	{
		if(!BinFolderInitial())
		{
			amprintf("Failed to Bin Folder Initial");
			return 0;
		}
	}

	sendConfigToUI();


	return 1;
}
void TestReportReset()
{
	list<TEST_ITEM>::iterator Cy;
	
	CTReport.SetStationName(gTI.TestStaInfo.TestStaName);
	CTReport.SetUUTModelName(gTI.UUTInfo.UUTName);


	CTReport.InputResult("SN","%s,","111111111111111");
	CTReport.InputResult("MAC_ID","%s,","111111111111111");
	CTReport.InputResult("Test_Result","%s,","1");
	CTReport.InputResult("PC_Name","%s,","test computer");
	CTReport.InputResult("Test_Date","%s,","0000");	
	CTReport.InputResult("Test_Time","%s,","00000000");
	CTReport.InputResult("Cycle_Time","%s,","00000000");


	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{
		if (strlen((*Cy).ResultType))
		{
			CTReport.InputResult((*Cy).ResultType,"%s,","-999999");
		}
	}
	
	CTReport.InputResult("ErrorCode","%s,","-999999");
    CTReport.InputResult("Error_Desc","%s,","-999999");

	CTReport.OutputFormatFile("format");

	return;
}
void TestUninitial()
{
	IperfUnInitial();//Add by Maxwell 2008/12/15 for close iperf in FTI 
	MultiEventClear();
	InstrumentUnInitial();
	CCDUnInitial();
	K31printUnInitial();//add by Liu-Chen for k31 Wip Label 2012/04/11
	VitestUnInitial();//haibin 2011/08/01
    UUTIOUnInitial();	
	IQUnInitial();//Maxwell 20110325
	return;
}


//Maxwell 110310
int __stdcall ItemTestTask(TEST_ITEM *pTI)
{
	CCenTime Time;
	int TestResult=0;
	char TimeBuf[50];
	int RetryCount=0;
	int RetryLimit=0;
	char RetryInformation[30]="";
	char ResultBuf[10240]="";//Maxwell 100223
	char ResultTemp[128]="";//Maxwell 100223
	int RetestNoShowFlag=0;//add by maxwell 2008/10/31 defined if it's retry,do not show item on UI 
	int ResultCount=0;//add by Talen 2011/04/12
	Time.TimeStartCount();//count the time


	if(!pTI->LoopRunNum)
	{
		if((!pTI->RetryJumpCount)&&(NO_TEST==pTI->ResultFlag)/*&&(!RetestNoShowFlag)*/)
		{
			//amprintf("*********************Testing NAME[%s]*********************;\n",pTI->Name);
			amprintf("ITEM=ID[%d]NAME[%s];\n",ItemTestID,pTI->Name);
		}

		//for retry test item cotrol
		//Add start by Maxwell 2008/10/13
		if(NO_TEST==pTI->ResultFlag)
		{
LABLE_ITEM_PROPERTY:

			RetryLimit=pTI->RtyFlag;
			RetryLimit+=2;

			for (RetryCount=1; RetryCount<RetryLimit; RetryCount++)
			{
				//add start by Talen 2011/04/12
				memset(pTI->Result,0,sizeof(pTI->Result));
				for(ResultCount=1;ResultCount<=pTI->ResultNumber;ResultCount++)
				{
					if(ResultCount==pTI->ResultNumber)
					{
						strcat_s(pTI->Result,sizeof(pTI->Result),"-999999");
					}
					else
					{
						strcat_s(pTI->Result,sizeof(pTI->Result),"-999999,");
					}
				}
				//add end by Talen 2011/04/12
				TestResult=ItemTest(pTI);
				pTI->ResultFlag=TestResult;//add by Talen 2011/08/30
				//amprintf("Testing......;\n");//DEBUG USE
				char temp[2560]="";
				if (!(pTI->RtyFlag))
				{
					break;
				}
				else if (TestResult)
				{
					//add start by Talen 2011/04/20
					sprintf_s(temp,sizeof(temp),"%s",pTI->Result);
					sprintf_s(pTI->Result,sizeof(pTI->Result),"%s%s",ResultBuf,temp);
					sprintf_s(ResultBuf,sizeof(ResultBuf),"%s,",pTI->Result);
					//add end by Talen 2011/04/20
					//Maxwell 110223
					for(int i=1; i<(RetryLimit-RetryCount); i++ )
					{
						for(int j = 0;j<pTI->ResultNumber;j++)
						{
							strcat_s(pTI->Result,sizeof(pTI->Result),",");
						}	
					}
					//Maxwell 110223
					break;
				}
				else
				{
					amprintf("RTY INDEX:%d\n;",RetryCount);
					sprintf_s(RetryInformation, sizeof(RetryInformation), "%d", RetryCount);
					pTI->RetryItemCount++;

					sprintf_s(temp,sizeof(temp),"%s",pTI->Result);//add by Talen 2011/03/31
					sprintf_s(pTI->Result,sizeof(pTI->Result),"%s%s",ResultBuf,temp);

					sprintf_s(ResultBuf,sizeof(ResultBuf),"%s,",pTI->Result);
				}

			}
			pTI->ResultFlag=TestResult;

			if(!TestResult&&(strcmp(pTI->RetryJumpID,"NULL")!=0)&&(pTI->RetryJumpCount==0))//Add by Maxwell for not show fail when retry 2008/10/20
			{

			}
			else if((2==pTI->ItemProperty)&&(TestResult==1)&&RetestNoShowFlag)//Add by Maxwell for not show information when retry 2008/10/31
			{
				if(TestResult)
				{
				}
				else
				{
					amprintf("ITEM=ID[%d]NAME[%s];\n",ItemTestID,pTI->Name);
					amprintf("ITEM=ID[%d]RESULT[FAIL];\n",ItemTestID);
				}
			}
			//else if((2==pTI->ItemProperty)&&(!TestResult)&&RetestNoShowFlag)//Add by Maxwell for not show information when retry 2009/06/23
			//{
			//	
			//}
			else if (TestResult)//perform the actual test action
			{
				//amprintf("RES;TBAR=1;RES1=%s Pass\n;",pTI->Name);     //Maxwell 090521
				//amprintf("ITEM=ID[%d]NAME[%s];\n",ItemTestID,pTI->Name);
				amprintf("ITEM=ID[%d]RESULT[PASS];\n",ItemTestID);
				ItemTestID++;//Maxwell 090521
			}
			else 
			{
				//amprintf("RES;TBAR=1;RES1=%s Fail\n;",pTI->Name);       //Maxwell 090521
				//amprintf("ITEM=ID[%d]NAME[%s];\n",ItemTestID,pTI->Name);
				amprintf("ITEM=ID[%d]RESULT[FAIL];\n",ItemTestID);
				ItemTestID++;//Maxwell 090521
				gTI.UUTInfo.TotalResultFlag=0;//Talen 2011/08/30
			}

			Time.GetElapseTime(TimeBuf,sizeof(TimeBuf));
			char tempTimeBuf[32]="";
			Time.CostTimeConvert(tempTimeBuf,sizeof(tempTimeBuf),TimeBuf);
			strcat_s(pTI->Result,sizeof(pTI->Result),",");//Talen 2012/03/12
			strcat_s(pTI->Result,sizeof(pTI->Result),tempTimeBuf);//Talen 2012/03/12
			//amprintf("Item[%d] test time:%s;\n",ItemTestID-1,TimeBuf);
			amprintf("Item test time:%s;\n",TimeBuf);
			//Maxwell 090702
			//#define OUTPUTTIME
#ifdef OUTPUTTIME
			CTReport.OutputTimeLogFile("Time.txt",TimeBuf);
#endif
			//Maxwell 090702
			if (pTI->hEndEvent)
			{
				SetEvent(pTI->hEndEvent);
			}
		}

		else if((2==pTI->ItemProperty)&&(pTI->ResultFlag))
		{
			RetestNoShowFlag=1;	
			goto LABLE_ITEM_PROPERTY;
		}
	}
	else
	{
		TestResult = 1;

		amprintf("ITEM=ID[%d]NAME[%s];\n",ItemTestID,pTI->Name);

		for (int runloop=0; runloop<pTI->LoopRunNum; runloop++)
		{
			//add start by Talen 2011/04/12
			memset(pTI->Result,0,sizeof(pTI->Result));
			for(ResultCount=1;ResultCount<=pTI->ResultNumber;ResultCount++)
			{
				if(ResultCount==pTI->ResultNumber)
				{
					strcat_s(pTI->Result,sizeof(pTI->Result),"-999999");
				}
				else
				{
					strcat_s(pTI->Result,sizeof(pTI->Result),"-999999,");
				}
			}
			//add end by Talen 2011/04/12
		

		//if(!ItemTest(pTI)) sam 11/03/11
			if(!ItemTest(pTI)&&(0==runloop))
			{
				TestResult = 0;
			}
			else
			{

			}

			/*sprintf_s(pTI->Result,sizeof(pTI->Result),"%s%s",pTI->Result,ResultBuf);
			sprintf_s(ResultBuf,sizeof(ResultBuf),",%s",pTI->Result);*/
		}

		if(TestResult)
		{
			amprintf("ITEM=ID[%d]RESULT[PASS];\n",ItemTestID);
		}
		else
		{
			gTI.UUTInfo.TotalResultFlag=0;//Talen 2011/08/30
			amprintf("ITEM=ID[%d]RESULT[FAIL];\n",ItemTestID);
		}

		pTI->ResultFlag=TestResult;

		ItemTestID++;

		Time.GetElapseTime(TimeBuf,sizeof(TimeBuf));

		amprintf("Item test time:%s;\n",TimeBuf);

		if (pTI->hEndEvent)
		{
			SetEvent(pTI->hEndEvent);
		}
	}

    return 1;
}
//Maxwell 110310

int InitialTestWork()
{
	int iItemCount=0;
	int MultiTestResult=0;

	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{			
		if (TFLAG_INI_RUN!=(*Cy).Flag)//judge the test flag
		{
			continue;
		}

		//add start by Talen 2011/10/26
		if(0==sfisStartFlag && strlen((*Cy).SFISData))
		{
			continue;
		}
		//add end by Talen 2011/10/26
		//add start by Talen 2011/11/03
		if(!(*Cy).selected)
		{
			continue;
		}
		//add end by Talen 2011/11/03


		ItemTestTask(&(*Cy));
	}
	return 1;
}
int EndTestWork()
{
	int iItemCount=0;
	int MultiTestResult=0;

	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{			
		if (TFLAG_END_RUN!=(*Cy).Flag)//judge the test flag
		{
			continue;
		}
		//add start by Talen 2011/10/26
		if(0==sfisStartFlag && strlen((*Cy).SFISData))
		{
			continue;
		}
		//add end by Talen 2011/10/26
		//add start by Talen 2011/11/03
		if(!(*Cy).selected)
		{
			continue;
		}
		//add end by Talen 2011/11/03

		ItemTestTask(&(*Cy));
	}
	return 1;
}

int AfterTestWork()
{
	int iItemCount=0;
	int MultiTestResult=0;

	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{			
		if (TFLAG_TERMINATE_RUN!=(*Cy).Flag)//judge the test flag
		{
			continue;
		}
		//add start by Talen 2011/10/26
		if(0==sfisStartFlag && strlen((*Cy).SFISData))
		{
			continue;
		}
		//add end by Talen 2011/10/26
		//add start by Talen 2011/11/03
		if(!(*Cy).selected)
		{
			continue;
		}
		//add end by Talen 2011/11/03

		ItemTest(&(*Cy));
	}

	return 1;
}


int LogicCheckJump(char *pCurrentItem, list<TEST_ITEM>::iterator *pJumpCy)
{
	char CurrentID[30];
	char Temp[30]="";
	char JumpID[30]="";
	char *pTag=NULL;
	int JumpIndex=0;
	int iItemCount=0;
	int FindFlag=0;
	if (!pCurrentItem)
	{
		return -1;
	}

	strcpy_s(CurrentID, sizeof(CurrentID), pCurrentItem);


	if (!ParseTestItem(CurrentID,"NULL"))
	{
		return 0;
	}




	strcpy_s(Temp, sizeof(Temp), CurrentID);
	pTag=strchr(Temp,'@');
	if (pTag)
	{
		*pTag=0;
		strcpy_s(JumpID, sizeof(JumpID), Temp);
		JumpIndex=atoi(pTag+1);
	}


	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{			
		if (!ParseTestItem(JumpID,(*Cy).ID))
		{
			iItemCount++;
			if (iItemCount==JumpIndex)
			{
               *pJumpCy=Cy;
			   FindFlag=1;
			   return 1;
			}
			else
			{
				continue;
			}			
		}
	}
	if (!FindFlag)
	{
		return 0;
	}
	return 1;

}

int PerfTest(list<TEST_ITEM>::iterator *pCy)
{
	gThreadExitFlag=0;
	ItemTestID = 1; // Maxwell 090521
	//SItemList.clear(); // Maxwell 090521
	int iItemCount=0;
	IsGolden =true;

	char TimeBuf[50];
	int MultiTestResult=0;
	//	TEST_ITEM TestITTemp;
	gTI.UUTInfo.TotalResultFlag=1;//Talen 2011/08/30
	gTI.UUTInfo.LedBlink=-999999;
	//if(gTI.TestStaInfo.retryNoticeFlag)
	//{
	//	gTI.UUTInfo.retryItemCount=0;//Talen 2012/03/13
	//}
	//else
	//{
	//	gTI.UUTInfo.retryItemCount=-999999;//Talen 2012/03/13
	//}

	CCenTime Time;
	CCenTime TotalTime;

	list<TEST_ITEM>::iterator Cy;
	list<TEST_ITEM>::iterator JumpCy;
//	list<TEST_ITEM>::iterator JumpAfterCy;//Add by Maxwell 2008/10/13

	//amprintf("CMD;CODE=%d;\n", T_START_TEST);//echo the start test command               //Maxwell 090521

	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{			
		(*Cy).ResultFlag=NO_TEST;
		strcpy_s((*Cy).Result,sizeof((*Cy).Result),"-999999");
		(*Cy).RetryJumpCount=0;//reset retry Jump counter
	//	(*Cy).RetryJumpAfterCount=0;//Add by Maxwell 2208/10/13,reset retry jump after counter
		(*Cy).RetryItemCount=0;//reset retry item counter
		//(*Cy).ResultFlag=0;//Add by Maxwell 2208/10/13	//remove by Talen 2011/04/12
		
		if (TFLAG_SKIP!=(*Cy).Flag && (*Cy).selected)//judge the test flag
		{
			if(0==sfisStartFlag && strlen((*Cy).SFISData))
			{
				continue;
			}
			iItemCount++;
		}
	}
	//amprintf("INI;TITEMNUM=%d;\n",iItemCount);// report test item number    //Maxwell 090520
	amprintf("ITEM=NUMBER[%d];\n",iItemCount);
	Time.GetDate(gTI.UUTInfo.TestDate,sizeof(gTI.UUTInfo.TestDate));
	Time.GenCurrentTime(gTI.UUTInfo.TestTime, sizeof(gTI.UUTInfo.TestTime));

	TotalTime.TimeStartCount();


	ResetMultiEvent();//reset all item event
	
	
	InitialTestWork();// do the work before testing
	
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{

LBEL_REPEAT:
		//add start by Talen 2011/10/26
		if(0==sfisStartFlag && strlen((*Cy).SFISData))
		{
			continue;
		}
		//add end by Talen 2011/10/26
		if (TFLAG_SKIP==(*Cy).Flag || 0==(*Cy).selected)//judge the test flag
		{
			continue;
		}
		if (TFLAG_INI_RUN==(*Cy).Flag)//judge the test flag
		{
			continue;
		}
		if (TFLAG_END_RUN==(*Cy).Flag)//judge the test flag
		{
			continue;
		}
		if (TFLAG_TERMINATE_RUN==(*Cy).Flag)//judge the test flag
		{
			continue;
		}

		if ((*Cy).Parallel)
		{	
			if (BeginItemParaRun(ItemTestTask,&(*Cy)))
			{
				amprintf("%s Parallel run\n;",(*Cy).Name);
			}

		}
		else
		{
			WaitMultiEvent(Cy);//wait for multi test finished
			MultiTestResult=0;
			MultiTestResult=JudgeMultiTestResult(Cy);
			if (MultiTestResult)//detect the multi test resutlt
			{
			
				if (LogicCheckJump((*Cy).RetryJumpID, &JumpCy)&&(0==(*Cy).RetryJumpCount))
				{
					(*Cy).RetryJumpCount++;
					Cy=JumpCy;
					tiScript.SetHDSpindownFlag=0;//Added by Maxwell 2008/10/14
					goto LBEL_REPEAT;
				}
/*
				//Add start by Maxwell 2008/10/13				
				if (LogicCheckJump((*Cy).RetryAfterJumpID, &JumpAfterCy)&&(1==(*Cy).RetryJumpCount)&&(0==(*Cy).RetryJumpAfterCount))
				{
					(*Cy).RetryJumpAfterCount++;
					amprintf("2RetryJumpCount %s\n;",(*Cy).RetryJumpCount);
					amprintf("RetryJumpAfterCount %s\n;",(*Cy).RetryJumpAfterCount);
					Cy=JumpAfterCy;
					goto LBEL_REPEAT;
				}
//Add end by Maxwell 2008/10/13
*/

				// test fail	
				switch (MultiTestResult)
				{
				case TFLAG_FAIL_STOP:
					*pCy=Cy;//RETURN THE current test item index
					goto LB_EXIT;
					break;
				case TFLAG_FAIL_CONTINUE:continue;break;
				case TFLAG_FAIL_PAUSE:;break;
				default:;break;
				}
			}
			ItemTestTask(&(*Cy));
			if (TEST_FAIL==(*Cy).ResultFlag)// test result process
			{   

				if (LogicCheckJump((*Cy).RetryJumpID, &JumpCy)&&(0==(*Cy).RetryJumpCount))
				{
					(*Cy).RetryJumpCount++;
					Cy=JumpCy;
					tiScript.SetHDSpindownFlag=0;//Added by Maxwell 2008/10/14
					//(*Cy).RetryJumpCount++;//Add by Maxwell 2008/10/13
					goto LBEL_REPEAT;
				}

/*				//Add start by Maxwell 2008/10/13				
				if (LogicCheckJump((*Cy).RetryAfterJumpID, &JumpAfterCy)&&(1==(*Cy).RetryJumpCount)&&(0==(*Cy).RetryJumpAfterCount))
				{
					(*Cy).RetryJumpCount--;
					(*Cy).RetryJumpAfterCount++;
//					amprintf("2RetryJumpCount %s\n;",(*Cy).RetryJumpCount);
	//				amprintf("RetryJumpAfterCount %s\n;",(*Cy).RetryJumpAfterCount);
					Cy=JumpAfterCy;
					goto LBEL_REPEAT;
				}
//Add end by Maxwell 2008/10/13*/
	
				// test fail	
				switch ((*Cy).Flag)
				{
				case TFLAG_FAIL_STOP:
					*pCy=Cy;//RETURN THE current test item index
					goto LB_EXIT;
					break;
				case TFLAG_FAIL_CONTINUE:continue;break;
				//case TFLAG_FAIL_PAUSE:;break;
					case TFLAG_FAIL_PAUSE:SuspendThread(hTestThread)/*FreezeFlag=1*/;break;//Modified by Maxwell 2008/11/6
				default:;break;
				}	
			}
			else// test pass
			{
/*				//Add start by Maxwell 2008/10/13				
				if (LogicCheckJump((*Cy).RetryAfterJumpID, &JumpAfterCy)&&(1==(*Cy).RetryJumpCount)&&(0==(*Cy).RetryJumpAfterCount))
				{
					(*Cy).RetryJumpAfterCount++;
//					amprintf("2RetryJumpCount %s\n;",(*Cy).RetryJumpCount);
	//				amprintf("RetryJumpAfterCount %s\n;",(*Cy).RetryJumpAfterCount);
					Cy=JumpAfterCy;
					goto LBEL_REPEAT;
				} */
			}
		}
	}
	WaitMultiEvent(TItemList.end());//wait for multi test finished
	*pCy=TItemList.end();//RETURN THE current test item index
LB_EXIT:
	

	//do the test end work,when all item was perform.
    EndTestWork();
	
	
	
	TotalTime.GetElapseTime(TimeBuf,sizeof(TimeBuf));
	strcpy_s(gTI.UUTInfo.TestTimeCost,sizeof(gTI.UUTInfo.TestTimeCost),TimeBuf);
	amprintf("Total test time:%s;\n",TimeBuf);
	gThreadExitFlag=1;
	return 1;    
}


UINT WINAPI AppParaFunc(PVOID pParam)
{

	PARALLEL_PARA *pPA=(PARALLEL_PARA *)pParam;
	(*(pPA->pFunc))(pPA->pTI);

	if (pPA)
	{
		delete pPA;
	}

	return 1;
}

int BeginItemParaRun(ParallelPerfCallFunc pFunc,TEST_ITEM *pTI)
{
	HANDLE hThread=0;
	UINT dwThreadID=0;
	PARALLEL_PARA *pPaa=new PARALLEL_PARA;
	(*pPaa).pFunc=pFunc;
    (*pPaa).pTI=pTI;
	hThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		AppParaFunc,
		pPaa,
		0,
		&dwThreadID 
		);

	if (hThread)
	{

	}
	else
	{
		return 0;
	}
	return 1;
}

int ReportTestResult(list<TEST_ITEM>::iterator StopCy)
{

	char logbuff[10240];
	char titlebuff[10240];
	char TempBuf[100];
	char LogName[100];
	char ErrorCode[512]="";
	char ErrorDescription[2048]="";
	char ErrorTotalNewMyDAS[2048]="";
	char ErrorMydas[1024]="";//Maxwell 100309
	char ErrorCodeMydas[512]="";//Maxwell 100309
	CCenTime Time;
	int TestPassFailCal=0;
	int TestPassFail=0;
	list<TEST_ITEM>::iterator Cy;
	
	CTReport.ClearBUF();
	
	
	CTReport.SetStationName(gTI.TestStaInfo.TestStaName);
	CTReport.SetUUTModelName(gTI.UUTInfo.UUTName);
	
	
	CTReport.HeadInsertStart();
	CTReport.HeadContentInsert(gTI.TestStaInfo.LogFileType);//log file type
	CTReport.HeadContentInsert(gTI.TestStaInfo.Area);//mfg area
	CTReport.HeadContentInsert(gTI.TestStaInfo.ManufactureLine);
	CTReport.HeadContentInsert(gTI.TestStaInfo.Sector);
	CTReport.HeadContentInsert(gTI.TestStaInfo.Post);
	CTReport.HeadContentInsert(gTI.TestStaInfo.TestStaName);
	CTReport.HeadContentInsert(gTI.TestStaInfo.Operator);	
	CTReport.HeadContentInsert(gTI.UUTInfo.TestDate);
	CTReport.HeadContentInsert(gTI.UUTInfo.TestTime);

		
	CTReport.FileNameInsertStart();
	CTReport.FileNameInsert(gTI.TestStaInfo.TestStaName);
    Time.DataFormatC(TempBuf,sizeof(TempBuf),gTI.UUTInfo.TestDate);
	CTReport.FileNameInsert(TempBuf);
	strcpy_s(LogName,sizeof(LogName),TempBuf);
	strcat_s(LogName,sizeof(LogName),".csv");

	CTReport.FileNameInsert(gTI.UUTInfo.TestTime);
	CTReport.FileNameInsert(gTI.TestStaInfo.Post);

	//Calculate the test pass/fail
	
	if (TItemList.empty())
	{
        TestPassFailCal++;
		amprintf("Warning!No item being tested;\n");	
	}

//Add start by Maxwell 2008/11/18 for errorcode when flag=1
	int ErrorCodeCount = 0;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{		
		if ((TEST_FAIL==(*Cy).ResultFlag) && ((*Cy).Flag == 1) && (ErrorCodeCount == 0))
		{
			char ErrorTemp[2048]="";
			TestPassFailCal++;
			strcat_s(ErrorCode,sizeof(ErrorCode),(*Cy).ErrorCode);

			strcat_s(ErrorDescription,sizeof(ErrorDescription),(*Cy).ErrorDes);
			//Maxwell 100420
			if(!strcmp((*Cy).ID,"GET_RESULT_TXT"))
			{
				sprintf_s(ErrorTemp, "%s",(*Cy).ErrorDes);
			}
			else if(!strcmp((*Cy).ID,"GET_IQ_TRESULT"))
			{
				sprintf_s(ErrorTemp, "%s, ,|",(*Cy).ErrorCode);
			}
			else
			{
				sprintf_s(ErrorTemp, "%s,%s,%s|",(*Cy).ErrorCode,(*Cy).ErrorDes,(*Cy).Result);
			}
			//Maxwell 100420
			strcat_s(ErrorTotalNewMyDAS,sizeof(ErrorTotalNewMyDAS),ErrorTemp);
			ErrorCodeCount++;
		}	
		else if ((TEST_FAIL==(*Cy).ResultFlag) && ((*Cy).Flag > 0) && ((*Cy).Flag != 1))
		{
			char ErrorTemp[2048]="";
			TestPassFailCal++;
			if(strlen(ErrorCode)<100)//add by Talen 2011/04/03
			{
				strcat_s(ErrorCode,sizeof(ErrorCode),(*Cy).ErrorCode);
			}
			if(strlen(ErrorDescription)<1024)//add by Talen 2011/04/03
			{
				strcat_s(ErrorDescription,sizeof(ErrorDescription),(*Cy).ErrorDes);
			}
			//Maxwell 100420
			if(!strcmp((*Cy).ID,"GET_RESULT_TXT"))
			{
				sprintf_s(ErrorTemp, "%s",(*Cy).ErrorDes);
			}
			else if(!strcmp((*Cy).ID,"GET_IQ_TRESULT"))
			{
				sprintf_s(ErrorTemp, "%s, ,|",(*Cy).ErrorCode);
			}
			else
			{
				sprintf_s(ErrorTemp, "%s,%s,%s|",(*Cy).ErrorCode,(*Cy).ErrorDes,(*Cy).Result);
			}
			//Maxwell 100420
			if(strlen(ErrorTotalNewMyDAS)<1024)//add by Talen 2011/04/03
			{
				strcat_s(ErrorTotalNewMyDAS,sizeof(ErrorTotalNewMyDAS),ErrorTemp);
			}
		}	
	}
//Add end by Maxwell 2008/11/18 for errorcode when flag=1


	if (TestPassFailCal)
	{ 
		
		ZeroMemory(ErrorCodeMydas,sizeof(ErrorCodeMydas));//Process for Mydas system,the maxmum length is 128 
        memcpy_s(ErrorCodeMydas,sizeof(ErrorCodeMydas),ErrorCode,sizeof(ErrorCodeMydas)-1);
		//CTReport.InputResult("ErrorCode","%s,",ErrorCodeMydas);
		
		ZeroMemory(ErrorMydas,sizeof(ErrorMydas));//Process for Mydas system,the maxmum length is 128 
		memcpy_s(ErrorMydas,sizeof(ErrorMydas),ErrorDescription,sizeof(ErrorMydas)-1);
		//CTReport.InputResult("Error_Desc","%s,",ErrorMydas);
	}
	else
	{

	}


		//////////////////////////maxwell 11224
	char ServerLogPath[512];
	char LogPathTemp[512];
	int err;
	FILE *fLog=NULL;
	SYSTEMTIME timeForTitle;
	GetLocalTime(&timeForTitle);
	int LogFilePathFlag=0;//add by Talen 2011/04/18
	char result_filename[512]="";

	int tryCount=0;
	
	//add start by Talen 2011/04/18
	if(!strcmp(gTI.TestStaInfo.TestLogPath,"No log file path config!"))
	{
		LogFilePathFlag=0;
	}
	else
	{
		LogFilePathFlag=1;
	}
	//add end by Talen 2011/04/18


	if(!LogFilePathFlag)//add by Talen 2011/04/18
	{
		sprintf(ServerLogPath,"%s",gTI.TestStaInfo.TestLogPath);
	}
	else
	{
		sprintf(ServerLogPath,"%s\\%s_%s",gTI.TestStaInfo.TestLogPath,gTI.UUTInfo.UUTPN,gTI.TestStaInfo.TestStaName);

		if(!PathFileExists(ServerLogPath))
		{	
			tryCount=0;
			while(tryCount<3)
			{
				if(CreateDirectory(ServerLogPath,NULL))
				{
					break;
				}
				if(tryCount>=2)
				{
					amprintf("Create Detail Log Data Folder 1 Fail!\n");
					return 0;
				}
				Sleep(10);
				tryCount++;
				
			}
		}
		//change start by LiYongjun 2011/03/18 (%d -> %2d)
		sprintf(LogPathTemp,"%04d%02d%02d",timeForTitle.wYear,timeForTitle.wMonth,timeForTitle.wDay);
		//change end by LiYongjun 2011/03/18 (%d -> %2d)
		strcat_s(ServerLogPath,sizeof(ServerLogPath),"\\");
		strcat_s(ServerLogPath,sizeof(ServerLogPath),LogPathTemp);

		if(!PathFileExists(ServerLogPath))
		{	
			tryCount=0;
			while(tryCount<3)
			{
				if(CreateDirectory(ServerLogPath,NULL))
				{
					break;
				}
				if(tryCount>=2)
				{
					amprintf("Create Detail Log Data Folder 2 Fail!\n");
					return 0;
				}
				Sleep(10);
				tryCount++;
				
			}
		}

		//char result_filename[512]="";
		sprintf_s(result_filename,sizeof(result_filename),"%s\\%s_%04d%02d%02d.csv",ServerLogPath,gTI.TestStaInfo.TesterPCName,timeForTitle.wYear,timeForTitle.wMonth,timeForTitle.wDay);



		strcat_s(ServerLogPath,sizeof(ServerLogPath),"\\");
		strcat_s(ServerLogPath,sizeof(ServerLogPath),gTI.TestStaInfo.TesterPCName);

		if(!PathFileExists(ServerLogPath))
		{	
			tryCount=0;
			while(tryCount<3)
			{
				if(CreateDirectory(ServerLogPath,NULL))
				{
					break;
				}
				if(tryCount>=2)
				{
					amprintf("Create Detail Log Data Folder 3 Fail!\n");
					return 0;
				}
				Sleep(10);
				tryCount++;
				
			}
		}

		strcat_s(ServerLogPath,sizeof(ServerLogPath),"\\");
		strcat_s(ServerLogPath,sizeof(ServerLogPath),gTI.UUTInfo.UUTSN);
		strcat_s(ServerLogPath,sizeof(ServerLogPath),"_");
		//change by LiYongjun 2011/03/18 (%d -> %2d)
		sprintf(LogPathTemp,"%02d%02d%02d",timeForTitle.wHour,timeForTitle.wMinute,timeForTitle.wSecond);
		//change by LiYongjun 2011/03/18 (%d -> %2d)
		strcat_s(ServerLogPath,sizeof(ServerLogPath),LogPathTemp);
		///MAXWELL 11224
	}
		

	char loglogbuff[10240]="";

	if(LogFilePathFlag)
	{
		if(TestPassFailCal)
		{
			strcat_s(ServerLogPath,sizeof(ServerLogPath),"_Fail.txt");
		}
		else
		{
			strcat_s(ServerLogPath,sizeof(ServerLogPath),"_Pass.txt");
		}
	}


	CTReport.InputResult("SFIS","%s,",SFIS.SSFISMode);
	CTReport.InputResult("ERROR_CODE","%s,",ErrorCode);
	CTReport.InputResult("OPID","%s,",gTI.TestStaInfo.opID);
	if(strstr(gTI.TestStaInfo.TestStaName,"MPT1")||strstr(gTI.TestStaInfo.TestStaName,"MPT3"))//add by Talen 2011/08/00
	{
		CTReport.InputResult("FIX_ID","%d,",gTI.TestStaInfo.fixID);
	}
	CTReport.InputResult("CHEK_LED_BLINK","%d,",gTI.UUTInfo.LedBlink);
	//if(!gTI.TestStaInfo.retryNoticeFlag)
	//{
	//	gTI.UUTInfo.retryItemCount=-999999;
	//}
	//CTReport.InputResult("RETRY_ITEM_COUNT","%d,",gTI.UUTInfo.retryItemCount);//Talen 2012/03/13
	//Maxwell modify for data log 2011/02/23
	char totalTitle[1024]="";
	char tempTitle[1024]="";
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
	{

		if (strlen((*Cy).ResultType)&&(*Cy).ResultFlag!=NO_TEST)	//add "(*Cy).ResultFlag!=NO_TEST" by Talen 2011/04/12
		{
			if ((*Cy).RetryItemCount>0)//record the item retry flag
			{
				//strcat_s((*Cy).Result, sizeof((*Cy).Result), ",*");
			}

			if ((*Cy).RetryJumpCount>0)//record the jump retry flag
			{
				//strcat_s((*Cy).Result, sizeof((*Cy).Result), ",#");
			}
			if((*Cy).RtyFlag)//add by Talen for write log title
			{
				memset(totalTitle,0,sizeof(totalTitle));
				CTReport.makeItemTitle(totalTitle,(*Cy).ResultType,(*Cy).RtyFlag);
				//strcat_s(totalTitle,sizeof(totalTitle),",RETY_STAT");//add by Talen 2012/02/21
				strcat_s(totalTitle,sizeof(totalTitle),",ITEM_TEST_TIME");//Talen 2012/03/13
				CTReport.InputResult(totalTitle,"%s,",(*Cy).Result);
			}
			else
			{
				//CTReport.InputResult((*Cy).ResultType,"%s,",(*Cy).Result);
				strcpy_s(tempTitle,sizeof(tempTitle),(*Cy).ResultType);//Talen 2012/03/13
				strcat_s(tempTitle,sizeof(tempTitle),",ITEM_TEST_TIME");//Talen 2012/03/13
				CTReport.InputResult(tempTitle,"%s,",(*Cy).Result);//Talen 2012/03/13

			}
		}

	}
	//Maxwell modify for data log 2011/02/23

	strcpy_s(titlebuff,sizeof(titlebuff),"SN,TEST_RESULT,LOG_PATH,DIAG_VERSION,PC_NAME,CYCLE_TIME,TEST_DATE&TIME,MAC1,MAC2,");

	char logTitleBuff[10240]="";
	memset(logTitleBuff,0,sizeof(logTitleBuff));
	CTReport.OutputAllResult(logbuff, sizeof(logbuff),logTitleBuff,sizeof(logTitleBuff));

	strcat_s(titlebuff,sizeof(titlebuff),logTitleBuff);



	if(TestPassFailCal)
	{
		amprintf("LOG=MAINFOR[%s,0,%s,%s,%s,%s,%s %s,%s,%s,];\n",gTI.UUTInfo.UUTSN,ServerLogPath,gTI.TestStaInfo.TestProgramVersion,gTI.TestStaInfo.TesterPCName,gTI.UUTInfo.TestTimeCost,gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,gTI.UUTInfo.UUTMAC1,gTI.UUTInfo.UUTMAC2);
		sprintf_s(loglogbuff, sizeof(loglogbuff), "%s,0,%s,%s,%s,%s,%s %s,%s,%s,",gTI.UUTInfo.UUTSN,ServerLogPath,gTI.TestStaInfo.TestProgramVersion,gTI.TestStaInfo.TesterPCName,gTI.UUTInfo.TestTimeCost,gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,gTI.UUTInfo.UUTMAC1,gTI.UUTInfo.UUTMAC2);
	}
	else
	{
		amprintf("LOG=MAINFOR[%s,1,%s,%s,%s,%s,%s %s,%s,%s,];\n",gTI.UUTInfo.UUTSN,ServerLogPath,gTI.TestStaInfo.TestProgramVersion,gTI.TestStaInfo.TesterPCName,gTI.UUTInfo.TestTimeCost,gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,gTI.UUTInfo.UUTMAC1,gTI.UUTInfo.UUTMAC2);
	
		sprintf_s(loglogbuff, sizeof(loglogbuff), "%s,1,%s,%s,%s,%s,%s %s,%s,%s,",gTI.UUTInfo.UUTSN,ServerLogPath,gTI.TestStaInfo.TestProgramVersion,gTI.TestStaInfo.TesterPCName,gTI.UUTInfo.TestTimeCost,gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,gTI.UUTInfo.UUTMAC1,gTI.UUTInfo.UUTMAC2);
		//Justin for Golden Select
		if(gTI.TestStaInfo.GoldenSelect)
		{
			if(IsGolden)
			{
				amprintf("MSGBOX=CONTENT[Golden Samples!Please keep it﹗請留下Golden卡];\n");
				amprintf("Golden card %d;\n",IsGolden);
				DWORD dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
				if (!ResetEvent(gTI.UiResMsg.hEvent)) 
				{ 
					printf("ResetEvent failed (%d)\n", GetLastError());
				}

			}
		}
	}

	
	amprintf("LOG=CONTENT[%s,%s,%s];\n",gTI.UUTInfo.TestDate,gTI.UUTInfo.TestTime,logbuff);

	if(strlen(ErrorTotalNewMyDAS))
	{
		//Maxwell 110215
		if(1024>strlen(ErrorTotalNewMyDAS))
		{
			amprintf("LOG=ERROR[%s];\n",ErrorTotalNewMyDAS);
		}
		else
		{
			amprintf("LOG=ERROR[Too many errors to parse!];\n");
		}
		//Maxwell 110215
	}
	else
	{
		amprintf("LOG=ERROR[ ];\n");
	}
	strcat_s(loglogbuff,sizeof(loglogbuff),logbuff);

	CTReport.OutputLogFile(LogName,titlebuff,loglogbuff);

	//CTReport.OutputTMMLogFile(logbuff, sizeof(logbuff));
	CTReport.ClearBUF();		

	//CTReport.WriteTMMLogFile(logbuff);

	//Maxwell 090521

	char SFISTemp[512]="";
	char SFISToUI[512]="";
	list<SFIS_ITEM>::iterator Csy;

	for (Csy=SItemList.begin(); Csy!=SItemList.end();Csy++)
	{
		sprintf_s(SFISTemp,sizeof(SFISTemp),"%s[%s]",(*Csy).Item,(*Csy).Data);
		strcat_s(SFISToUI,sizeof(SFISToUI),SFISTemp);
	}

	if (TestPassFailCal)
	{
	//Maxwell 090521

		TestPassFail=0;
		//add start by Talen 2011/08/04
		char *ppTemp=ErrorCode;
		int errFlag=0;
		while(*ppTemp!=0)
		{
			if(strncmp(ppTemp,"NF",2))
			{
				errFlag=1;//not NF fail
				break;
			}
			ppTemp+=4;
		}
		if(errFlag==0 && strstr(ErrorCode,"NF"))
		{
			memset(ErrorCode,0,sizeof(ErrorCode));
			strcpy(ErrorCode,"NFTO");
		}
		//add end by Talen 2011/08/04
		amprintf("SFIS=%sERRORCODE[%s]RESULT[FAIL];CONTROL=RESULT[FAIL];\n",SFISToUI,ErrorCode);
		

		/**********************************************************************/
		//Add by Sam 2011/03/30
		if(gTI.TestStaInfo.Control_Board_Para.ConnectFlag&&gTI.TestStaInfo.Control_Board_Para.LedShowFlag)
		{
			LEDShowFailStatus();
		}
		/**********************************************************************/
		//SFISToUI
		//amprintf("STS;ERRCODE=%s;",ErrorCode);	 //Maxwel  090521
		//amprintf("STS;STS1=fail;");
	}
	else
	{
		TestPassFail=1;
		DWORD dwWaitResult;
		//amprintf("DAT;ITEM1=%s;ITEM2=%s;ITEM4=%s;\n;", gTI.UUTInfo.UUTMAC1, gTI.UUTInfo.UUTMAC2, gTI.UUTInfo.UUTMAC3);
		amprintf("SFIS=%sRESULT[PASS];CONTROL=RESULT[PASS];\n",SFISToUI);	
		memset(gTI.TestStaInfo.errorCode,0,sizeof(gTI.TestStaInfo.errorCode));//add by talen 2011/11/10
		gTI.TestStaInfo.errorCodeCount=0;//add by Talen 2011/11/10
		/**********************************************************************/
		//Add by Sam 2011/03/30
		if(gTI.TestStaInfo.Control_Board_Para.ConnectFlag&&gTI.TestStaInfo.Control_Board_Para.LedShowFlag)
		{
			LEDShowPassStatus();
		}
		/**********************************************************************/
	}	

	//Clear the test input informations
	strcpy_s(gTI.UUTInfo.TestDate,sizeof(gTI.UUTInfo.TestDate),"");
	strcpy_s(gTI.UUTInfo.TestTimeCost,sizeof(gTI.UUTInfo.TestTimeCost),"");
	strcpy_s(gTI.UUTInfo.TestTime,sizeof(gTI.UUTInfo.TestTime),"");
	strcpy_s(gTI.UUTInfo.UUTSN,sizeof(gTI.UUTInfo.UUTSN),"");
	strcpy_s(gTI.UUTInfo.UUTMAC1,sizeof(gTI.UUTInfo.UUTMAC1),"");
	strcpy_s(gTI.UUTInfo.UUTMAC2,sizeof(gTI.UUTInfo.UUTMAC2),"");
	strcpy_s(gTI.UUTInfo.UUTMAC3,sizeof(gTI.UUTInfo.UUTMAC3),"");
	gTI.UUTInfo.TotalResultFlag=1;//Talen 2011/08/30
	ClearSFISBufSpec();

	//Maxwell 090521
	SItemList.clear();

	if(LogFilePathFlag)//add by Talen 2011/04/18
	{
		//Add start by Maxwell 2008/11/12 for operation log on TE Server
		WaitForSingleObject(hWriteServerLogMutex, INFINITE); //add by Talen 2011/09/01
		Sleep(1);
		if(err = fopen_s(&fLog, ServerLogPath, "a+")==0)
		{
			if(NULL == fLog)
			{
				return FALSE;
			}
			fprintf_s(fLog,"%s\n",DetailLogBuffer);
			fclose(fLog);
		}
		else
		{
			amprintf("Detail write to data server fail!\n");
		}////
		////Add end by Maxwell 2008/11/12 for operation log on TE Server

		//add start by LiYongjun 2011/03/18  for write log to server
		int serverLogExistFlag = PathFileExists(result_filename);
		if(err = fopen_s(&fLog,result_filename, "a+")==0)
		{
			if(NULL == fLog)
			{
				return FALSE;
			}
			if(!serverLogExistFlag)
			{
				fprintf_s(fLog,"%s\n",titlebuff);
			}
			fprintf_s(fLog,"%s\n",loglogbuff);
			fclose(fLog);
		}
		else
		{
			amprintf("log write to data server fail!\n");
		}//add end by LiYongjun 2011/03/18  for write log to server
		ReleaseMutex(hWriteServerLogMutex);//add by Talen 2011/09/01
	}


	//add start by LiYongjun 2011/03/18 for debug
	//amprintf("\n\nSFISMode:%d\n\n",SFIS.SFISMode);
	//add end by LiYongjun 2011/03/18 for debug

	/////////////////////////////////////////////////////////////////////////////////////////////


	//add start by Talen 2011/11/10
	if(strncmp(gTI.TestStaInfo.errorCode,ErrorCode,4))
	{
		strncpy_s(gTI.TestStaInfo.errorCode,sizeof(gTI.TestStaInfo.errorCode),ErrorCode,4);
		gTI.TestStaInfo.errorCodeCount=1;
	}
	else
	{
		gTI.TestStaInfo.errorCodeCount++;
	}
	if(gTI.TestStaInfo.errorCodeCount>=gTI.TestStaInfo.errLimitTimes)
	{
		amprintf("MSGBOX=NOTICE[%s appears %d times continually, Plaease check!];\n",gTI.TestStaInfo.errorCode,gTI.TestStaInfo.errorCodeCount);
		//DWORD dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		//if (!ResetEvent(gTI.UiResMsg.hEvent)) 
		//{ 
		//	printf("ResetEvent failed (%d)\n", GetLastError());
		//}
	}
	//add end by Talen 2011/11/10
	
	strcpy_s(DetailLogBuffer,sizeof(DetailLogBuffer),"");//Maxwell 11228
	//if (!TestPassFailCal && gTI.UUTInfo.retryItemCount && gTI.TestStaInfo.retryNoticeFlag)
	//{
	//	amprintf("MSGBOX=NOTICE[Put the DUT right side!];");
	//}

	return 1;
}
