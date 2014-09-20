// DialogAmber.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "DialogAmber.h"
//#include "MainTestPanel.h"


// CDialogAmber dialog
extern char CheckLed[600];//HAIBIN 09.30
extern int fix_id;
//extern CMainTestPanel *pTemp;//haibin.li 2011/11/09

IMPLEMENT_DYNAMIC(CDialogAmber, CDialog)

CDialogAmber::CDialogAmber(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAmber::IDD, pParent)
{

}

CDialogAmber::~CDialogAmber()
{
}

void CDialogAmber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogAmber, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogAmber::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogAmber message handlers

void CDialogAmber::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
BOOL CDialogAmber::PreTranslateMessage(MSG* pMsg)
{ 	
	if(pMsg->message==WM_KEYDOWN) 
	{
		switch(pMsg->wParam) 
		{ 
		case VK_ADD:         //處理加號
			pMsg->wParam=VK_RETURN;
			return CDialog::PreTranslateMessage(pMsg);
		} 			
	} 
	return FALSE;	
}  
BOOL CDialogAmber::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  Add extra initialization here
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	}
	//SetWindowPos(&this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	if(strstr(CheckLed,"Check UUT system LED amber"))
	{
		GetDlgItem(IDC_AMBER_PIC)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT system LED green"))
	{
		GetDlgItem(IDC_GREEN_PIC)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT system LED OFF"))
	{
		GetDlgItem(IDC_LED_OFF)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT WAN(PT) Port LED ON"))
	{
		GetDlgItem(IDC_PT_WAN)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT LAN(PT) Port LED ON"))
	{
		GetDlgItem(IDC_PT_LAN)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT WAN(FT) Port LED ON"))
	{
		GetDlgItem(IDC_FT_WAN)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Check UUT LAN(FT) Port LED ON"))
	{
		GetDlgItem(IDC_FT_LAN)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Top panel pull down"))
	{
		GetDlgItem(IDC_DOWN_PIC)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Is system led blink normally"))
	{
		GetDlgItem(IDC_BLINK_PIC)->ShowWindow(TRUE);
	}
	else if(strstr(CheckLed,"Put the DUT right side"))
	{
		GetDlgItem(IDC_ANALYSE_PIC)->ShowWindow(TRUE);
		GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
		GetDlgItem(IDOK)->MoveWindow(100,150,100,40);
		GetDlgItem(IDOK)->SetWindowText(_T("OK"));
	}
	
	if(gStationInfo.MultiTaskSetting.Number==2)
	{
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		if(fix_id==1)
		{
			this->MoveWindow(cx/4-135,cy/3-50,320,250);
		}
		else if(fix_id==2)
		{
			this->MoveWindow(3*cx/4-145,cy/3-50,320,250);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
