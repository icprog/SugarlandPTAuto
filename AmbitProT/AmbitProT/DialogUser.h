#pragma once
#include "AmbitProTSysDefine.h"


// CDialogUser dialog

class CDialogUser : public CDialog
{
	DECLARE_DYNAMIC(CDialogUser)

public:
	CDialogUser(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUser();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	//BOOL PreTranslateMessage(MSG* pMsg);
};
