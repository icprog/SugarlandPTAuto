#pragma once

#include "AmTesterIO.h"
#include "DetailLogDlg.h"
#include "AmbitProTGlobalDefine.h"
#include "TextProgressCtrl.h"
#include "AClientSockets.h" //Maxwell 1222
#include "DialogAmber.h"//haibin.li 2011/09/16
#include "DialogConf.h"//haibin.li 2011/11/07
#include "DialogUser.h"//haibin.li 2011/11/09
#include "afxcmn.h"


enum 
{
	PASSINDEX,
	FAILINDEX,
	TOTALINDEX,
	YEILDINDEX,
};

#define TEST_TIMER 1
#define STANDBY_TIMER 2
#define RESPONSE_TIMER 3
#define TPTIMEOUT_TIMER 4
#define RUN_TIMER 5

// CMainTestPanel dialog

class CMainTestPanel : public CDialog
{
	DECLARE_DYNAMIC(CMainTestPanel)

public:
	CMainTestPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainTestPanel();

// Dialog Data
	enum { IDD = IDD_MAIN_TEST_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	int StartTesterCtrlTask();
	int StartServer1Connect();
public:
	afx_msg void OnBnClickedBtnShowlog();


private:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	unsigned int TaskID;

	CAmTesterIO *pTIO;
	int TesterIOFlag;
	CDetailLogDlg DetailLogDlg;
	CDialogAmber DialogAmber;//haibin.li 09.16
	CDialogConf DlgConf;//haibin.li 2011/11/07
	CDialogUser DlgUser;//haibin.li 2011/11/09

public:
    int LogDlgFlag;	
	HANDLE hMydasMutex;
	HANDLE hExit;//haibin.li 2011/11/29
	CBrush m_brushdlg;
	CBrush m_brushstatic;
public:
	int ParseTestPerfData(unsigned char *pBuf, DWORD ulSize);// parse testperf data
	int ParseSFISData(unsigned char *pBuf, DWORD ulSize);//parse sfis data from sfis dlg.
	DWORD SendTestPerfData(unsigned char *pBuf, DWORD dwSize);//send test perf data.
	int ParseEquipData(unsigned char *pBuf, DWORD ulSize);//parse equip data
	int ParseServer1Data(unsigned char *pBuf, DWORD ulSize);//parse AUTOFIXTURE data Maxwell 1224
	int IntialTester();//
	void ExitTester();//
	void PostExitMessage();//


	TEST_ITEM TestItemInfo;
	
	//list<TEST_ITEM> TestItemInfoList;

    PTS_INFO PtsInformation;
	
	SFIC_INFO SFISInformation;//SFIS information.
	TEST_RESULT TestResult;

	CListCtrl TestStat;
	UUT_TEST_COUNTER TestResultCounter;//for test result stat.
	UI_UTI StationUti;//haibin.li 2011/12/1
	int noticeflag;//haibin.li 2012/3/14
	

	wchar_t SFISData[512];

public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnClose();
public:
	afx_msg void OnBnClickedBtnStartTest();


public:
	int ControlPerf(wchar_t *pBuf);
	int SfisPerf(wchar_t *pBuf);
	int LogPerf(wchar_t *pBuf);
	int ItemPerf(wchar_t *pBuf);
	int SfisStartPerf(wchar_t *pBuf);
	int MsgBoxPerf(wchar_t *pBuf);
	int AutoFixPerf(wchar_t *pBuf); //Maxwell 1224
	int FixtureIdPerf(wchar_t *pBuf);//haibin.li 07.27
	int SensorPerf(wchar_t *pBuf);//haibin.li 08.16	
	void MyWideCharToMultiByte(WCHAR* wchars,CHAR* schars,int scharsLen);//haibin.li 2011/11/07
	void ShowSnMac(wchar_t widestr[500]);

	int OnTestFinished(wchar_t *pBuf);
    int SendResultToSFIS();
    int SendResultToMYDAS();


public://UI control parameter set.
	void SetBtnParameter(int CtrlID, int enable, wchar_t *pContent);
	void StandbyUIType();
    void PassedUIType();
    void FailedUIType();
	void TestingUIType();
	//CListCtrl TestItemSummary;

private:
	CBrush	m_Redbh, m_Greenbh, m_Yellowbh, m_Grancybh, m_Bkbh, m_SmallResultbh;
	CFont BigResultFont;
	CFont SmallResultFont;
	CFont TestTimeFont;
	CFont ErrorCodeFont;
	CFont ButtonFont;
	CFont ItemFont;
	CTextProgressCtrl m_TestProgressBar;
	CTime m_time;
	int StandbyTimeFlag;
	int StandbyTime;
	int StandbyTimeLimit;
	int EquipFlag;
	int ResponseTime;
	int TPTimeOutTime;
	int TPTimeOutFlag;	
	
private:
	int TesterPort;//Index tester server port, start range is 10000
public:
	afx_msg void OnBnClickedBtnSfis();
	void SetSfisLink();
	int TestPort;//the window number index
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ResetTestTime();
	void ShowTestResultStat(int Result);
public:
	int RepeatTime;
	DWORD dwRepeat_TimeID;
	static DWORD WINAPI Repeat_Time_Thread(LPVOID lpPara);//Maxwell 101015
	HANDLE hRepeat_Time;
	void RepeatTest(void);
public:
	afx_msg void OnEditModify();
    BOOL PreTranslateMessage(MSG* pMsg);//haibin.li 2011/10/27
public:
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CListCtrl m_list;
};
