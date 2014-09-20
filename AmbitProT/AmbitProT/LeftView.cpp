// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "AmbitProT.h"

#include "AmbitProTDoc.h"
#include "LeftView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CEditShow *pEditPort;//haibin.li 2011/11/07

list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store

CMainControlPanelDlg *gpMainCtrlPanel;
//CMainControlPanelDlg gMainCtrlPanel;

/////////////////////////////////////////////////////////////////////////
//For total management all UI information and control logic
//It distribute UI control message for all sub dialog.
//UINT WINAPI UICentreControlTask(LPVOID lpPara);


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{
	// TODO: add construction code here
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
    //Create the main control panel dialog
	CRect DlgSize;
	GetClientRect(DlgSize);
	gpMainCtrlPanel = (CMainControlPanelDlg* )new CMainControlPanelDlg;
	if (gpMainCtrlPanel)
	{
		gpMainCtrlPanel->Create(IDD_MAIN_CONTROL_PANEL, this);
		gpMainCtrlPanel->SetWindowPos(NULL,DlgSize.top,DlgSize.left,DlgSize.right,DlgSize.bottom,SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		gpMainCtrlPanel->ShowWindow(SW_SHOW);
	}
}

// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CAmbitProTDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAmbitProTDoc)));
	return (CAmbitProTDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView message handlers

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	//run main logic task
    //MainLogicInitialization();

	return 0;
}

void CLeftView::OnDestroy()
{
	CTreeView::OnDestroy();

	// TODO: Add your message handler code here
	//Destroy the main control dialog
	KillTimer(TESTING_TIMER);//haibin.li 2011/11/09
	if (pEditPort)//haibin.li 2011/11/07
	{
		delete pEditPort;
	}
	if (gpMainCtrlPanel)
	{
		gpMainCtrlPanel->DestroyWindow();
		delete gpMainCtrlPanel;
	}
}