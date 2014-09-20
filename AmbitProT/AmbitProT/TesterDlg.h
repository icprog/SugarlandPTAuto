#pragma once
#include "EquipInfoShow.h"
#include "Tool.h"
#include "AmbitEquip.h"

#define UTI_TIMER 6 //haibin.li 2011/11/05
// CTesterDlg dialog

class CTesterDlg : public CDialog
{
	DECLARE_DYNAMIC(CTesterDlg)

public:
	CTesterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTesterDlg();

// Dialog Data
	enum { IDD = IDD_TESTER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
public:
	/*CEquipInfoShow gEQIP;*/
	//CTool Tool;
	CFont UtiliFont;//haibin.li 2011/11/07
public:
	virtual BOOL OnInitDialog();
	int StartEquipTask(void);
	//bool CountNum(UINT CountModal);
	//090611
	//int ShowInfomation(CString temp);
	/*CString strTemp;
	CFont m_Font;*/
	/*UINT WINAPI EQUIPTask(LPVOID lpPara);*/
	//090611
	//private:
	//	int equipfileflag;
	//	int equipflag;
	//	int equipyellowflag;
	CAmbitEquip AmbitEquip;
public:
	afx_msg void OnBnClickedEquipDlgOpen();
public:
	afx_msg void OnBnClickedEquipDlgClose();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
