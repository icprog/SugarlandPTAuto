// DialogMain.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "DialogMain.h"
#include "AmThreadAdmin.h"

// CDialogMain dialog
extern G_VARIABLE gVariable;//haibin.li 2011/11/08
extern int SfisStatus;//haibin.li 2011/11/08
extern AM_PROT_INFO gStationInfo;
CTabCtrl m_tab;
extern CMainTestPanel *pTemp;//haibin.li 2011/11/09


IMPLEMENT_DYNAMIC(CDialogMain, CDialog)

CDialogMain::CDialogMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMain::IDD, pParent)
{

}

CDialogMain::~CDialogMain()
{

}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(CDialogMain, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDialogMain::OnTcnSelchangeTab1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogMain message handlers
BOOL CDialogMain::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here	
	CRect ViewRect;	
	GetClientRect(ViewRect);

	m_tab.InsertItem(0,_T("TESTER")); 
	m_tab.InsertItem(1,_T("DETAIL")); 
	m_tab.InsertItem(2,_T("SFIS")); 
	m_tab.InsertItem(3,_T("MYDAS")); 
	m_tab.InsertItem(4,_T("CONF")); 
	m_tab.InsertItem(5,_T("EQUIP")); 
	m_tab.InsertItem(6,_T("RTM")); 
	m_tab.SetWindowPos(NULL, ViewRect.top, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size

	SetTimer(ICON_TIMER,1000, (TIMERPROC)NULL);//haibin.li 2011/11/05
	tempsmoflag=0;
	tempsfisflag=0;
	tempequipflag=0;
	tempptsflag=0;
	m_tab.SetCurSel(0); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMain::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int CurSel;  
	CurSel=m_tab.GetCurSel();  
	switch(CurSel)  
	{  
	case 0:  
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);Invalidate();
		//UpdateWindow();
		break;  
	case 1:  	
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_DETAIL_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;  
	case 2:  	
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_SFIS_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;  
	case 3:  		
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MYDAS_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;
	case 4:  		
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_CONF_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;
	case 5:  		
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_TESTER_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;
	case 6:  		
		AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_RTM_DLG, 0, 0);Invalidate();
		//UpdateWindow();
		break;
	default: ;  
	}  
	
	*pResult = 0;
}

void CDialogMain::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default	
	if (ICON_TIMER==nIDEvent)
	{
		if (tempsmoflag!=gVariable.SMOConnFlag)
		{
			if (gVariable.SMOConnFlag)
			{
				((CStatic*)GetDlgItem(IDC_PIC_SMO))->SetIcon(AfxGetApp()->LoadIcon(IDI_SMO_ON));
				SendRTM("[LINKSTATUS:SMO-ON]",ThreadID);
			}
			else 
			{
				((CStatic*)GetDlgItem(IDC_PIC_SMO))->SetIcon(AfxGetApp()->LoadIcon(IDI_SMO_OFF));
				SendRTM("[LINKSTATUS:SMO-OFF]",ThreadID);
			}
			tempsmoflag=gVariable.SMOConnFlag;
		}

		if (tempequipflag!=gStationInfo.EquipInfo.EqipCtlFlag)
		{
			if (gStationInfo.EquipInfo.EqipCtlFlag==1)
			{
				((CStatic*)GetDlgItem(IDC_PIC_EQUIP))->SetIcon(AfxGetApp()->LoadIcon(IDI_EQUI_ON));
				dlgrtm->m_list_linkstatus.SetItemText(2,1,_T("ON"));
				SendRTM("[LINKSTATUS:EQUIPMENT-ON]",ThreadID);
			}
			else
			{
				((CStatic*)GetDlgItem(IDC_PIC_EQUIP))->SetIcon(AfxGetApp()->LoadIcon(IDI_EQUI_OFF));
				dlgrtm->m_list_linkstatus.SetItemText(2,1,_T("OFF"));
				SendRTM("[LINKSTATUS:EQUIPMENT-OFF]",ThreadID);
			}
			tempequipflag=gStationInfo.EquipInfo.EqipCtlFlag;
		}	

		if (tempsfisflag!=SfisStatus)
		{
			if (SfisStatus==1||SfisStatus==2)
			{
				((CStatic*)GetDlgItem(IDC_PIC_SFIS))->SetIcon(AfxGetApp()->LoadIcon(IDI_SFIS_ON));
				dlgrtm->m_list_linkstatus.SetItemText(0,1,_T("ON"));
			    SendRTM("[LINKSTATUS:SFIS-ON]",ThreadID);
			}
			else
			{
				((CStatic*)GetDlgItem(IDC_PIC_SFIS))->SetIcon(AfxGetApp()->LoadIcon(IDI_SFIS_OFF));
			    dlgrtm->m_list_linkstatus.SetItemText(0,1,_T("OFF"));
				SendRTM("[LINKSTATUS:SFIS-OFF]",ThreadID);
			}
			tempsfisflag=SfisStatus;
		}	

		if (tempptsflag!=gStationInfo.PtsInfo.ConnectStatus)
		{
			if (gStationInfo.PtsInfo.ConnectStatus==1)
			{
				((CStatic*)GetDlgItem(IDC_PIC_PTS))->SetIcon(AfxGetApp()->LoadIcon(IDI_PTS_ON));
			    dlgrtm->m_list_linkstatus.SetItemText(1,1,_T("ON"));
				SendRTM("[LINKSTATUS:PTS-ON]",ThreadID);
			}
			else
			{
				((CStatic*)GetDlgItem(IDC_PIC_PTS))->SetIcon(AfxGetApp()->LoadIcon(IDI_PTS_OFF));
			    dlgrtm->m_list_linkstatus.SetItemText(1,1,_T("OFF"));
				SendRTM("[LINKSTATUS:PTS-OFF]",ThreadID);
			}
			tempptsflag=gStationInfo.PtsInfo.ConnectStatus;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
BOOL CDialogMain::PreTranslateMessage(MSG* pMsg)   
{ 
	if(pMsg-> message==WM_LBUTTONDOWN) 
	{ 
		if(GetDlgItem(IDC_PIC_SFIS)-> m_hWnd==pMsg-> hwnd)
			if (pTemp->GetDlgItem(IDC_BTN_SFIS)->IsWindowEnabled())
			{
				//((CStatic*)GetDlgItem(IDC_PIC_SFIS))->SetIcon(AfxGetApp()->LoadIcon(IDI_SFIS_OFF));
				pTemp->OnBnClickedBtnSfis();
				//GetDlgItem(IDC_BTN_SFIS)->SetWindowText(_T("SFIS OFF"));
				//pTemp->GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(true);
			}
	} 
	
	return   CDialog::PreTranslateMessage(pMsg); 
}