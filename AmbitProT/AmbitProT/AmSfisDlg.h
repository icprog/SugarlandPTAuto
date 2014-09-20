//#include "stdafx.h"
//#include "SFISDefine.h"

#pragma once


// CAmSfisDlg dialog
//#define WM_GUIIO WM_USER+1113
#define SFIS_START 1
#define SFIS_RESULT 2
#define SFIS_LINK_STATUS 3
#define DCT_DATA 4



class CAmSfisDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmSfisDlg)

public:
	CAmSfisDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAmSfisDlg();

// Dialog Data
	enum { IDD = IDD_SFIS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	HICON m_hIcon;
	CString showDctBuf;
	CString showRecvBuf;
	wchar_t SFISInformation[1024];
	CFont staFont;

public:
	int OnSFISAction(char* pRevStr,int flag);
	int OnPN(void);
	int FindResult(char* pRevStr);
	void OnSfisToPef(void);
	void OnSendMsgToUI(int flag);
	void OnSend(wchar_t *RevBuf,int DataLen,int Flag);
	void WAdjustLength(int nLength, wchar_t* pDesStr,int DesStrLength, wchar_t* pRevStr,wchar_t* fillstr);
public:
	int m_connFlag;
public:
	void OnShowRecv(char *pBuf,int flag);

	static DWORD WINAPI ConnThreadProcess(LPVOID lpPara);
	void ConnThread(void);
public:
	int ParseUIData(char *pBuf, DWORD ulSize);
	int OnSendDCT(char *pBuf);
public:
	afx_msg void OnBnClickedVirtOpen();
public:
	afx_msg void OnClose();
};
