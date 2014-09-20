// DialogRtm.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "DialogRtm.h"
#include   "winsock2.h " 
#pragma comment(lib,"WS2_32.LIB ") 


CDialogRtm *dlgrtm;

IMPLEMENT_DYNAMIC(CDialogRtm, CDialog)

CDialogRtm::CDialogRtm(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRtm::IDD, pParent)
{
	dlgrtm=this;
     
}

CDialogRtm::~CDialogRtm()
{
}

void CDialogRtm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_version);
	DDX_Control(pDX, IDC_LIST2, m_list_runstatus);
	DDX_Control(pDX, IDC_LIST3, m_list_userinfo);
	DDX_Control(pDX, IDC_LIST4, m_list_result);
	DDX_Control(pDX, IDC_LIST5, m_list_linkstatus);
	DDX_Control(pDX, IDC_LIST6, m_list_hostinfo);
}


BEGIN_MESSAGE_MAP(CDialogRtm, CDialog)
END_MESSAGE_MAP()


// CDialogRtm message handlers
BOOL CDialogRtm::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	InitialHostInfo();
	InitialSWInfo();
	InitialUserInfo();
	InitialUIStatus();
	InitialTestResult();
	InitialLinkStatus();
	ListFont.CreatePointFont(120, _T("Arial"));
	LittleFont.CreatePointFont(90, _T("Arial"));
	m_list_version.SetFont(&LittleFont);
	m_list_runstatus.SetFont(&LittleFont);
	m_list_userinfo.SetFont(&ListFont);
	m_list_result.SetFont(&LittleFont);
	m_list_linkstatus.SetFont(&ListFont);
	m_list_hostinfo.SetFont(&ListFont);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
int CDialogRtm::InitialHostInfo()
{
	m_list_hostinfo.InsertColumn(0, _T("ITEM"), LVCFMT_LEFT, 105);
	m_list_hostinfo.InsertColumn(1, _T("CONTENT"), LVCFMT_LEFT, 130);
	m_list_hostinfo.InsertItem(0,_T("Host Name:"));
	m_list_hostinfo.SetItemText(0,1,gStationInfo.PcInfo.HostName);
	m_list_hostinfo.InsertItem(1,_T("IP:"));
	m_list_hostinfo.SetItemText(1,1,gStationInfo.PcInfo.IPAddress);
	m_list_hostinfo.InsertItem(2,_T("SYSTEM ID:"));
	m_list_hostinfo.SetItemText(2,1,gStationInfo.TestStation.TestStaID);
	return 1;
}
int CDialogRtm::InitialSWInfo()
{
	CString strTemp;
	m_list_version.InsertColumn(0, _T("ITEM"), LVCFMT_LEFT, 100);
	m_list_version.InsertColumn(1, _T("CONTENT"), LVCFMT_LEFT, 130);
	m_list_version.InsertItem(0,_T("UI:"));
	m_list_version.SetItemText(0,1, gStationInfo.TestStation.TestProgramVersion);
	m_list_version.InsertItem(1,_T("TestProPerf:"));
	m_list_version.SetItemText(1,1,gStationInfo.TestStation.TestProgramVersion);
	/*FileVersion(_T("arp.exe"));
	m_list_version.InsertItem(2,_T("Arp:"));
	m_list_version.SetItemText(2,1, strTemp);
    FileVersion(_T("dns-sd.exe"));
	m_list_version.InsertItem(3,_T("Dns-sd:"));
	m_list_version.SetItemText(3,1,strTemp);*/
	return 1;
}

int CDialogRtm::InitialUserInfo()
{
	m_list_userinfo.InsertColumn(0, _T("ITEM"), LVCFMT_LEFT, 120);
	m_list_userinfo.InsertColumn(1, _T("CONTENT"), LVCFMT_LEFT, 110);
	m_list_userinfo.InsertItem(0,_T("User ID:"));
	m_list_userinfo.SetItemText(0,1,userinfo.name);
	m_list_userinfo.InsertItem(1,_T("User Mode:"));
	m_list_userinfo.SetItemText(1,1,userinfo.password);	
	return 1;
}

int CDialogRtm::InitialUIStatus()
{
	m_list_runstatus.InsertColumn(0, _T(""), LVCFMT_LEFT, 90);
	m_list_runstatus.InsertColumn(1, _T("1"), LVCFMT_LEFT, 80);
	m_list_runstatus.InsertColumn(2, _T("2"), LVCFMT_LEFT, 55);
	m_list_runstatus.InsertItem(0,_T("Station:"));
	m_list_runstatus.SetItemText(0,1,_T(""));	
	m_list_runstatus.SetItemText(0,2,_T(""));	
	m_list_runstatus.InsertItem(1,_T("ProductName:"));
	m_list_runstatus.SetItemText(1,1,_T(""));	
	m_list_runstatus.SetItemText(1,2,_T(""));
	m_list_runstatus.InsertItem(2,_T("Test status:"));
	m_list_runstatus.SetItemText(2,1,_T("standby"));
	if (2==gStationInfo.MultiTaskSetting.Number)
	{
		m_list_runstatus.SetItemText(2,2,_T("standby"));
	}	
	
	return 1;
}

int CDialogRtm::InitialTestResult()
{
	m_list_result.InsertColumn(0, _T(""), LVCFMT_LEFT, 90);
	m_list_result.InsertColumn(1, _T("1"), LVCFMT_LEFT, 65);
	m_list_result.InsertColumn(2, _T("2"), LVCFMT_LEFT, 65);
	m_list_result.InsertItem(0,_T("Passed:"));
	m_list_result.SetItemText(0,1,_T("0"));
	m_list_result.InsertItem(1,_T("Failed:"));
	m_list_result.SetItemText(1,1,_T("0"));
	m_list_result.InsertItem(2,_T("Total:"));
	m_list_result.SetItemText(2,1,_T("0"));
	m_list_result.InsertItem(3,_T("Yield Rate:"));
	m_list_result.SetItemText(3,1,_T("0.0%"));
	m_list_result.InsertItem(4,_T("Total Util:"));
	m_list_result.SetItemText(4,1,_T("0.0%"));
	if (2==gStationInfo.MultiTaskSetting.Number)
	{
		m_list_result.SetItemText(0,2,_T("0"));
		m_list_result.SetItemText(1,2,_T("0"));
		m_list_result.SetItemText(2,2,_T("0"));
		m_list_result.SetItemText(3,2,_T("0.0%"));
		m_list_result.SetItemText(4,2,_T("0.0%"));
	}
	return 1;
}

int CDialogRtm::InitialLinkStatus()
{
	m_list_linkstatus.InsertColumn(0, _T(""), LVCFMT_LEFT, 120);
	m_list_linkstatus.InsertColumn(1, _T("1"), LVCFMT_LEFT, 110);
	m_list_linkstatus.InsertItem(0,_T("SFIS:"));
	m_list_linkstatus.SetItemText(0,1,_T("OFF"));
	m_list_linkstatus.InsertItem(1,_T("PTS:"));
	m_list_linkstatus.SetItemText(1,1,_T("OFF"));
	m_list_linkstatus.InsertItem(2,_T("EQUIP:"));
	m_list_linkstatus.SetItemText(2,1,_T("OFF"));	
	return 1;
}
