#include "StdAfx.h"
#include "SfisLogicControl.h"
#include "AmSfisDlg.h"
#include "SfisSelector.h"
#include "AmThreadAdmin.h"
#include "AmbitProT.h"
#include "SFISDefine.h"

extern HANDLE gACKEvent[50];
UINT WINAPI SFISTask(LPVOID lpPara);
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store

extern CAmSfisDlg *pSfisDlgDlg;
extern SFIS_VARIABLE s_variable;//Store global information for all test
char SwitchControlBuf[50][500]={0};
extern CSfisLogicControl SfisLogicControl;

int FixtureFlag=0;//the fixture number must>0
int FixtureNum=0;

CSfisLogicControl::CSfisLogicControl(void)
{
	for(int index=0;index<SWITCH_NUM;index++)
	{
		memset(*(SwitchControlBuf+index),0,sizeof(*(SwitchControlBuf+index)));
	}
	//StartSFISTask();
}

CSfisLogicControl::~CSfisLogicControl(void)
{
}


int CSfisLogicControl::StartSFISTask(void) 
{

	AM_TASK_INFO TaskTemp;

	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		SFISTask,
		this,
		0,
		&TaskTemp.uiTaskID
		);

	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=SFIS_TASK;         //generate task id
		gUITaskInfoList.push_back(TaskTemp);//Insert the task information to mamagement list
	}
	else
	{
		return 0;
	}
	return 1;
}

UINT WINAPI SFISTask(LPVOID lpPara)
{
	
	MSG msg;
	int i=0;
	size_t ConvertedChars=0;
	char TempBuf[1024]="";
	wchar_t RevBuf[1024]= _T("");
	UINT DataLen=0;
	int bufL=0;
	
	while(TRUE)
	{
		if (!WaitMessage())
		{
			continue;
		}
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_SFIS_DATA:
				DataLen=(UINT)msg.lParam;
				memset(TempBuf,0,sizeof(TempBuf));
				memcpy_s(TempBuf, sizeof(TempBuf),(char *)msg.wParam,DataLen);
				if ((char *)msg.wParam)              // clear the buffer
				{
					free((char *)msg.wParam);
				}
				pSfisDlgDlg->ParseUIData(TempBuf,DataLen);
				break;
			case WM_DCT_DATA:
				DataLen=(UINT)msg.lParam;
				memset(TempBuf,0,sizeof(TempBuf));
				memcpy_s(TempBuf, sizeof(TempBuf),(char *)msg.wParam,DataLen);
				if ((char *)msg.wParam)              // clear the buffer
				{
					free((char *)msg.wParam);
				}
				SfisLogicControl.ClearTestBuf(TempBuf);
				break;
			case WM_EXIT:
				//pSfisDlgDlg->OnClose();
				goto ExitLabel;
				break;
			default:
				;
			}
		}
	}

ExitLabel:
	return 0;
}


int CSfisLogicControl::SendUIData(wchar_t *pBuf,int flag)
{
	int switchFlag=0;
	wchar_t TempBuf[1024]=_T("");
	char *pRecordData=0;
	size_t ConvertedChars=0;
	wcscpy_s(TempBuf,sizeof(TempBuf)/2,pBuf);
	size_t SizeLen= wcslen(TempBuf);
	if(SizeLen>500)
	{
		AfxMessageBox(_T("The sfis data is too long"));
		return 0;
	}
	if(gVariable.Repeat_Time_Flag)
	{
		return 0;
	}
	pRecordData = (char *)malloc(SizeLen+1);
	wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, TempBuf, SizeLen);

	char aBuf[2000]="";
	strcpy_s(aBuf,sizeof(aBuf),pRecordData);

	if(SFIS_START==flag)
	{
		if(0==s_variable.ModeOptionFlag)
		{
			switchFlag=0;
		}
		else if(1==s_variable.ModeOptionFlag)
		{
			switchFlag=FixtureFlag-1;
			FixtureFlag=0;
		}
		else
		{
		}

		if(switchFlag>=0&&switchFlag<SWITCH_NUM)
		{
			if(!strlen(*(SwitchControlBuf+switchFlag)))
			{
				strcpy_s(*(SwitchControlBuf+switchFlag),sizeof(*(SwitchControlBuf+switchFlag)),aBuf);
				AmPostThreadMes(TEST_TASK_TYPE+switchFlag, WM_SFIS_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
			}
			else
			{
				return 0;
			}
		}
		else
		{
		}		
	}
	else if(SFIS_RESULT==flag)
	{
		int sfisFlag=-1;

		sfisFlag=GetTestTaskID(pRecordData,SizeLen);
		if(sfisFlag>=0&&sfisFlag<SWITCH_NUM)
		{
			memset(*(SwitchControlBuf+sfisFlag),0,sizeof(*(SwitchControlBuf+sfisFlag)));
			SetEvent(gACKEvent[sfisFlag+1]);
		}
		//haibin.li 2011/11/07 to avoid memory leak
		if (pRecordData)
		{
			free(pRecordData);
		}
		//haibin.li 2011/11/07
	}
	else if(SFIS_LINK_STATUS==flag)
	{
		AmPostThreadMes(TEST_TASK_TYPE+0, WM_SFIS_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
	}
	else if(DCT_DATA==flag)
	{
		AmPostThreadMes(MAIN_CONTROL_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
		for(int index=0;index<gStationInfo.MultiTaskSetting.Number;index++)
		{
			AmPostThreadMes(TEST_TASK_TYPE+index, WM_DCT_DATA, 0, 0);
		}		
	}
	
	return 1;
}

int CSfisLogicControl::GetTestTaskID(char *pBuf,int SizeLen)
{
	int index=0;
	char Buf[1024]="";
	char *pStart=NULL;
	char *pEnd=NULL;
	pStart=strstr(pBuf,"[");
	if(pStart)
	{
		pStart=pStart+1;
		if(']'==*(pStart+index))
			return -1;
		for(index=0;index<500;index++)
		{
			if (']'==*(pStart+index))
			{
				Buf[index]='\0';
				break;
			}
			else
			{
				Buf[index]=*(pStart+index);
			}
		}
		if(!strlen(Buf))
		{
			return -1;
		}

		for(index=0;index<SWITCH_NUM;index++)
		{
			if(strstr(*(SwitchControlBuf+index),Buf))
			{
				return index;
			}
		}
		return -1;
	}
	else
	{
		return -1;
	}
}


int CSfisLogicControl::StartTestTask(char *pBuf)
{

	char *pStart=NULL;
	char TempBuf[20]="";
	char TempChar[100]="";
	char BarCode[20]="";
	if(!(pBuf||s_variable.BarCode))
	{
		return 0;
	}
	strcpy_s(TempChar,sizeof(TempChar),pBuf);
	_strupr_s(TempChar,strlen(TempChar)+1);	
	size_t ConvertedChars=0;
	size_t SizeLen= wcslen(s_variable.BarCode);
	wcstombs_s(&ConvertedChars, BarCode,SizeLen+1, s_variable.BarCode, SizeLen);
	_strupr_s(TempChar,strlen(TempChar)+1);
	pStart=strstr(TempChar,BarCode);
	if(!pStart)
	{
		return 0;
	}
	else
	{
		size_t ulSize=0;
		ulSize=strlen(BarCode);
		int index=0;
		while(TempChar[ulSize+index])
		{
			TempBuf[index]=TempChar[ulSize+index];
			index++;
		}
		FixtureFlag=atoi(TempBuf);
		FixtureNum=FixtureFlag;
		if(!(FixtureFlag&&s_variable.ModeOptionFlag))
		{
			return 0;
		}

		/*if(FixtureFlag>gStationInfo.MultiTaskSetting.Number)
		{
			AfxMessageBox(_T("The PORT number is error"));
			return 0;
		}*/
		if(FixtureFlag>2)
		{
			AfxMessageBox(_T("The PORT number is error"));
			return 0;
		}
		gVariable.UutScannerFlag=0;
		char pData[128]="SCANNER=0;";
		size_t CLen=strlen(pData);
		AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_SCAN, (unsigned int)pData, (unsigned int)CLen);
		//SendUIData(pSfisDlgDlg->SFISInformation,SFIS_START);
	    //char  FixtureID[32]="";
		//sprintf(FixtureID,"FIXTURE_ID=[%d]",FixtureFlag);

		//size_t cLen;
		//wchar_t wData[2048]=_T("");
		//mbstowcs_s(&cLen,wData,sizeof(wData)/2,FixtureID,strlen(FixtureID));
		//wcscat_s(pSfisDlgDlg->SFISInformation,sizeof(pSfisDlgDlg->SFISInformation)/2,wData);
		SendUIData(pSfisDlgDlg->SFISInformation,SFIS_START);
		wmemset(pSfisDlgDlg->SFISInformation,0,sizeof(pSfisDlgDlg->SFISInformation)/2);
	}
	return 0;
}

void CSfisLogicControl::ClearSfisBuf(int flag)
{
	memset((*(SwitchControlBuf+flag)),0,sizeof(*(SwitchControlBuf+flag)));
}

void CSfisLogicControl::ClearTestBuf(char *pBuf)
{
	int flag=-1;
	flag=GetTestTaskID(pBuf,sizeof(pBuf));
	if(-1!=flag)
	{
		memset((*(SwitchControlBuf+flag)),0,sizeof(*(SwitchControlBuf+flag)));
	}
	else
	{
		return;
	}
}