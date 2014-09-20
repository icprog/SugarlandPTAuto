#pragma once
#include "afxcmn.h"
#include "AmbitProTSysDefine.h"
#include "MainTestPanel.h"


#define  ICON_TIMER 8//haibin.li 2011/11/08
// CDialogMain dialog

class CDialogMain : public CDialog
{
	DECLARE_DYNAMIC(CDialogMain)

public:
	CDialogMain(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogMain();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN };
public:
	G_VARIABLE smoflag;
	SFIC_INFO sfisflag;
	EQIP_INFO equipflag;
	int tempsmoflag;
	int tempsfisflag;
	int tempequipflag;
	int tempptsflag;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	//CTabCtrl m_tab;
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL PreTranslateMessage(MSG* pMsg);//haibin.li 2011/11/08
};
