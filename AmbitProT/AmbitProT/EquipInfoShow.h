#pragma once
#include "afxwin.h"
#include "list"
#include "Tool.h"
#include "AmbitProTSysDefine.h"


//typedef struct EquipmentInfoTag
//{
//	wchar_t EquipmentName[100];
//	int Usedtime;
//	//char DeviceInfo[1000];
//	int Maxusetime;
//
//}EQUIP_USE;

class CEquipInfoShow :public CEdit
{
public:
	CEquipInfoShow(void);
public:
	~CEquipInfoShow(void);

public:
	//SER_INFO ServerInfo;
public:
	//int ShowInfomation(void);

	int ShowInfomation(CString temp);
	int EquipShowInfo();
	bool CountNum(UINT CountModal);
	bool CountNumXml(UINT CountModal);
	int EquipShowInfoXml();
	/*int ShowInfomationRed(CString temp);*/
	CString strTemp;
	CFont m_Font;
	CTool Tool;
private:
	int equipfileflag;
	int equipflag;
	int equipyellowflag;
public:
	int InitialEQUIP();
	int ParsePtsData(char *pBuf);
	int QueryPerf(wchar_t *pBuf);
};