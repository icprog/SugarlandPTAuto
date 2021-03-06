// AmMydasDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "AmMydasDlg.h"
#include "AmThreadAdmin.h"

extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
UINT WINAPI MYDASTask(LPVOID lpPara);

// CAmMydasDlg dialog

IMPLEMENT_DYNAMIC(CAmMydasDlg, CDialog)

CAmMydasDlg::CAmMydasDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmMydasDlg::IDD, pParent)
{
	ConnectStatus=0;//
	SendtoPDBSFlag=0;
	hPTSDev=0;
	RecordCounter=0;
}

CAmMydasDlg::~CAmMydasDlg()
{
	//haibin.li 2011/11/07
	if (pLogMyDas)
	{
		delete pLogMyDas;
	}
}

void CAmMydasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAmMydasDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_LOG, &CAmMydasDlg::OnBnClickedButtonLog)
END_MESSAGE_MAP()


// CAmMydasDlg message handlers

BOOL CAmMydasDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ConnectStatusFont.CreatePointFont(100, _T("Arial"));
	// TODO:  Add extra initialization here
	m_Redbh.CreateSolidBrush(RGB(255,0,0));
	m_Greenbh.CreateSolidBrush(RGB(0,255,0));
	m_Yellowbh.CreateSolidBrush(RGB(255,255,0));
	m_Grancybh.CreateSolidBrush(RGB(82,102,251));
	m_Bkbh.CreateSolidBrush(RGB(255,255,255));
	m_SmallResultbh.CreateSolidBrush(RGB(255,255,255));

    StartMYDASTask();
	pLogMyDas=NULL;//haibin.li 2011/11/07
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CAmMydasDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//// TODO:  Add your specialized creation code here

	return 0;
}

int CAmMydasDlg::InitialPTSC()
{
	size_t ConvertedChars=0;

	if(gStationInfo.PtsInfo.PtsSwitch)
	{

		wcstombs_s(&ConvertedChars, PTS_info.Ip, sizeof(PTS_info.Ip), gStationInfo.PtsInfo.PtsServerIP,wcslen(gStationInfo.PtsInfo.PtsServerIP));
		wcstombs_s(&ConvertedChars, PTS_info.TitleVersion, sizeof(PTS_info.TitleVersion), gStationInfo.PtsInfo.PtsTitleVer,wcslen(gStationInfo.PtsInfo.PtsTitleVer));
		wcstombs_s(&ConvertedChars, PTS_info.ProductPN, sizeof(PTS_info.ProductPN), gStationInfo.uut.UUTPN,wcslen(gStationInfo.uut.UUTPN));
		wcstombs_s(&ConvertedChars, PTS_info.Product, sizeof(PTS_info.Product), gStationInfo.uut.UUTName,wcslen(gStationInfo.uut.UUTName));
		wcstombs_s(&ConvertedChars, PTS_info.ComName, sizeof(PTS_info.ComName), gStationInfo.PcInfo.HostName,wcslen(gStationInfo.PcInfo.HostName));
		wcstombs_s(&ConvertedChars, PTS_info.TestStaName, sizeof(PTS_info.TestStaName), gStationInfo.TestStation.Station_Name,wcslen(gStationInfo.TestStation.Station_Name));
		wcstombs_s(&ConvertedChars, PTS_info.DaigVersion, sizeof(PTS_info.DaigVersion), gStationInfo.TestStation.TestProgramVersion,wcslen(gStationInfo.TestStation.TestProgramVersion));

		if (!hPTSDev)
		{
			if (!PTSC_Connect(&hPTSDev,&PTS_info))
			{
				//printf("connect the PTS failed.\n");
				//AfxMessageBox(_T("connect the PTS failed."));
				gStationInfo.PtsInfo.ConnectStatus=0;
				AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_MYDAS,0, 0);
				ShowStatus(_T("connect the PTS failed."));
				SendtoPDBSFlag=0;
				return 0;
			}
			else
			{
				SendtoPDBSFlag=1;
				ShowStatus(_T("connect the PTS pass."));
				gStationInfo.PtsInfo.ConnectStatus=1;
				AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_MYDAS,0, 0);
			}
		}
		if (hPTSDev)
		{
			//PTSC_RegOnRev(hPTSDev,PTSRevOn);
		}
	}
	else
	{
		ShowStatus(_T("連接開關(MYDAS not switch ON)"));
	}

	return 1;
}

void CAmMydasDlg::ShowStatus(CString str)
{
	CString strTemp;

	SetDlgItemText(IDC_STC_MYDAS_STATUS, str);
	if (SendtoPDBSFlag)
	{
         SetDlgItemText(IDC_STC_CONNECT_FLAG, _T("CONNECTED"));
	}
	else
	{
         SetDlgItemText(IDC_STC_CONNECT_FLAG, _T("NOT CONNECTED"));
	}
	strTemp.Format(_T("傳送數據記錄次數：%d"),RecordCounter);
	SetDlgItemText(IDC_STC_SEND_RECORD_NUM, strTemp);

	return;
}

int CAmMydasDlg::PostDataToPTS()
{
	int Exit=1;
	int RtyConnectTime=2;
	int SendStatus=-1;
	int ForceBreak=0;
	//HANDLE hPRIOS = NULL;
	CString strTemp=_T("Send data ");

	while (RtyConnectTime--)
	{
		if (hPTSDev)
		{
			//Maxwell 100317
			if (!PTSC_GetLinkStatus(hPTSDev))
			//printf("the PTS not link failed.\n");
			//if (!SendtoPDBSFlag)
			{
				//PTSC_Disconnect(hPTSDev);

				//Maxwell use
				//HANDLE hPRIOS=hPTSDev;
				hPTSDev=NULL;

				if (!PTSC_Connect(&hPTSDev,&PTS_info))
				{
					//printf("connect the PTS failed.\n");
					SendtoPDBSFlag=0;
                    strTemp+=_T("to PTS fail, reconnect the PTS failed.");
					gStationInfo.PtsInfo.ConnectStatus=0;
					AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_MYDAS,0, 0);
					break;
				}
				else
				{
					SendtoPDBSFlag=1;
					//PTSC_RegOnRev(hPTSDev,PTSRevOn);
					strTemp+=_T("reconnect pts pass,");
					//hPTSDev = hPRIOS;
					gStationInfo.PtsInfo.ConnectStatus=1;
					AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_MYDAS,0, 0);

					//Maxwell 100317
					if(!PTSC_SetSendData(hPTSDev,pMainBuf,strlen(pMainBuf),2)||!PTSC_SetSendData(hPTSDev,pLogBuf,strlen(pLogBuf),1)||!PTSC_SetSendData(hPTSDev,pRecordBuf,strlen(pRecordBuf),0))
						//if(!WriteMainData(pMainBuf,strlen(pMainBuf))||!WriteRecordData(pRecordBuf,strlen(pRecordBuf))||!WriteLogData(pLogBuf,strlen(pLogBuf)))
					{
						/*!PTSC_SetSendData(hPTSDev,pMainBuf,strlen(pMainBuf),2)
						!PTSC_SetSendData(hPTSDev,pLogBuf,strlen(pLogBuf),1)
						!PTSC_SetSendData(hPTSDev,pRecordBuf,strlen(pRecordBuf),0)*/

						strTemp+=_T("Rewrite data fail!");
						return 0;
					}
					else
					{
						memset(pMainBuf,'\0',sizeof(pMainBuf));
						memset(pLogBuf,'\0',sizeof(pLogBuf));
						memset(pRecordBuf,'\0',sizeof(pRecordBuf));
					}
					//Maxwell 100317
					continue;
				}
			}

			if (!PTSC_Send(hPTSDev))
			{
                 SendtoPDBSFlag=0;
				 strTemp+=_T("failed");
			}
			else
			{
				RecordCounter++;
				strTemp+=_T("successfully.");
				ShowStatus(strTemp);
				
				return 1;
			}
		}
	}

    ShowStatus(strTemp);
	return 0;
}

int CAmMydasDlg::WriteLogData(char *pBuf, DWORD Len)
{
	if (!pBuf)
	{
		return 0;
	}

	//this->pLogBuf = pBuf;//Maxwell 100317
	strcpy_s(pLogBuf, sizeof(pLogBuf), pBuf);

	if (hPTSDev)
	{
		if (!PTSC_SetSendData(hPTSDev,pBuf,Len,1))//index 1 //Maxwell 100316
			//if (!PTSC_SetSendData(hPTSDev,pBuf,Len,1))//index 1 //Maxwell 100316
		{
			ShowStatus(_T("Set send log data error."));
			return 0;
		}
	}
	if (pBuf)
	{
		free(pBuf);// clear the buffer 
	}

	return 1;
}

//Maxwell 100316
int CAmMydasDlg::WriteMainData(char *pBuf, DWORD Len)
{

	char *pTempBuf=0;
	if (!pBuf)
	{
		return 0;
	}

	//this->pMainBuf = pBuf;//Maxwell 100317
	strcpy_s(pMainBuf, sizeof(pMainBuf), pBuf);

	if (hPTSDev)
	{
		if (!PTSC_SetSendData(hPTSDev,pBuf,Len,2))
		{
			ShowStatus(_T("Set send record data error."));
			return 0;
		}
	}
	if (pBuf)
	{
		free(pBuf);// clear the buffer 
	}

	return 1;
}
//Maxwell 100316

int CAmMydasDlg::WriteRecordData(char *pBuf, DWORD Len)
{
	char *pTempBuf=0;
	if (!pBuf)
	{
		return 0;
	}

	//this->pRecordBuf = pBuf;//Maxwell 100317
	strcpy_s(pRecordBuf, sizeof(pRecordBuf), pBuf);

	if (hPTSDev)
	{
		if (!PTSC_SetSendData(hPTSDev,pBuf,Len,0))//Maxwell 100316
			//if (!PTSC_SetSendData(hPTSDev,pBuf,Len,0))//Maxwell 100316
		{
			ShowStatus(_T("Set send record data error."));
			return 0;
		}
	}
	if (pBuf)
	{
		free(pBuf);// clear the buffer 
	}

	return 1;
}

void CAmMydasDlg::ExitPTSC()
{

	//disconnect PTS site
	if (hPTSDev)
	{
		PTSC_Disconnect(hPTSDev);
		hPTSDev=0;
	}
	return;
}



/////////////////////////////////////////////////////////////////////////
//For total managemnt all UI information and control logic
//It distribute UI control message for all sub dialog.
UINT WINAPI MYDASTask(LPVOID lpPara)
{
	CAmMydasDlg *pMydas=(CAmMydasDlg *)lpPara;
	MSG msg;
	int i=0;

	//Initial tester
	//pMydas->InitialPTSC();

	//process tester
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
			case WM_MYDAS_OPEN:
				pMydas->InitialPTSC();
				break;
					//Maxwell 100316
			case WM_MYDAS_MAIN_DATA:
				pMydas->WriteMainData((char *)msg.wParam, (DWORD)msg.lParam);
				pMydas->PostDataToPTS();
				break;
				//Maxwell 100316
			case WM_MYDAS_RECORD_DATA:
				pMydas->WriteRecordData((char *)msg.wParam, (DWORD)msg.lParam);
				break;
			case WM_MYDAS_LOG_DATA:
				pMydas->WriteLogData((char *)msg.wParam, (DWORD)msg.lParam);
				break;
			case WM_EXIT:
				goto ExitLabel;
				break;
			default:
				;
			}
		}
	}

ExitLabel:
	//exit tester
	pMydas->ExitPTSC();
	return 0;
}

int CAmMydasDlg::StartMYDASTask(void)
{

	AM_TASK_INFO TaskTemp;

	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		MYDASTask,
		this,
		0,
		&TaskTemp.uiTaskID 
		);

	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=MYDAS_TASK;//generate task id
		gUITaskInfoList.push_back(TaskTemp);//Insert the task information to mamagement list
	}
	else
	{
		return 0;
	}
	return 1;
}
HBRUSH CAmMydasDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	if (pWnd->GetDlgCtrlID() == IDC_STC_CONNECT_FLAG)
	{
		CString str;
		GetDlgItemText(IDC_STC_CONNECT_FLAG,str);
		pDC->SelectObject(&ConnectStatusFont);
		pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(0,0,0));
		if(!_tcscmp(_T("CONNECTED"), str))
		{
			return m_Greenbh;
		}
		if(!_tcscmp(_T("NOT CONNECTED"), str))
		{
			return m_Redbh;
		}
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
int strparse(char *pDes, int DesSize, char *pSrc,char start,char end)
{	
	char *pToke=NULL;
	char *pTempDes=NULL;
	char Temp[1024]="";
	int i=0;
	if ((!pDes)||(!pSrc))
	{
		return 0;
	}
	pTempDes=pDes;
	strcpy_s(Temp, sizeof(Temp), pSrc);
	pToke=strchr(Temp, start);
	if (pToke)
	{
		pToke+=1;
		for (i=0;i<1024;i++)
		{
			if ((end==*(pToke+i))||(0==*(pToke+i)))
			{
				*(pTempDes+i)=0;
				break;
			}
			*(pTempDes+i)=*(pToke+i);
		}
	}
	else
	{
		for (i=0;i<1024;i++)
		{
			if ((end==*(Temp+i))||(0==*(Temp+i)))
			{
				*(pTempDes+i)=0;
				break;
			}
			*(pTempDes+i)=*(Temp+i);
		}	
	}
	return 1;
}

void CAmMydasDlg::OnBnClickedButtonLog()
{
	// TODO: Add your control notification handler code here	
	int RowNum=0;
	int ColNum[10000]={0};
	int j=0,MaxCol=0;
	wchar_t FilePath[500];
	CString FullFileName;
	char strname[2048]="";
	char time[32];
	CString strTemp=_T("");
	CString strInst=_T("");
	int findstr=0;
	int sLen=0;
	CString strtime;
	CStdioFile StdioFile; 


	CDateTimeCtrl* pCtrl = (CDateTimeCtrl*) GetDlgItem(IDC_DATETIMEPICKER1);
	ASSERT(pCtrl != NULL);
	pCtrl->SetFormat(_T("yyyyMMdd"));    

	GetCurrentDirectory(sizeof(FilePath)/2, FilePath);
	FullFileName+=FilePath;
	FullFileName+=_T("\\TestResult\\");
	GetDlgItemText(IDC_DATETIMEPICKER1,strtime);
	
	FullFileName+=strtime+_T(".csv");
	WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)FullFileName,-1,strname,2048,0,false);
	//-----------------calculate row and column-----------------------	
	if (StdioFile.Open(FullFileName,CFile::modeRead)==TRUE)
	{
		while(StdioFile.ReadString(strTemp)!=FALSE)
		{ 
			if (strTemp.Right(1)!=_T(","))
			{
				ColNum[RowNum]++;
			}
			findstr=strTemp.Find(',',0);
			while (findstr!=-1)
			{
				ColNum[RowNum]++;
				strTemp=strTemp.Right(strTemp.GetLength()-findstr-1);
				findstr=strTemp.Find(',',0);
			}
			RowNum++;
		} 
		StdioFile.Close();
	}
	else
	{
		MessageBox(_T("Can not open file!"),_T("Tip"),MB_OK|MB_ICONWARNING);
		return;
	}
	
	/*if( (fopen_s( &stream,strname, "r+" )) != 0 ) 
	{
		if (pLogMyDas)
		{
			pLogMyDas->DeleteAllItems();
			pLogMyDas->RemoveAllGroups();
		}
		MessageBox( _T("Cannot open file\n"),_T("Tip"),MB_OK|MB_ICONERROR);
		return;
	}
	else
	{		
		while((ch=getc(stream))!=EOF)
		{
			if(ch=='\n') 
			{
				RowNum++;
			}
			else 
				if(ch==',') 
					ColNum[RowNum]++;				
		}
	}    */
	MaxCol=ColNum[0];
	for(j=1;j<RowNum;j++)
	if (MaxCol<ColNum[j])
	{
		MaxCol=ColNum[j];
	}
	//fclose(stream);
	//---------------initialize list-control--------------
	
	int m=0;
	CString strng=_T("");	
	/*if( (fopen_s( &stream,strname, "r+" )) != NULL ) 
	{
		MessageBox( _T("Cannot open file\n"),_T("Tip"),MB_OK|MB_ICONERROR);
		return;
	}*/
	
	if (pLogMyDas)
	{
		pLogMyDas->DestroyWindow();
		delete pLogMyDas;
		pLogMyDas=0;
	}
	pLogMyDas = (CListCtrl *)new CListCtrl;
	if (pLogMyDas)
	{
		pLogMyDas->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,	CRect(50,180,860, 545), this, 70006);
		pLogMyDas->ModifyStyle(0,LVS_REPORT);
		pLogMyDas->SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);		

		
		if (StdioFile.Open(FullFileName,CFile::modeRead)==TRUE)
		{
			StdioFile.ReadString(strTemp);
		}
		else
		{
            MessageBox(_T("Can not open file!"),_T("Tip"),MB_OK|MB_ICONWARNING);
			return;
		}
		//str=fgets(line,1024,stream); 
		//strTemp=CString(str);//		
		pLogMyDas->InsertColumn(0,_T("NUM"),LVCFMT_LEFT,50);
		for(m=1;m<ColNum[0];m++)
		{
			findstr=strTemp.Find(',',0);
			strng=strTemp.Mid(0,findstr);
			sLen=strTemp.GetLength();
			strTemp=strTemp.Mid(findstr+1,sLen-1);
			pLogMyDas->InsertColumn(m,strng,LVCFMT_LEFT,80);
		}
		strTemp.Remove(',');//add
		pLogMyDas->InsertColumn(m,strTemp,LVCFMT_LEFT,80);//add
		for (j=m+1;j<MaxCol+5;j++)
		{
			pLogMyDas->InsertColumn(j,_T(""),LVCFMT_LEFT,80);
		}
		pLogMyDas->ShowWindow(SW_SHOW);		
	}
	//-------------insert data---------------
	if (pLogMyDas)
	{
		pLogMyDas->DeleteAllItems();
		pLogMyDas->RemoveAllGroups();
	}		
	for (j=0;j<RowNum-1;j++)
	{
		strng.Format(_T("%d"),j+1);
		pLogMyDas->InsertItem(j, strng);
		StdioFile.ReadString(strTemp);
		//str=fgets(line,1024,stream); 
		//strTemp=CString(str);//		
		for(m=1;m<ColNum[j+1];m++)
		{
			findstr=strTemp.Find(',',0);
			strng=strTemp.Mid(0,findstr);
			sLen=strTemp.GetLength();
			strTemp=strTemp.Mid(findstr+1,sLen-1);
			pLogMyDas->SetItemText(j, m,strng);
		}
		strTemp.Remove(',');//add
		pLogMyDas->SetItemText(j, m,strTemp);//add
	}
	//fclose(stream);
	StdioFile.Close();
}