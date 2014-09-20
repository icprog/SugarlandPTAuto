// MainControlPanelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "MainControlPanelDlg.h"
#include "AmThreadAdmin.h"
#include "ConfigFile.h"
#include "AmParser.h"
#include "DialogUser.h"//haibin.li 2011/11/11


G_VARIABLE gVariable;//Store global information for all test.
AM_PROT_INFO gStationInfo;//Store global information for all test.
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
extern int sfisStartFlag;//add by Talen 2011/10/26
CMainControlPanelDlg *pTempCtr;//haibin.li 2011/11/10
//extern USER_INFO userinfo;//haibin.li 2011/11/10
USER_PRIO UserPrio;//haibin.li 2012/1/18

//extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
UINT WINAPI UICentreControlTask(LPVOID lpPara);

CEditShow *pEditPort;//haibin.li 2011/11/07

//AM_PROT_INFO gAmProtInfo;//test station over all information storege

// CMainControlPanelDlg dialog

IMPLEMENT_DYNAMIC(CMainControlPanelDlg, CDialog)

CMainControlPanelDlg::CMainControlPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainControlPanelDlg::IDD, pParent)
{
	gVariable.SMOConnFlag=0;
	gVariable.UutScannerFlag=0;
	gStationInfo.PtsInfo.ConnectStatus=0;
	UserPrio.num=0;
	testflag[0]=false;//haibin.li 2012/2/9
	testflag[1]=false;//haibin.li 2012/2/9
}

CMainControlPanelDlg::~CMainControlPanelDlg()
{
	/*if(pEditPort) 
	{ 
		delete pEditPort; 
	}*/
}

void CMainControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainControlPanelDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_SWTO_TESTER, &CMainControlPanelDlg::OnBnPostSwitchTester)
	ON_BN_CLICKED(IDC_BTN_SWTO_MT_DLG, &CMainControlPanelDlg::OnBnPostSwitchMTestDlg)
	ON_BN_CLICKED(IDC_BTN_SWTO_MYDAS_DLG, &CMainControlPanelDlg::OnBnPostSwitchMydasDlg)
	ON_BN_CLICKED(IDC_BTN_SWTO_SFIS_DLG, &CMainControlPanelDlg::OnBnPostSwitchSfisDlg)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMainControlPanelDlg message handlers

BOOL CMainControlPanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here 

	ControlCreate();

	MainLogicInitialization();
	Sleep(100);
	m_brushdlg.CreateSolidBrush(RGB(245,245,245));
	m_brushstatic.CreateSolidBrush(RGB(240,240,240));	
	//((CStatic*)GetDlgItem(IDC_SMO_PICTURE))->SetIcon(AfxGetApp()->LoadIcon(IDI_UNLINK));
	static CFont m_Font;
	m_Font.CreateFont(25,0,0,0,0,0,0,0,0,0,0,0,0,_T("Arial"));  
	GetDlgItem(IDC_SFIS_LINK)->SetFont(&m_Font);
	
	GetDlgItem(IDC_STA_NAME)->SetFont(&m_Font);//Haibin.Li 06.27
	GetDlgItem(IDC_UUT_PNNAME)->SetFont(&m_Font);//Haibin.Li 06.27	

	GetDlgItem(IDC_SFIS_LINK)->SetWindowText(_T("SMO OFF"));

	static CFont m_Font1;
	m_Font1.CreateFont(15,0,0,0,0,0,0,0,0,0,0,0,0,_T("Arial"));  
	GetDlgItem(IDC_EDIT_DCT_SHOW)->SetFont(&m_Font1);
	GetDlgItem(IDC_UUT_SFIS_INFO)->SetFont(&m_Font1);

	m_Redbh.CreateSolidBrush(RGB(255,0,0));
	m_Greenbh.CreateSolidBrush(RGB(0,255,0));
	m_Yellowbh.CreateSolidBrush(RGB(255,255,0));
	m_Grancybh.CreateSolidBrush(RGB(82,102,251));
	m_Bkbh.CreateSolidBrush(RGB(255,255,255));

	GetDlgItem(IDC_TIME)->MoveWindow(825,10,80,50);
	UtiliFont.CreatePointFont(250, _T("Arial"));
	Scan.CreatePointFont(90, _T("Arial"));
	GetDlgItem(IDC_TIME)->SetFont(&UtiliFont);
	GetDlgItem(IDC_STATIC_SCAN)->SetFont(&Scan);
	temptime=_T("00:00");
	SetTimer(TESTING_TIMER,500, (TIMERPROC)NULL);//haibin.li 2011/11/05
	GetDlgItem(IDC_USER_ID)->MoveWindow(920,10,104,20);
	GetDlgItem(IDC_STATIC_IN)->MoveWindow(920,30,40,20);
	GetDlgItem(IDC_STATIC_OUT)->MoveWindow(965,30,40,20);
	//m_hCursor=AfxGetApp()->LoadCursor(IDC_ARROW);//haibin.li 2011/11/10 load mouse
	if(gStationInfo.MultiTaskSetting.Number==2)
	{
		GetDlgItem(IDC_TIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_USER_ID)->MoveWindow(870,10,104,20);
	    GetDlgItem(IDC_STATIC_IN)->MoveWindow(870,30,40,20);
	    GetDlgItem(IDC_STATIC_OUT)->MoveWindow(915,30,40,20);
	}
	GetDlgItem(IDC_STATIC_OUT)->GetWindowRect(&m_StaticRectOut);//haibin.li 2011/11/10
	ScreenToClient(&m_StaticRectOut);//haibin.li 2011/11/10
	GetDlgItem(IDC_STATIC_IN)->GetWindowRect(&m_StaticRectIn);//haibin.li 2011/11/11
	ScreenToClient(&m_StaticRectIn);//haibin.li 2011/11/11
	LoadPriority(_T("Priority.txt"));//haibin.li 2012/1/18
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMainControlPanelDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CMainControlPanelDlg::OnBnPostSwitchTester()
{
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_TESTER_DLG, 0, 0);
}

void CMainControlPanelDlg::OnBnPostSwitchMTestDlg()
{
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);
}

void CMainControlPanelDlg::OnBnPostSwitchMydasDlg()
{
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MYDAS_DLG, 0, 0);
}

void CMainControlPanelDlg::OnBnPostSwitchSfisDlg()
{
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_SFIS_DLG, 0, 0);
}

int CMainControlPanelDlg::ControlCreate(void)
{
	ListControlCreate();
	StationNameFont.CreatePointFont(200, _T("Arial"));
	SetDlgItemText(IDC_STA_NAME, _T("-----------"));
	SetDlgItemText(IDC_UUT_PNNAME, _T("-----------"));

	return 0;
}

void CMainControlPanelDlg::InitialMainPanelInfor()
{
	//initial test station name and UUT PN name
	if (!wcslen(gStationInfo.TestStation.Station_Name))
	{
		SetDlgItemText(IDC_STA_NAME, _T("-----------"));
	}
	else
	{
		SetDlgItemText(IDC_STA_NAME, gStationInfo.TestStation.Station_Name);
	}

	DisplayUUTPnName(_T("-----------"));

	//Initial system information
	ListShow(&StationList, 0, _T("Version"), gStationInfo.TestStation.TestProgramVersion);
	ListShow(&StationList, 1, _T("System ID"),gStationInfo.TestStation.TestStaID);
	ListShow(&StationList, 2, _T("Host Name"), gStationInfo.PcInfo.HostName);
	ListShow(&StationList, 3, _T("IP"), gStationInfo.PcInfo.IPAddress);

	//product information
	ListShow(&UUTInfoList, 0, _T("Product"), gStationInfo.uut.UUTName);
	ListShow(&UUTInfoList, 1, _T("MO"), gStationInfo.uut.UUTMO);
	ListShow(&UUTInfoList, 2, _T("PN"), gStationInfo.uut.UUTPN);	

	//SFIS&MYDAS information	
	//ListShow(&SFICInfoList, 0, _T("SFIS INFO:"), gStationInfo.uut.UUTName);
	//ListShow(&SFICInfoList, 1, _T("LINK STATUS:"), gStationInfo.uut.UUTMO);
	//ListShow(&SFICInfoList, 2, _T("EMPLOYE ID:"), gStationInfo.uut.UUTPN);

	ListShow(&MYDASInfoList, 0, _T("PTS IP:"), gStationInfo.PtsInfo.PtsServerIP);
	ListShow(&MYDASInfoList, 1, _T("LINK STATUS:"), _T("OFF"));
	ListShow(&MYDASInfoList, 2, _T("RECORD:"), gStationInfo.uut.UUTPN);

	return;
}

void CMainControlPanelDlg::UpdateSystemInfo()
{
	ListShowUpdate(&StationList, 0, gStationInfo.TestStation.TestProgramVersion);
	ListShowUpdate(&StationList, 1, gStationInfo.TestStation.TestStaID);
	ListShowUpdate(&StationList, 2, gStationInfo.PcInfo.HostName);
	ListShowUpdate(&StationList, 3, gStationInfo.PcInfo.IPAddress);
	
	SetDlgItemText(IDC_STA_NAME, gStationInfo.TestStation.Station_Name);
	if (dlgrtm)
	{
		dlgrtm->m_list_runstatus.SetItemText(0,1,gStationInfo.TestStation.Station_Name);//haibin.li 2011/12/5	
	}
	return;
}

void CMainControlPanelDlg::UpdateProductInfo()
{
	ListShowUpdate(&UUTInfoList, 0, gStationInfo.uut.UUTName);
	ListShowUpdate(&UUTInfoList, 1, gStationInfo.uut.UUTMO);
	ListShowUpdate(&UUTInfoList, 2, gStationInfo.uut.UUTPN);
	DisplayUUTPnName(gStationInfo.uut.UUTPN);
	
	return;
}
void CMainControlPanelDlg::UpdateSFISInfo()
{
	//CString strTemp;
	//ListShowUpdate(&SFICInfoList, 0, gStationInfo.SfisInfo.ServerIP);
	//if (gStationInfo.SfisInfo.SfisON_OFF)
	//{
    //   strTemp=_T("ON");
	//}
	//else
	//{
    //   strTemp=_T("OFF");
	//}
	//ListShowUpdate(&SFICInfoList, 1, strTemp);
	//ListShowUpdate(&SFICInfoList, 2, gStationInfo.Operator.EmployeID);
	return;
}
void CMainControlPanelDlg::UpdateMYDASInfo()
{
	CString strTemp;
	ListShowUpdate(&MYDASInfoList, 0,gStationInfo.PtsInfo.PtsServerIP);
	
	if (gStationInfo.PtsInfo.ConnectStatus)
	{
       strTemp=_T("ON");
	}
	else
	{
       strTemp=_T("OFF");
	}
	ListShowUpdate(&MYDASInfoList, 1, strTemp);
	ListShowUpdate(&MYDASInfoList, 2, gStationInfo.uut.UUTPN);
	
	return;
}

void CMainControlPanelDlg::AmGetBasicInfo()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Get host name 
	char HostName[100];
	size_t cLen=0;
	BYTE IP[4];

	gethostname(HostName, sizeof(HostName));
	mbstowcs_s(&cLen, gStationInfo.PcInfo.HostName, sizeof(gStationInfo.PcInfo.HostName)/2, HostName, sizeof(HostName));

	//Get Ip address.
	struct hostent FAR * lpHostEnt=gethostbyname(HostName);
	if(lpHostEnt!=NULL)
	{
		//get ip
		LPSTR lpAddr=lpHostEnt->h_addr_list[0];
		if(lpAddr)
		{
			struct in_addr inAddr;
			memmove(&inAddr,lpAddr,4);
			IP[0]=inAddr.S_un.S_un_b.s_b1;
			IP[1]=inAddr.S_un.S_un_b.s_b2;
			IP[2]=inAddr.S_un.S_un_b.s_b3;
			IP[3]=inAddr.S_un.S_un_b.s_b4;

			swprintf_s(gStationInfo.PcInfo.IPAddress, sizeof(gStationInfo.PcInfo.IPAddress)/2, _T("%d.%d.%d.%d"), IP[0],IP[1],IP[2],IP[3]);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return;
}

int CMainControlPanelDlg::ListShow(CListCtrl *pList, int index, wchar_t *pName, wchar_t *pContent)
{
	if (!pList || !pName || !pContent)
	{
		return 0;
	}
	pList->InsertItem(index,pName);
	pList->SetItemText(index,1,pContent);
	return 1;
}
int CMainControlPanelDlg::ListShowUpdate(CListCtrl *pList, int index, LPCTSTR pContent)
{
    pList->SetItemText(index,1,pContent);
	return 1;
}
//changed start by Haibin.Li 06.24
int CMainControlPanelDlg::ListControlCreate()
{
	CRect DlgRect;
	LONG HSize=178+50;
	LONG VSize=50;
	LONG XStart=1;
	LONG YStart=95;
	LONG Interval=30;
	LONG lTemp=0;
	lTemp=YStart;
	VSize=60;
	StationList.Create(
	WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOCOLUMNHEADER,
	CRect(XStart,lTemp, HSize, lTemp+VSize-3), this, 70001);
	StationList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 70+25);
	StationList.InsertColumn(1, _T("Content"), LVCFMT_LEFT, 100+25);
	GetDlgItem(IDC_BORDER1)->MoveWindow(0, lTemp-2, HSize,1);
	lTemp = YStart+VSize;
	UUTInfoList.Create(
	WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOCOLUMNHEADER,
	CRect(XStart,lTemp,HSize, lTemp+VSize), this, 70002);
	UUTInfoList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 70+25);
	UUTInfoList.InsertColumn(1, _T("Content"), LVCFMT_LEFT, 100+25);
	GetDlgItem(IDC_BORDER2)->MoveWindow(0, lTemp-1, HSize,1);
	GetDlgItem(IDC_BORDER3)->MoveWindow(0, lTemp+VSize, HSize,1);
	lTemp = lTemp+Interval+VSize+3;
	MYDASInfoList.Create(
	WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_NOCOLUMNHEADER,
	CRect(XStart,lTemp,HSize, lTemp+VSize), this, 70004);
	MYDASInfoList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 90+25);
	MYDASInfoList.InsertColumn(1, _T("Content"), LVCFMT_LEFT, 80+25);
	GetDlgItem(IDC_EDIT_DNBORDER)->MoveWindow(0, lTemp-1, HSize,1);
	lTemp = lTemp+VSize+3;
	pEditPort=new CEditShow; 
	if(pEditPort!=NULL)
	{
		pEditPort->Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_READONLY, 
		CRect(0,0,10,10), this, IDC_EDIT_DCT_SHOW); 
	}
	else
	{
		AfxMessageBox(_T("Error Creating pEditPort"));
	}
	if(pEditPort)
	{
		pEditPort->MoveWindow(595, 5, 225, 60);
		//pEditPort->MoveWindow(XStart, lTemp, HSize, VSize);
		GetDlgItem(IDC_EDIT_UPBORDER)->MoveWindow(0, lTemp-1, HSize,1);
		GetDlgItem(IDC_BORDER4)->MoveWindow(0, lTemp+VSize+1, HSize,1);
	}

	return 0;	
}
//changed end by Haibin.Li 06.24

HBRUSH CMainControlPanelDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);    
	
	if (pWnd->GetDlgCtrlID() == IDC_STA_NAME||pWnd->GetDlgCtrlID() == IDC_UUT_PNNAME)
	{
		pDC->SelectObject(&StationNameFont);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_DCT_SHOW)
	{
		//pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(RGB(255,255,255));
		CBrush m_brush;
		m_brush.CreateSolidBrush(RGB(225,255,255));
		return m_brush;
	}
	if (pWnd->GetDlgCtrlID() == IDC_SFIS_LINK)
	{
		if(gVariable.SMOConnFlag)
		{
			pDC->SetTextColor(RGB(0,255,0));
			pDC->SetBkMode(TRANSPARENT);
		}
		else
		{
			pDC->SetTextColor(RGB(255,0,0));
			pDC->SetBkMode(TRANSPARENT);
		}
	}
	if (pWnd->GetDlgCtrlID() == IDC_UUT_SFIS_INFO)
	{
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
		if(gVariable.UutScannerFlag)
		{
			return m_Greenbh;
		}
		else
		{
			CBrush m_brush;
			m_brush.CreateSolidBrush(RGB(225,255,255));
			return m_brush;
			//return hbr;
		}
	}
	
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_SCAN)
	{
		pDC->SetBkMode(RGB(255,255,255));		
	}
	//haibin.li 2011/11/08
	if (pWnd->GetDlgCtrlID()==IDC_TIME)
	{
		pDC->SetBkMode(TRANSPARENT);
	}
	if (pWnd->GetDlgCtrlID()==IDC_USER_ID)
	{
		pDC->SetBkMode(TRANSPARENT);
	}
	if (pWnd->GetDlgCtrlID()==IDC_STATIC_OUT)
	{
		pDC->SetBkMode(TRANSPARENT);
	}
	if (pWnd->GetDlgCtrlID()==IDC_STATIC_IN)
	{
		pDC->SetBkMode(TRANSPARENT);
	}

	// TODO:  Return a different brush if the default is not desired
	//return hbr;
	return m_brushstatic;
}

void CMainControlPanelDlg::DisplayStationName(WCHAR *p)
{
	if (p)
	{
		SetDlgItemText(IDC_STA_NAME, p);
		dlgrtm->m_list_runstatus.SetItemText(0,1,p);//haibin.li 2011/12/5	
	}
	return;
}

void CMainControlPanelDlg::DisplayUUTPnName(WCHAR *p)
{
	if (p)
	{
		SetDlgItemText(IDC_UUT_PNNAME, p);
	    //dlgrtm->m_list_runstatus.SetItemText(1,1,p);//haibin.li 2011/12/5	
	}
	return;
}

int CMainControlPanelDlg::MainLogicInitialization(void)
{
	AM_TASK_INFO TaskTemp;
	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		UICentreControlTask,
		this,
		0,
		&TaskTemp.uiTaskID 
		);

	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=MAIN_CONTROL_TASK;
        gUITaskInfoList.push_back(TaskTemp);//Insert the task information to management list
	}
	else
	{
		return 0;
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////
//For total management all UI information and control logic
//It distribute UI control message for all sub dialog.
UINT WINAPI UICentreControlTask(LPVOID lpPara)
{
	CMainControlPanelDlg *pMCPDlg=(CMainControlPanelDlg *)lpPara;
	pTempCtr=pMCPDlg;//haibin.li 2011/11/10
//
	MSG msg;
	int i=0;
	char TempBuf[1024]="";
	DWORD DataLen=0;

	//Initial tester	
	if (!pMCPDlg->LoadSystemConfigration(_T("UIConfig.ini")))
	{
		//need to terminate all task.
		//return 1;
	}

	pMCPDlg->AmGetBasicInfo();
	pMCPDlg->InitialMainPanelInfor();
	sfisStartFlag=0;
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
			case WM_UPDATE_INFO_STATION:
                pMCPDlg->UpdateSystemInfo();
				//090618
				AmPostThreadMes(MYDAS_TASK, WM_MYDAS_OPEN, 0, 0);
				AmPostThreadMes(EQUIP_VIEW_TASK, WM_EQUIPMENT_OPEN, 0, 0);//vincent 090916
				break;
			case WM_UPDATE_INFO_SFIS:
			    pMCPDlg->UpdateSFISInfo();
				break;
			case WM_UPDATE_INFO_MYDAS:
				pMCPDlg->UpdateMYDASInfo();	
				break;
			case WM_UPDATE_INFO_PRODUCT:
				pMCPDlg->UpdateProductInfo();	
				break;
			case WM_UPDATE_INFO_SCAN: //wait for the port or the sn&mac?
				pMCPDlg->UpdateScanInfo((unsigned char *)msg.wParam, (DWORD)msg.lParam);	
				break;
			case WM_DCT_DATA://Show the sfis link status and the data which recv from the smo
				DataLen=(UINT)msg.lParam;
				memset(TempBuf,0,sizeof(TempBuf));
				memcpy_s(TempBuf, sizeof(TempBuf),(char *)msg.wParam,DataLen);
				if ((char *)msg.wParam)              // clear the buffer
				{
					free((char *)msg.wParam);
				}	
				pMCPDlg->ParseDctData(TempBuf);
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
	return 0;
}

int CMainControlPanelDlg::LoadSystemConfigration(CString FileName)
{
	CConfigFile ConfigFileParse;

	if (!ConfigFileParse.PerformParse(FileName))
	{
		MessageBox(_T("Can not load system configuration. please check file 'UIConfig.ini.'"),_T("SYSTEM ERROR 01"), MB_OK);
		return 0;
	}
	return 1;
}

void CMainControlPanelDlg::ParseDctData(char *pBuf)
{
	CString str;
	char TempBuf[1024]="";
	wchar_t wTempBuf[1024]=_T("");
	size_t bufL;
	size_t cLen;
	strcpy_s(TempBuf,sizeof(TempBuf),pBuf);
	bufL=strlen(TempBuf);
	mbstowcs_s(&cLen,wTempBuf,bufL+1,TempBuf,bufL);
	str=wTempBuf;
	pEditPort->SetWindowText(str);
	if(gVariable.SMOConnFlag)
	{
		GetDlgItem(IDC_SFIS_LINK)->SetWindowText(_T("SMO ON"));
		//((CStatic*)GetDlgItem(IDC_SMO_PICTURE))->SetIcon(AfxGetApp()->LoadIcon(IDI_LINK));
		//for(int index=0;index<gStationInfo.MultiTaskSetting.Number;index++)
		//{
		//	AmPostThreadMes(TEST_TASK_TYPE+index, WM_DCT_DATA, 0, 0);
		//}
	}
}

void CMainControlPanelDlg::UpdateScanInfo(unsigned char *pBuf, DWORD ulSize)
{
	char TempBuf[1000]="";
	wchar_t wcBuf[1000];
	wchar_t ResultBuf[200];
	size_t cLen;
	CAmParser Parser;
	if (!pBuf)
	{
		return;
	}
	strcpy_s(TempBuf,sizeof(TempBuf),(char *)pBuf);
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, TempBuf, ulSize);
	if(cLen>=200||cLen==0)
	{
		return;
	}
	else
	{
		if(Parser.ParserGetPara(wcBuf,  _T("SCANNER"), ResultBuf, sizeof(ResultBuf)/2))
		{
			gVariable.UutScannerFlag=_wtoi(ResultBuf);
			if(gVariable.UutScannerFlag)
			{
				GetDlgItem(IDC_UUT_SFIS_INFO)->SetWindowText(_T("PORT NUM"));
			}
			else
			{
				GetDlgItem(IDC_UUT_SFIS_INFO)->SetWindowText(_T("BARCODE"));
			}
		}
		else
		{
			return ;
		}
	}
}
void CMainControlPanelDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TESTING_TIMER==nIDEvent)
	{	
		if (TestingTime!=temptime)
		{
			SetDlgItemText(IDC_TIME,TestingTime);		
			temptime=TestingTime;
		}
	}	

	CDialog::OnTimer(nIDEvent);
}
void CMainControlPanelDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( point.x > m_StaticRectOut.left && point.x < m_StaticRectOut.right &&
		point.y > m_StaticRectOut.top  && point.y < m_StaticRectOut.bottom ) 
	{ 
		//SetCursor(m_hCursor); 
	}

	CDialog::OnMouseMove(nFlags, point);
}
void CMainControlPanelDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( point.x > m_StaticRectOut.left && point.x < m_StaticRectOut.right &&point.y > m_StaticRectOut.top  &&
		point.y < m_StaticRectOut.bottom && pTempCtr->GetDlgItem(IDC_STATIC_OUT)->IsWindowEnabled())
	{
		//SetCursor(m_hCursor);
		if(MessageBox(_T("確定要登出嗎?"),_T("Tip"),MB_OKCANCEL|MB_ICONWARNING)==IDOK)
		{		
			/*ShellExecute(NULL,"open",TEXT("http://www.tlu.edu.cn"),NULL,NULL, SW_SHOWNORMAL);*/
			dlgrtm->m_list_userinfo.SetItemText(0,1,_T(""));
			GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_IN)->EnableWindow(TRUE);
			GetDlgItem(IDC_USER_ID)->SetWindowText(_T("UserID:----------"));
			CString strSend=_T("[LOGOUT:")+userinfo.name+_T("]");
			USES_CONVERSION;LPCSTR lpcs = NULL;lpcs =T2A(strSend.GetBuffer(strSend.GetLength()));
			SendRTM(lpcs,ThreadID);	
			userinfo.name=_T("");
		}
	}
	if( point.x > m_StaticRectIn.left && point.x < m_StaticRectIn.right &&point.y > m_StaticRectIn.top  &&
		point.y < m_StaticRectIn.bottom && pTempCtr->GetDlgItem(IDC_STATIC_IN)->IsWindowEnabled())
	{
		CDialogUser dlguser;
		if(dlguser.DoModal()==IDCANCEL)
		{
			return;
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}
void CMainControlPanelDlg::LoadPriority(CString filename)
{
	CString strTemp=filename;
	CString content=_T("");
	CString str=_T("");
	int i=0;	
	if (!strTemp.Compare(_T("")))
	{
		return;
	}

	CStdioFile   cFile; 
	if(cFile.Open(strTemp,CFile::modeRead)==TRUE)
	{ 				
		cFile.ReadString(content);
		
		while (content.Compare(_T("")))
		{
			i=content.Find(',');
			if (i==-1)
			{
				str=content.Trim();
				if (str.Compare(_T("")))
				{
					UserPrio.UserId[UserPrio.num++]=str;
				}
				break;
			}
			else if(i==0)
			{
				content=content.Right(content.GetLength()-1);
			}
			else
			{
				str=content.Left(i).Trim();
				if(str.Compare(_T("")))
				{
					UserPrio.UserId[UserPrio.num++]=str;;
				}
				content=content.Right(content.GetLength()-i-1);
			}                
		}
		
		cFile.Close();
		DeleteFile(strTemp);
	}
}

