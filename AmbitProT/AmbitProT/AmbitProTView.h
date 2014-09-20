// AmbitProTView.h : interface of the CAmbitProTView class
//


#pragma once


class CAmbitProTView : public CListView
{
protected: // create from serialization only
	CAmbitProTView();
	DECLARE_DYNCREATE(CAmbitProTView)

// Attributes
public:
	CAmbitProTDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CAmbitProTView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnDestroy();

public:
	int AmCreateMTestDlg(UINT Number);
public:
	void AmDestroyMTestDlg(void);
	void SetMTestDlgShow(int Bev);

public:
	int AmCreateTesterDlg();
    int AmDestroyTesterDlg();

	int AmCreateMydasDlg();
    int AmDestroyMydasDlg();

    int AmCreateSfisDlg();
    int AmDestroySfisDlg();

	int AmCreateVirtualSmoDlg();
	int AmDestroyVirtualSmoDlg();

    int AmCreateMainDlg();//haibin.li 2011/11/07
	int AmDestroyMainDlg();//haibin.li 2011/11/07
	int AmCreateConfDlg();//haibin.li 2011/11/07
	int AmDestroyConfDlg();//haibin.li 2011/11/07
	int AmCreateDetailDlg();//haibin.li 2011/11/07
	int AmDestroyDetailDlg();//haibin.li 2011/11/07
	int AmCreateRtmDlg();//haibin.li 2011/11/11
	int AmDestroyRtmDlg();//haibin.li 2011/11/11

	void ShowConfDlg();//haibin.li 2011/11/07
	void ShowDetailDlg();//haibin.li 2011/11/07
	void ShowRtmDlg();//haibin.li 2011/11/11
	void ShowTesterDlg();
    void ShowMTestDlg();
    void ShowSfisDlg();
    void ShowMydasDlg();


public:
	virtual BOOL DestroyWindow();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	int MainViewTaskStart(void);
public:
	afx_msg void OnViewCount();
};

#ifndef _DEBUG  // debug version in AmbitProTView.cpp
inline CAmbitProTDoc* CAmbitProTView::GetDocument() const
   { return reinterpret_cast<CAmbitProTDoc*>(m_pDocument); }
#endif

