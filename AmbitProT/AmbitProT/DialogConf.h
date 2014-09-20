#pragma once
#include "DialogUser.h"



// CDialogConf dialog

class CDialogConf : public CDialog
{
	DECLARE_DYNAMIC(CDialogConf)

public:
	CDialogConf(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogConf();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONF };
public:
	void ItemConfiguration(wchar_t *pBuf,int ID);
public:
	CDialogUser dlguser;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedItemModify();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedItemTwo();
public:
	afx_msg void OnBnClickedCheckOne();
public:
	afx_msg void OnBnClickedCheckTwo();
};
