#pragma once

#include "EditShow.h"
// CMainControlPanelDlg dialog
//typedef struct ListControlTag
//{
	//CListCtrl *pList;
	//char Name[100];
//}LIST_CONTROL; 
//#define MMC_LIST_NUM 100

#define IDC_EDIT_DCT_SHOW 31111

#define  TESTING_TIMER 7//haibin.li 2011/11/08
extern CString TestingTime;//haibin.li 2011/11/08


class CMainControlPanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainControlPanelDlg)

public:
	CMainControlPanelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainControlPanelDlg();

// Dialog Data
	enum { IDD = IDD_MAIN_CONTROL_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnDestroy();
public:
	CListCtrl StationList;
	CListCtrl UUTInfoList;
	CListCtrl SFICInfoList;
	CListCtrl MYDASInfoList;
	//CEditShow *pEditPort;

private:
	CFont StationNameFont;
	CBrush m_brushdlg;
	CBrush m_brushstatic;
private:
	CBrush	m_Redbh, m_Greenbh, m_Yellowbh, m_Grancybh, m_Bkbh, m_SmallResultbh;
	CFont UtiliFont;//haibin.li 2011/11/08
	CFont Scan;//haibin.li 2011/11/10
	CString temptime;//haibin.li 2011/11/09
	RECT  m_StaticRectOut;//haibin.li 2011/11/10
	RECT  m_StaticRectIn;//haibin.li 2011/11/10
	
protected:
	HCURSOR  m_hCursor;//haibin.li 2011/11/10
public:
	bool testflag[2];//haibin.li 2012/2/9


	//LIST_CONTROL ListControl[MMC_LIST_NUM];
public:
	int ControlCreate(void);
public:
	int ListControlCreate();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnPostSwitchTester();
	afx_msg void OnBnPostSwitchMTestDlg();
	afx_msg void OnBnPostSwitchMydasDlg();
	afx_msg void OnBnPostSwitchSfisDlg();
	afx_msg void OnTimer(UINT_PTR nIDEvent);//haibin.li 2011/11/08
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);//haibin.li 2011/11/10
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);//haibin.li 2011/11/10
public:
	void UpdateSystemInfo();
	void UpdateProductInfo();
    void UpdateSFISInfo();
    void UpdateMYDASInfo();

public:
	void LoadPriority(CString filename);//haibin.li 2012/1/18
	int MainLogicInitialization(void);
	void AmGetBasicInfo();
	void DisplayStationName(WCHAR *p);
	void DisplayUUTPnName(WCHAR *p);
	int ListShow(CListCtrl *pList, int index, wchar_t *pName, wchar_t *pContent);
	int ListShowUpdate(CListCtrl *pList, int index, LPCTSTR pContent);

	void InitialMainPanelInfor();
	int LoadSystemConfigration(CString FileName);
	void ParseDctData(char *pBuf);
	void UpdateScanInfo(unsigned char *pBuf, DWORD ulSize);
	CString showDctBuf;
};
