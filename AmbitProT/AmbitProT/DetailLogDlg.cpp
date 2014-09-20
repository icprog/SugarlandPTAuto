// DetailLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "MainTestPanel.h"
#include "DetailLogDlg.h"
#include "AmParser.h"
//
CListCtrl *pTestItemSummary;//haibin.li 2011/11/07
CListCtrl *pTestItemSummaryTwo;//haibin.li 2011/11/08
CEditShow *pLogDisplay;//haibin.li 2011/11/07
CEditShow *pLogDisplayTwo;//haibin.li 2011/11/07
int TempID;
// CDetailLogDlg dialog


IMPLEMENT_DYNAMIC(CDetailLogDlg, CDialog)

CDetailLogDlg::CDetailLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDetailLogDlg::IDD, pParent),pMPT(0)
{
	
	pLogDisplay=0;
	pTestItemSummary=0;
	LogSwitchFlag=0;
	LogSwitchFlagTwo=0;//haibin.li 2011/11/14
	IDIndex=0;
    //pTestItemSummary1=0;//haibin.li 2011/11/08
}

CDetailLogDlg::~CDetailLogDlg()
{


}

void CDetailLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDetailLogDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SWITCH_SHOW, &CDetailLogDlg::OnBnClickedBtnSwitchShow)
	ON_BN_CLICKED(IDC_BUTTON_TWO, &CDetailLogDlg::OnBnClickedButtonTwo)
END_MESSAGE_MAP()


// CDetailLogDlg message handlers

void CDetailLogDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL CDetailLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//Maxwell 101006
	CString strTitle;
	CString strButton;
	strTitle.Format(_T("%d. Detail Log Information"),this->TaskID);
	//SetDlgItemText(IDC_BTN_SWITCH_SHOW, strResult);
	//SetWindowTextW(_T("Detail Log Information"));
	SetWindowTextW(strTitle);

	strButton.Format(_T("%d   -> LOG"),this->TaskID);
	//SetDlgItemText(IDC_BTN_SWITCH_SHOW, strButton);//haibin.li 2011/11/14

	pLogDisplay=0;
	pLogDisplay = (CEditShow *)new CEditShow;
	if (pLogDisplay)
	{
		pLogDisplay->Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE |WS_BORDER| WS_TABSTOP |ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_WANTRETURN|WS_HSCROLL|WS_VSCROLL|ES_READONLY, 
			CRect(50,100,505,545), this, 89907876); //CRect(5,30,460,445)
		pLogDisplay->ShowWindow(SW_HIDE);
		pLogDisplay->ShowInfo(DetailLog);	
	}

	pTestItemSummary=0;
	pTestItemSummary = (CListCtrl *)new CListCtrl;
	if (pTestItemSummary)
	{
		pTestItemSummary->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT/*|LVS_NOCOLUMNHEADER*/,
		CRect(50,100,505, 545), this, 70006);
		pTestItemSummary->ModifyStyle(0,LVS_REPORT);
		pTestItemSummary->SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);

		//pTestItemSummary->ShowInfo(DetailLog);
		pTestItemSummary->InsertColumn(0, _T("ITEM"), LVCFMT_LEFT, 383);
	    pTestItemSummary->InsertColumn(1, _T("RESULT"), LVCFMT_LEFT, 70);
		pTestItemSummary->ShowWindow(SW_SHOW);
		ShowHistoryItem(DetailLog);
		//m_totallist.SetBkColor(RGB(230,230,230));
		//m_totallist.SetTextBkColor(RGB(230,255,255));
		//m_totallist.SetTextColor(RGB(0,0,100));	
		//m_totallist.ModifyStyle (0,LVS_REPORT);
		//m_totallist.SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);
		//m_totallist.SetColumnWidth(10, LVSCW_AUTOSIZE_USEHEADER);
	}

	if(gStationInfo.MultiTaskSetting.Number==2)
	{
		pLogDisplayTwo=0;
		pLogDisplayTwo = (CEditShow *)new CEditShow;
		if (pLogDisplayTwo)
		{
			pLogDisplayTwo->Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE |WS_BORDER| WS_TABSTOP |ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_WANTRETURN|WS_HSCROLL|WS_VSCROLL|ES_READONLY, 
				CRect(510,100,965, 545), this, 89907876);
			pLogDisplayTwo->ShowWindow(SW_HIDE);
			pLogDisplayTwo->ShowInfo(DetailLogTwo);
		}
		pTestItemSummaryTwo=NULL;
		pTestItemSummaryTwo = (CListCtrl *)new CListCtrl;
		if (pTestItemSummaryTwo)
		{
			pTestItemSummaryTwo->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT,CRect(510,100,965, 545), this, 70006);
			pTestItemSummaryTwo->SetExtendedStyle (LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);				
			pTestItemSummaryTwo->InsertColumn(0,_T("ITEM"),LVCFMT_LEFT,383);			
			pTestItemSummaryTwo->InsertColumn(1,_T("RESULT"),LVCFMT_LEFT,70);
			pTestItemSummaryTwo->ShowWindow(SW_SHOW);		
		}
		GetDlgItem(IDC_BUTTON_TWO)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON_TWO)->MoveWindow(510,60,85,30);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDetailLogDlg::ShowLog(wchar_t *pwcBuf,int ID)
{
	TempID=ID;
	if (ID==1)
	{
		CString strTemp;
		if (pwcBuf)
		{
			strTemp=pwcBuf;
			strTemp+=_T("\r\n");
			DetailLog+=strTemp;
			if (pLogDisplay)
			{
				pLogDisplay->ShowInfo(strTemp);
			}
			LogShowItem(pwcBuf,ID);
		}
	}
	else if (ID==2)
	{
		CString strTemp;
		if (pwcBuf)
		{
			strTemp=pwcBuf;
			strTemp+=_T("\r\n");
			DetailLogTwo+=strTemp;
			if (pLogDisplayTwo)
			{
				pLogDisplayTwo->ShowInfo(strTemp);
			}
			LogShowItem(pwcBuf,ID);
		}
	}	
	
	return;
}
int CDetailLogDlg::GetHistoryLog(CString &strRtn)
{
	if (DetailLog.IsEmpty())
	{
		return 0;
	}
    strRtn=DetailLog;
	return 1;
}
int CDetailLogDlg::GetLogSize()
{
	return (DetailLog.GetLength());
}
int CDetailLogDlg::ItemPerf(wchar_t *pBuf,int ID)
{	
	CAmParser Parser;
	wchar_t ResultBuf[300];

	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}

	//TEST->UI:ITEM=NUMER[12];
	//TEST->UI:ITEM=ID[1]NAME[Check video quality];
	//TEST->UI:ITEM=ID[1]RESULT[PASS];	
		
	if(Parser.ParserGetPara(pBuf, _T("ID"), ResultBuf, sizeof(ResultBuf)/2))
	{
		IDIndex=_wtoi(ResultBuf)-1;
	}  
	if (ID==1)
	{
		if(Parser.ParserGetPara(pBuf, _T("NAME"), ResultBuf, sizeof(ResultBuf)/2))
		{		
			if (pTestItemSummary)
			{
				pTestItemSummary->InsertItem(IDIndex,ResultBuf);
			}
		}
		if(Parser.ParserGetPara(pBuf, _T("RESULT"), ResultBuf, sizeof(ResultBuf)/2))
		{
			if (pTestItemSummary)
			{
				pTestItemSummary->SetItemText(IDIndex, 1, ResultBuf);
			}
		}
	}
	else if (ID==2)
	{
		if(Parser.ParserGetPara(pBuf, _T("NAME"), ResultBuf, sizeof(ResultBuf)/2))
		{		
			if (pTestItemSummaryTwo)
			{
				pTestItemSummaryTwo->InsertItem(IDIndex,ResultBuf);
			}
		}
		if(Parser.ParserGetPara(pBuf, _T("RESULT"), ResultBuf, sizeof(ResultBuf)/2))
		{
			if (pTestItemSummaryTwo)
			{
				pTestItemSummaryTwo->SetItemText(IDIndex, 1, ResultBuf);
			}
		}
	}
	
    
	return 1;
}
void CDetailLogDlg::LogShowItem(wchar_t *pwcBuf,int ID)
{
	CAmParser Parser;
	wchar_t ResultBuf[1024];
	if (pwcBuf)
	{
		if(Parser.ParserGetPara(pwcBuf,  _T("ITEM"), ResultBuf, sizeof(ResultBuf)/2))
		{
			ItemPerf(ResultBuf,ID);
		}
	}
	
	return;
}
void CDetailLogDlg::ShowHistoryItem(CString strTemp)
{
	wchar_t  tempBuf[2048];
	CString resToken;
	int curPos= 0;

	resToken= strTemp.Tokenize(_T("\r\n"),curPos);
	wcscpy_s(tempBuf, sizeof(tempBuf)/2, resToken);
	LogShowItem(tempBuf,1);//haibin.li 2012/2/1
	while (resToken != "")
	{
		//printf("Resulting token: %s\n", resToken);
		resToken= strTemp.Tokenize(_T("\r\n"),curPos);
		wcscpy_s(tempBuf, sizeof(tempBuf)/2, resToken);
		LogShowItem(tempBuf,1);//haibin.li 2012/2/1
	};

	return;
}
void CDetailLogDlg::ClearLog(int ID)
{
	DetailLog.Empty();
	if (ID==1)
	{
		if (pLogDisplay)
		{
			pLogDisplay->Clear();
			pLogDisplay->ShowInfo(DetailLog);
		}
		if (pTestItemSummary)
		{
			pTestItemSummary->DeleteAllItems();
			pTestItemSummary->RemoveAllGroups();
		}
	}
	else if (ID==2)
	{
		if (pLogDisplayTwo)
		{
			pLogDisplayTwo->Clear();
			pLogDisplayTwo->ShowInfo(DetailLog);
		}
		if (pTestItemSummaryTwo)
		{
			pTestItemSummaryTwo->DeleteAllItems();
			pTestItemSummaryTwo->RemoveAllGroups();
		}
	}
	

	return;
}

void CDetailLogDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default	
	pMPT->LogDlgFlag=1;//reset log dialog  flag
	if (pLogDisplay)
	{
		pLogDisplay->DestroyWindow();
		delete pLogDisplay;
		pLogDisplay=0;
	}
	if (pTestItemSummary)
	{
		pTestItemSummary->DestroyWindow();
		delete pTestItemSummary;
		pTestItemSummary=0;
	}
	if(gStationInfo.MultiTaskSetting.Number==2)
	{
		if (pLogDisplayTwo)
		{
			pLogDisplayTwo->DestroyWindow();
			delete pLogDisplayTwo;
			pLogDisplayTwo=0;
		}
		if (pTestItemSummaryTwo)
		{
			pTestItemSummaryTwo->DestroyWindow();
			delete pTestItemSummaryTwo;
			pTestItemSummaryTwo=0;
		}
	}

	DestroyWindow();
	CDialog::OnClose();
}

int CDetailLogDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}
int CDetailLogDlg::SetAmParent(CMainTestPanel* pParent)
{
	if (pParent)
	{
		pMPT=pParent;
	}
	return 1;
}

void CDetailLogDlg::OnBnClickedBtnSwitchShow()
{
	// TODO: Add your control notification handler code here

	//Maxwell 101006
	CString strResult;

	if (1==LogSwitchFlag)
	{
		if (pLogDisplay)
		{
			pLogDisplay->ShowWindow(SW_HIDE);
		}
		if (pTestItemSummary)
		{
			pTestItemSummary->ShowWindow(SW_SHOW);
		}
		LogSwitchFlag=0;

		//Maxwell 101006
		strResult.Format(_T("%d   -> LOG"),this->TaskID);
		//SetDlgItemText(IDC_BTN_SWITCH_SHOW, strResult);//haibin.li 2011/11/14
	}
	else
	{
		if (pLogDisplay)
		{
			pLogDisplay->ShowWindow(SW_SHOW);
		}
		if (pTestItemSummary)
		{
			pTestItemSummary->ShowWindow(SW_HIDE);
		}
		
		//Maxwell 101006
		strResult.Format(_T("%d   -> ITEM"),this->TaskID);
		//SetDlgItemText(IDC_BTN_SWITCH_SHOW, strResult);//haibin.li 2011/11/14
		LogSwitchFlag=1;
	}
}
void CDetailLogDlg::OnBnClickedButtonTwo()
{
	// TODO: Add your control notification handler code here
	if (1==LogSwitchFlagTwo)
	{
		if (pLogDisplay)
		{
			pLogDisplayTwo->ShowWindow(SW_HIDE);
		}
		if (pTestItemSummaryTwo)
		{
			pTestItemSummaryTwo->ShowWindow(SW_SHOW);
		}
		LogSwitchFlagTwo=0;
	}
	else
	{
		if (pLogDisplayTwo)
		{
			pLogDisplayTwo->ShowWindow(SW_SHOW);
		}
		if (pTestItemSummaryTwo)
		{
			pTestItemSummaryTwo->ShowWindow(SW_HIDE);
		}		
		LogSwitchFlagTwo=1;
	}
}
