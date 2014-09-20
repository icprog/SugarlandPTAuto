#pragma once

#include "EditShow.h"


// CDetailLogDlg dialog
class CMainTestPanel;
class CDetailLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CDetailLogDlg)

public:
	CDetailLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDetailLogDlg();

// Dialog Data
	enum { IDD = IDD_DETAIL_LOG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnClose();

private:
	CMainTestPanel *pMPT;
public:
	int SetAmParent(CMainTestPanel* pParent);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	//CEditShow *pLogDisplay; 
	void ShowLog(wchar_t *pwcBuf,int ID);
	void ClearLog(int ID);
	int TaskID; //Maxwell 101006
private:
	CString DetailLog;
	CString DetailLogTwo;
	int LogSwitchFlag;
	int LogSwitchFlagTwo;//haibin.li 2011/11/14
	int IDIndex;
public:
	//CListCtrl *pTestItemSummary;
	//int ItemPerf(wchar_t *pBuf);//haibin.li 2012/2/1
	//void LogShowItem(wchar_t *pwcBuf);//haibin.li 2012/2/1
	int ItemPerf(wchar_t *pBuf,int ID);
	void LogShowItem(wchar_t *pwcBuf,int ID);
	void ShowHistoryItem(CString strTemp);
	int GetHistoryLog(CString &strRtn);
	int GetLogSize();

public:
	afx_msg void OnBnClickedBtnSwitchShow();
public:
	afx_msg void OnBnClickedButtonTwo();
};
