// TesterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "TesterDlg.h"
#include "EquipInfoShow.h"
#include "LCC.h"

int EquipDlgFlag=1;
CEquipInfoShow gEQIP;
extern list<EQUIP_USE> DeviceList[50];//090612
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store


IMPLEMENT_DYNAMIC(CTesterDlg, CDialog)

CTesterDlg::CTesterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTesterDlg::IDD, pParent)
{
	/*pdialog=this;*/

}

CTesterDlg::~CTesterDlg()
{
}

void CTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTesterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CTesterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_EQUIP_DLG_OPEN, &CTesterDlg::OnBnClickedEquipDlgOpen)
	ON_BN_CLICKED(IDC_EQUIP_DLG_CLOSE, &CTesterDlg::OnBnClickedEquipDlgClose)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTesterDlg message handlers

void CTesterDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}



BOOL CTesterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(gStationInfo.EquipInfo.EqipEnableFlag)
	{
		gEQIP.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP |ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_WANTRETURN|WS_HSCROLL|WS_VSCROLL|ES_READONLY, 
			CRect(15,30,400,600), this, 89907876); 
		gEQIP.ShowWindow(SW_SHOW);
		//remove start by Talen 2011/07//21
		//////if(gStationInfo.EquipInfo.EqipXmlFlag)
		//////{
		//////	gEQIP.EquipShowInfoXml();
		//////	GetDlgItem(IDC_EQUIP_DLG_OPEN)->EnableWindow(false);
		//////	GetDlgItem(IDC_EQUIP_DLG_CLOSE)->EnableWindow(false);
		//////}	
		//remove end by Talen 2011/07//21
		gEQIP.EquipShowInfo();//add by Talen 2011/07/21
		StartEquipTask();
	}
	else
	{

	}

	UtiliFont.CreatePointFont(150, _T("Arial"));
	GetDlgItem(IDC_TESTER_UTI)->SetFont(&UtiliFont);
	GetDlgItem(IDC_TESTER_UTI2)->SetFont(&UtiliFont);
	SetTimer(UTI_TIMER,5000, (TIMERPROC)NULL);//haibin.li 2011/11/07
	/*if (gStationInfo.MultiTaskSetting.Number==2)
	{
		GetDlgItem(IDC_TESTER_UTI2)->ShowWindow(TRUE);
	}*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

UINT WINAPI EQUIPTask(LPVOID lpPara)
{
	//CTesterDlg *pTester=(CTesterDlg *)lpPara;
	MSG msg;
	int i=0;
	size_t ConvertedChars=0;
	char TempBuf[1024]="";
	wchar_t RevBuf[1024]= _T("");
	UINT FixNum=0;
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
			case WM_UPDATE_INFO_EQUIP:
				FixNum=(UINT)msg.lParam-UI_SERVER_PORT_START+1;
				if ((char *)msg.wParam)              // clear the bufferWM_EQUIPMENT_OPEN
				{
					free((char *)msg.wParam);
				}
				//remove start by Talen 2011/07/21
			////////	if(gStationInfo.EquipInfo.EqipXmlFlag)
			////////	{
			////////		gEQIP.CountNumXml(FixNum);
			////////	}
			////////	else
			////////	{
			////////		gEQIP.CountNum(FixNum);
			////////	}
			////////	break;
			////////case WM_EQUIPMENT_OPEN:
			////////	if(!gStationInfo.EquipInfo.EqipXmlFlag)
			////////	{
			////////		gEQIP.InitialEQUIP();					
			////////	}
				//remove end by Talen 2011/07/21
				gEQIP.CountNum(FixNum);
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
	return 0;
}

int CTesterDlg::StartEquipTask(void) 
{

	AM_TASK_INFO TaskTemp;

	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		EQUIPTask,
		this,
		0,
		&TaskTemp.uiTaskID 
		);

	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=EQUIP_VIEW_TASK;         //generate task id
		gUITaskInfoList.push_back(TaskTemp);//Insert the task information to management list
	}
	else
	{
		return 0;
	}
	return 1;
}


void CTesterDlg::OnBnClickedEquipDlgOpen()
{
	// TODO: Add your control notification handler code here
	if (EquipDlgFlag)
	{
		EquipDlgFlag=0;
		AmbitEquip.Create(IDD_EQUIPMENT_DLG, this);
		AmbitEquip.ShowWindow(SW_SHOW);
	}
	else
	{
		AmbitEquip.ShowWindow(SW_SHOW);
		//AfxMessageBox(_T("Have already log dialog open!"));//vincent 20090706
	}

}

void CTesterDlg::OnBnClickedEquipDlgClose()
{
	// TODO: Add your control notification handler code here
	if(!EquipDlgFlag)
	{
		AmbitEquip.ShowWindow(SW_HIDE);
	}
}

HBRUSH CTesterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	if (pWnd->GetDlgCtrlID() == IDC_TESTER_UTI)
	{
		if(Utilization[0]>=80.0)
			pDC->SetTextColor(RGB(0,255,0));		
		else
			pDC->SetTextColor(RGB(255,0,0));
	}
	if (pWnd->GetDlgCtrlID() == IDC_TESTER_UTI2)
	{		
		if(Utilization[1]>=80.0)
			pDC->SetTextColor(RGB(0,255,0));
		else
			pDC->SetTextColor(RGB(255,0,0));
	}
	return hbr;
}

void CTesterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (UTI_TIMER==nIDEvent)
	{
		CString str;
		str.Format(_T("Utilization 1:     %.1f"),Utilization[0]);
		str+=_T("%");
		SetDlgItemText(IDC_TESTER_UTI,str);
		str.Format(_T("%.1f"),Utilization[0]);str+=_T("%");
		dlgrtm->m_list_result.SetItemText(4,1,str);
		str.Format(_T("Utilization 2:     %.1f"),Utilization[1]);
		str+=_T("%");
		SetDlgItemText(IDC_TESTER_UTI2,str);
		str.Format(_T("%.1f"),Utilization[1]);str+=_T("%");
		if (2==gStationInfo.MultiTaskSetting.Number)
		{
			dlgrtm->m_list_result.SetItemText(4,2,str);
		}		
	}	

	CDialog::OnTimer(nIDEvent);
}
