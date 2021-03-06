// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AmbitProT.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "AmbitProTView.h"
#include "ProcessControl.h"
#include "AmThreadAdmin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, &CMainFrame::OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, &CMainFrame::OnViewStyle)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	totaltime=0;//haibin.li 2011/11/07
	testtime[0]=0;
	testtime[1]=0;
	Utilization[0]=0.0;
	Utilization[1]=0.0;
	WriteTime(1,&(testtime[0]),&(testtime[1]),&totaltime);//haibin.li 2011/11/26
}

CMainFrame::~CMainFrame()
{
	KillTimer(TOTAL_TIMER);//haibin.li 2011/11/09
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.ShowWindow(SW_HIDE);

	//Get system screen size for windows create
		
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&CWnd::wndTopMost, 0, 0, cx, cy-30, SWP_NOZORDER);// Initial windows size
	ShowWindow(SW_SHOW);
	UpdateWindow();

	SetMenu(NULL);//Don't load Menu   haibin.li 2011/11//07
	SetTimer(TOTAL_TIMER,1000,(TIMERPROC)NULL);//haibin.li 2011/11/07
	return 0;
}
//BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
//	CCreateContext* pContext)
//{
//	// create splitter window
//	if (!m_wndSplitter.CreateStatic(this, 1, 2))
//		return FALSE;
//
//	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(230, 100), pContext) ||
//		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CAmbitProTView), CSize(100, 100), pContext))
//	{
//		m_wndSplitter.DestroyWindow();
//		return FALSE;
//	}
//
//	return TRUE;
//}

//changed start by Haibin.Li 06.24
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT,	CCreateContext* pContext)
{	
	ctrpanel.LoadSystemConfigration(_T("UIConfig.ini"));
	if(2==gStationInfo.MultiTaskSetting.Number||3==gStationInfo.MultiTaskSetting.Number)
	{		
		if (!m_wndSplitter.CreateStatic(this, 2, 1))//two column,one row
			return FALSE;	
		if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(230, 50), pContext) ||
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CAmbitProTView), CSize(100, 100), pContext))
		{
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}
		return TRUE;
	}
	else 
	{		
		if (!m_wndSplitter.CreateStatic(this, 2, 1))//two column,one row
			return FALSE;	
		if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(230, 50), pContext) ||
			!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CAmbitProTView), CSize(100, 100), pContext))
		{
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}
		return TRUE;
	}
}
//changed end by Haibin.Li 06.24

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	/*CWnd *m_pMainWnd;
	m_pMainWnd=AfxGetMainWnd();
	m_pMainWnd->SetWindowText(_T("MFG-AmbitProT"));*/
	//cs.style=~FWS_ADDTOTITLE;
	//cs.lpszName=_TEXT("joyce");
	

	//当改变多视MDI的子窗口的标题时,用:
	//GetParentFrame()->SetWindowText(_T("MDI Child改变标题"));

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


CAmbitProTView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CAmbitProTView* pView = DYNAMIC_DOWNCAST(CAmbitProTView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	if (!pCmdUI)
		return;

	// TODO: customize or extend this code to handle choices on the View menu

	CAmbitProTView* pView = GetRightPane(); 

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the View menu
	CAmbitProTView* pView = GetRightPane();

	// if the right-hand pane has been created and is a CAmbitProTView,
	// process the menu commands...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// change the style; window will repaint automatically
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	//wcsncat_s(FileNameBuf, sizeof(FileNameBuf)/2,_T("\\TestPROPerf.exe"), wcslen(_T("\\TestPROPerf.exe")));
	//HWND hWnd = ::FindWindow(NULL, FileNameBuf);
	//while(hWnd) 
	//{
		//::SendMessage(hWnd, WM_CLOSE, 0, 0);
		//hWnd = ::FindWindow(NULL, FileNameBuf);
	//}
	//CProcessControl ProcessControl;
	//while(ProcessControl.KillTargetProcess(_T("TestPROPerf.exe"))) 
	//{
	//	;
	//}
	WriteTime(0,&(testtime[0]),&(testtime[1]),&totaltime);//haibin.li 2011/11/26
	AmExitAllTask();
	CFrameWnd::OnClose();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TOTAL_TIMER==nIDEvent)
	{		
		totaltime+=1;
		if (totaltime==0)
		{
			Utilization[0]=0.0;
			Utilization[1]=0.0;
		}
		else
		{
			Utilization[0]=testtime[0]*100.0/totaltime;
			Utilization[1]=testtime[1]*100.0/totaltime;
		}
	}

	CFrameWnd::OnTimer(nIDEvent);
}
void CMainFrame::WriteTime(int InitialFlag,long *testT,long *testT1,long *totalT)
{
	FILE *stream;
	char ch[512]="";
	wchar_t FilePath[500];
	CString FullFileName;
	CTime m_time;

	GetCurrentDirectory(sizeof(FilePath)/2, FilePath);
	FullFileName=FilePath;
	FullFileName+=_T("\\TestResult\\Utilization\\");
	if(!PathFileExists(FullFileName))
	{	
		if(!CreateDirectory(FullFileName,NULL))
		{
			return;
		}
	}
	m_time=CTime::GetCurrentTime();
	CString strT=m_time.Format("%Y%m%d");
	FullFileName+=strT+_T(".txt");
	WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)FullFileName,-1,ch,FullFileName.GetLength()+4,0,false);

	if (InitialFlag==1)
	{
		char line[1024]="";
		if(!fopen_s(&stream,ch, "r+")) 	
		{	
			fgets(line,1024,stream);
			CString strTemp=(CString)line;
			int chrFind=0;
			chrFind=strTemp.Find(',');
			*testT=_ttoi(strTemp.Left(chrFind));

			strTemp=strTemp.Right(strTemp.GetLength()-chrFind-1);

			chrFind=strTemp.Find(',');
			*testT1=_ttoi(strTemp.Left(chrFind));

			strTemp=strTemp.Right(strTemp.GetLength()-chrFind-1);
			*totalT=_ttoi(strTemp);
		}	
		if (stream)
		{
			fclose(stream);
		}
	}
	else if (InitialFlag==0)
	{
		char ch1[32]="";
		char ch2[32]="";
		sprintf_s(ch1,sizeof(ch1),"%d",*testT);
		strcat_s(ch1,sizeof(ch1),",");
		sprintf_s(ch2,sizeof(ch2),"%d",*testT1);
		strcat_s(ch1,sizeof(ch1),ch2);
		strcat_s(ch1,sizeof(ch1),",");
		sprintf_s(ch2,sizeof(ch2),"%d",*totalT);
		strcat_s(ch1,sizeof(ch1),ch2);
		size_t len=strlen(ch1);
		if(!fopen_s(&stream,ch, "w+"))
		{	
			fwrite(ch1, sizeof(char), len, stream);				
		}
		if (stream)
		{
			fclose(stream);
		}	
	}
}
