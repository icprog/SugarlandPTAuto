#pragma once

#include "PTSC.h"
// CAmMydasDlg dialog

class CAmMydasDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmMydasDlg)

public:
	CAmMydasDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAmMydasDlg();

// Dialog Data
	enum { IDD = IDD_MYDAS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	int StartMYDASTask(void);
	int InitialPTSC();
    void ExitPTSC();
    int PostDataToPTS();
    int WriteLogData(char *pBuf, DWORD Len);
    int WriteRecordData(char *pBuf, DWORD Len);
	int WriteMainData(char *pBuf, DWORD Len);//Maxwell 100316


	
public:  // for UI 
	void ShowStatus(CString str);
private:
	//PTS_INFO PtsInfo;
	
	HANDLE hPTSDev;
	INIT_INFO PTS_info;
	int ConnectStatus;
	char PDBS_IP[128];
	int PDBS_FLAG;
    int  SendtoPDBSFlag;

	//Maxwell 100317
	char pMainBuf[1024];
	char pRecordBuf[8096];
	char pLogBuf[1024];
	//Maxwell 100317
	CListCtrl *pLogMyDas;//haibin.li 2011/11/07

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


private:
	CBrush	m_Redbh, m_Greenbh, m_Yellowbh, m_Grancybh, m_Bkbh, m_SmallResultbh;
	CFont ConnectStatusFont;
	int RecordCounter;
	afx_msg void OnBnClickedButtonLog();
};
