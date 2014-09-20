#pragma once


// CDialogAmber dialog

class CDialogAmber : public CDialog
{
	DECLARE_DYNAMIC(CDialogAmber)

public:
	BOOL PreTranslateMessage(MSG* pMsg);//09.06
	CDialogAmber(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogAmber();

// Dialog Data
	enum { IDD = IDD_AMBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL OnInitDialog();
};
