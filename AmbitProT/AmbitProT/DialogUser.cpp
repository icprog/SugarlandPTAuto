// DialogUser.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "DialogUser.h"
#include <atlrx.h>//正則表達式庫
extern CMainControlPanelDlg *pTempCtr;
//extern USER_INFO userinfo;
CString UserTemp1;
CString UserTemp2;



// CDialogUser dialog

IMPLEMENT_DYNAMIC(CDialogUser, CDialog)

CDialogUser::CDialogUser(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUser::IDD, pParent)
{

}

CDialogUser::~CDialogUser()
{
}

void CDialogUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogUser, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogUser::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogUser message handlers
BOOL CDialogUser::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDialogUser::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	CString   strTemp; 
	GetDlgItemText(IDC_USER_ID,strTemp);    
	if(strTemp==_T(""))
	{
		MessageBox(_T("輸入不能為空，請重新輸入！"),_T("Tip"),MB_ICONERROR);
		GetDlgItem(IDC_USER_ID)->SetFocus();
		return;
	}
	CAtlRegExp<> reUrl;
	REParseError status = reUrl.Parse(_T("^{(a|A|F|f)[0-9][0-9][0-9][0-9][0-9][0-9][0-9]}$"));
	if(REPARSE_ERROR_OK != status)
	{
		MessageBox(_T("Syntax error!"));
	}
	CAtlREMatchContext<> mcUrl;
	if (!reUrl.Match(strTemp,&mcUrl))
	{
		MessageBox(_T("輸入錯誤，請重新輸入！"),_T("Tip"),MB_ICONERROR);
		GetDlgItem(IDC_USER_ID)->SetFocus();
		//GetDlgItem(IDC_USER_ID)->SetWindowText(_T(""));
        return;
	}
	//WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)strTemp,-1,userinfo.name,16,0,false);
	userinfo.name=strTemp;
	dlgrtm->m_list_userinfo.SetItemText(0,1,strTemp);//haibin.li 2011/12/5
	UserTemp1=_T("CONFIG=OPID[")+userinfo.name+_T("];");
	UserTemp2=_T("CONFIG=OPID[")+userinfo.name+_T("];");
	strTemp=_T("UserID:")+userinfo.name;
	pTempCtr->GetDlgItem(IDC_USER_ID)->SetWindowText(strTemp);
	pTempCtr->GetDlgItem(IDC_STATIC_IN)->EnableWindow(FALSE);
	pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(TRUE);
	CString strSend=_T("[LOGIN:")+userinfo.name+_T("]");
	USES_CONVERSION;LPCSTR lpcs = NULL;lpcs =T2A(strSend.GetBuffer(strSend.GetLength()));
	SendRTM(lpcs,ThreadID);	
	OnOK();
}
//BOOL CDialogUser::PreTranslateMessage(MSG* pMsg) 
//{ 	
//	if(pMsg->message==WM_KEYDOWN) 
//	{
//		switch(pMsg->wParam) 
//		{ 
//		case VK_RETURN:
//			//pMsg->wParam=VK_RETURN;
//			//return CDialog::PreTranslateMessage(pMsg);
//			return FALSE;
//		} 			
//	} 
//	return FALSE;	
//}  