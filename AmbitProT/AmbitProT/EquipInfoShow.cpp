#include "StdAfx.h"
#include "EquipInfoShow.h"
#include "AmThreadAdmin.h"
#include "list"
#include "LCC.h"
#include "AmParser.h"

extern CEquipInfoShow gEQIP;
HANDLE hEventRev=NULL;
HANDLE hDev=NULL;
EQUIP_USE gServerInfo;
list<EQUIP_USE> DeviceList[50];//090612
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
extern AM_PROT_INFO gStationInfo;



int __stdcall RevOn(char *p,unsigned int I)
{
	char recbuf[1024]="";
	strcpy(recbuf,p);
	gEQIP.ParsePtsData(recbuf);
	return 1;

}


CEquipInfoShow::CEquipInfoShow(void)
{
	m_Font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Fixedsys"));                 

	//strcpy_s(ServerInfo.ClientNum, sizeof(ServerInfo.ClientNum),"0");
	//strcpy_s(ServerInfo.HostName, sizeof(ServerInfo.HostName),"Barcode Server");
	//strcpy_s(ServerInfo.IP,sizeof(ServerInfo.IP),"0.0.0.0");
}

CEquipInfoShow::~CEquipInfoShow(void)
{
	//if(hDev)
	//{
	//	LCC_Disconnect(hDev);
	//}
	if(hEventRev)
	{
		CloseHandle(hEventRev);
	}
}

int CEquipInfoShow::ShowInfomation(CString temp)
{	               
	SetFont(&m_Font); 
	strTemp=temp;
    //SetWindowText(strTemp);//haibin.li 2012/3/20

	return 0;
}

int CEquipInfoShow::EquipShowInfoXml()
{
	list<EQUIP_USE>::iterator Cy;
	wchar_t Temp[100]=_T("");
	wchar_t p[10000]=_T("");
	CString ShowBUF;
	memset(p,0,sizeof(p));
	int BufL=10000;
	char szHostName[256]="";
	char *pTemp,*pTemp2;
	char tempHostName[256]="";
	wchar_t fullName[256]=_T("");
	wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
	//Add start by Maxwell 2008/11/12 for operation log on TE Server
	wchar_t FileName[256]=_T("");
	//errno_t err;
	//wcscpy_s(FileName,sizeof(FileName)/2,gStationInfo.EquipInfo.EqipOperPath);
	//wcscat_s(FileName, sizeof(FileName)/2, _T("EquipmentOperation.txt"));
	//FILE *fLog;
	//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
	//{	
	//	fclose(fLog);
	//}
	//else
	//{
	//	equipfileflag=1;
	//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
	//}

	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode==0)
	{
		if(pTemp=strstr(szHostName,"-"))
		{
			if(pTemp2=strstr(pTemp+1,"-"))
			{
				strcpy_s(tempHostName,sizeof(tempHostName),pTemp2+1);
				strcat(tempHostName,".xml");
				wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
				size_t ConvertedChars=0;
				wchar_t TempHostName[256]=_T("");
				mbstowcs_s(&ConvertedChars, TempHostName, sizeof(TempHostName)/2, tempHostName,strlen(tempHostName));
				wcscat(fullName,TempHostName);
			}
		}
	}
	CString  temp;

	size_t ConvertedChars=0;
	//wcstombs_s(&ConvertedChars, fullName, sizeof(fullName), temp,wcslen(temp));

	temp.Format(_T("%s"), fullName);
	/*temp.Format("%s", gSfisData.Equip_LOG_FILE_PATH);*/
	Tool.m_path = temp;
	/*int flag=*/
	//Tool.GetNum();
	CString ShowTemp(" ");
	if(!Tool.GetNum())//add 2008/11/1
	{		    
		//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_ICONGREEN));
		ShowTemp="The equipment usage file doesn't exist";
		ShowInfomation(ShowTemp);
		equipfileflag=1;
		AfxMessageBox(_T("The equipment usage file doesn't exist\nCan not manage usage status!"));
	}
	else
	{
		CString stemp;
		double timetemp;
		//090612,090616
		int iDev=0;
		//while(wcscmp(DeviceList[iDev].begin()->FixCountName,_T("")))
		while(!DeviceList[iDev].empty())
		{
			//swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment In Fixture %d*****************\r\n"),iDev+1);
			swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
			ShowTemp+=Temp;

			for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
			{
				swprintf_s(Temp,sizeof(Temp)/2,_T("     --------------------------------------------\r\n"));
				wcscat_s(p,sizeof(p)/2,Temp);		
				swprintf_s(Temp,sizeof(Temp)/2,_T("     Equipment:%s\r\n"),(*Cy).EquipmentName);
				wcscat_s(p,sizeof(p)/2,Temp);
				swprintf_s(Temp,sizeof(Temp)/2,_T("          Current Used Time:             %d\r\n"),(*Cy).Usedtime);
				wcscat_s(p,sizeof(p)/2,Temp);
				swprintf_s(Temp,sizeof(Temp)/2,_T("          Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
				wcscat_s(p,sizeof(p)/2,Temp);
				ShowBUF.Format(_T("%s"), p);
				timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;

				if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
				{
					if((*Cy).Usedtime>=(*Cy).Maxusetime)
					{
						//ShowTemp+=_T("     --------------------------------------------\r\n");
						//stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
						//ShowTemp+=stemp;
						//ShowTemp+=_T(" has been used out of time\r\n");
						//char pData[1024]="";
						//SYSTEMTIME	SystemTime;
						//char szMsg[256]="";
						//GetLocalTime(&SystemTime);
						//sprintf_s(szMsg, sizeof(szMsg),  "%04d/%02d/%02d,%02d:%02d:%02d,",   SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
						//	SystemTime.wHour, SystemTime.wMinute, 
						//	SystemTime.wSecond);

						//strcat_s(pData,sizeof(pData),"System_Status:	");
						//strcat_s(pData,sizeof(pData),szMsg);
						//strcat_s(pData,sizeof(pData),szHostName);
						//strcat_s(pData,sizeof(pData),",");
						//ConvertedChars=0;
						//char Stemp[256]="";
						//wcstombs_s(&ConvertedChars, Stemp, sizeof(Stemp), stemp,wcslen(stemp));
						//strcat_s(pData,sizeof(pData),Stemp);
						//strcat_s(pData,sizeof(pData)," has been used out of time");

						//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
						//{
						//	if(NULL == fLog)
						//	{
						//		return FALSE;
						//	}
						//	fprintf_s(fLog,"%s\n",pData);
						//	fclose(fLog);
						//}
						//else
						//{
						//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
						//}//Add end by Maxwell 2008/11/12 for operation log on TE Server
						equipflag=1;
					}
					else if((*Cy).Usedtime>=timetemp) 
					{
						ShowTemp+="     --------------------------------------------\r\n";
						stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
						ShowTemp+=stemp;
						ShowTemp+=" will be used out of time\r\n";
						equipyellowflag=1;//2008/11/3
					}
				}
			}
			ShowTemp+=ShowBUF;
			ShowBUF=_T("");
			wcscpy(p,_T(""));
			ShowTemp+=_T("\r\n\r\n\r\n");
			//090616
			//090616
			if(equipflag)
			{
				char pRecordData[100]="EQUIPFLAG=3;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
				//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
			}
			else if(equipyellowflag)
			{
				char pRecordData[100]="EQUIPFLAG=2;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
				//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
			}
			else 
			{
				char pRecordData[100]="EQUIPFLAG=1;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
			}
			//090616
			iDev++;
			equipflag=0;
		}
		iDev=0;
		ShowInfomation(ShowTemp);
	} 
	return  1;
}



//UI-->LCC:CMD:=RESET;LINE:=LI;STATION:=PT;PC:=1;MATERIAL:=LAN1 CABLE;MATERIAL:=USB CABLE;AMENDERNAME:=F6825884;AMENDERPASSWORD:=F6825884;


//UI-->LCC:CMD:=QUERY;LINE:=L1;STATION:=PT;PC:=1;
//UI-->LCC:CMD:=AMEND;LINE:=L1;STATION:=PT;PC:=1;

//LCC-->UI:STATUS_0=[LAN1 CABLE,2000,0][LAN2 CABLE,2000,0];//successed
//LCC-->UI:STATUS_1;//produce perform fail
//LCC-->UI:STATUS_2;//no data in the database
//LCC-->UI:STATUS_X;//data format error

//LCC-->UI:AMEND_0;//amend

////////int CEquipInfoShow::EquipShowInfo()
////////{
////////	list<EQUIP_USE>::iterator Cy;
////////	wchar_t Temp[100]=_T("");
////////	wchar_t p[10000]=_T("");
////////	CString ShowBUF;
////////	memset(p,0,sizeof(p));
////////	int BufL=10000;
////////	//size_t cLen;
////////	//wchar_t wcBuf[1024]="";
////////	//mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, (char *)pBuf, ulSize);
////////
////////	//for(int iDev=0;iDev<50;iDev++)
////////	//{
////////	//	DeviceList[iDev].clear();
////////	//}
////////	char szHostName[256]="";
////////	char *pTemp,*pTemp2;
////////	char PCNumber[256]="";
////////	int qpc=0;
////////
////////
////////	//////////////////////////
////////	int nRetCode;
////////	char buflog[1024]="";
////////	nRetCode=gethostname(szHostName,sizeof(szHostName));
////////	if(nRetCode==0)
////////	{
////////		if(pTemp=strstr(szHostName,"-"))
////////		{
////////			if(pTemp2=strstr(pTemp+1,"-"))
////////			{
////////				strcpy_s(PCNumber,sizeof(PCNumber),pTemp2+1);
////////				qpc=atoi(PCNumber);
////////				
////////				wchar_t TempFullName[1024]=_T("");
////////
////////				swprintf(TempFullName,sizeof(TempFullName)/2,_T("CMD:=QUERY;LINE:=%s;STATION:=%s;PC:=%d;"),gStationInfo.EquipInfo.EqipLine,gStationInfo.EquipInfo.EqipStation,qpc);
////////
////////				size_t ConvertedChars=0;
////////				wcstombs_s(&ConvertedChars, buflog, sizeof(buflog), TempFullName,wcslen(TempFullName));
////////
////////				LCC_SetSendData(hDev,buflog,strlen(buflog),0);	
////////				LCC_Send(hDev);
////////				DWORD res=WaitForSingleObject(hEventRev,5000);
////////				if(res==WAIT_OBJECT_0)
////////				{
////////				}
////////				else
////////				{
////////					//printf("%s\n","SEND STRING TIME OUT");
////////					if( MessageBox(_T("SEND EQUIPMENT TIME OUT OR THE LINK FAIL?\n?"),  _T("ASk?"),   MB_OK|MB_OKCANCEL)==IDOK)
////////					{
////////						//::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
////////					}
////////					else
////////					{
////////						//::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
////////					}
////////				}
////////				ResetEvent(hEventRev);
////////
////////			}
////////		}
////////	}
////////
////////	CString  temp;
////////
////////	CString ShowTemp(" ");
////////	//if(!Tool.GetNum())//add 2008/11/1
////////	//{		    
////////
////////	//}
////////	//else
////////	//{
////////		CString stemp;
////////		double timetemp;
////////		int iDev=0;
////////		while(!DeviceList[iDev].empty())
////////		{
////////			swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment Status*****************\r\n"));
////////			//swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment In Fixture %d*****************\r\n"),iDev+1);
////////			//swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
////////			ShowTemp+=Temp;
////////
////////			for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
////////			{
////////				swprintf_s(Temp,sizeof(Temp)/2,_T("     --------------------------------------------\r\n"));
////////				wcscat_s(p,sizeof(p)/2,Temp);		
////////				swprintf_s(Temp,sizeof(Temp)/2,_T("     Equipment:%s\r\n"),(*Cy).EquipmentName);
////////				wcscat_s(p,sizeof(p)/2,Temp);
////////				swprintf_s(Temp,sizeof(Temp)/2,_T("          Current Used Time:             %d\r\n"),(*Cy).Usedtime);
////////				wcscat_s(p,sizeof(p)/2,Temp);
////////				swprintf_s(Temp,sizeof(Temp)/2,_T("          Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
////////				wcscat_s(p,sizeof(p)/2,Temp);
////////				ShowBUF.Format(_T("%s"), p);
////////				timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;
////////
////////				if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
////////				{
////////					if((*Cy).Usedtime>=(*Cy).Maxusetime)
////////					{
////////						ShowTemp+=_T("     --------------------------------------------\r\n");
////////						stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
////////						ShowTemp+=stemp;
////////						ShowTemp+=_T(" has been used out of time\r\n");
////////						equipflag=1;
////////					}
////////					else if((*Cy).Usedtime>=timetemp) 
////////					{
////////						ShowTemp+="     --------------------------------------------\r\n";
////////						stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
////////						ShowTemp+=stemp;
////////						ShowTemp+=" will be used out of time\r\n";
////////						equipyellowflag=1;//2008/11/3
////////					}
////////				}
////////			}
////////			ShowTemp+=ShowBUF;
////////			ShowBUF=_T("");
////////			wcscpy(p,_T(""));
////////			ShowTemp+=_T("*********************END************************");
////////			ShowTemp+=_T("\r\n\r\n\r\n");
////////
////////			if(equipflag)
////////			{
////////				char pRecordData[100]="EQUIPFLAG=3;";
////////				size_t SizeLen=strlen(pRecordData);
////////				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
////////			}
////////			else if(equipyellowflag)
////////			{
////////				char pRecordData[100]="EQUIPFLAG=2;";
////////				size_t SizeLen=strlen(pRecordData);
////////				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);				
////////			}
////////			else 
////////			{
////////				char pRecordData[100]="EQUIPFLAG=1;";
////////				size_t SizeLen=strlen(pRecordData);
////////				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
////////			}
////////			iDev++;
////////			equipflag=0;
////////		}
////////		iDev=0;
////////		ShowInfomation(ShowTemp);
////////	/*} */
////////	return false;
////////}

int CEquipInfoShow::EquipShowInfo()
{
	list<EQUIP_USE>::iterator Cy;
	wchar_t Temp[100]=_T("");
	wchar_t p[10000]=_T("");
	CString ShowBUF;
	memset(p,0,sizeof(p));
	int BufL=10000;
	//modify start by joyce 2008/11/6
	char szHostName[256]="";
	char *pTemp,*pTemp2;
	char tempHostName[256]="";
	wchar_t fullName[256]=_T("");
	wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
	//Add start by Maxwell 2008/11/12 for operation log on TE Server
	wchar_t FileName[256]=_T("");
	errno_t err;
	wcscpy_s(FileName,sizeof(FileName)/2,gStationInfo.EquipInfo.EqipOperPath);
	wcscat_s(FileName, sizeof(FileName)/2, _T("EquipmentOperation.txt"));
	FILE *fLog;
	//remove start by Talen 2011/10/24
	//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
	//{	
	//	fclose(fLog);
	//}
	//else
	//{
	//	equipfileflag=1;
	//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
	//}
	//remove end by Talen 2011/10/24

	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	//sprintf_s(szHostName,256,"%s","K31-MPT1-1");//DEBUG USE
	if(nRetCode==0)
	{
		if(pTemp=strstr(szHostName,"-"))
		{
			if(pTemp2=strstr(pTemp+1,"-"))
			{
				strcpy_s(tempHostName,sizeof(tempHostName),pTemp2+1);
				strcat(tempHostName,".xml");
				wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
				size_t ConvertedChars=0;
				wchar_t TempHostName[256]=_T("");
				mbstowcs_s(&ConvertedChars, TempHostName, sizeof(TempHostName)/2, tempHostName,strlen(tempHostName));
				wcscat(fullName,TempHostName);
			}
		}
	}
	CString  temp;

	size_t ConvertedChars=0;
	//wcstombs_s(&ConvertedChars, fullName, sizeof(fullName), temp,wcslen(temp));

	temp.Format(_T("%s"), fullName);
	/*temp.Format("%s", gSfisData.Equip_LOG_FILE_PATH);*/
	Tool.m_path = temp;
	/*int flag=*/
	//Tool.GetNum();
	CString ShowTemp(" ");
	if(!Tool.GetNum())//add 2008/11/1
	{		    
		//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_ICONGREEN));
		ShowTemp="The equipment usage file doesn't exist";
		ShowInfomation(ShowTemp);
		equipfileflag=1;
		//MessageBox(_T("The equipment usage file doesn't exist\nCan not manage usage status!"),_T("Tip"),MB_OK);
		AfxMessageBox(_T("The equipment usage file doesn't exist\nCan not manage usage status!"));
		exit(0);//haibin.li 2012/3/20
	}
	else
	{
		CString stemp;
		double timetemp;
		//090612,090616
		int iDev=0;
		//while(wcscmp(DeviceList[iDev].begin()->FixCountName,_T("")))
		while(!DeviceList[iDev].empty())
		//for(int iDev=0;iDev<gStationInfo.MultiTaskSetting.Number;iDev++)
		{
			//swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment In Fixture %d*****************\r\n"),iDev+1);
			swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
			ShowTemp+=Temp;

			for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
				//for (int i =0;i<10;i++)
			{
				swprintf_s(Temp,sizeof(Temp)/2,_T("     --------------------------------------------\r\n"));
				wcscat_s(p,sizeof(p)/2,Temp);		
				swprintf_s(Temp,sizeof(Temp)/2,_T("     Equipment:%s\r\n"),(*Cy).EquipmentName);
				wcscat_s(p,sizeof(p)/2,Temp);
				swprintf_s(Temp,sizeof(Temp)/2,_T("          Current Used Time:             %d\r\n"),(*Cy).Usedtime);
				wcscat_s(p,sizeof(p)/2,Temp);
				swprintf_s(Temp,sizeof(Temp)/2,_T("          Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
				wcscat_s(p,sizeof(p)/2,Temp);
				ShowBUF.Format(_T("%s"), p);
				timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;

				if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
				{
					if((*Cy).Usedtime>=(*Cy).Maxusetime)
					{
						ShowTemp+=_T("     --------------------------------------------\r\n");
						stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
						ShowTemp+=stemp;
						ShowTemp+=_T(" has been used out of time\r\n");
						char pData[1024]="";
						SYSTEMTIME	SystemTime;
						char szMsg[256]="";
						GetLocalTime(&SystemTime);
						sprintf_s(szMsg, sizeof(szMsg),  "%04d/%02d/%02d,%02d:%02d:%02d,",   SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
							SystemTime.wHour, SystemTime.wMinute, 
							SystemTime.wSecond);

						strcat_s(pData,sizeof(pData),"System_Status:	");
						strcat_s(pData,sizeof(pData),szMsg);
						strcat_s(pData,sizeof(pData),szHostName);
						strcat_s(pData,sizeof(pData),",");
						ConvertedChars=0;
						char Stemp[256]="";
						wcstombs_s(&ConvertedChars, Stemp, sizeof(Stemp), stemp,wcslen(stemp));
						strcat_s(pData,sizeof(pData),Stemp);
						strcat_s(pData,sizeof(pData)," has been used out of time");

						if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
						{
							if(NULL == fLog)
							{
								return FALSE;
							}
							fprintf_s(fLog,"%s\n",pData);
							fclose(fLog);
						}
						else
						{
							AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
						}//Add end by Maxwell 2008/11/12 for operation log on TE Server
						equipflag=1;
					}
					else if((*Cy).Usedtime>=timetemp) 
					{
						ShowTemp+="     --------------------------------------------\r\n";
						stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
						ShowTemp+=stemp;
						ShowTemp+=" will be used out of time\r\n";
						equipyellowflag=1;//2008/11/3
					}
				}
			}
			ShowTemp+=ShowBUF;
			ShowBUF=_T("");
			wcscpy(p,_T(""));
			ShowTemp+=_T("\r\n\r\n\r\n");
			//090616
			//090616
			if(equipflag)
			{
				char pRecordData[100]="EQUIPFLAG=3;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
				//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
			}
			else if(equipyellowflag)
			{
				char pRecordData[100]="EQUIPFLAG=2;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
				//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
			}
			else 
			{
				char pRecordData[100]="EQUIPFLAG=1;";
				size_t SizeLen=strlen(pRecordData);
				AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);

			}
			//090616
			iDev++;
			equipflag=0;
		}
		iDev=0;
		ShowInfomation(ShowTemp);
	} 
	return 1;//haibin.li 2011/11/08 no value return
}


//09/06/12
bool CEquipInfoShow::CountNumXml(UINT CountModal)
{
	char szHostName[256]="";
	char *pTemp,*pTemp2;
	char tempHostName[256]="";
	wchar_t fullName[256]=_T("");
	wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
	wchar_t FileName[256]=_T("");
	//wcscpy_s(FileName,sizeof(FileName)/2,gStationInfo.EquipInfo.EqipOperPath);
	//wcscat_s(FileName, sizeof(FileName)/2, _T("EquipmentOperation.txt"));
	//FILE *fLog;
	//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
	//{	
	//	fclose(fLog);
	//}
	//else
	//{
	//	equipfileflag=1;
	//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
	//}

	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode==0)
	{
		if(pTemp=strstr(szHostName,"-"))
		{
			if(pTemp2=strstr(pTemp+1,"-"))
			{
				strcpy_s(tempHostName,sizeof(tempHostName),pTemp2+1);
				strcat(tempHostName,".xml");
				wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
				size_t ConvertedChars=0;
				wchar_t TempHostName[256]=_T("");
				mbstowcs_s(&ConvertedChars, TempHostName, sizeof(TempHostName)/2, tempHostName,strlen(tempHostName));
				wcscat(fullName,TempHostName);
			}
		}
	}
	CString  temp;

	size_t ConvertedChars=0;
	//wcstombs_s(&ConvertedChars, fullName, sizeof(fullName), temp,wcslen(temp));

	temp.Format(_T("%s"), fullName);
	Tool.m_path = temp;
	CString ShowTemp(" ");
	if(!gStationInfo.UiDisplay.RepeatTime)//Add by Maxwell for not count equip num when repeat time > 0 
	{
		if(!Tool.count(CountModal))//add 2008/11/1 //090612
		{
			ShowTemp=_T("The equipment usage file didn't exist");
			ShowInfomation(ShowTemp);
			equipfileflag=1;
		}
		else
		{
			Tool.GetNum();//add start by maxwell 2008/10/31
			CString stemp;
			double timetemp;
			list<EQUIP_USE>::iterator Cy;
			wchar_t Temp[100]=_T("");
			wchar_t p[10000]=_T("");
			CString ShowBUF;
			memset(p,0,sizeof(p));
			int iDev=0;
			while(!DeviceList[iDev].empty())
			{
				//swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment In Fixture %d*****************\r\n"),iDev+1);
				swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
				ShowTemp+=Temp;
				for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
				{
					swprintf_s(Temp,sizeof(Temp)/2,_T("--------------------------------------------\r\n"));
					wcscat_s(p,sizeof(p)/2,Temp);		
					swprintf_s(Temp,sizeof(Temp)/2,_T("Equipment:%s\r\n"),(*Cy).EquipmentName);
					wcscat_s(p,sizeof(p)/2,Temp);
					swprintf_s(Temp,sizeof(Temp)/2,_T("Current Used Time:             %d\r\n"),(*Cy).Usedtime);
					wcscat_s(p,sizeof(p)/2,Temp);
					swprintf_s(Temp,sizeof(Temp)/2,_T("Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
					wcscat_s(p,sizeof(p)/2,Temp);
					ShowBUF.Format(_T("%s"), p);
					timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;
					CString show;
					if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
					{
						if((*Cy).Usedtime>=(*Cy).Maxusetime)
						{
							//ShowTemp+=_T("--------------------------------------------\r\n");
							//stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
							//ShowTemp+=stemp;
							//ShowTemp+=_T(" has been used out of time\r\n");
							//char pData[1024]="";
							//SYSTEMTIME	SystemTime;
							//char szMsg[256]="";
							//GetLocalTime(&SystemTime);
							//sprintf_s(szMsg, sizeof(szMsg),  "%04d/%02d/%02d,%02d:%02d:%02d,",   SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
							//	SystemTime.wHour, SystemTime.wMinute, 
							//	SystemTime.wSecond);

							//strcat_s(pData,sizeof(pData),"System_Status:	");
							//strcat_s(pData,sizeof(pData),szMsg);
							//strcat_s(pData,sizeof(pData),szHostName);
							//strcat_s(pData,sizeof(pData),",");
							//ConvertedChars=0;
							//char Stemp[256]="";
							//wcstombs_s(&ConvertedChars, Stemp, sizeof(Stemp), stemp,wcslen(stemp));
							//strcat_s(pData,sizeof(pData),Stemp);
							//strcat_s(pData,sizeof(pData)," has been used out of time");

							//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
							//{
							//	if(NULL == fLog)
							//	{
							//		return FALSE;
							//	}
							//	fprintf_s(fLog,"%s\n",pData);
							//	fclose(fLog);
							//}
							//else
							//{
							//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
							//}//Add end by Maxwell 2008/11/12 for operation log on TE Server
							equipflag=1;
						}
						else if((*Cy).Usedtime>=timetemp) 
						{
							ShowTemp+="--------------------------------------------\r\n";
							stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
							ShowTemp+=stemp;
							ShowTemp+=" will be used out of time\r\n";
							equipyellowflag=1;//2008/11/3
						}
					}
				}
				//ShowTemp+=_T("************************************************");
				ShowTemp+=ShowBUF;
				ShowBUF=_T("");
				wcscpy(p,_T(""));
				ShowTemp+=_T("\r\n\r\n\r\n");
				//090616
				if(equipflag)
				{
					char pRecordData[100]="EQUIPFLAG=3;";
					size_t SizeLen=strlen(pRecordData);
					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
					//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
				}
				else if(equipyellowflag)
				{
					char pRecordData[100]="EQUIPFLAG=2;";
					size_t SizeLen=strlen(pRecordData);
					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
					//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
				}
				else 
				{
					char pRecordData[100]="EQUIPFLAG=1;";
					size_t SizeLen=strlen(pRecordData);
					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
				}
				//090616
				iDev++;
				equipflag=0;
			}
			iDev=0;
			ShowInfomation(ShowTemp);
		}
	}

	return false;
}


////////////bool CEquipInfoShow::CountNum(UINT CountModal)
////////////{
////////////
////////////	char szHostName[256]="";
////////////	char *pTemp,*pTemp2;
////////////	char PCNumber[256]="";
////////////	int qpc=0;
////////////
////////////	//////////////////////////
////////////	int nRetCode;
////////////	char buflog[1024]="";
////////////	nRetCode=gethostname(szHostName,sizeof(szHostName));
////////////	if(nRetCode==0)
////////////	{
////////////		if(pTemp=strstr(szHostName,"-"))
////////////		{
////////////			if(pTemp2=strstr(pTemp+1,"-"))
////////////			{
////////////				strcpy_s(PCNumber,sizeof(PCNumber),pTemp2+1);
////////////				qpc=atoi(PCNumber);
////////////				
////////////				wchar_t TempFullName[1024]=_T("");
////////////				//mbstowcs_s(&ConvertedChars,wcPCNumber,sizeof(wcPCNumber)/2,PCNumber,sizeof(PCNumber));
////////////				//sprintf_s(buflog,sizeof(buflog),"CMD:=QUERY;LINE:=L1;STATION:=PT;PC:=1;");
////////////				swprintf(TempFullName,sizeof(TempFullName)/2,_T("CMD:=AMEND;LINE:=%s;STATION:=%s;PC:=%d;"),gStationInfo.EquipInfo.EqipLine,gStationInfo.EquipInfo.EqipStation,qpc);
////////////
////////////				size_t ConvertedChars=0;
////////////				wcstombs_s(&ConvertedChars, buflog, sizeof(buflog), TempFullName,wcslen(TempFullName));
////////////
////////////				LCC_SetSendData(hDev,buflog,strlen(buflog),0);	
////////////				LCC_Send(hDev);
////////////
////////////			}
////////////		}
////////////	}
////////////
////////////	////////////////////
////////////	CString  temp;
////////////
////////////	size_t ConvertedChars=0;
////////////
////////////	CString ShowTemp(" ");
////////////	if(!gStationInfo.UiDisplay.RepeatTime)//Add by Maxwell for not count equip num when repeat time > 0 
////////////	{
////////////
////////////			/////////////////////////////////////
////////////			list<EQUIP_USE>::iterator Cx;
////////////			for (Cx=DeviceList[CountModal-1].begin(); Cx!=DeviceList[CountModal-1].end();Cx++)
////////////			{
////////////				(*Cx).Usedtime++;				
////////////			}
////////////
////////////			/////////////////////////////////////
////////////			CString stemp;
////////////			double timetemp;
////////////			list<EQUIP_USE>::iterator Cy;
////////////			wchar_t Temp[100]=_T("");
////////////			wchar_t p[10000]=_T("");
////////////			CString ShowBUF;
////////////			memset(p,0,sizeof(p));
////////////			int iDev=0;
////////////			while(!DeviceList[iDev].empty())
////////////			{
////////////				swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment Status*****************\r\n"));
////////////				//swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
////////////				ShowTemp+=Temp;
////////////				for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
////////////				{
////////////					swprintf_s(Temp,sizeof(Temp)/2,_T("--------------------------------------------\r\n"));
////////////					wcscat_s(p,sizeof(p)/2,Temp);		
////////////					swprintf_s(Temp,sizeof(Temp)/2,_T("Equipment:%s\r\n"),(*Cy).EquipmentName);
////////////					wcscat_s(p,sizeof(p)/2,Temp);
////////////					swprintf_s(Temp,sizeof(Temp)/2,_T("Current Used Time:             %d\r\n"),(*Cy).Usedtime);
////////////					wcscat_s(p,sizeof(p)/2,Temp);
////////////					swprintf_s(Temp,sizeof(Temp)/2,_T("Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
////////////					wcscat_s(p,sizeof(p)/2,Temp);
////////////					ShowBUF.Format(_T("%s"), p);
////////////					timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;
////////////					CString show;
////////////					if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
////////////					{
////////////						if((*Cy).Usedtime>=(*Cy).Maxusetime)
////////////						{
////////////							ShowTemp+=_T("--------------------------------------------\r\n");
////////////							stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
////////////							ShowTemp+=stemp;
////////////							ShowTemp+=_T(" has been used out of time\r\n");
////////////							equipflag=1;
////////////						}
////////////						else if((*Cy).Usedtime>=timetemp) 
////////////						{
////////////							ShowTemp+="--------------------------------------------\r\n";
////////////							stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
////////////							ShowTemp+=stemp;
////////////							ShowTemp+=" will be used out of time\r\n";
////////////							equipyellowflag=1;//2008/11/3
////////////						}
////////////					}
////////////				}
////////////				//ShowTemp+=_T("************************************************");
////////////				ShowTemp+=ShowBUF;
////////////				ShowBUF=_T("");
////////////				wcscpy(p,_T(""));
////////////				ShowTemp+=_T("*********************END************************");
////////////				ShowTemp+=_T("\r\n\r\n\r\n");
////////////				//090616
////////////				if(equipflag)
////////////				{
////////////					char pRecordData[100]="EQUIPFLAG=3;";
////////////					size_t SizeLen=strlen(pRecordData);
////////////					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
////////////					//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
////////////				}
////////////				else if(equipyellowflag)
////////////				{
////////////					char pRecordData[100]="EQUIPFLAG=2;";
////////////					size_t SizeLen=strlen(pRecordData);
////////////					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
////////////					//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
////////////				}
////////////				else 
////////////				{
////////////					char pRecordData[100]="EQUIPFLAG=1;";
////////////					size_t SizeLen=strlen(pRecordData);
////////////					AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
////////////				}
////////////				//090616
////////////				iDev++;
////////////				equipflag=0;
////////////			}
////////////			iDev=0;
////////////			ShowInfomation(ShowTemp);
////////////	}
////////////
////////////	return false;
////////////}

//09/06/12
bool CEquipInfoShow::CountNum(UINT CountModal)
{
	//2008/10/29
	////CTool EquipmentCount;
	//GetTeserverPathName(gSfisData.Equip_LOG_FILE_PATH,"","",g_variable.TEquipPath,sizeof(g_variable.TEquipPath));
	//modify start by joyce 2008/11/6
		char szHostName[256]="";
		char *pTemp,*pTemp2;
		char tempHostName[256]="";
		wchar_t fullName[256]=_T("");
		wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
		//Add start by Maxwell 2008/11/12 for operation log on TE Server
		wchar_t FileName[256]=_T("");
		errno_t err;
		wcscpy_s(FileName,sizeof(FileName)/2,gStationInfo.EquipInfo.EqipOperPath);
		wcscat_s(FileName, sizeof(FileName)/2, _T("EquipmentOperation.txt"));
		FILE *fLog;
		//remove start by Talen 2011/10/24
		//if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
		//{	
		//	fclose(fLog);
		//}
		//else
		//{
		//	equipfileflag=1;
		//	AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
		//}
		//remove end by Talen 2011/10/24

		int nRetCode;
		nRetCode=gethostname(szHostName,sizeof(szHostName));
		if(nRetCode==0)
		{
			if(pTemp=strstr(szHostName,"-"))
			{
				if(pTemp2=strstr(pTemp+1,"-"))
			 {
				 strcpy_s(tempHostName,sizeof(tempHostName),pTemp2+1);
				 strcat(tempHostName,".xml");
				 wcscpy_s(fullName,sizeof(fullName)/2,gStationInfo.EquipInfo.EqipLogPath);
				 size_t ConvertedChars=0;
				 wchar_t TempHostName[256]=_T("");
				 mbstowcs_s(&ConvertedChars, TempHostName, sizeof(TempHostName)/2, tempHostName,strlen(tempHostName));
				 wcscat(fullName,TempHostName);
				}
			}
		}
		CString  temp;

		size_t ConvertedChars=0;
		//wcstombs_s(&ConvertedChars, fullName, sizeof(fullName), temp,wcslen(temp));
		
		temp.Format(_T("%s"), fullName);
		/*temp.Format("%s", gSfisData.Equip_LOG_FILE_PATH);*/
		Tool.m_path = temp;
		/*int flag=*/
		//Tool.GetNum();
		CString ShowTemp(" ");
			//pEdit->AddInfro(EquipmentCount.m_path.GetBuffer());
			//EquipmentCount.m_path = "P:\station1.xml";
			if(!gStationInfo.UiDisplay.RepeatTime)//Add by Maxwell for not count equip num when repeat time > 0 
			{
				if(!Tool.count(CountModal))//add 2008/11/1 //090612
				{
					ShowTemp=_T("The equipment usage file didn't exist");
					ShowInfomation(ShowTemp);
					equipfileflag=1;
				}
				else
				{
					Tool.GetNum();//add start by maxwell 2008/10/31
					CString stemp;
					double timetemp;
					list<EQUIP_USE>::iterator Cy;
					wchar_t Temp[100]=_T("");
					wchar_t p[10000]=_T("");
					CString ShowBUF;
					memset(p,0,sizeof(p));
					int iDev=0;
					while(!DeviceList[iDev].empty())
					//for(int iDev=0;iDev<gStationInfo.MultiTaskSetting.Number;iDev++)
					{
						//swprintf_s(Temp,sizeof(Temp)/2,_T("*****************Equipment In Fixture %d*****************\r\n"),iDev+1);
						swprintf_s(Temp,sizeof(Temp)/2,_T("**********************************  %s  **********************************\r\n"),DeviceList[iDev].begin()->FixCountName);
						ShowTemp+=Temp;
						for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
						{
							swprintf_s(Temp,sizeof(Temp)/2,_T("--------------------------------------------\r\n"));
							wcscat_s(p,sizeof(p)/2,Temp);		
							swprintf_s(Temp,sizeof(Temp)/2,_T("Equipment:%s\r\n"),(*Cy).EquipmentName);
							wcscat_s(p,sizeof(p)/2,Temp);
							swprintf_s(Temp,sizeof(Temp)/2,_T("Current Used Time:             %d\r\n"),(*Cy).Usedtime);
							wcscat_s(p,sizeof(p)/2,Temp);
							swprintf_s(Temp,sizeof(Temp)/2,_T("Maximum Use Time:            %d\r\n"),(*Cy).Maxusetime);
							wcscat_s(p,sizeof(p)/2,Temp);
							ShowBUF.Format(_T("%s"), p);
							timetemp=(*Cy).Maxusetime*gStationInfo.EquipInfo.EqipAlarmRate;
							CString show;
							/*if((*Cy).Usedtime>=(*Cy).Maxusetime)
							{
							show=(*Cy).EquipmentName;
							show+="has been out of time";
							AfxMessageBox(show);
							equipflag=1;
							}*/
							if(gStationInfo.EquipInfo.EqipCtlFlag)//Add by Maxwell 2008/11/12 for EQUIP_CONTROL_FLAG,1 for control 0 for no control 
							{
								if((*Cy).Usedtime>=(*Cy).Maxusetime)
								{
									ShowTemp+=_T("--------------------------------------------\r\n");
									stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
									ShowTemp+=stemp;
									ShowTemp+=_T(" has been used out of time\r\n");
									char pData[1024]="";
									SYSTEMTIME	SystemTime;
									char szMsg[256]="";
									GetLocalTime(&SystemTime);
									sprintf_s(szMsg, sizeof(szMsg),  "%04d/%02d/%02d,%02d:%02d:%02d,",   SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
										SystemTime.wHour, SystemTime.wMinute, 
										SystemTime.wSecond);

									strcat_s(pData,sizeof(pData),"System_Status:	");
									strcat_s(pData,sizeof(pData),szMsg);
									strcat_s(pData,sizeof(pData),szHostName);
									strcat_s(pData,sizeof(pData),",");
									ConvertedChars=0;
									char Stemp[256]="";
									wcstombs_s(&ConvertedChars, Stemp, sizeof(Stemp), stemp,wcslen(stemp));
									strcat_s(pData,sizeof(pData),Stemp);
									strcat_s(pData,sizeof(pData)," has been used out of time");

									if(err = _wfopen_s(&fLog, FileName, _T("a+"))==0)
									{
										if(NULL == fLog)
										{
											return FALSE;
										}
										fprintf_s(fLog,"%s\n",pData);
										fclose(fLog);
									}
									else
									{
										AfxMessageBox(_T("The equipment usage management log file doesn't exist!\n"));
									}//Add end by Maxwell 2008/11/12 for operation log on TE Server
									equipflag=1;
								}
								else if((*Cy).Usedtime>=timetemp) 
								{
									ShowTemp+="--------------------------------------------\r\n";
									stemp.Format(_T("%s's %s"),(*Cy).FixCountName,(*Cy).EquipmentName);
									ShowTemp+=stemp;
									ShowTemp+=" will be used out of time\r\n";
									equipyellowflag=1;//2008/11/3
								}
							}
						}
						//ShowTemp+=_T("************************************************");
						ShowTemp+=ShowBUF;
						ShowBUF=_T("");
						wcscpy(p,_T(""));
						ShowTemp+=_T("\r\n\r\n\r\n");
						//090616
						if(equipflag)
						{
							char pRecordData[100]="EQUIPFLAG=3;";
							size_t SizeLen=strlen(pRecordData);
							AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
							//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
						}
						else if(equipyellowflag)
						{
							char pRecordData[100]="EQUIPFLAG=2;";
							size_t SizeLen=strlen(pRecordData);
							AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
							//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
						}
						else 
						{
							char pRecordData[100]="EQUIPFLAG=1;";
							size_t SizeLen=strlen(pRecordData);
							AmPostThreadMes(TEST_TASK_TYPE+iDev, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
						}
						//090616
						iDev++;
						equipflag=0;
					}
					iDev=0;
					ShowInfomation(ShowTemp);
				}
			}


			//if(equipflag)
			//{
			//	char pRecordData[100]="EQUIPFLAG=3;";
			//	size_t SizeLen=strlen(pRecordData);
			//	AmPostThreadMes(TEST_TASK_TYPE+0, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
			//	//pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));//vincent for the equipment;
			//}
			//else if(equipyellowflag)
			//{
			//	char pRecordData[100]="EQUIPFLAG=2;";
			//	size_t SizeLen=strlen(pRecordData);
			//	AmPostThreadMes(TEST_TASK_TYPE+0, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
			//	//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
			//}
			//else 
			//{
			//	char pRecordData[100]="EQUIPFLAG=1;";
			//	size_t SizeLen=strlen(pRecordData);
			//	AmPostThreadMes(TEST_TASK_TYPE+0, WM_UPDATE_INFO_EQUIP, (unsigned int)pRecordData, (unsigned int)SizeLen);
			//}
			//pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_GREEN));


	//		//add start 2008/11/4
	//		pStatic=(CStatic*)GetDlgItem(IDC_EQUIP_PICTURE); 
	//		if(equipflag&&pStatic)
	//		{
	//			//pStatic=(CStatic*)GetDlgItem(IDC_EQUIP_PICTURE);   
	//			pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_RED));
	//		}
	//		else if(equipyellowflag&&pStatic)
	//		{
	//			// pStatic=(CStatic*)GetDlgItem(IDC_EQUIP_PICTURE);
	//			pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_YELLOW));
	//		}
	//		else if(pStatic)
	//			pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_GREEN));
	//		//add end 2008/11/4
	//		/*if(equipflag)  
	//		::PostMessage(m_hWnd,WM_QUIT,NULL,NULL);*/
	//		//gSIS.ShowInfomation(ShowBUF);
	//		//add end by maxwell 2008/10/31
	//		//WriteDataToDB(str);
	//		//2008/10/29
	//
	//		if(0==CountModal)
	//		{
	//			m_FailNum++;
	//			CString strfail;
	//			CString strtoal;
	//			CString strpercent;
	//			strfail.Format("%d", m_FailNum); 
	//			strtoal.Format("%d", m_FailNum+m_PassNum); 
	//			strpercent.Format("%.2f%%",m_PassNum/( m_FailNum+m_PassNum)*100); 
	//			this->GetDlgItem(IDC_FAILITEM)->SetWindowText(strfail);
	//			this->GetDlgItem(IDC_TOTALITEM)->SetWindowText(strtoal);
	//			this->GetDlgItem(IDC_PERCENTITEM)->SetWindowText(strpercent);
	//			return true;


	//		}
	//if(1==CountModal)
	//{
	//	m_PassNum++;
	//	CString strpass;
	//	CString strtoal;
	//	CString strpercent;
	//	strpass.Format("%d", m_PassNum); 
	//	strtoal.Format("%d", m_FailNum+m_PassNum); 
	//	strpercent.Format("%.2f%%",m_PassNum/( m_FailNum+m_PassNum)*100); 
	//	//m_PassItem.SetWindowText(strpass);
	//	//m_ToalItem.SetWindowText(strtoal);
	//	this->GetDlgItem(IDC_PASSITEM)->SetWindowText(strpass);
	//	this->GetDlgItem(IDC_TOTALITEM)->SetWindowText(strtoal);
	//	this->GetDlgItem(IDC_PERCENTITEM)->SetWindowText(strpercent);
	//	
	//	return true;
	//}
	return false;
}

int CEquipInfoShow::InitialEQUIP()
{

	INIT_INFO EquipMentDev;

	size_t ConvertedChars=0;

	if(gStationInfo.EquipInfo.EqipEnableFlag)
	{

		wcstombs_s(&ConvertedChars, EquipMentDev.Ip, sizeof(EquipMentDev.Ip), gStationInfo.EquipInfo.EqipServerIP,wcslen(gStationInfo.EquipInfo.EqipServerIP));
		wcstombs_s(&ConvertedChars, EquipMentDev.ProductPN, sizeof(EquipMentDev.ProductPN), gStationInfo.uut.UUTPN,wcslen(gStationInfo.uut.UUTPN));
		wcstombs_s(&ConvertedChars, EquipMentDev.Product, sizeof(EquipMentDev.Product), gStationInfo.uut.UUTName,wcslen(gStationInfo.uut.UUTName));
		wcstombs_s(&ConvertedChars, EquipMentDev.ComName, sizeof(EquipMentDev.ComName), gStationInfo.PcInfo.HostName,wcslen(gStationInfo.PcInfo.HostName));
		wcstombs_s(&ConvertedChars, EquipMentDev.TestStaName, sizeof(EquipMentDev.TestStaName), gStationInfo.TestStation.Station_Name,wcslen(gStationInfo.TestStation.Station_Name));
		wcstombs_s(&ConvertedChars, EquipMentDev.DaigVersion, sizeof(EquipMentDev.DaigVersion), gStationInfo.TestStation.TestProgramVersion,wcslen(gStationInfo.TestStation.TestProgramVersion));

		if (!LCC_Connect(&hDev,&EquipMentDev))
		{
			AfxMessageBox(_T("connect the EquipSever failed.\n"));
			::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
			//return 0;
		}
		else
		{
			equipfileflag=1;
		}

		hEventRev=CreateEvent( 
			NULL,         // default security attributes
			TRUE,         // auto-reset event
			FALSE,         // initial state is unsignaled
			NULL  // object name
			); 

		LCC_RegOnRev(hDev,RevOn);

		EquipShowInfo();
	}
	else
	{
		//ShowStatus(_T("連接開關ゐ(MYDAS not switch ON)"));
	}

	return 1;
}

int CEquipInfoShow::ParsePtsData(char *pBuf)
{
	wchar_t wcBuf[1024];
	char Buf[1024]="";
	wchar_t ResultBuf[1024];
	size_t cLen;
	CAmParser Parser;
	if(!pBuf)
	{
		return 0;
	}
	strcpy_s(Buf,sizeof(Buf),pBuf);
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, Buf,strlen(Buf));
//UI-->LCC:CMD:=RESET;LINE:=LI;STATION:=PT;PC:=1;MATERIAL:=LAN1 CABLE;MATERIAL:=USB CABLE;AMENDERNAME:=F6825884;AMENDERPASSWORD:=F6825884;

//UI-->LCC:CMD:=QUERY;LINE:=L1;STATION:=PT;PC:=1;
//UI-->LCC:CMD:=AMEND;LINE:=L1;STATION:=PT;PC:=1;

//LCC-->UI:STATUS_0=[LAN1 CABLE,2000,0][LAN2 CABLE,2000,0];//successed
//LCC-->UI:STATUS_1;//produce perform fail
//LCC-->UI:STATUS_2;//no data in the database
//LCC-->UI:STATUS_X;//data format error

//LCC-->UI:AMEND_0;//amend

	if(Parser.ParserGetPara(wcBuf,  _T("STATUS_0"), ResultBuf, sizeof(ResultBuf)/2))
	{
        QueryPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("STATUS_1"), ResultBuf, sizeof(ResultBuf)/2))
	{
		
	}
	if(Parser.ParserGetPara(wcBuf,  _T("STATUS_2"), ResultBuf, sizeof(ResultBuf)/2))
	{
		AfxMessageBox(_T("The station name does not exist in the database"));
	}
	if(Parser.ParserGetPara(wcBuf,  _T("STATUS_3"), ResultBuf, sizeof(ResultBuf)/2))
	{
		SetEvent(hEventRev);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("AMEND_0"), ResultBuf, sizeof(ResultBuf)/2))
	{
		
	}
	if(Parser.ParserGetPara(wcBuf,  _T("AMEND_1"), ResultBuf, sizeof(ResultBuf)/2))
	{
		
	}
	if(Parser.ParserGetPara(wcBuf,  _T("AMEND_2"), ResultBuf, sizeof(ResultBuf)/2))
	{
		
	}
	if(Parser.ParserGetPara(wcBuf,  _T("RESET_0"), ResultBuf, sizeof(ResultBuf)/2))
	{
		SetEvent(hEventRev);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("RESET_1"), ResultBuf, sizeof(ResultBuf)/2))
	{
		
	}
	if(Parser.ParserGetPara(wcBuf,  _T("RESET_2"), ResultBuf, sizeof(ResultBuf)/2))
	{
		AfxMessageBox(_T("The station name does not exist in the database"));
	}
	return 1;
}
int CEquipInfoShow::QueryPerf(wchar_t *pBuf)
{

	wchar_t *pTokeStart=NULL;
	wchar_t *pTokeEnd=NULL;

	wchar_t *token, *next_token;


	wchar_t Equipment[256]=_T("");
	int MaxTime=0;
	int CurrentTime=0;

	//wchar_t string3[1024] =_T("");
	wchar_t string1[1024] =_T("");
	//wchar_t string2[1024] = _T("[LAN1 CABLE,2004,20][LAN2 CABLE,2003,30][LAN3 CABLE,2002,40][WAN CABLE,2001,50][USB CABLE,5000,100]");
	wchar_t seps[]   = _T(",");
	EQUIP_USE tempList;
	int iDev=0;
	for(iDev=0;iDev<50;iDev++)
	{
		DeviceList[iDev].clear();
	}
	iDev=0;

	//[LAN1 CABLE,2004,20][LAN2 CABLE,2003,30][LAN3 CABLE,2002,40][WAN CABLE,2001,50][USB CABLE,5000,100]
	pTokeStart=wcsstr(pBuf,_T("["));
	pTokeEnd=wcsstr(pBuf,_T("]"));

	while (pTokeStart != NULL)
	{
		pTokeStart++;
		wcsncpy_s(string1,sizeof(string1)/2,pTokeStart,pTokeEnd-pTokeStart);
		token = wcstok_s( string1, seps, &next_token);
		wcscpy_s(tempList.EquipmentName,sizeof(tempList.EquipmentName)/2,token);
		token = wcstok_s( NULL, seps, &next_token);
		tempList.Maxusetime=_wtoi(token);
		token = wcstok_s( NULL, seps, &next_token);
		tempList.Usedtime=_wtoi(token);
		DeviceList[iDev].push_back(tempList);
		pTokeStart=wcsstr(pTokeStart,_T("["));
		if(!pTokeStart)
		{
			break;
		}
		pTokeEnd=wcsstr(pTokeStart,_T("]"));

		//swprintf(string3,sizeof(string3)/2,_T("the equipment is:%s,the MaxTime is:%d,the current time is:%d\n"),Equipment,MaxTime,CurrentTime);
	}
	SetEvent(hEventRev);
	return 1;
}
