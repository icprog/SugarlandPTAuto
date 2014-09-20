#pragma once


// CVirtualSmo dialog

class CVirtualSmo : public CDialog
{
	DECLARE_DYNAMIC(CVirtualSmo)

public:
	CVirtualSmo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVirtualSmo();

// Dialog Data
	enum { IDD = IDD_VIR_SMO };
public:
	int vSmoShowFlag;//haibin.li 2012/4/26

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVirSend();
public:
	wchar_t StrItem[4][30];
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnClose();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
};
