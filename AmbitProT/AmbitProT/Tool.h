#pragma once

#include "Markup.h"
//#include "EquipInfoShow.h"
#include "afxcmn.h"
#include "afxwin.h"

//struct toolinfo
//{
//	CString name;
//	CString curtime;
//	CString maxtime;
//};

class CTool
{
public:
	CTool(void);
public:
	~CTool(void);
public:
    CMarkup xml;
	int count(UINT Fixture);
	CString m_path;
	int GetNum();
	int Redirect(char *LocalName,char *RemoteName,char * UserName,char *Password);//haibin.li 2012/3/20
	//bool GetNum();//add 2008/11/1
};
