// VirtualSmo.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "VirtualSmo.h"

#include "SFISDefine.h"
#include "AmSfisDlg.h"


// CVirtualSmo dialog
extern SFIS_VARIABLE s_variable;
extern list <SFIS_TRANSMIT_ITEM> ReadList;
extern CAmSfisDlg *pSfisDlgDlg;


CVirtualSmo *pVirtualSmo;

IMPLEMENT_DYNAMIC(CVirtualSmo, CDialog)

CVirtualSmo::CVirtualSmo(CWnd* pParent /*=NULL*/)
	: CDialog(CVirtualSmo::IDD, pParent)
{
	pVirtualSmo=this;
	vSmoShowFlag=0;
}

CVirtualSmo::~CVirtualSmo()
{
}

void CVirtualSmo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVirtualSmo, CDialog)
	ON_BN_CLICKED(IDC_VIR_SEND, &CVirtualSmo::OnBnClickedVirSend)
	ON_BN_CLICKED(IDCANCEL, &CVirtualSmo::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CVirtualSmo::OnBnClickedOk)
END_MESSAGE_MAP()


// CVirtualSmo message handlers

void CVirtualSmo::OnBnClickedVirSend()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString str;
	this->GetDlgItem(IDC_VIR_SMO_ITEM1)->GetWindowText(str);
	wcscpy_s(*(StrItem+0),sizeof(*(StrItem+0))/2,str);
	this->GetDlgItem(IDC_VIR_SMO_ITEM2)->GetWindowText(str);
	wcscpy_s(*(StrItem+1),sizeof(*(StrItem+1))/2,str);
	this->GetDlgItem(IDC_VIR_SMO_ITEM3)->GetWindowText(str);
	wcscpy_s(*(StrItem+2),sizeof(*(StrItem+2))/2,str);
	this->GetDlgItem(IDC_VIR_SMO_ITEM4)->GetWindowText(str);
	wcscpy_s(*(StrItem+3),sizeof(*(StrItem+3))/2,str);
	if(!wcslen(*(StrItem+0)))
	{
		AfxMessageBox(_T("The ITEM1 is illegal,Please retype..."));
		this->SetDlgItemText(IDC_VIR_SMO_ITEM1,_T(""));
	}
	int index=0;
	list<SFIS_TRANSMIT_ITEM>::iterator Cy;
	for (Cy=ReadList.begin(); Cy!=ReadList.end();Cy++)
	{
		if((*Cy).readFlag)
		{
			TCHAR temp[128]=_T("");
			TCHAR *pToke=NULL;
			wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
			if(s_variable.ReadItemNum-1==index)
			{
				wcsncpy_s((*Cy).Val,sizeof((*Cy).Val)/2,s_variable.PN,sizeof(s_variable.PN)/2);
				break;
			}
			if(!wcslen(*(StrItem+index)))
			{
				wcsncpy_s((*Cy).Val,sizeof((*Cy).Val)/2,s_variable.PN,sizeof(s_variable.PN)/2);
				break;
			}
			wcsncpy_s((*Cy).Val,sizeof((*Cy).Val)/2,*(StrItem+index),sizeof(*(StrItem+index))/2);
			index+=1;
		}
	}
	if(s_variable.ReadItemNum-1!=index)
	{
		AfxMessageBox(_T("The type format is error"));
		return;
	}
	pSfisDlgDlg->OnSendMsgToUI(SFIS_START);
	gVariable.virtualflag=1;
	//m_tab.SetCurSel(0);//haibin.li 2011/11/08
	SetDlgItemText(IDC_VIR_SMO_ITEM1,_T(""));
	GetDlgItem(IDC_VIR_SMO_ITEM1)->SetFocus();
}

void CVirtualSmo::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CVirtualSmo::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	this->ShowWindow(SW_HIDE);
	vSmoShowFlag=0;
	CDialog::OnClose();
}

BOOL CVirtualSmo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	this->SetDlgItemText(IDC_VIR_SMO_ITEM2,_T("111111111110"));
	this->SetDlgItemText(IDC_VIR_SMO_ITEM3,_T("111111111111"));


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE	
}
void CVirtualSmo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
