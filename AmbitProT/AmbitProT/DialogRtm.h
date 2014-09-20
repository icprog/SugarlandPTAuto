#pragma once
#include "afxcmn.h"


// CDialogRtm dialog

class CDialogRtm : public CDialog
{
	DECLARE_DYNAMIC(CDialogRtm)

public:
	CDialogRtm(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogRtm();

// Dialog Data
	enum { IDD = IDD_DIALOG_RTM };
public:
	int InitialHostInfo();
	int InitialSWInfo();
	int InitialUserInfo();
	int InitialUIStatus();
	int InitialTestResult();
	int InitialLinkStatus();
public:
	CFont ListFont;
	CFont LittleFont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_version;
public:
	CListCtrl m_list_runstatus;
public:
	CListCtrl m_list_userinfo;
public:
	CListCtrl m_list_result;
public:
	CListCtrl m_list_linkstatus;
public:
	CListCtrl m_list_hostinfo;
};
