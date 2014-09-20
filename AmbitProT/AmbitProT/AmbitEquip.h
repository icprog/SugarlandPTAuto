#pragma once
#include "afxcmn.h"


// CAmbitEquip dialog

class CAmbitEquip : public CDialog
{
	DECLARE_DYNAMIC(CAmbitEquip)

public:
	CAmbitEquip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAmbitEquip();

// Dialog Data
	enum { IDD = IDD_EQUIPMENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnQuery();

public:
	afx_msg void OnBnClickedBtnClear();
public:
	virtual BOOL OnInitDialog();
	CString strusr;
	CString strpwd;
public:
	CListCtrl m_equiplist;
public:
	afx_msg void OnClose();
};
