#include "StdAfx.h"
#include "Tool.h"
//#include "PropOne.h"
//#include "windows.h"

extern EQUIP_USE gServerInfo;//add by maxwell 2008/10/30
extern list<EQUIP_USE> DeviceList[50];//add by maxwell 2008/10/30//090612
extern AM_PROT_INFO gStationInfo;
#pragma comment(lib,"mpr.lib")//NetWork Enum

CTool::CTool(void)
{
	 m_path="";
}

CTool::~CTool(void)
{
}
//090612
int CTool::count(UINT Fixture)
{
	CString name;
	/* CString retvalue="ok";*/
	if(!xml.Load(m_path))
		return 0;
	int curtime;
	int maxtime;
	CString temp("STATION");
	xml.FindElem(temp);
	xml.IntoElem();

//Common use equipment type
	xml.FindElem(_T("COMMON"));
	xml.IntoElem();
	temp="TOOL";

	while(xml.FindElem(temp))
	{
		xml.IntoElem();
		temp="TOOLNAME";
		xml.FindElem(temp);
		name=xml.GetData();

		temp="CURTIME";
		xml.FindElem(temp);
		curtime=_wtoi(xml.GetData())+1;
		xml.SetData(curtime);

		temp="MAXTIME";
		xml.FindElem(temp);
		maxtime=_wtoi(xml.GetData());

		temp="TOOL";
		xml.OutOfElem();	
	}
	xml.OutOfElem();
	xml.OutOfElem();
	xml.ResetMainPos();

	temp="STATION";
	xml.FindElem(temp);
	name= xml.GetTagName();
	xml.IntoElem();
//Private fixture
	for(int i = 1;i<=50;i++)
	{
		xml.FindElem();
		name= xml.GetTagName();
		if(name.Trim(_T("FIXTURE")))
		{
			if(Fixture==_wtoi(name))
				break;
		}
	}
	xml.IntoElem();
	temp="TOOL";

	while(xml.FindElem(temp))
	{
		xml.IntoElem();
		temp="TOOLNAME";
		xml.FindElem(temp);
		name=xml.GetData();

		temp="CURTIME";
		xml.FindElem(temp);
		curtime=_wtoi(xml.GetData())+1;
		xml.SetData(curtime);

		temp="MAXTIME";
		xml.FindElem(temp);
		maxtime=_wtoi(xml.GetData());

		temp="TOOL";
		xml.OutOfElem();	
	}
	xml.OutOfElem();
	xml.OutOfElem();
	xml.Save(m_path);
	/*return retvalue;*/
	return 1;
}
//090612


int CTool::GetNum() 
{
	CString name;
	CString nameFix;
	//----haibin.li 2012/3/20----
	//if(!xml.Load(m_path)) return 0;//add 2008/11/1
	
	if (!xml.Load(m_path))
	{
		int res=Redirect("F:","\\\\10.117.2.5\\mfg","oper","mfg-cp2");		
		if (!xml.Load(m_path))
		{
			return 0;
		}			
	}
	//----haibin.li 2012/3/20----

	//090612
	//for(int iDev=0;iDev<=gStationInfo.MultiTaskSetting.Number;iDev++)
	for(int iDev=0;iDev<50;iDev++)
	{
		DeviceList[iDev].clear();
	}//090612
	int Curtime=0;
	int Maxtime=0;
	int FixNum=0;
	CString temp("STATION");
	xml.FindElem();
	name = xml.GetTagName();
	xml.IntoElem();
	while(xml.FindElem())
	{
		nameFix = xml.GetTagName();
		xml.IntoElem();
		temp="TOOL";
		while(xml.FindElem(temp))
		{
			xml.IntoElem();

			temp="TOOLNAME";
			xml.FindElem(temp);
			name=xml.GetData();

			temp="CURTIME";
			xml.FindElem(temp);
			Curtime=_wtoi(xml.GetData());
			//xml.SetData(Curtime);

			temp="MAXTIME";
			xml.FindElem(temp);
			Maxtime=_wtoi(xml.GetData());

			temp="TOOL";
			xml.OutOfElem();

			wcscpy_s(gServerInfo.FixCountName,sizeof(gServerInfo.FixCountName)/2,nameFix);
			wcscpy_s(gServerInfo.EquipmentName,sizeof(gServerInfo.EquipmentName)/2,name);
			gServerInfo.Usedtime=Curtime;
			gServerInfo.Maxusetime=Maxtime;
			DeviceList[FixNum].push_back(gServerInfo);
		}
		xml.OutOfElem();
		FixNum++;
	}
	
	/*while(xml.FindElem(temp))
	{
		xml.IntoElem();
		temp="TOOLNAME";
		xml.FindElem(temp);
		name=xml.GetData();
		temp="CURTIME";
		xml.FindElem(temp);
		curtime=_wtoi(xml.GetData());
		xml.SetData(curtime);

		temp="MAXTIME";
		xml.FindElem(temp);
		maxtime=_wtoi(xml.GetData());
	
		temp="TOOL";
		xml.OutOfElem();
		
		wcscpy_s(gServerInfo.EquipmentName,sizeof(gServerInfo.EquipmentName)/2,name);
		gServerInfo.Usedtime=curtime;
		gServerInfo.Maxusetime=maxtime;
		DeviceList.push_back(gServerInfo);
	}*/

     //090612
   	/*while(xml.FindElem(temp))
	{
		xml.FindElem();
		xml.IntoElem();
		temp="TOOLNAME";
		xml.FindElem(temp);
		name=xml.GetData();
		temp="CURTIME";
		xml.FindElem(temp);
		Curtime=_wtoi(xml.GetData());
		xml.SetData(Curtime);

		temp="MAXTIME";
		xml.FindElem(temp);
		Maxtime=_wtoi(xml.GetData());
	
		temp="TOOL";
		xml.OutOfElem();
		
		wcscpy_s(gServerInfo.EquipmentName,sizeof(gServerInfo.EquipmentName)/2,name);
		gServerInfo.Usedtime=Curtime;
		gServerInfo.Maxusetime=Maxtime;
		DeviceList.push_back(gServerInfo);
	}*/
//090612

//Add end by  maxwell 2008/10/30
	int m=0;
	xml.OutOfElem();
	xml.OutOfElem();
	xml.Save(m_path);
	return 1;
 /* return flag;*/
	//return true;
	//std::list<SER_INFO>::iterator countCy;
	//for(countCy=DeviceList.begin();/*countCy!=DeviceList.end()*/;countCy++)
}
//------haibin.li 2012/3/20------
int CTool::Redirect(char *LocalName,char *RemoteName,char * UserName,char *Password)
{
	NETRESOURCE nr;
	DWORD res;
	char szUserName[32],szPasswrod[32],szLocalName[32],szRemoteName[MAX_PATH];
	strcpy_s(szUserName,UserName);
	strcpy_s(szPasswrod,Password);
	strcpy_s(szLocalName,LocalName);
	strcpy_s(szRemoteName,RemoteName);
	nr.dwType=RESOURCETYPE_ANY;
	nr.lpLocalName=NULL;
	nr.lpRemoteName=NULL;
	nr.lpProvider=NULL;


	USES_CONVERSION; 
	LPWSTR lpcs1 = NULL; 
	LPWSTR lpcs2 = NULL;
	LPTSTR lpt1=NULL;
	//lpt1=szLocalName;

	nr.lpLocalName=A2W((LPCSTR)szLocalName);
	nr.lpRemoteName=A2W((LPCSTR)szRemoteName);
	lpcs1=A2W((LPCSTR)szPasswrod);
	lpcs2=A2W((LPCSTR)szUserName);	

	CString strTemp=_T("");

	res=WNetAddConnection2(&nr,lpcs1,lpcs2,FALSE);
	if (res!=NO_ERROR)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	/*switch(res)
	{
	case NO_ERROR:
		AfxMessageBox(_T("NetWork Driver Direct Success."));
		break;
	default:
		strTemp.Format(_T("%d"),res);
		strTemp=_T("Error : ")+strTemp;
		AfxMessageBox(strTemp);
		break;
	}*/
	return 1;
}
//------haibin.li 2012/3/20------
