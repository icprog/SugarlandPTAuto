// DialogConf.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "DialogConf.h"
#include "AmThreadAdmin.h"


// CDialogConf dialog
CListCtrl *pLog;
CListCtrl *pLogTwo;
static int itemnum;
static int itemnumtwo;
CString sendstartitem;
CString sendstartitemtwo;
CDialogConf *pDialogConf;
extern USER_PRIO UserPrio;//haibin.li 2012/1/18


IMPLEMENT_DYNAMIC(CDialogConf, CDialog)

CDialogConf::CDialogConf(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConf::IDD, pParent)
{
	pDialogConf=this;

}

CDialogConf::~CDialogConf()
{
	
}

void CDialogConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogConf, CDialog)
	ON_BN_CLICKED(IDC_ITEM_MODIFY, &CDialogConf::OnBnClickedItemModify)
	ON_BN_CLICKED(IDC_ITEM_TWO, &CDialogConf::OnBnClickedItemTwo)
	ON_BN_CLICKED(IDC_CHECK_ONE, &CDialogConf::OnBnClickedCheckOne)
	ON_BN_CLICKED(IDC_CHECK_TWO, &CDialogConf::OnBnClickedCheckTwo)
END_MESSAGE_MAP()


// CDialogConf message handlers

BOOL CDialogConf::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here	
	pLog=NULL;
	pLog = (CListCtrl *)new CListCtrl;
	if (pLog)
	{
		pLog->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,CRect(50,100,500, 545), this, 70006);
		pLog->SetExtendedStyle (LVS_EX_CHECKBOXES|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);				
		pLog->InsertColumn(0,_T("NUM"),LVCFMT_LEFT,40);			
		pLog->InsertColumn(1,_T("TEST ITEM"),LVCFMT_LEFT,410);
		pLog->ShowWindow(SW_SHOW);		
	}

	if(gStationInfo.MultiTaskSetting.Number==2)
	{
		pLogTwo=NULL;
		pLogTwo = (CListCtrl *)new CListCtrl;
		if (pLog)
		{
			pLogTwo->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,CRect(510,100,960, 545), this, 70006);
			pLogTwo->SetExtendedStyle (LVS_EX_CHECKBOXES|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);				
			pLogTwo->InsertColumn(0,_T("NUM"),LVCFMT_LEFT,40);			
			pLogTwo->InsertColumn(1,_T("TEST ITEM"),LVCFMT_LEFT,410);
			pLogTwo->ShowWindow(SW_SHOW);		
		}
		GetDlgItem(IDC_ITEM_TWO)->MoveWindow(510,50,85,38);
		GetDlgItem(IDC_ITEM_TWO)->ShowWindow(TRUE);
		GetDlgItem(IDC_CHECK_TWO)->MoveWindow(610,50,40,38);
		GetDlgItem(IDC_CHECK_TWO)->ShowWindow(TRUE);
	}
	CheckDlgButton(IDC_CHECK_ONE,1);
	CheckDlgButton(IDC_CHECK_TWO,1);
	
	
	itemnum=0;
	itemnumtwo=0;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
int itemparse(char *pDes, int DesSize, char *pSrc,char start,char end)
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
//void CDialogConf::ItemConfiguration(wchar_t *pBuf,int ID)
//{
//	if (ID==1)
//	{
//		int j=0;
//		CString FullFileName;
//		char strname[2048]="";
//		char *str;
//		char *ptr;
//		int num=0;
//
//		FullFileName.Format(_T("%s"),pBuf);
//		WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)FullFileName,-1,strname,2048,0,false);
//		str=strname;
//		ptr=strchr(str,'|');
//		while (ptr)
//		{
//			num+=1;
//			str=ptr+1;
//			ptr=strstr(str,"|");
//		}
//		//---------------initialize list-control--------------
//		char pS[200];
//		char *pToke;
//		int k=1,m=0;
//		CString strng=_T("");	
//		str=strname;
//		str+=5;
//		int itemtemp=itemnum;
//		for (j=0;j<num;j++)
//		{
//			strng.Format(_T("%d"),itemnum+1);
//			pLog->InsertItem(itemnum, strng);
//			itemparse(pS,200,str,0xff,'|');
//			strng=CString(pS);
//			pLog->SetItemText(itemnum, 1,strng);
//			pLog->SetCheck(itemnum);
//			pToke=strchr(str,'|');
//			str=pToke+1;
//			itemnum+=1;
//		}	
//	}
//	else if (ID==2)
//	{
//		int j=0;
//		CString FullFileName;
//		char strname[2048]="";
//		char *str;
//		char *ptr;
//		int num=0;
//
//		FullFileName.Format(_T("%s"),pBuf);
//		WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)FullFileName,-1,strname,2048,0,false);
//		str=strname;
//		ptr=strchr(str,'|');
//		while (ptr)
//		{
//			num+=1;
//			str=ptr+1;
//			ptr=strstr(str,"|");
//		}
//		//---------------initialize list-control--------------
//		char pS[200];
//		char *pToke;
//		int k=1,m=0;
//		CString strng=_T("");	
//		str=strname;
//		str+=5;
//		int itemtemp=itemnumtwo;
//		for (j=0;j<num;j++)
//		{
//			strng.Format(_T("%d"),itemnumtwo+1);
//			pLogTwo->InsertItem(itemnumtwo, strng);
//			itemparse(pS,200,str,0xff,'|');
//			strng=CString(pS);
//			pLogTwo->SetItemText(itemnumtwo, 1,strng);
//			pLogTwo->SetCheck(itemnumtwo);
//			pToke=strchr(str,'|');
//			str=pToke+1;
//			itemnumtwo+=1;
//		}	
//	}
//	
//}
void CDialogConf::ItemConfiguration(wchar_t *pBuf,int ID)
{	
	if (ID==1&&pLog)
	{
		int j=0;
		char strname[2048]="";
		char *str;
		char *ptr;
		int num=0;
		CString strTemp;
		CString strInst;
		int findstr=0;
		int sLen=0;

		WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)pBuf,-1,strname,2048,0,false);
		str=strname;
		ptr=strchr(str,'|');
		while (ptr)
		{
			num+=1;
			str=ptr+1;
			ptr=strstr(str,"|");
		}
		//---------------initialize list-control--------------
		CString strng=_T("");	
		str=strname;
		str+=5;
		strTemp=(CString)str;
		for (j=0;j<num;j++)
		{
			strng.Format(_T("%d"),itemnum+1);
			pLog->InsertItem(itemnum, strng);
			findstr=strTemp.Find('|',0);
			strng=strTemp.Mid(0,findstr);
			sLen=strTemp.GetLength();
			strTemp=strTemp.Mid(findstr+1,sLen-1);
			pLog->SetItemText(itemnum, 1,strng);
			pLog->SetCheck(itemnum);
			itemnum+=1;
		}	
	}
	else if (ID==2&&pLogTwo)
	{
		int j=0;
		char strname[2048]="";
		char *str;
		char *ptr;
		int num=0;

		WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)pBuf,-1,strname,2048,0,false);
		str=strname;
		ptr=strchr(str,'|');
		while (ptr)
		{
			num+=1;
			str=ptr+1;
			ptr=strstr(str,"|");
		}
		//---------------initialize list-control--------------
		CString strTemp;
		CString strInst;
		int findstr=0;
		int sLen=0;
		CString strng=_T("");	
		str=strname;
		str+=5;
		strTemp=(CString)str;
		for (j=0;j<num;j++)
		{
			strng.Format(_T("%d"),itemnumtwo+1);
			pLogTwo->InsertItem(itemnumtwo, strng);
			findstr=strTemp.Find('|',0);
			strng=strTemp.Mid(0,findstr);
			sLen=strTemp.GetLength();
			strTemp=strTemp.Mid(findstr+1,sLen-1);
			pLogTwo->SetItemText(itemnumtwo, 1,strng);
			pLogTwo->SetCheck(itemnumtwo);
			itemnumtwo+=1;
		}	
	}	
}
void CDialogConf::OnBnClickedItemModify()
{
	// TODO: Add your control notification handler code here
	if (!userinfo.name.Compare(_T("")))
	{
		if (dlguser.DoModal()==IDCANCEL)
		{
			return;
		}
	}
	int Idnum=0;
	bool AuthFlag=false;
	for (Idnum=0;Idnum<UserPrio.num;Idnum++)
	{
		if (userinfo.name.Compare(_T(""))&&UserPrio.UserId[Idnum]==userinfo.name)
		{
			AuthFlag=true;
			break;
		}		
	}
	if (!AuthFlag)
	{
		MessageBox(_T("You don't have the authority to modify."),_T("Tip"),MB_OK|MB_ICONWARNING);
		return;
	}
	
	CString str=_T("CONFIG=RESPONSE[");
	CString temp=_T("");
	for(int i=0; i<pLog->GetItemCount(); i++)
	{
		if( pLog->GetCheck(i))
		{
			temp.Format(_T("%d,"), i+1);
			str+=temp;
		}
	}
	str+=_T("]");
	if (!temp.Compare(_T("")))
	{
		MessageBox(_T("No Item.Pls select at least one item."),_T("Tip"),MB_OK|MB_ICONWARNING);
		return;
	}
	if( MessageBox(_T("是否保存?"),_T("ASk"),MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		return;
	}
	int i=str.GetLength();
	if (i>17)
	{
		str.Delete(i-2,1);
	}
	str+=_T(";");
	sendstartitem=str;
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);
	m_tab.SetCurSel(0);
	//WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)str,-1,pbufitemconfi,512,0,false);	
	//SendMessage(WM_MyMessage,0,0);
	//CDialog::OnCancel();
}

void CDialogConf::OnBnClickedItemTwo()
{
	// TODO: Add your control notification handler code here
	if (!userinfo.name.Compare(_T("")))
	{
		if (dlguser.DoModal()==IDCANCEL)
		{
			return;
		}
	}
	int Idnum=0;
	bool AuthFlag=false;
	for (Idnum=0;Idnum<UserPrio.num;Idnum++)
	{
		if (userinfo.name.Compare(_T(""))&&UserPrio.UserId[Idnum]==userinfo.name)
		{
			AuthFlag=true;
			break;
		}		
	}
	if (!AuthFlag)
	{
		MessageBox(_T("You don't have the authority to modify."),_T("Tip"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString str=_T("CONFIG=RESPONSE[");
	CString temp=_T("");
	for(int i=0; i<pLog->GetItemCount(); i++)
	{
		if( pLogTwo->GetCheck(i))
		{
			temp.Format(_T("%d,"), i+1);
			str+=temp;
		}
	}
	str+=_T("]");
	if (!temp.Compare(_T("")))
	{
		MessageBox(_T("No Item.Pls select at least one item."),_T("Tip"),MB_OK|MB_ICONWARNING);
		return;
	}
	if( MessageBox(_T("是否保存?"),_T("ASk"),MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		return;
	}
	int i=str.GetLength();
	if (i>17)
	{
		str.Delete(i-2,1);
	}
	str+=_T(";");
	sendstartitemtwo=str;
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);
	m_tab.SetCurSel(0);
}

void CDialogConf::OnBnClickedCheckOne()
{
	// TODO: Add your control notification handler code here
	int i=0;
	if (IsDlgButtonChecked(IDC_CHECK_ONE))
	{
		GetDlgItem(IDC_CHECK_ONE)->SetWindowText(_T("NULL"));
		for (i=0;i<itemnum;i++)
			pLog->SetCheck(i);
	}
	else
	{
		GetDlgItem(IDC_CHECK_ONE)->SetWindowText(_T("ALL"));
		for(i=0;i<itemnum;i++)
			pLog->SetCheck(i,0);
	}
}

void CDialogConf::OnBnClickedCheckTwo()
{
	// TODO: Add your control notification handler code here
	int i=0;
	if (IsDlgButtonChecked(IDC_CHECK_TWO))
	{
		GetDlgItem(IDC_CHECK_TWO)->SetWindowText(_T("NULL"));
		for (i=0;i<itemnum;i++)
			pLogTwo->SetCheck(i);
	}
	else
	{
		GetDlgItem(IDC_CHECK_TWO)->SetWindowText(_T("ALL"));
		for(i=0;i<itemnumtwo;i++)
			pLogTwo->SetCheck(i,0);
	}
}
