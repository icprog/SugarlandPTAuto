// AmbitProT.h : main header file for the AmbitProT application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
//#include "RTMTest.h"//haibin.li 2011/11/19
#include "Test.h"//haibin.li 2012/1/5
#pragma comment(lib,"RTMTest.lib")//haibin.li 2011/11/19
#include "DialogRtm.h"


// CAmbitProTApp:
// See AmbitProT.cpp for the implementation of this class
//

class CAmbitProTApp : public CWinApp
{
public:
	CAmbitProTApp();
	bool FirstInstance(void);


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

//extern CAmbitProTApp theApp;
//
extern AM_PROT_INFO gStationInfo;//Store global information for all test.
extern G_VARIABLE gVariable;
extern CAmbitProTApp theApp;
extern double Utilization[2];//haibin.li 2011/11/07
extern long totaltime;//haibin.li 2011/10/31
extern long testtime[2];//haibin.li 2011/10/31
extern CTabCtrl m_tab;//haibin.li 2011/11/08
extern int ThreadID;
extern USER_INFO userinfo;//haibin.li 2011/11/10
extern CDialogRtm *dlgrtm;//haibin.li 2011/12/5
