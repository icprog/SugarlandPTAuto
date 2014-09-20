// AmbitEquip.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "AmbitEquip.h"
#include "EquipInfoShow.h"
#include "LCC.h"


extern HANDLE hEventRev;
extern HANDLE hDev;
extern CEquipInfoShow gEQIP;
extern list<EQUIP_USE> DeviceList[50];//090612


// CAmbitEquip dialog

IMPLEMENT_DYNAMIC(CAmbitEquip, CDialog)

CAmbitEquip::CAmbitEquip(CWnd* pParent /*=NULL*/)
	: CDialog(CAmbitEquip::IDD, pParent)
{

}

CAmbitEquip::~CAmbitEquip()
{
}

void CAmbitEquip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EQUIP_LIST, m_equiplist);
}


BEGIN_MESSAGE_MAP(CAmbitEquip, CDialog)
	ON_BN_CLICKED(IDC_BTN_QUERY, &CAmbitEquip::OnBnClickedBtnQuery)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CAmbitEquip::OnBnClickedBtnClear)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAmbitEquip message handlers

void CAmbitEquip::OnBnClickedBtnQuery()
{
	// TODO: Add your control notification handler code here
	m_equiplist.DeleteAllItems();
	int iCount =0;// m_equiplist.GetItemCount();
	int iDev=0;
	list<EQUIP_USE>::iterator Cy;
	while(!DeviceList[iDev].empty())
	{
		for (Cy=DeviceList[iDev].begin(); Cy!=DeviceList[iDev].end();Cy++)
		{
			CString str;
			iCount = m_equiplist.GetItemCount();
			m_equiplist.InsertItem(iCount, (*Cy).EquipmentName);
			str.Format(_T("%d"),(*Cy).Usedtime);
			m_equiplist.SetItemText(iCount,1,str);
			str.Format(_T("%d"),(*Cy).Maxusetime);
			m_equiplist.SetItemText(iCount,2,str);
		}
		iDev++;
	}
	iDev=0;
}

void CAmbitEquip::OnBnClickedBtnClear()
{
	// TODO: Add your control notification handler code here
	wchar_t TempStr[200]=_T("");
	int iCount =m_equiplist.GetItemCount();
	GetDlgItem(IDC_EDEQUIP_USER)->GetWindowText(strusr);
	GetDlgItem(IDC_EDEQUIP_PWD)->GetWindowText(strpwd);	

	///////////////////////////////////////////////////////////////////////////
	char szHostName[256]="";
	char *pTemp,*pTemp2;
	char PCNumber[256]="";
	int qpc=0;
	int nRetCode;
	char buflog[1024]="";
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode==0)
	{
		if(pTemp=strstr(szHostName,"-"))
		{
			if(pTemp2=strstr(pTemp+1,"-"))
			{
				strcpy_s(PCNumber,sizeof(PCNumber),pTemp2+1);
				qpc=atoi(PCNumber);
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////

	POSITION pos = m_equiplist.GetFirstSelectedItemPosition(); 
	if( pos ) 
	{ 
		int nItem = m_equiplist.GetNextSelectedItem(pos); 
		CString str=m_equiplist.GetItemText(nItem, 0); 
		wcscpy_s(TempStr,sizeof(TempStr)/2,str);
		
		//CMD:=RESET;LINE:=LI;STATION:=PT;PC:=1;MATERIAL:=LAN1 CABLE;MATERIAL:=USB CABLE;AMENDERNAME:=F6825884;AMENDERPASSWORD:=F6825884;
		wchar_t TempFullName[1024]=_T("");
		
		swprintf(TempFullName,sizeof(TempFullName)/2,_T("CMD:=RESET;LINE:=%s;STATION:=%s;PC:=%d;MATERIAL:=%s;AMENDERNAME:=%s;AMENDERPASSWORD:=%s;"),
			gStationInfo.EquipInfo.EqipLine,gStationInfo.EquipInfo.EqipStation,qpc
			,TempStr,strusr,strpwd);

		size_t ConvertedChars=0;
		wcstombs_s(&ConvertedChars, buflog, sizeof(buflog), TempFullName,wcslen(TempFullName));

		LCC_SetSendData(hDev,buflog,strlen(buflog),0);	
		LCC_Send(hDev);

		//m_equiplist.SetFocus();
		//UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
		//if(nItem=iCount-1)
		//{
		//	nItem=0;
		//}
		//m_equiplist.SetItemState(nItem+1, flag, flag);
		
	}
	gEQIP.EquipShowInfo();
	OnBnClickedBtnQuery();
}

BOOL CAmbitEquip::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_equiplist.ModifyStyle(0,LVS_REPORT);
	m_equiplist.SetExtendedStyle (LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_INFOTIP|LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);
	m_equiplist.InsertColumn(0, _T("ITEM"), LVCFMT_LEFT, 200);
	m_equiplist.InsertColumn(1, _T("CURTIME"), LVCFMT_LEFT, 70);
	m_equiplist.InsertColumn(2, _T("MAXTIME"), LVCFMT_LEFT, 70);
	m_equiplist.ShowWindow(SW_SHOW);

	GetDlgItem(IDC_EDEQUIP_USER)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDEQUIP_PWD)->SetWindowText(_T(""));	

	//GetDlgItem(IDC_EDEQUIP_USER)->SetWindowText(_T("F6825884"));
	//GetDlgItem(IDC_EDEQUIP_PWD)->SetWindowText(_T("19840325"));	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAmbitEquip::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	this->ShowWindow(SW_HIDE);

	//CDialog::OnClose();
}