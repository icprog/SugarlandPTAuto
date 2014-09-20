// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "MySplitterWnd.h"
//#include "MainControlPanelDlg.h"//haibin.li 2011/11/07
#define TOTAL_TIMER 5//haibin.li 2011/11/07


class CAmbitProTView;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	//CSplitterWnd m_wndSplitter;
	CMySplitterWnd m_wndSplitter;
	//CToolBar    m_wndToolBar;
public:
	CMainControlPanelDlg ctrpanel;//haibin.li 2011/11/07
	void WriteTime(int InitialFlag,long *testT,long *testT1,long *totalT);

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
	CAmbitProTView* GetRightPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
