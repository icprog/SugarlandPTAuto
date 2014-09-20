// MainTestPanel.cpp : implementation file
//

#include "stdafx.h"
#include "AmbitProT.h"
#include "MainTestPanel.h"
#include "AmParser.h"
#include "AmThreadAdmin.h"
#include "TesterControl.h"
#include "SFISDefine.h"
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib")
extern int FixtureNum;//haibin.li 07.01
char CheckLed[600]="";//haibin.li 09.30
extern CString sendstartitem;//haibin.li 2011/11/07
extern CString sendstartitemtwo;//haibin.li 2011/11/15
double Utilization[2];//haibin.li 2011/11/07
CString TestingTime;//haibin.li 2011/11/07
long totaltime;//haibin.li 2011/10/31
long testtime[2];//haibin.li 2011/10/31
int SfisStatus;//haibin.li 2011/11/08
CMainTestPanel *pTemp;//haibin.li 2011/11/09
extern CMainControlPanelDlg *pTempCtr;//haibin.li 2011/11/10
USER_INFO userinfo;//haibin.li 2011/11/10
extern CString UserTemp1;//haibin.li 2011/11/15
extern CString UserTemp2;//haibin.li 2011/11/15
int ThreadID;//haibin.li 2011/11/25
extern CDialogConf *pDialogConf;//haibin.li 2012/1/18

extern SFIS_VARIABLE s_variable;
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
extern CLIENT_TYPE Server1Client;//Maxwell 1222
UINT WINAPI UITesterCtrlTask(LPVOID lpPara);
//DWORD WINAPI Repeat_Time_Thread(LPVOID lpPara);//Maxwell 101015
DWORD AutoFixtureThreadID;//Maxwell 1224
HANDLE hDlgMutex;//09.09
HANDLE Hmutex;
HANDLE gACKEvent[50];//for the sfis have not feedback within 20 second
static unsigned int gTestTaskCounter=0;// for test task ID generate, 
// CMainTestPanel dialog
CMainTestPanel *pTestPanel;// for repeat time vincent
//CMainTestPanel *pRepeatTestPanel[100];// for repeat time Maxwell 101015
int sfisStartFlag;//add by Talen 2011/10/26 for distinguish SFIS start or CLICK start

IMPLEMENT_DYNAMIC(CMainTestPanel, CDialog)

CMainTestPanel::CMainTestPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CMainTestPanel::IDD, pParent)
{

	TaskID=0;
	pTIO=0;
	TesterIOFlag=0;
	LogDlgFlag=1;
	memset((void *)&SFISInformation,0,sizeof(SFISInformation));
    memset((void *)&TestResult,0,sizeof(TestResult));
    memset((void *)&PtsInformation,0,sizeof(PtsInformation));
	
	StandbyTimeFlag=0;//
	StandbyTime=0;	
	EquipFlag=0;
	ResponseTime=0;
	RepeatTime=1;
	TPTimeOutTime=0;
	StationUti.TestTime=0;//haibin.li 2011/12/1
	StationUti.TotalTime=0;//haibin.li 2011/12/1
	StationUti.Uti=0.0;//haibin.li 2011/12/1
	noticeflag=0;//haibin.li 2012/3/14
	
}

CMainTestPanel::~CMainTestPanel()
{
}

void CMainTestPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CMainTestPanel, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SHOWLOG, &CMainTestPanel::OnBnClickedBtnShowlog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_START_TEST, &CMainTestPanel::OnBnClickedBtnStartTest)
	ON_BN_CLICKED(IDC_BTN_SFIS, &CMainTestPanel::OnBnClickedBtnSfis)
	ON_WM_TIMER()
	//ON_COMMAND(ID_EDIT_MODIFY, &CMainTestPanel::OnEditModify)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CMainTestPanel::OnNMCustomdrawList1)
END_MESSAGE_MAP()


// CMainTestPanel message handlers	
int CMainTestPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	if(gStationInfo.Server1Client.Enable)
	{
		if(StartServer1Connect())
		{

		}
		else
		{
			AfxMessageBox(_T("Failed to connect Server1!","Client"));
			::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
			return 0;
		}
	}
	else
	{

	}
	//StartServer1Connect();
	StartTesterCtrlTask();
	AutoFixtureThreadID = NULL;
	TestPort=gTestTaskCounter;
	hMydasMutex= CreateMutex(NULL,NULL,_T("MydasMutex"));
	hExit=CreateMutex(NULL,NULL,_T("Exit"));
	return 0;
}

int CMainTestPanel::StartServer1Connect(void)
{
	char buflog[256]="";
	if (!InitSocket())
	{
		return 0;
	}
	else
	{
        //----------------------------------------------------------------------------------------------------------
		//initialization the client socket type informations
		size_t ConvertedChars=0;
		wcstombs_s(&ConvertedChars, buflog, sizeof(buflog), gStationInfo.Server1Client.IP,wcslen(gStationInfo.Server1Client.IP));
		sprintf_s(Server1Client.IPAddr, sizeof(Server1Client.IPAddr), "%s",buflog);
		Server1Client.uPort = gStationInfo.Server1Client.Port;
		sprintf_s(Server1Client.AmSkcInfo, sizeof(Server1Client.AmSkcInfo), "%s", "socket comm with UI");
		Server1Client.hRevEvent = NULL;
		Server1Client.pgRevBufType = NULL;
		Server1Client.iRevBufTypeLen = 0;

		Server1Client.dwRevThreadID = NULL;
		Server1Client.iConnectSerFlag = 0;
		
		// now start load the socket and connect the server
		if (LoadWinsock(&Server1Client))
		{
			return 0;
		}
	}
	return 1;
}

int CMainTestPanel::StartTesterCtrlTask(void)
{

	AM_TASK_INFO TaskTemp;

	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		UITesterCtrlTask,
		this,
		0,
		&TaskTemp.uiTaskID 
		);
	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=TEST_TASK_TYPE+gTestTaskCounter;//generate task id
		TesterPort= UI_SERVER_PORT_START+gTestTaskCounter;//Generate tester port
		DetailLogDlg.TaskID = TesterPort-9999;//Maxwell 101006
		gTestTaskCounter++;
        TaskID=TaskTemp.uiTaskID;
        ThreadID=DetailLogDlg.TaskID;//haibin.li 2011/11/25
		gUITaskInfoList.push_back(TaskTemp);//Insert the task information to management list
	}
	else
	{
		return 0;
	}
	return 1;
}

int CMainTestPanel::ParseEquipData(unsigned char *pBuf, DWORD ulSize)
{
	char TempBuf[2000]="";
	wchar_t wcBuf[2000];
	wchar_t ResultBuf[500];
	size_t cLen;
	CAmParser Parser;

	if (!pBuf)
	{
		return 0;
	}
	strcpy_s(TempBuf,sizeof(TempBuf),(char *)pBuf);
	//free(pBuf);
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, TempBuf, ulSize);
	if(cLen>=200||cLen==0)
	{
		return 0;
	}
	else
	{	
		if(Parser.ParserGetPara(wcBuf,  _T("EQUIPFLAG"), ResultBuf, sizeof(ResultBuf)/2))
		{
			CStatic *pStatic;
			pStatic=(CStatic*)GetDlgItem(IDC_EQUIP_PICTURE); 
			int flag=_wtoi(ResultBuf);
			if(1==flag)
			{
				SetDlgItemText(IDC_EQUIP_STATUS,_T("The Equipment can be used "));
				pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_PASS));
			}
			if(2==flag)
			{
				SetDlgItemText(IDC_EQUIP_STATUS,_T("The Equipment  will be out of used time"));
				pStatic->SetIcon(AfxGetApp()->LoadIcon(IDI_WARN));
			}
			if(3==flag)
			{
				EquipFlag=flag;
				SetDlgItemText(IDC_EQUIP_STATUS,_T("The Equipment is out of used time"));
				pStatic->SetIcon( AfxGetApp()->LoadIcon(IDI_FAIL));
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int CMainTestPanel::ParseSFISData(unsigned char *pBuf, DWORD ulSize)//vincent
{
	char TempBuf[2000]="";
	wchar_t wcBuf[2000];
	wchar_t ResultBuf[500];
	size_t cLen;
	CAmParser Parser;

	if (!pBuf)
	{
		return 0;
	}
	strcpy_s(TempBuf,sizeof(TempBuf),(char *)pBuf);
	free(pBuf);
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, TempBuf, ulSize);
	if(cLen>=300||cLen==0)
	{
		return 0;
	}
	else
	{
		if(Parser.ParserGetPara(wcBuf,  _T("SFISSTART"), ResultBuf, sizeof(ResultBuf)/2))
		{
			///////////////////////////////////
			gVariable.UutScannerFlag=0;
			char pData[128]="SCANNER=0;";
			size_t CLen=strlen(pData);
			AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_SCAN, (unsigned int)pData, (unsigned int)CLen);
			///////////////////////////////////
			wcscpy_s(SFISInformation.SMOData,sizeof(SFISInformation.SMOData)/2,ResultBuf);
			wcscpy_s(SFISData,sizeof(SFISData)/2,ResultBuf);  //vincent 20090706
			if(!SFISInformation.SFICSwitchFlag) //vincent 20090706
			{
				AfxMessageBox(_T("Please Link the SFIS"));
				char *pRecordData=0;
				size_t ConvertedChars=0;
				size_t SizeLen= wcslen(SFISData);
				pRecordData = (char *)malloc(SizeLen+1);
				if (pRecordData)
				{   	
					wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISData, SizeLen);
					AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
				}
				return 0;
			}
			SfisStartPerf(ResultBuf);////renew the pn name
			sfisStartFlag=1;//add by Talen 2011/10/26
			OnBnClickedBtnStartTest();
		}
		else if(Parser.ParserGetPara(wcBuf,  _T("SFISRESULT"), ResultBuf, sizeof(ResultBuf)/2))
		{

		}
		else if(Parser.ParserGetPara(wcBuf,  _T("SMOCONNFLAG"), ResultBuf, sizeof(ResultBuf)/2))
		{
			gVariable.SMOConnFlag=1;//set the smo link status is ok
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int CMainTestPanel::SfisStartPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}
		
	if(Parser.ParserGetPara(pBuf, _T("PNNAME"), ResultBuf, sizeof(ResultBuf)/2))
	{
		wcscpy_s(gStationInfo.uut.UUTPN, sizeof(gStationInfo.uut.UUTPN)/2,ResultBuf);
		AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_PRODUCT, 0, 0);
	}

	return 1;
}

int CMainTestPanel::ParseTestPerfData(unsigned char *pBuf, DWORD ulSize)
{
	wchar_t wcBuf[2000];
	wchar_t ResultBuf[2048];
	size_t cLen;
	CAmParser Parser;

	if(TPTimeOutFlag)
	{
		return 0;
	}

	if (!pBuf)
	{
		return 0;
	}
	
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, (char *)pBuf, ulSize);
	//DetailLogDlg.ShowLog(wcBuf);

	//memset(Buf, 0, sizeof(Buf));////////////////////vincent for small buf09-06-15
	//memcpy_s(Buf, sizeof(Buf), pBuf, ulSize);
	//strcpy_s(AckBuf, sizeof(AckBuf), "ACK:");
	//strcat_s(AckBuf, sizeof(AckBuf),(char *)pBuf);
	free(pBuf);
	
    if(Parser.ParserGetPara(wcBuf,  _T("SFIS"), ResultBuf, sizeof(ResultBuf)/2))
	{
        SfisPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("LOG"), ResultBuf, sizeof(ResultBuf)/2))
	{
        LogPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("ITEM"), ResultBuf, sizeof(ResultBuf)/2))
	{
		ItemPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf, _T("CONTROL"), ResultBuf, sizeof(ResultBuf)/2))
	{
		ControlPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("MSGBOX"), ResultBuf, sizeof(ResultBuf)/2))
	{
		WaitForSingleObject(hDlgMutex,INFINITE);//haibin.li 2011/08/12
        MsgBoxPerf(ResultBuf);
		ReleaseMutex(hDlgMutex);//haibin.li 2011/08/12	
	}
    //Maxwell 1224
	if(Parser.ParserGetPara(wcBuf,  _T("AUTOFIX"), ResultBuf, sizeof(ResultBuf)/2))
	{
		AutoFixPerf(ResultBuf);
	}
	//Maxwell 1224

	//haibin.li 07.27
	if(Parser.ParserGetPara(wcBuf,  _T("FIXTURE_ID"), ResultBuf, sizeof(ResultBuf)/2))
	{
		FixtureIdPerf(ResultBuf);
	}
	if(Parser.ParserGetPara(wcBuf,  _T("SENSOR"), ResultBuf, sizeof(ResultBuf)/2))
	{
		SensorPerf(ResultBuf);
	}	
	//haibin.li 07.27

	//haibin.li 2011/11/01
	if(Parser.ParserGetPara(wcBuf,  _T("CONFIG"), ResultBuf, sizeof(ResultBuf)/2))
	{
		char res[1024];
		MyWideCharToMultiByte(ResultBuf,res,sizeof(res));
		if(!strstr(res,"[START]")&&!strstr(res,"[END]"))
		{	
			WaitForSingleObject(hDlgMutex,INFINITE);//haibin.li 2011/11/30		
			DlgConf.ItemConfiguration(ResultBuf,DetailLogDlg.TaskID);
			ReleaseMutex(hDlgMutex);//haibin.li 2011/11/30	
		}		
	}	
	//haibin.li 2011/11/01
	DetailLogDlg.ShowLog(wcBuf,DetailLogDlg.TaskID);

	return 1;
}

int CMainTestPanel::ParseServer1Data(unsigned char *pBuf, DWORD ulSize)//MAXWELL 1224
{
	wchar_t wcBuf[2000];
	wchar_t ResultBuf[1024];
	size_t cLen;
	CAmParser Parser;

	if(TPTimeOutFlag)
	{
		return 0;
	}

	if (!pBuf)
	{
		return 0;
	}
	mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, (char *)pBuf, ulSize);
	//DetailLogDlg.ShowLog(wcBuf);

	//memset(Buf, 0, sizeof(Buf));////////////////////vincent for small buf09-06-15
	//memcpy_s(Buf, sizeof(Buf), pBuf, ulSize);
	//strcpy_s(AckBuf, sizeof(AckBuf), "ACK:");
	//strcat_s(AckBuf, sizeof(AckBuf),(char *)pBuf);
	free(pBuf);

	if(Parser.ParserGetPara(wcBuf,  _T("AUTOFIX"), ResultBuf, sizeof(ResultBuf)/2))
	{
        AutoFixPerf(ResultBuf);
	}
	DetailLogDlg.ShowLog(wcBuf,DetailLogDlg.TaskID);

	return 1;
}

int CMainTestPanel::AutoFixPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	char CmdBuf[128]="";
	size_t cLen;
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}
	else
	{

	}
	
	if(Parser.ParserGetPara(pBuf, _T("CMD"), ResultBuf, sizeof(ResultBuf)/2))
	{
		if(wcsstr(ResultBuf,_T("CLOSE")))
		{
			strcpy_s(CmdBuf,sizeof(CmdBuf),"AUTOFIX=CMD[CLOSE];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
		}
		else if(wcsstr(ResultBuf,_T("OPEN")))
		{
			strcpy_s(CmdBuf,sizeof(CmdBuf),"AUTOFIX=CMD[OPEN];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
		}
		else
		{

		}
	}
	else if(Parser.ParserGetPara(pBuf, _T("STATUS"), ResultBuf, sizeof(ResultBuf)/2))
	{
		if(wcsstr(ResultBuf,_T("OPENERROR")))
		{
			CString strServer =_T("fail");
			//str+=TestResult.ErrorCode;
			//Maxwell 1222
			//char temp[16] = "fail";
			char buf[32] = "";

			strServer += gStationInfo.Server1Client.ClientNumber;
			strServer += _T("COMERO");;
			size_t ConvertedChars=0;
			wcstombs_s(&ConvertedChars, buf, sizeof(buf), strServer,wcslen(strServer));
			//sprintf_s(Server1Client.IPAddr, sizeof(Server1Client.IPAddr), "%s",buflog);
			//strcat_s(temp,sizeof(temp),buf);

			AmbitSend(&Server1Client,buf,(int)(sizeof(buf)));
		}

		else if(wcsstr(ResultBuf,_T("EMERGENCY")))
		{
			CString strServer =_T("fail");
			//str+=TestResult.ErrorCode;
			//Maxwell 1222
			//char temp[16] = "fail";
			char buf[32] = "";

			strServer += gStationInfo.Server1Client.ClientNumber;
			strServer += _T("EMERGENCY");
			size_t ConvertedChars=0;
			wcstombs_s(&ConvertedChars, buf, sizeof(buf), strServer,wcslen(strServer));
			//sprintf_s(Server1Client.IPAddr, sizeof(Server1Client.IPAddr), "%s",buflog);
			//strcat_s(temp,sizeof(temp),buf);

			AmbitSend(&Server1Client,buf,(int)(sizeof(buf)));
		}
		/*else if(wcsstr(ResultBuf,_T("OPEN")))
		{
			strcpy_s(CmdBuf,sizeof(CmdBuf),"AUTOFIX=CMD[OPEN];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
		}*/
		else
		{

		}
	}
	else
	{			
		
	}
	return 1;
}
//add start by haibin.li 07.27

void CMainTestPanel::MyWideCharToMultiByte(WCHAR* wchars,CHAR* schars,int scharsLen) 
{
	memset(schars,0,scharsLen);
	CString m_snd = wchars;
	int len = m_snd.GetLength();
	CString tmpstr(m_snd); 
	int multibytelen=WideCharToMultiByte( 
		CP_ACP, 
		WC_COMPOSITECHECK | WC_DEFAULTCHAR, 
		(LPCWSTR)tmpstr.GetBuffer(len),
		len,
		0,
		0,
		0,
		0 
		);
	WideCharToMultiByte( 
		CP_ACP,
		WC_COMPOSITECHECK | WC_DEFAULTCHAR,
		(LPCWSTR)tmpstr.GetBuffer(len),
		len,
		(char *)schars,
		scharsLen, 
		0,
		0
		);
}
int fix_id;
int CMainTestPanel::FixtureIdPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	char result[300]="";	
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}	
	if(Parser.ParserGetPara(pBuf, _T("CONTENT"), ResultBuf, sizeof(ResultBuf)/2))
	{		
		MyWideCharToMultiByte(ResultBuf,result,sizeof(result));
		fix_id=atoi(result);		
	}
	else 
		return 0;
	
	return 1;
}
HHOOK hMsgBoxHook; 
//HWND hwnd; 
LRESULT CALLBACK CBTProc(INT nCode,WPARAM wParam,LPARAM lParam)   
{   
	HWND hParentWnd,hChildWnd; 
	int cx = GetSystemMetrics(SM_CXSCREEN);
	
	if(nCode==HCBT_ACTIVATE) 
	{  		
		hParentWnd=GetForegroundWindow(); 
		hChildWnd=(HWND)wParam;	
		if(1==gStationInfo.MultiTaskSetting.Number)
		{
			UnhookWindowsHookEx(hMsgBoxHook);
		}
		else
		{
			//SetDlgItemText((HWND)wParam,IDYES,_T("&¬O"));
			//SetDlgItemText((HWND)wParam,IDNO,_T("&§_"));
			if(1==fix_id)
			{
				MoveWindow(hChildWnd,cx/4-110,300,250,120,TRUE);
			}
			else if(2==fix_id)
			{
				MoveWindow(hChildWnd,3*cx/4-140,300,250,120,TRUE);
			}	
			else
			{
				MoveWindow(hChildWnd,cx/4-110,300,250,120,TRUE);
			}
			UnhookWindowsHookEx(hMsgBoxHook);
		}
	}  	
	else 
		CallNextHookEx(hMsgBoxHook,nCode,wParam,lParam); 
	return 0; 
} 
//add start by haibin.li 07.27

//haibin.li 08.16
int CMainTestPanel::SensorPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	char res[300]="";
	char CmdBuf[128]="";
	size_t cLen;
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}	
	if(Parser.ParserGetPara(pBuf, _T("RESPONSE"), ResultBuf, sizeof(ResultBuf)/2))
	{   		
		MyWideCharToMultiByte(ResultBuf,res,sizeof(res));	
		if(strstr(res,"@E1#")||strstr(res,"@E2#")||strstr(res,"@E3#")||strstr(res,"@E4#")||strstr(res,"@E5#")||strstr(res,"@E6#")||strstr(res,"@E7#")||strstr(res,"@E8#")||strstr(res,"@E9#")||strstr(res,"@EA#"))
		{
			SetDlgItemText(IDC_BIG_RESULT, ResultBuf);			
			if(MessageBox(_T("Pls find TE to check!"),_T("Sensor Error"),MB_OK|MB_OKCANCEL|MB_ICONINFORMATION)==IDOK)	
			{
				strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[1];");
				cLen=strlen(CmdBuf);
				if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
				{
					AfxMessageBox(_T("Can not send start cmd to testperf."));
					return 0;
				}
			}
			else
			{			
				strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[2];");
				cLen=strlen(CmdBuf);
				if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
				{
					AfxMessageBox(_T("Can not send start cmd to testperf."));
					return 0;
				}
			}
		}
	}	
	
	return 1;
}
//haibin.li 08.16
int CMainTestPanel::MsgBoxPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	char CmdBuf[128]="";
	char res[256]="";
	size_t cLen;
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}		
	if(Parser.ParserGetPara(pBuf, _T("LED"), ResultBuf, sizeof(ResultBuf)/2))
	{  			
		MyWideCharToMultiByte(ResultBuf,CheckLed,sizeof(CheckLed));		
		if(DialogAmber.DoModal()==IDOK)
		{				
			strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[1];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}	
			return 1;
		}
		else
		{			
			strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[2];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
			return 1;
		}				
	}
	else if(Parser.ParserGetPara(pBuf, _T("CONTENT"), ResultBuf, sizeof(ResultBuf)/2))
	{
		hMsgBoxHook=SetWindowsHookEx(  
			WH_CBT, 
			CBTProc, 
			NULL,   
			GetCurrentThreadId() 
			);	
		MSGBOXPARAMS msgbox;
		msgbox.cbSize = sizeof(MSGBOXPARAMS);
		msgbox.dwContextHelpId = 0;
		msgbox.dwStyle = MB_ICONQUESTION|MB_YESNO|MB_TOPMOST;
		msgbox.hInstance = AfxGetApp()->m_hInstance;
		msgbox.hwndOwner = NULL;
		//msgbox.lpfnMsgBoxCallback = NULL;
		msgbox.lpszCaption = _T("ASK");
		msgbox.lpszText = ResultBuf;		

		MyWideCharToMultiByte(ResultBuf,res,sizeof(res)/2);	
		if(strstr(res,"Top panel pull down"))
		{
			msgbox.lpszText = _T("Top panel pull down?");
		}		
		else if(strstr(res,"Is system led blink normally"))
		{
			msgbox.lpszText = _T("System led blink normally?");
		}
		if(MessageBoxIndirect(&msgbox)==IDYES)
		{
			strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[1];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
		}
		else
		{				
			strcpy_s(CmdBuf,sizeof(CmdBuf),"MSGBOX=RESPONSE[2];");
			cLen=strlen(CmdBuf);
			if (!SendTestPerfData((unsigned char *)CmdBuf, cLen))
			{
				AfxMessageBox(_T("Can not send start cmd to testperf."));
				return 0;
			}
		}
	}
	else if(Parser.ParserGetPara(pBuf, _T("NOTICE"), ResultBuf, sizeof(ResultBuf)/2))
	{
		//hMsgBoxHook=SetWindowsHookEx(  
		//	WH_CBT, 
		//	CBTProc, 
		//	NULL,   
		//	GetCurrentThreadId() 
		//	);	
		//MSGBOXPARAMS msgbox;
		//msgbox.cbSize = sizeof(MSGBOXPARAMS);
		//msgbox.dwContextHelpId = 0;
		//msgbox.hInstance = AfxGetApp()->m_hInstance;
		//msgbox.hwndOwner = NULL;
		////msgbox.lpfnMsgBoxCallback = NULL;
		//msgbox.lpszText = ResultBuf;
		//msgbox.dwStyle = MB_ICONINFORMATION|MB_OK|MB_TOPMOST;
		//msgbox.lpszCaption = _T("ATTENTION");
		//MessageBoxIndirect(&msgbox);
		MyWideCharToMultiByte(ResultBuf,res,sizeof(res)/2);	
		if(strstr(res,"Put the DUT right side!"))
		{
			//msgbox.lpszText = _T("Top panel pull down?");
			sprintf_s(CheckLed,sizeof(res),"%s",res);
			noticeflag=1;
			if (DialogAmber.DoModal()==IDOK)
			{
				noticeflag=0;
			}			
			//AfxMessageBox(_T("Keep the Board.\nAsk PE to analyse."));
		}
		else
		{
			MessageBox(ResultBuf,_T("Tip"),MB_OK);
		}
		
	}		
	return 1;
}

int CMainTestPanel::ControlPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}
	
	if(Parser.ParserGetPara(pBuf, _T("RESULT"), ResultBuf, sizeof(ResultBuf)/2))
	{
		//wcscpy_s(TestResult.Content, sizeof(TestResult.Content)/2, ResultBuf);
		OnTestFinished(ResultBuf);	
	}
	
	if(Parser.ParserGetPara(pBuf, _T("TSNAME"), ResultBuf, sizeof(ResultBuf)/2))
	{
		wcscpy_s(gStationInfo.TestStation.Station_Name, sizeof(gStationInfo.TestStation.Station_Name)/2,ResultBuf);
	}
	if(Parser.ParserGetPara(pBuf, _T("TSMODEL"), ResultBuf, sizeof(ResultBuf)/2))
	{
		wcscpy_s(gStationInfo.uut.UUTName, sizeof(gStationInfo.uut.UUTName)/2,ResultBuf);
	}
	if(Parser.ParserGetPara(pBuf, _T("TSVER"), ResultBuf, sizeof(ResultBuf)/2))
	{
        wcscpy_s(gStationInfo.TestStation.TestProgramVersion, sizeof(gStationInfo.TestStation.TestProgramVersion)/2,ResultBuf);
		wcscpy_s(gStationInfo.TestStation.TestStaID,sizeof(gStationInfo.TestStation.TestStaID)/2,_T("815208"));////Vincent 090708
		AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_STATION, 0, 0);
		GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(true);

		if (dlgrtm)
		{
			CString strSend=_T("[TESTPROPERFVER:");//haibin.li 2011/11/25
			strSend+=gStationInfo.TestStation.TestProgramVersion;
			dlgrtm->m_list_version.SetItemText(0,1,_T(""));
			//dlgrtm->m_list_version.SetItemText(0,1,_T("3.0.2.1"));//haibin.li 2011/12/5
			dlgrtm->m_list_version.SetItemText(1,1,gStationInfo.TestStation.TestProgramVersion);//haibin.li 2011/12/5

			strSend+=_T("][TESTSTATIONID:");
			strSend+=gStationInfo.TestStation.TestStaID;

			dlgrtm->m_list_hostinfo.SetItemText(2,1,gStationInfo.TestStation.TestStaID);//haibin.li 2011/12/5
			strSend+=_T("][HOSTNAME:");
			strSend+=gStationInfo.PcInfo.HostName;

			strSend+=_T("][IPADDRESS:");
			strSend+=gStationInfo.PcInfo.IPAddress;

			strSend+=_T("]");
			//SendRTM((LPCSTR)(LPCWSTR)strSend,DetailLogDlg.TaskID);//haibin.li 2011/11/25	
			USES_CONVERSION;LPCSTR lpcs = NULL;lpcs=T2A(strSend.GetBuffer(strSend.GetLength()));
			SendRTM(lpcs,DetailLogDlg.TaskID);
		}		
	}
	return 1;
}

int CMainTestPanel::OnTestFinished(wchar_t *pBuf)
{
	int TempVal=0;
	char TimeStrBuf[100];
	char Showbuf[1000];
	wchar_t wcTempBuf[1000]=_T("");

	wchar_t wcBuf[1000];
	size_t cLen;
	KillTimer(TPTIMEOUT_TIMER);
	TPTimeOutTime=0;

	//Send result to MYDAS
	if (gStationInfo.PtsInfo.PtsSwitch)
	{
		WaitForSingleObject(hMydasMutex,INFINITE);
        SendResultToMYDAS();
		ReleaseMutex(hMydasMutex);
	}

	//Send result to SFIS
	if(gVariable.Repeat_Time_Flag)
	{
		SFISInformation.SFICSwitchFlag=0;
		SfisStatus=SFISInformation.SFICSwitchFlag;//haibin.li 2011/11/08
	}
	if (!wcscmp(_T("FAIL"), pBuf))
	{
		if (SFISInformation.SFICSwitchFlag&&!gVariable.virtualflag)//SFIS ON haibin.li 2011/12/5
		{
			if (SendResultToSFIS())
			{
				DetailLogDlg.ShowLog(_T("Sending Test Result to SFIS ..."),DetailLogDlg.TaskID);
				//SetTimer(RESPONSE_TIMER,1000, (TIMERPROC)NULL);//Modify Maxwell 20101101 
				SetTimer(RESPONSE_TIMER+TestPort+10,1000, (TIMERPROC)NULL);//Modify Maxwell 20101101
				DWORD WaitRevRtn = 0;
				WaitRevRtn = WaitForSingleObject(gACKEvent[TestPort],gStationInfo.UiDisplay.SfisTimeout*1000L); // 20-second time-out,20 need be set in config
				if (!ResetEvent(gACKEvent[TestPort])) 
				{ 
				}  	
				if (WAIT_TIMEOUT == WaitRevRtn)
				{
					char *pRecordData=0;
					size_t ConvertedChars=0;
					size_t SizeLen= wcslen(SFISInformation.TesterData);
					pRecordData = (char *)malloc(SizeLen+1);
					if (pRecordData)
					{   	
						wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISInformation.TesterData, SizeLen);
						AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
					}
				}
				/////////////////090617
				//KillTimer(RESPONSE_TIMER);//Maxwell 20101101
				KillTimer(RESPONSE_TIMER+TestPort+10);//Maxwell 20101101
				TempVal=ResponseTime;
				ResponseTime=0;
				sprintf_s(TimeStrBuf,sizeof(TimeStrBuf),"%d",TempVal);
				memset(Showbuf,0,sizeof(Showbuf));
				strcpy_s(Showbuf,sizeof(Showbuf), "SFIS Response Time Elapse:  ");	
				strcat_s(Showbuf,sizeof(Showbuf),TimeStrBuf);
				mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, Showbuf, strlen(Showbuf));
				DetailLogDlg.ShowLog(wcBuf,DetailLogDlg.TaskID);
				/////////////////090617
			}
		}
		else////for virtual smo
		{
			char *pRecordData=0;
			size_t ConvertedChars=0;
			size_t SizeLen= wcslen(SFISData);
			pRecordData = (char *)malloc(SizeLen+1);
			if (pRecordData)
			{   	
				wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISData, SizeLen);
				AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
			}
		}
		FailedUIType();
		GetDlgItem(IDC_BTN_SFIS)->EnableWindow(true);
	}
	else if (!wcscmp(_T("PASS"),pBuf))
	{
		if (SFISInformation.SFICSwitchFlag&&!gVariable.virtualflag)//SFIS ON haibin.li 2011/12/5 
		{
			DetailLogDlg.ShowLog(_T("Sending Test Result to SFIS ..."),DetailLogDlg.TaskID);
			//SetTimer(RESPONSE_TIMER,1000, (TIMERPROC)NULL);///////////////////start count time //Maxwell 20101101
			SetTimer(RESPONSE_TIMER+TestPort+10,1000, (TIMERPROC)NULL);///////////////////start count time //Maxwell 20101101

			if (SendResultToSFIS())
			{
				DWORD WaitRevRtn = 0;
				WaitRevRtn = WaitForSingleObject(gACKEvent[TestPort],gStationInfo.UiDisplay.SfisTimeout*1000); // 20-second time-out
				if (!ResetEvent(gACKEvent[TestPort])) 
				{ 

				}  	
				if (WAIT_TIMEOUT == WaitRevRtn)
				{
					char *pRecordData=0;
					size_t ConvertedChars=0;
					size_t SizeLen= wcslen(SFISInformation.TesterData);
					pRecordData = (char *)malloc(SizeLen+1);
					if (pRecordData)
					{   	
						wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISInformation.TesterData, SizeLen);
						AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
					}
					//AfxMessageBox(_T("The SMO has not feedback"));
					wcscpy_s(TestResult.ErrorCode,sizeof(TestResult.ErrorCode)/2,_T("SN00_03"));
					//////////////////////////////
					//KillTimer(RESPONSE_TIMER);//Maxwell 20101101
					KillTimer(RESPONSE_TIMER+TestPort+10);//Maxwell 20101101
					ResponseTime=0;
					//haibin.li 2012/2/9
					//DetailLogDlg.ShowLog(_T("SFIS Response Time Elapse more than 20s"),DetailLogDlg.TaskID);
					sprintf_s(TimeStrBuf,sizeof(TimeStrBuf),"%d",gStationInfo.UiDisplay.SfisTimeout);
					memset(Showbuf,0,sizeof(Showbuf));
					strcpy_s(Showbuf,sizeof(Showbuf), "SFIS Response Time Elapse more than ");	
					strcat_s(Showbuf,sizeof(Showbuf),TimeStrBuf);
					strcat_s(Showbuf,sizeof(Showbuf),"s.");
					mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, Showbuf, strlen(Showbuf));
					DetailLogDlg.ShowLog(wcBuf,DetailLogDlg.TaskID);
					//haibin.li 2012/2/9
					///////////////////////////////
					FailedUIType();
					this->RepeatTest();
					return 0;
				}
			}
			/////////////////090617
			//KillTimer(RESPONSE_TIMER);//Maxwell 20101101
			KillTimer(RESPONSE_TIMER+TestPort+10);//Maxwell 20101101
			TempVal=ResponseTime;
			ResponseTime=0;
			sprintf_s(TimeStrBuf,sizeof(TimeStrBuf),"%d",TempVal);
			memset(Showbuf,0,sizeof(Showbuf));
			strcpy_s(Showbuf,sizeof(Showbuf), "SFIS Response Time Elapse:  ");	
			strcat_s(Showbuf,sizeof(Showbuf),TimeStrBuf);
			mbstowcs_s(&cLen, wcBuf, sizeof(wcBuf)/2, Showbuf, strlen(Showbuf));
			DetailLogDlg.ShowLog(wcBuf,DetailLogDlg.TaskID);
			/////////////////090617
		}
		else
		{
			char *pRecordData=0;
			size_t ConvertedChars=0;
			size_t SizeLen= wcslen(SFISData);
			pRecordData = (char *)malloc(SizeLen+1);
			if (pRecordData)
			{   	
				wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISData, SizeLen);
				AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
			}
			//pRecordData=0;//haibin.li 2011/11/11 to release buffer
		}
		PassedUIType();
		GetDlgItem(IDC_BTN_SFIS)->EnableWindow(true);
	}
    gVariable.virtualflag=0;//haibin.li 2011/12/5
	this->RepeatTest();
	return 1;
}

int CMainTestPanel::SendResultToSFIS()
{
	char *pRecordData=0;
	size_t ConvertedChars=0;
	size_t SizeLen= wcslen(SFISInformation.TesterData);
	pRecordData = (char *)malloc(SizeLen+1);
	if (pRecordData)
	{   	
		wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISInformation.TesterData, SizeLen);
		AmPostThreadMes(SFIS_TASK, WM_SFIS_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
		//free(pRecordData);//haibin.li 2011/11/12 release memory buffer
	}
	else
	{
		AfxMessageBox(_T("SFIS DATA: log data empty"));
		return 0;
	}
	return 1;
}
int CMainTestPanel::SendResultToMYDAS()
{
	//Get DCT information
	char *pRecordData=0;
	char *pErrorData=0;
	char *pMainInforData=0;//Maxwell 100316
	char *pDetailLog=0;	
	char *pwcsDetailLog=0;
	CString strDetailLog;

	size_t ConvertedChars=0;
	size_t ConvertedCharsError=0;
	size_t ConvertedCharsMain=0;//Maxwell 100316

	//need continue debug
	size_t SizeLen= wcslen(PtsInformation.RecordData);
	pRecordData = (char *)malloc(SizeLen+1);
	if (pRecordData)
	{   	
		//Unicode to non unicode.
		wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, PtsInformation.RecordData, SizeLen);
		//wcscpy_s((wchar_t *)pRecordData, SizeLen/2, PtsInformation.RecordData);
		AmPostThreadMes(MYDAS_TASK, WM_MYDAS_RECORD_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
	}

	size_t SizeLenError = wcslen(PtsInformation.ErrorData);
	pErrorData = (char *)malloc(SizeLenError+1);
	if (pErrorData)
	{   	
		//Unicode to non unicode.
		wcstombs_s(&ConvertedCharsError, pErrorData,SizeLenError+1, PtsInformation.ErrorData, SizeLenError);
		//wcscpy_s((wchar_t *)pRecordData, SizeLen/2, PtsInformation.RecordData);
		AmPostThreadMes(MYDAS_TASK, WM_MYDAS_LOG_DATA, (unsigned int)pErrorData, (unsigned int)SizeLenError);
	}

	//Maxwell 100316
	size_t SizeLenMain = wcslen(PtsInformation.MainInforData);
	pMainInforData = (char *)malloc(SizeLenMain+1);
	if (pMainInforData)
	{   	
		//Unicode to non unicode.
		wcstombs_s(&ConvertedCharsMain, pMainInforData,SizeLenMain+1, PtsInformation.MainInforData, SizeLenMain);
		//wcscpy_s((wchar_t *)pRecordData, SizeLen/2, PtsInformation.RecordData);
		AmPostThreadMes(MYDAS_TASK, WM_MYDAS_MAIN_DATA, (unsigned int)pMainInforData, (unsigned int)SizeLenMain);
	}
	//Maxwell 100316
	//Get detail test log data
	//Maxwell 100311
	//int DetailLogSize=DetailLogDlg.GetLogSize();
	//if (DetailLogSize)
	//{
	//	pDetailLog=0;
	//	pDetailLog=(char *)malloc(DetailLogSize+1);
	//	if (pDetailLog)
	//	{
	//		DetailLogDlg.GetHistoryLog(strDetailLog);
	//		//Unicode to non unicode.
	//		wcstombs_s(&ConvertedChars, pDetailLog,DetailLogSize+1, strDetailLog, DetailLogSize);
	//		AmPostThreadMes(MYDAS_TASK, WM_MYDAS_LOG_DATA, (unsigned int)pDetailLog, DetailLogSize);
	//	}
	//}
	//else
	//{
	//	AfxMessageBox(_T("MYDAS DATA: log data empty"));
	//}
	//Maxwell 100311

//	DetailLogDlg.GetHistoryLog();
	return 1;
}

int CMainTestPanel::SfisPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[500];
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}

	//Show the error code
	if(Parser.ParserGetPara(pBuf, _T("ERRORCODE"), ResultBuf, sizeof(ResultBuf)/2))
	{
		wcscpy_s( TestResult.ErrorCode, sizeof(TestResult.ErrorCode)/2, ResultBuf);
	}

    wcscpy_s( SFISInformation.TesterData, sizeof(SFISInformation.TesterData)/2, pBuf);
	
	return 1;
}

int CMainTestPanel::LogPerf(wchar_t *pBuf)
{
    CAmParser Parser;
	Parser.SetStartStopTag(_T("["),_T("]"));

	//get record data
	if(wcsstr(pBuf,_T("CONTENT[")))
	{
		memset((void *)&PtsInformation.RecordData, 0, sizeof( PtsInformation.RecordData));
	}
	else if(wcsstr(pBuf,_T("ERROR[")))
	{
		memset((void *)&PtsInformation.ErrorData, 0, sizeof( PtsInformation.ErrorData));
	}
	//Maxwell 100316
	else if(wcsstr(pBuf,_T("MAINFOR[")))
	{
		memset((void *)&PtsInformation.MainInforData, 0, sizeof( PtsInformation.MainInforData));
	}
	//Maxwell 100316
	else
	{

	}

	if(Parser.ParserGetPara(pBuf, _T("CONTENT"), PtsInformation.RecordData, sizeof(PtsInformation.RecordData)/2))
	{

	}
	else
	{

	}
	//get error data
	if(Parser.ParserGetPara(pBuf, _T("ERROR"), PtsInformation.ErrorData, sizeof(PtsInformation.ErrorData)/2))
	{

	}
	else
	{

	}
	//Maxwell 100316
	//get Main information data
	if(Parser.ParserGetPara(pBuf, _T("MAINFOR"), PtsInformation.MainInforData, sizeof(PtsInformation.MainInforData)/2))
	{

	}
	else
	{

	}
	//Maxwell 100316

	//send record data

	//get all log data	

	//send all log data
	
	return 1;
}

int CMainTestPanel::ItemPerf(wchar_t *pBuf)
{
	CAmParser Parser;
	wchar_t ResultBuf[300];
	TEST_ITEM TestItemInfoTemp;
	Parser.SetStartStopTag(_T("["),_T("]"));
	if (!pBuf)
	{
		return 0;
	}

	CString strCurrentTestItem;
	
	//TEST->UI:ITEM=NUMBER[12];
	//TEST->UI:ITEM=ID[0]NAME[Check-video-quality];
	//TEST->UI:ITEM=ID[0]RESULT[PASS];	
	
	if (Parser.ParserGetPara(pBuf, _T("NUMBER"), ResultBuf, sizeof(ResultBuf)/2))
	{
		TestItemInfoTemp.ItemNum=_wtoi(ResultBuf);
		if (TestItemInfoTemp.ItemNum>0)
		{
			m_TestProgressBar.SetRange(0,TestItemInfoTemp.ItemNum);
			TestItemInfo.ItemNum=TestItemInfoTemp.ItemNum;
		}

	}
	if(Parser.ParserGetPara(pBuf, _T("ID"), ResultBuf, sizeof(ResultBuf)/2))
	{
		TestItemInfo.ID=TestItemInfoTemp.ID=_wtoi(ResultBuf);

	}    
	if(Parser.ParserGetPara(pBuf, _T("NAME"), ResultBuf, sizeof(ResultBuf)/2))
	{
		wcscpy_s(TestItemInfoTemp.Name, sizeof(TestItemInfoTemp.Name)/2, ResultBuf);
		//Show current test item
		//strCurrentTestItem.Format(_T("%d. "),TestItemInfo.ID);
		//strCurrentTestItem+=TestItemInfoTemp.Name;
		//haibin.li 2012/1/12
		CString strTemp;
		strTemp.Format(_T("%d. "),TestItemInfo.ID);
		strCurrentTestItem=TestItemInfoTemp.Name;
		int len=strCurrentTestItem.Find(':');
		strCurrentTestItem=strCurrentTestItem.Right(strCurrentTestItem.GetLength()-len-1);
		strCurrentTestItem=strTemp+strCurrentTestItem;
		//haibin.li 2012/1/12
		SetDlgItemText(IDC_STC_CURRENT_ITEM,strCurrentTestItem);//Show current test item
		DetailLogDlg.ShowLog(_T("   "),DetailLogDlg.TaskID);
		DetailLogDlg.ShowLog(_T("----------------------------------------------------------------------------------"),DetailLogDlg.TaskID);
		//TestItemSummary.InsertItem(TestItemInfo.ID,ResultBuf);
	}
	if(Parser.ParserGetPara(pBuf, _T("RESULT"), ResultBuf, sizeof(ResultBuf)/2))
	{
     	wcscpy_s(TestItemInfoTemp.Result, sizeof(TestItemInfoTemp.Result)/2, ResultBuf);
		//TestItemSummary.SetItemText(TestItemInfo.ID, 1, TestItemInfoTemp.Result);
		m_TestProgressBar.SetPos(TestItemInfo.ID);
		//TestItemInfoList.push_back(TestItemInfoTemp);
	}

	return 1;
}

void CMainTestPanel::SetBtnParameter(int CtrlID, int enable, wchar_t *pContent)
{
	GetDlgItem(CtrlID)->EnableWindow(enable);
	if (pContent)
	{
		SetDlgItemText(CtrlID,pContent);
	}
	return;
}
//////////////////////////////////////////////////////////////////////////////////////
void CMainTestPanel::StandbyUIType()
{
	//haibin.li 2011/12/5
	SendRTM("[STANDBY]",DetailLogDlg.TaskID);
	if (userinfo.name.Compare(_T("")))
	{	
		pTempCtr->testflag[DetailLogDlg.TaskID-1]=false;
		if (pTempCtr->testflag[0]==false&&pTempCtr->testflag[1]==false)
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(TRUE);
		}
		else
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
		}		
	}
	//pDialogConf->GetDlgItem(IDC_ITEM_MODIFY)->EnableWindow(true);
	//pDialogConf->GetDlgItem(IDC_ITEM_TWO)->EnableWindow(true);
	//haibin.li 2011/12/5
	TestingTime=_T("00:00");
	SetDlgItemText(IDC_BIG_RESULT, _T("STANDBY"));
	SetDlgItemText(IDC_STC_TESTTIME, _T("00:00"));
	if(!SFISInformation.SFICSwitchFlag)
	{
		SetBtnParameter(IDC_BTN_START_TEST, 1, _T("TEST"));
	}
	else
	{
		SetBtnParameter(IDC_BTN_START_TEST, 0, _T("TEST"));
	}
	m_TestProgressBar.SetPos(0);//set initial progress bar.

	wcscpy_s(TestResult.ErrorCode, sizeof(TestResult.ErrorCode)/2,_T(""));
	SetDlgItemText(IDC_STC_ERROR_CODE,TestResult.ErrorCode);//clear error code display
	SetDlgItemText(IDC_ERRORCODE,_T(""));//haibin.li 2012/1/18

	SetDlgItemText(IDC_STC_CURRENT_ITEM,_T(""));//clear current test item

	SetDlgItemText(IDC_STC_SMALL_RESULT,_T(""));
	SetDlgItemText(IDC_SN,_T(""));//haibin.li 2011/12/1

	if (gStationInfo.UiDisplay.StandByTime<=0)
	{
		StandbyTimeLimit=30;
	}
	else
	{
		StandbyTimeLimit=gStationInfo.UiDisplay.StandByTime;
	}
	if (1==StandbyTimeFlag)
	{
		KillTimer(STANDBY_TIMER);
		StandbyTimeFlag=0;
		StandbyTime=0;
	}
	//pTestPanel->Invalidate(NULL);
	return;
}

void CMainTestPanel::PassedUIType()
{
    //------haibin.li 2011/12/5
	dlgrtm->m_list_runstatus.SetItemText(2,DetailLogDlg.TaskID,_T("Passed"));
	SendRTM("[PASSED]",DetailLogDlg.TaskID);
	if (userinfo.name.Compare(_T("")))
	{	
		pTempCtr->testflag[DetailLogDlg.TaskID-1]=false;
		if (pTempCtr->testflag[0]==false&&pTempCtr->testflag[1]==false)
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(TRUE);
		}
		else
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
		}		
	}
	if (DetailLogDlg.TaskID==1)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_MODIFY)->EnableWindow(true);
	}
	if (DetailLogDlg.TaskID==2)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_TWO)->EnableWindow(true);
	}
	//------haibin.li 2011/12/5
	//set progress bar to 100%
	CString strTemp;
	CString strServer= _T("pass");
	int Low,Uper;
	m_TestProgressBar.GetRange(Low, Uper);
	m_TestProgressBar.SetPos(Uper);

	ShowTestResultStat(1);
    SetDlgItemText(IDC_BIG_RESULT, _T("PASSED"));
	//Maxwell 1222
	//char temp[16] = "fail";
	char buf[32] = "";

	strServer += gStationInfo.Server1Client.ClientNumber;
	//strServer += TestResult.ErrorCode;
	size_t ConvertedChars=0;
	wcstombs_s(&ConvertedChars, buf, sizeof(buf), strServer,wcslen(strServer));
	//sprintf_s(Server1Client.IPAddr, sizeof(Server1Client.IPAddr), "%s",buflog);
	//strcat_s(temp,sizeof(temp),buf);

	AmbitSend(&Server1Client,buf,(int)(sizeof(buf)));
	GetDlgItem(IDC_BTN_SFIS)->EnableWindow(true);
	if(!SFISInformation.SFICSwitchFlag)
	{

		SetBtnParameter(IDC_BTN_START_TEST, 1, _T("TEST"));
	}
	else
	{
		SetBtnParameter(IDC_BTN_START_TEST, 0, _T("TEST"));
	}

	//Show small status
	//strTemp=_T("PASS: ");
	strTemp=_T("");//haibin.li 2011/11/10
	if (wcslen(SFISInformation.TesterData))
	{
		strTemp+=SFISInformation.TesterData;
	}
	//SetDlgItemText(IDC_STC_SMALL_RESULT,strTemp);
	ShowSnMac(SFISInformation.TesterData);//haibin.li 2011/12/1

	KillTimer(TEST_TIMER);
	KillTimer(TPTIMEOUT_TIMER);	

	if (0==StandbyTimeFlag)
	{
		SetTimer(STANDBY_TIMER,1000, (TIMERPROC)NULL);
		StandbyTimeFlag=1;
	}
	///////////////////////////////////////////////clear sfisbuffer
	if(SFISInformation.SFICSwitchFlag)//vincent 20090709
	{
		char *pRecordData=0;
		size_t ConvertedChars=0;
		size_t SizeLen= wcslen(SFISData);
		pRecordData = (char *)malloc(SizeLen+1);
		if (pRecordData)
		{   	
			wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISData, SizeLen);
			AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
		}
	}
	//////////////////////////////////////////////

	//add start by Talen 2011/09/10
	wchar_t localFilePath[512];

	GetCurrentDirectory(sizeof(localFilePath)/2, localFilePath);
	wcscat_s(localFilePath,sizeof(localFilePath)/2,_T("\\testing.txt"));
	DeleteFile(localFilePath);
	//add end by Talen 2011/09/10

	return;
}

void CMainTestPanel::FailedUIType()
{
	//haibin.li 2011/12/5
	dlgrtm->m_list_runstatus.SetItemText(2,DetailLogDlg.TaskID,_T("Failed"));
	CString   strSend;
	//char str[64]="";
	strSend=_T("[FAILED|ERRORCODE:")+(CString)TestResult.ErrorCode+_T("]");
	//WideCharToMultiByte(CP_OEMCP,0,(LPCTSTR)strSend,-1,str,sizeof(str),0,false);
	USES_CONVERSION;LPCSTR lpcs = NULL;lpcs=T2A(strSend.GetBuffer(strSend.GetLength()));
	SendRTM(lpcs,DetailLogDlg.TaskID);
	if (userinfo.name.Compare(_T("")))
	{	
		pTempCtr->testflag[DetailLogDlg.TaskID-1]=false;
		if (pTempCtr->testflag[0]==false&&pTempCtr->testflag[1]==false)
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(TRUE);
		}
		else
		{
			pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
		}
		
	}
	if (DetailLogDlg.TaskID==1)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_MODIFY)->EnableWindow(true);
	}
	if (DetailLogDlg.TaskID==2)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_TWO)->EnableWindow(true);
	}
	//haibin.li 2011/12/5
    CString strTemp;
	ShowTestResultStat(0);
	SetDlgItemText(IDC_BIG_RESULT, _T("FAILED"));
	//TestPort
	GetDlgItem(IDC_BTN_SFIS)->EnableWindow(true);
	if(!SFISInformation.SFICSwitchFlag)
	{
		SetBtnParameter(IDC_BTN_START_TEST, 1, _T("TEST"));
	}
	else
	{
		SetBtnParameter(IDC_BTN_START_TEST, 0, _T("TEST"));
	}	

	//Show error code
	if (wcslen(TestResult.ErrorCode))
	{
		//CString str =_T("ERRORCODE:");
		CString str =_T("");
		CString strServer =_T("fail");
		str+=TestResult.ErrorCode;
		SetDlgItemText(IDC_ERRORCODE,_T("ERRORCODE:"));//haibin.li 2012/1/18
		SetDlgItemText(IDC_STC_ERROR_CODE,str);//Show the error code
		//Maxwell 1222
		//char temp[16] = "fail";
		char buf[512] = "";

		strServer += gStationInfo.Server1Client.ClientNumber;
		strServer += TestResult.ErrorCode;
		size_t ConvertedChars=0;
		wcstombs_s(&ConvertedChars, buf, sizeof(buf), strServer,wcslen(strServer));
		//sprintf_s(Server1Client.IPAddr, sizeof(Server1Client.IPAddr), "%s",buflog);
		//strcat_s(temp,sizeof(temp),buf);

		AmbitSend(&Server1Client,buf,(int)(sizeof(buf)));
	}	

	//Show small status
	//strTemp=_T("FAIL: ");
	strTemp=_T("");//haibin.li 2011/11/10
	if (wcslen(SFISInformation.TesterData))
	{
		strTemp+=SFISInformation.TesterData;
	}
	//SetDlgItemText(IDC_STC_SMALL_RESULT,strTemp);
	ShowSnMac(SFISInformation.TesterData);//haibin.li 2011/12/1

	KillTimer(TEST_TIMER);
	KillTimer(TPTIMEOUT_TIMER);	
	if (0==StandbyTimeFlag)
	{
		SetTimer(STANDBY_TIMER,1000, (TIMERPROC)NULL);
		StandbyTimeFlag=1;
	}
	///////////////////////////////////////////////clear sfis-buffer
	if(SFISInformation.SFICSwitchFlag)//vincent 20090709
	{
		char *pRecordData=0;
		size_t ConvertedChars=0;
		size_t SizeLen= wcslen(SFISData);
		pRecordData = (char *)malloc(SizeLen+1);
		if (pRecordData)
		{   	
			wcstombs_s(&ConvertedChars, pRecordData,SizeLen+1, SFISData, SizeLen);
			AmPostThreadMes(SFIS_TASK, WM_DCT_DATA, (unsigned int)pRecordData, (unsigned int)SizeLen);
		}
	}
	//////////////////////////////////////////////

	//add start by Talen 2011/09/10
	wchar_t localFilePath[512];

	GetCurrentDirectory(sizeof(localFilePath)/2, localFilePath);
	wcscat_s(localFilePath,sizeof(localFilePath)/2,_T("\\testing.txt"));
	DeleteFile(localFilePath);
	//add end by Talen 2011/09/10

	return;
}

void CMainTestPanel::TestingUIType()
{
	//haibin.li 2011/12/5
	dlgrtm->m_list_runstatus.SetItemText(2,DetailLogDlg.TaskID,_T("Testing"));
	SendRTM("[TESTING]",DetailLogDlg.TaskID);
	if (DetailLogDlg.TaskID==1)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_MODIFY)->EnableWindow(false);
	}
	if (DetailLogDlg.TaskID==2)
	{
		pDialogConf->GetDlgItem(IDC_ITEM_TWO)->EnableWindow(false);
	}	
	//haibin.li 2011/12/5
	CString strTemp;
	SetBtnParameter(IDC_BTN_START_TEST, 0, _T("STOP"));
	SetDlgItemText(IDC_BIG_RESULT, _T("TESTING"));
	GetDlgItem(IDC_BTN_SFIS)->EnableWindow(false);
	ResetTestTime();
	
	wcscpy_s(TestResult.ErrorCode, sizeof(TestResult.ErrorCode)/2,_T(""));
	SetDlgItemText(IDC_STC_ERROR_CODE,TestResult.ErrorCode);//clear error code display	

	SetDlgItemText(IDC_STC_CURRENT_ITEM,_T(""));//clear current test item
	SetDlgItemText(IDC_ERRORCODE,_T(""));//haibin.li 2012/1/18

	//Show small status
	strTemp=_T("TESTING: ");
	if (wcslen(SFISInformation.SMOData))
	{
		strTemp+=SFISInformation.SMOData;
	}
	//SetDlgItemText(IDC_STC_SMALL_RESULT,strTemp);
	ShowSnMac(SFISInformation.SMOData);//haibin.li 2011/12/1

	SetTimer(TEST_TIMER,1000, (TIMERPROC)NULL);
	SetTimer(TPTIMEOUT_TIMER,1000, (TIMERPROC)NULL); 
	
	if (1==StandbyTimeFlag)
	{
		KillTimer(STANDBY_TIMER);
		StandbyTimeFlag=0;
		StandbyTime=0;
	}
	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);////vincent 090706
	m_tab.SetCurSel(0);//haibin.li 2011/11/30
	return;
}

void CMainTestPanel::OnBnClickedBtnStartTest()
{
	//Maxwell add 100930 for SFIS no response

	// TODO: Add your control notification handler code here 
	//haibin.li 
	pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
	if (!userinfo.name.Compare(_T("")))
	{
		if (DlgUser.DoModal()==IDCANCEL)
		{
			return;
		}
	}
	if (noticeflag==1)
	{
		//AfxMessageBox(_T("Please contact PE to analyse."));
		return;
	}
	if (SfisStatus==0)
	{	
		if(RepeatTime==1||RepeatTime>gStationInfo.UiDisplay.RepeatTime)
		{
			if( MessageBox(_T("Are you sure to test?"),_T("ASk"),MB_OK|MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
			{
				if (pTempCtr->testflag[0]==false&&pTempCtr->testflag[1]==false)
				{
					pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(TRUE);
				}
				else
				{
					pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
				}
				return;
			}
			else
			{				
				pTempCtr->GetDlgItem(IDC_STATIC_OUT)->EnableWindow(FALSE);
			}
		}
		if(RepeatTime==gStationInfo.UiDisplay.RepeatTime)
		{
			RepeatTime++;	
		}
	}	
	pTempCtr->testflag[DetailLogDlg.TaskID-1]=true;
	//haibin.li 
	
	if(gStationInfo.EquipInfo.EqipEnableFlag)
	{
		if(EquipFlag)  
		{
			AfxMessageBox(_T("Some of the equipment has been used out of time!\nPlease reload the program!"));
			return;
			//::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
		}
	}
	if(gStationInfo.PtsInfo.PtsSwitch)
	{
		if(!gStationInfo.PtsInfo.ConnectStatus)
		{
			AfxMessageBox(_T("The PTS is not connect\nPlease reload the program!"),MB_OK);
			::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
		}
	}
	char CmdBuf[1024];
	size_t ConvertedChars=0;
	m_TestProgressBar.SetPos(0);//090728
    //UI->TEST:CONTROL=CMD[START];SFIS=SWITCH[ON/OFF];SFIS=SN[6F8821345]WIFIMAC[001122334455]ETHMAC[0011223344455]HDCKEY[1234567];
	
	CString CmdStr;
	if(DetailLogDlg.TaskID==1)
	{
	    if(sfisStartFlag)
		{
			CmdStr=UserTemp1+sendstartitem+_T("CONTROL=CMD[SFIS_START];");//Start the test
		}
		//add end by Talen 2011/10/26
		else
		{
			CmdStr=UserTemp1+sendstartitem+_T("CONTROL=CMD[START];");//Start the test
		}
		sendstartitem=_T("");//haibin.li 2011/11/07
		UserTemp1=_T("");//haibin.li 2011/11/15
	}
	else if(DetailLogDlg.TaskID==2)
	{
		if(sfisStartFlag)
		{
			CmdStr=UserTemp2+sendstartitemtwo+_T("CONTROL=CMD[SFIS_START];");//Start the test
		}
		//add end by Talen 2011/10/26
		else
		{
			CmdStr=UserTemp2+sendstartitemtwo+_T("CONTROL=CMD[START];");//Start the test
		}
		sendstartitemtwo=_T("");//haibin.li 2011/11/15
		UserTemp2=_T("");//haibin.li 2011/11/15
	}
	
	sfisStartFlag=0;//add by Talen 2011/10/26

	CString SFISData;
	
	if (SFISInformation.SFICSwitchFlag)
	{
		//add start by LiYongjun 2011/03/21
		if(s_variable.VirtSmoFlag)
		{
			CmdStr+=_T("SFIS=SWITCH[VISUAL_ON];");
		}
		else
		{
		//add end by LiYongjun 2011/03/21
		CmdStr+=_T("SFIS=SWITCH[ON];");//set SFIS ON
		}
		SFISData = SFISInformation.SMOData;
	}
	else
	{
        CmdStr+=_T("SFIS=SWITCH[OFF];");//set SFIS OFF
		SFISData = _T("");
	}

	//set sfis data
	CmdStr+=_T("SFIS=");
	if (SFISData.GetLength())
	{
        CmdStr+=SFISData;
	}
	else
	{
		CmdStr+=_T("EMPTY[EMPTY]");
	}
	CmdStr+=_T(";");
	if(FixtureNum==1)
	{	
		CString temp_fixid=_T("FIXTURE=FIXID[1];");
		wcstombs_s(&ConvertedChars, CmdBuf,sizeof(CmdBuf), temp_fixid, temp_fixid.GetLength());
		if (!SendTestPerfData((unsigned char *)CmdBuf, ConvertedChars))
		{
			AfxMessageBox(_T("Can not send fix id cmd to testperf."));
			return;
		}
	}
	if(FixtureNum==2)
	{	
		CString temp_fixid=_T("FIXTURE=FIXID[2];");
		wcstombs_s(&ConvertedChars, CmdBuf,sizeof(CmdBuf), temp_fixid, temp_fixid.GetLength());
		if (!SendTestPerfData((unsigned char *)CmdBuf, ConvertedChars))
		{
			AfxMessageBox(_T("Can not send fix id cmd to testperf."));
			return;
		}
	}	

	//Unicode to non unicode.
	wcstombs_s(&ConvertedChars, CmdBuf,sizeof(CmdBuf), CmdStr, CmdStr.GetLength());	

	if (!SendTestPerfData((unsigned char *)CmdBuf, ConvertedChars))
	{
         AfxMessageBox(_T("Can not send start cmd to testperf."));
		 return;
	}
	//SetTimer(TPTIMEOUT_TIMER,1000, (TIMERPROC)NULL); 

	//Clear the log display buf
    DetailLogDlg.ClearLog(DetailLogDlg.TaskID);

	//Control UI type
	TestingUIType();
	return;
}

DWORD CMainTestPanel::SendTestPerfData(unsigned char *pBuf, DWORD dwSize)
{
	if (pBuf)
	{
		if (pTIO)
		{
			return ((*pTIO).AmSend(pBuf, dwSize));//time out 1 second
		}
	}
	return 0;
}

int CMainTestPanel::IntialTester()
{
	CTesterControl TestControler;
	//Prepare Tester command line
	CString strTesterCMDLine;
	CString strTemp;

	strTesterCMDLine=_T("TestPROPerf.exe -i 127.0.0.1");

	strTemp.Format(_T(" -p %d"),TesterPort);
	strTesterCMDLine+=strTemp;
	//MessageBox(strTesterCMDLine);
	if (!TestControler.StartTeser(strTesterCMDLine,LOCAL_TESTER))
	{
		strTesterCMDLine+=_T(" Can not be opened.");
		MessageBox(strTesterCMDLine);
		SetBtnParameter(IDC_BTN_START_TEST, 0, _T("TEST"));
		return 0;
	}

	pTIO=(CAmTesterIO *)new(CAmTesterIO);
	if (pTIO)
	{
		TesterIOFlag=1;
		if (!(*pTIO).Start(TesterPort))
		{
			SetBtnParameter(IDC_BTN_START_TEST, 0, _T("TEST"));
			return 0;
		}
		(*pTIO).SetRevDataThreadID(TaskID);

		AutoFixtureThreadID = TaskID;//Maxwell 1224		
		if (TaskID)
		{
		}	
	}

	return 1;
}

void CMainTestPanel::ExitTester()
{
	CString CmdStr;
	size_t ConvertedChars;
	char CmdBuf[500];
	//Send exit cmd to tester perform.
    //CONTROL=CMD[EXIT];
	//Unicode to non unicode.
	CmdStr=_T("CONTROL=CMD[EXIT];");
	wcstombs_s(&ConvertedChars, CmdBuf,sizeof(CmdBuf), CmdStr, CmdStr.GetLength());
	SendRTM("[STOPPED]",DetailLogDlg.TaskID);//haibin.li 2011/11/19
	
	if (!SendTestPerfData((unsigned char *)CmdBuf, ConvertedChars))
    {
        // MessageBox(_T("Can not send start cmd to test-perf."));
		// return;
	}	

	if (1==TesterIOFlag)
	{
		if (pTIO)
		{
			(*pTIO).Stop();
			Sleep(1000);
			delete pTIO;
			pTIO=0;
		}
		TesterIOFlag=0;
	}	
	
	return;
}


/////////////////////////////////////////////////////////////////////////
//For total management all UI information and control logic
//It distribute UI control message for all sub dialog.
UINT WINAPI UITesterCtrlTask(LPVOID lpPara)
{
	CMainTestPanel *pMtp=(CMainTestPanel *)lpPara;
	MSG msg;
	int i=0;

	//HANDLE Hmutex;
	//Hmutex = CreateMutex(NULL,NULL,_T("WORKTESTER"));
	//HANDLE Hmutex;
	CString IPName;
	IPName.Format(_T("WORKTESTER%d"),pMtp->TestPort);
	Hmutex = CreateMutex(NULL,NULL,IPName);

	//Initial tester
	Sleep(100);//Maxwell 101011
	pMtp->IntialTester();	
	pTemp=pMtp;//haibin.li 2011/11/09
	//process tester
	while(TRUE)
	{
		if (!WaitMessage())
		{
			continue;
		}
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_TESTPERF_DATA:
				pMtp->ParseTestPerfData((unsigned char *)msg.wParam, (DWORD)msg.lParam);
				break;
				//Maxwell 1224
			case WM_AUTOFIX_DATA:
				pMtp->ParseServer1Data((unsigned char *)msg.wParam, (DWORD)msg.lParam);
				break;
				//Maxwell 1224
			case WM_SFIS_DATA:
				WaitForSingleObject(Hmutex,INFINITE);
				pMtp->ParseSFISData((unsigned char *)msg.wParam, (DWORD)msg.lParam);
				ReleaseMutex(Hmutex);
				break;
			case WM_DCT_DATA:
				pMtp->SetSfisLink();
				break;
			case WM_UPDATE_INFO_EQUIP:
				pMtp->ParseEquipData((unsigned char *)msg.wParam, (DWORD)msg.lParam);
				break;				
			case WM_EXIT:
				goto ExitLabel;
				break;
			default:
				;
			}
		}
	}

ExitLabel:

	//exit tester
	//WaitForSingleObject(pMtp->hExit,INFINITE);//haibin.li 2011/11/29
	pMtp->ExitTester();
	//ReleaseMutex(pMtp->hExit);//haibin.li 2011/11/29
	return 0;
}

void CMainTestPanel::OnBnClickedBtnShowlog()
{
	// TODO: Add your control notification handler code here

	if (LogDlgFlag)
	{
		LogDlgFlag=0;
		DetailLogDlg.Create(IDD_DETAIL_LOG_DLG, this);
		DetailLogDlg.ShowWindow(SW_SHOW);
	}
	else
	{
		DetailLogDlg.ShowWindow(SW_SHOW);
		//AfxMessageBox(_T("Have already log dialog open!"));//vincent 20090706
	}

	AmPostThreadMes(MAIN_VIEW_TASK, WM_SWTO_MAIN_TEST_PANEL, 0, 0);
	m_tab.SetCurSel(0);//haibin.li 2011/11/30
	return;
}

//changed start by Haibin.Li 06.25
BOOL CMainTestPanel::OnInitDialog()
{
	CDialog::OnInitDialog();
	hDlgMutex = CreateMutex(NULL,NULL,_T("DlgMutex"));//haibin.li 09.09
	
	TPTimeOutFlag=0;
	m_brushdlg.CreateSolidBrush(RGB(245,245,245));
	m_brushstatic.CreateSolidBrush(RGB(240,240,240));    
	if(1==gStationInfo.MultiTaskSetting.Number)
	{
		BigResultFont.CreatePointFont(800-gStationInfo.MultiTaskSetting.Number*20, _T("Arial"));
		TestTimeFont.CreatePointFont(350, _T("Arial"));
	}
	else
	{
		BigResultFont.CreatePointFont(650-gStationInfo.MultiTaskSetting.Number*20, _T("Arial"));
		TestTimeFont.CreatePointFont(400, _T("Arial"));
	}
	ItemFont.CreatePointFont(500-gStationInfo.MultiTaskSetting.Number*20, _T("Arial"));
	SmallResultFont.CreatePointFont(150, _T("Arial"));
    ErrorCodeFont.CreatePointFont(230, _T("Arial"));
	TestStat.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_NOSCROLL,
		CRect(15,250,180, 285), this, 70005);
	TestStat.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 124-gStationInfo.MultiTaskSetting.Number*4);
	TestStat.InsertColumn(1, _T("Content"), LVCFMT_LEFT, 100);

	TestStat.InsertItem(PASSINDEX,_T("PASSED:"));
	TestStat.InsertItem(FAILINDEX,_T("FAILED:"));
	TestStat.InsertItem(TOTALINDEX,_T("TOTAL:"));
    TestStat.InsertItem(YEILDINDEX,_T("YIELD RATE:"));
    ShowTestResultStat(-1);//reset the number

	m_Redbh.CreateSolidBrush(RGB(255,0,0));
	m_Greenbh.CreateSolidBrush(RGB(0,255,0));
	m_Yellowbh.CreateSolidBrush(RGB(255,255,0));
	//m_Grancybh.CreateSolidBrush(RGB(82,102,251));
	m_Grancybh.CreateSolidBrush(RGB	(30,150,255));
	m_Bkbh.CreateSolidBrush(RGB(255,255,255));
	m_SmallResultbh.CreateSolidBrush(RGB(255,255,255));

	DetailLogDlg.SetAmParent(this);
	m_TestProgressBar.Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
		CRect(80,140,445, 160), this, 70006);

	m_TestProgressBar.SetPos(0);
	SendRTM("[START]",DetailLogDlg.TaskID);//haibin.li 2011/12/1
	
	StandbyUIType();
	

	SetBtnParameter(IDC_BTN_SFIS,1, _T("SFIS ON"));
	GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(false);
	CString StrTemp,ACKEvent;
	StrTemp.Format(_T("%d"),TestPort);
	ACKEvent.Format(_T("ACKEvent%d"),TestPort);
	//GetDlgItem(IDC_THREAD_ID)->SetWindowText(StrTemp);

	gACKEvent[TestPort] = CreateEvent( 
		NULL,         // no security attributes
		TRUE,         // manual-reset event
		FALSE,         // initial state is signaled
		ACKEvent); 
		//_T("ACKEvent")  // object name
		//);
    
	if(1==gStationInfo.MultiTaskSetting.Number)
	{			
		ButtonFont.CreatePointFont(120, _T("Arial"));
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		int x=cx-40;
		int y=cy-490;
		GetDlgItem(IDC_BIG_RESULT)->MoveWindow(5,5,x,y+263);
		GetDlgItem(IDC_STC_SMALL_RESULT)->MoveWindow(5,y+153,x,110);
		//GetDlgItem(IDC_STC_SMALL_RESULT)->MoveWindow(5,(y+220)/2+100,x,40);
		GetDlgItem(IDC_SN)->MoveWindow(10,y+153,x-20,100);
		m_TestProgressBar.MoveWindow(5,y+290,x-10,30);
		GetDlgItem(IDC_STC_CURRENT_ITEM)->MoveWindow(5,y+265,x-10,25);
		GetDlgItem(IDC_ERRORCODE)->MoveWindow(5,y+335,70,20);
		GetDlgItem(IDC_STC_ERROR_CODE)->MoveWindow(75,y+325,x-80,30);
		
		GetDlgItem(IDC_STC_TESTTIME)->MoveWindow(x-310,y+265,120,60);
		GetDlgItem(IDC_STC_TESTTIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_STC_TESTTIME)->SetFont(&TestTimeFont);

		//button and control-list
		TestStat.MoveWindow(x/2-100,cy/2-300,200,90);
		m_list.MoveWindow(x/2-100,cy/2-300,200,100);
		TestStat.SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_START_TEST)->MoveWindow(x-150,y+280,120,50);//*y/330
		GetDlgItem(IDC_BTN_SHOWLOG)->MoveWindow(x-150,y+324,120,35);
		GetDlgItem(IDC_BTN_SFIS)->MoveWindow(x-150,y+324,120,35);

		GetDlgItem(IDC_EQUIP_PICTURE)->MoveWindow(x-310,y+320,40,40);
		GetDlgItem(IDC_EQUIP_STATUS)->MoveWindow(100,y+295,300,30);
		GetDlgItem(IDC_STATIC_ES)->MoveWindow(40,y+270,400,60);
		
		GetDlgItem(IDC_BTN_START_TEST)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SHOWLOG)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SFIS)->SetFont(&ButtonFont);

		GetDlgItem(IDC_STC_CURRENT_ITEM)->SetFont(&SmallResultFont);
		GetDlgItem(IDC_STC_SMALL_RESULT)->SetFont(&SmallResultFont);
		GetDlgItem(IDC_STC_ERROR_CODE)->SetFont(&SmallResultFont);
	}	
	else if(2==gStationInfo.MultiTaskSetting.Number)
	{			GetDlgItem(IDC_BTN_SFIS)->ShowWindow(TRUE);
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		int number=gStationInfo.MultiTaskSetting.Number;
		CString strTemp;
		strTemp.Format(_T("%d"),DetailLogDlg.TaskID);
		GetDlgItem(IDC_ITEM)->ShowWindow(TRUE);
		GetDlgItem(IDC_ITEM)->SetWindowText(strTemp);
		GetDlgItem(IDC_ITEM)->SetFont(&ItemFont);
		GetDlgItem(IDC_ITEM)->MoveWindow(5,5,cx/2-20,60);

		GetDlgItem(IDC_BIG_RESULT)->MoveWindow(5,65,cx/2-20,350);
		GetDlgItem(IDC_STC_SMALL_RESULT)->MoveWindow(5,313,cx/2-20,100);
		//GetDlgItem(IDC_STC_TESTTIME)->MoveWindow(10,460,50,30);
		GetDlgItem(IDC_SN)->MoveWindow(10,313,cx/2-30,100);
		m_TestProgressBar.MoveWindow(10,420,cx/2-40,30);
		GetDlgItem(IDC_STC_CURRENT_ITEM)->MoveWindow(20,460,cx/2-40,50);
		GetDlgItem(IDC_ERRORCODE)->MoveWindow(20,575,70,20);
		GetDlgItem(IDC_STC_ERROR_CODE)->MoveWindow(90,565,cx/2-110,60);
		GetDlgItem(IDC_STC_TESTTIME)->MoveWindow(cx/4-75,510,140,55);

		////button and control-list
		ButtonFont.CreatePointFont(120, _T("Arial"));
		GetDlgItem(IDC_BTN_START_TEST)->MoveWindow(400-number*23,360,100-number*4,40-number*1);
		GetDlgItem(IDC_BTN_SHOWLOG)->MoveWindow(400-number*23,405,100-number*4,40-number*1);
		GetDlgItem(IDC_BTN_SFIS)->MoveWindow(cx/2-150,520,100-number*4,40-number*1);
		GetDlgItem(IDC_BTN_START_TEST)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SHOWLOG)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SFIS)->SetFont(&ButtonFont);

		m_list.MoveWindow(cx/4-100,90,200,100);
		TestStat.MoveWindow(10,375,280-number*2,100);	
		TestStat.SetFont(&ButtonFont);

		GetDlgItem(IDC_EQUIP_PICTURE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EQUIP_STATUS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ES)->ShowWindow(SW_HIDE);
	}
	else if(3==gStationInfo.MultiTaskSetting.Number)
	{
		int number=gStationInfo.MultiTaskSetting.Number;
		GetDlgItem(IDC_BIG_RESULT)->MoveWindow(5,5,400-number*25,180);
		GetDlgItem(IDC_STC_SMALL_RESULT)->MoveWindow(5,183,400-number*25,30);
		GetDlgItem(IDC_STC_TESTTIME)->MoveWindow(10,240,50,30);
		m_TestProgressBar.MoveWindow(70,240,320-number*25,30-number*1);
		GetDlgItem(IDC_STC_CURRENT_ITEM)->MoveWindow(20,280,350-number*25,20);
		GetDlgItem(IDC_STC_ERROR_CODE)->MoveWindow(20,305,350-number*25,20);

		////button and control-list
		ButtonFont.CreatePointFont(100, _T("Arial"));
		GetDlgItem(IDC_BTN_START_TEST)->MoveWindow(310-number*23,330,90-number*4,40-number*1);
		GetDlgItem(IDC_BTN_SHOWLOG)->MoveWindow(310-number*23,375,90-number*4,40-number*1);
		GetDlgItem(IDC_BTN_SFIS)->MoveWindow(310-number*23,420,90-number*4,40-number*1);
		GetDlgItem(IDC_BTN_START_TEST)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SHOWLOG)->SetFont(&ButtonFont);
		GetDlgItem(IDC_BTN_SFIS)->SetFont(&ButtonFont);
		
		TestStat.MoveWindow(10,340,200-number*2,100);
		ButtonFont.CreatePointFont(110, _T("Arial"));
		TestStat.SetFont(&ButtonFont);

		GetDlgItem(IDC_EQUIP_PICTURE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EQUIP_STATUS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ES)->ShowWindow(SW_HIDE);
	}	
	else
	{
		int number=gStationInfo.MultiTaskSetting.Number;
		if(4==number)
			number=2;
		if(9==number)
			number=6;			
		GetDlgItem(IDC_BIG_RESULT)->MoveWindow(5,5,420-number*25,82);
		GetDlgItem(IDC_STC_SMALL_RESULT)->MoveWindow(5,85,420-number*25,30);
		GetDlgItem(IDC_STC_TESTTIME)->MoveWindow(10,120,50,30);
		m_TestProgressBar.MoveWindow(70,130,340-number*25,30-number*1);
		GetDlgItem(IDC_STC_CURRENT_ITEM)->MoveWindow(20,160,400-number*25,30);
		GetDlgItem(IDC_STC_ERROR_CODE)->MoveWindow(20,190,400-number*25,30);

		////button and control-list
		GetDlgItem(IDC_BTN_START_TEST)->MoveWindow(320-number*23,230,80-number*4,30-number*1);
		GetDlgItem(IDC_BTN_SHOWLOG)->MoveWindow(320-number*23,260,80-number*4,30-number*1);
		GetDlgItem(IDC_BTN_SFIS)->MoveWindow(320-number*23,290,80-number*4,30-number*1);
		TestStat.MoveWindow(10,240,170-number*2,56);

		GetDlgItem(IDC_EQUIP_PICTURE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EQUIP_STATUS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ES)->ShowWindow(SW_HIDE);
	}
	
	TestStat.ShowWindow(FALSE);//haibin.li 2011/11/07
	userinfo.name=_T("");
	m_list.InsertColumn(0,_T("status"),LVCFMT_LEFT,120);
	m_list.InsertColumn(1,_T("number"),LVCFMT_LEFT,80);
	m_list.InsertItem(0,_T("Passed:"));
	m_list.SetItemText(0,1,_T("0"));
	m_list.InsertItem(1,_T("Failed:"));
	m_list.SetItemText(1,1,_T("0"));
	m_list.InsertItem(2,_T("Total:"));
	m_list.SetItemText(2,1,_T("0"));
	m_list.InsertItem(3,_T("Yield Rate:"));
	m_list.SetItemText(3,1,_T("0.0%"));
	m_list.InsertItem(4,_T("Utilization:"));
	m_list.SetItemText(4,1,_T("0.0%"));
	GetDlgItem(IDC_LIST1)->SetFont(&ButtonFont);
	SetTimer(RUN_TIMER,5000, (TIMERPROC)NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control	
}
//changed end by Haibin.Li 06.25

HBRUSH CMainTestPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_BIG_RESULT)
	{
		CString str;
		GetDlgItemText(IDC_BIG_RESULT,str);
		pDC->SelectObject(&BigResultFont);
		pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(0,0,0));
		if(!_tcscmp(_T("STANDBY"), str))
		{
			//pDC->SetTextColor(RGB(255,255,0));
			return m_Grancybh;
		}
		if(!_tcscmp(_T("TESTING"), str))
		{
			//pDC->SetTextColor(RGB(0,255,0));
			return m_Yellowbh;
		}
		if(!_tcscmp(_T("@E1#"), str)||!_tcscmp(_T("@E2#"), str)||!_tcscmp(_T("@E3#"), str)||!_tcscmp(_T("@E4#"), str)||!_tcscmp(_T("@E5#"), str)
			||!_tcscmp(_T("@E6#"), str)||!_tcscmp(_T("@E7#"), str)||!_tcscmp(_T("@E8#"), str)||!_tcscmp(_T("@E9#"), str)||!_tcscmp(_T("@EA#"), str))
		{
			return m_Redbh;
		}
		if(!_tcscmp(_T("PASSED"), str))
		{
			//PlaySound(MAKEINTRESOURCE(IDR_WAVE1),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT|SND_LOOP);
			//PlaySound(_T("water.wav"),NULL, SND_ASYNC);
			//pDC->SetTextColor(RGB(0,0,255));
			return m_Greenbh;
		}
		if(!_tcscmp(_T("FAILED"), str))
		{
			return m_Redbh;
		}
	}
	if (pWnd->GetDlgCtrlID() == IDC_STC_SMALL_RESULT)
	{
       	CString str;
		GetDlgItemText(IDC_BIG_RESULT,str);
		pDC->SelectObject(&SmallResultFont);
		pDC->SetBkMode(TRANSPARENT);		
		//if(wcsstr(_T("FAIL:"), str))
		//{
		//	pDC->SetTextColor(RGB(0,255,0));
		//}  
		//	
		//return m_SmallResultbh;
		 //pDC->SetTextColor(RGB(0,0,0));
		if(!_tcscmp(_T("STANDBY"), str))
		{
			return m_Grancybh;
		}
		else if(!_tcscmp(_T("TESTING"), str))
		{
			return m_Yellowbh;
		}		
		else if(!_tcscmp(_T("PASSED"), str))
		{
			return m_Greenbh;
		}
		else if(!_tcscmp(_T("FAILED"), str))
		{
			return m_Redbh;
		}
		else
		{
			return m_Redbh;
		}
	}
	if (pWnd->GetDlgCtrlID() == IDC_SN)
	{
		CString str;
		GetDlgItemText(IDC_BIG_RESULT,str);
		pDC->SelectObject(&SmallResultFont);
		pDC->SetBkMode(TRANSPARENT);		
		//if(wcsstr(_T("FAIL:"), str))
		//{
		//	pDC->SetTextColor(RGB(0,255,0));
		//}  
		//	
		//return m_SmallResultbh;
		//pDC->SetTextColor(RGB(0,0,0));
		if(!_tcscmp(_T("STANDBY"), str))
		{
			return m_Grancybh;
		}
		if(!_tcscmp(_T("TESTING"), str))
		{
			return m_Yellowbh;
		}		
		if(!_tcscmp(_T("PASSED"), str))
		{
			return m_Greenbh;
		}
		if(!_tcscmp(_T("FAILED"), str))
		{
			return m_Redbh;
		}
	}
	
    if(pWnd->GetDlgCtrlID() == IDC_STC_TESTTIME)
	{
		pDC->SelectObject(&TestTimeFont);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}
    if(pWnd->GetDlgCtrlID() == IDC_STC_ERROR_CODE)
	{
		pDC->SelectObject(&ErrorCodeFont);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}
	 if(pWnd->GetDlgCtrlID() == IDC_STC_CURRENT_ITEM)
	{
		pDC->SelectObject(&SmallResultFont);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	 if(pWnd->GetDlgCtrlID() == IDC_EQUIP_STATUS)
	 {
		 pDC->SelectObject(&ErrorCodeFont);
		 pDC->SetBkMode(TRANSPARENT);
	 }
	 if(pWnd->GetDlgCtrlID() == IDC_ITEM)
	 {
		 pDC->SetBkMode(TRANSPARENT);
	 }
	 if(pWnd->GetDlgCtrlID() == IDC_ERRORCODE)
	 {		 
		 pDC->SetTextColor(RGB(255,0,0));
		 pDC->SetBkMode(TRANSPARENT);
	 }
	
	// TODO:  Return a different brush if the default is not desired
	return m_brushstatic;
}

void CMainTestPanel::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CMainTestPanel::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
	//090616
	if(Hmutex)
	{
		CloseHandle(Hmutex);
	}
	if(hMydasMutex)
	{
		CloseHandle(hMydasMutex);
	}
	if (hExit)//haibin.li 2011/11/29
	{
		CloseHandle(hExit);
	}
}

void CMainTestPanel::PostExitMessage()
{	
	if (TaskID)
	{
		PostThreadMessage(TaskID, WM_EXIT, 0, 0);
	}
	return;
}

void CMainTestPanel::OnBnClickedBtnSfis()
{                    
	// TODO: Add your control notification handler code here
	CString str;

	GetDlgItemText(IDC_BTN_SFIS,str);
	if(!str.Compare(_T("SFIS OFF")))
	{
/*		//add start by LiYongjun 2011/03/21
		if(s_variable.VirtSmoFlag)
		{
			SFISInformation.SFICSwitchFlag=2;
		}
		else
		{
		//add end by LiYongjun 2011/03/21	*/
			SFISInformation.SFICSwitchFlag=0;
			SfisStatus=SFISInformation.SFICSwitchFlag;//haibin.li 2011/11/08
//		}
		GetDlgItem(IDC_BTN_SFIS)->SetWindowText(_T("SFIS ON"));
		GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(true);
	}
	else if(gVariable.SMOConnFlag)
	{
/*		//add start by LiYongjun 2011/03/21
		if(s_variable.VirtSmoFlag)
		{
			SFISInformation.SFICSwitchFlag=2;
		}
		else
		{
		//add end by LiYongjun 2011/03/21	*/
			SFISInformation.SFICSwitchFlag=1;
			SfisStatus=SFISInformation.SFICSwitchFlag;//haibin.li 2011/11/08
//		}
		GetDlgItem(IDC_BTN_SFIS)->SetWindowText(_T("SFIS OFF"));
		GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(false);
	}
	else
	{
	}	
}

void CMainTestPanel::SetSfisLink()////vincent 090716 it's a debug
{
	//gVariable.SMOConnFlag=1;
	SFISInformation.SFICSwitchFlag=1;
	SfisStatus=SFISInformation.SFICSwitchFlag;//haibin.li 2011/11/08
	GetDlgItem(IDC_BTN_SFIS)->SetWindowText(_T("SFIS OFF"));
	GetDlgItem(IDC_BTN_START_TEST)->EnableWindow(false);
}

void CMainTestPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TEST_TIMER==nIDEvent)
	{
		StationUti.TestTime++;//haibin.li 2011/12/1
		testtime[DetailLogDlg.TaskID-1]+=1;//haibin.li 2011/11/07
		m_time += 1;
		CString str = m_time.Format("%M:%S");
		TestingTime=str;
		SetDlgItemText(IDC_STC_TESTTIME,str);
	}
	if (STANDBY_TIMER==nIDEvent)
	{	
		if ((StandbyTime++)>StandbyTimeLimit)
		{
			StandbyUIType();
		}
	}
	//if (RESPONSE_TIMER==nIDEvent)//Maxwell 101101 sfis timer
	if (RESPONSE_TIMER+TestPort+10==nIDEvent)//Maxwell 101101 sfis timer
	{
		ResponseTime += 1;
	}
	if (TPTIMEOUT_TIMER==nIDEvent)
	{
		if ((TPTimeOutTime++)>gStationInfo.UiDisplay.TPtimeout)
		{
			TPTimeOutFlag=1;
			wcscpy_s(TestResult.ErrorCode,sizeof(TestResult.ErrorCode)/2,_T("SN00_00"));
			if (gStationInfo.PtsInfo.PtsSwitch)
			{
				WaitForSingleObject(hMydasMutex,INFINITE);
				SendResultToMYDAS();
				ReleaseMutex(hMydasMutex);
			}
			FailedUIType();
			MessageBox(_T("The TestPEROPerf TimeOut\nPlease reload the program!"),MB_OK);
			::PostMessage(this->m_hWnd,WM_QUIT,NULL,NULL);
		}
	}	
	if (RUN_TIMER==nIDEvent)
	{
		StationUti.TotalTime+=5;//haibin.li 2011/12/1
		StationUti.Uti=StationUti.TestTime*100.0/StationUti.TotalTime;
		CString str;		
		str.Format(_T("%.1f"),StationUti.Uti);
		str+=_T("%");
		m_list.SetItemText(4,1,str);//haibin.li 2011/11/12	
		//dlgrtm->m_list_result.SetItemText(4,1,str);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CMainTestPanel::ResetTestTime()
{
	m_time = 0;
	CString str = m_time.Format("%M:%S");
	SetDlgItemText(IDC_STC_TESTTIME,str);
}

void CMainTestPanel::ShowTestResultStat(int Result)
{
	//UUT_TEST_COUNTER TestResultCounter;//for test result stat.
	CString strTemp;
	if (1==Result)
	{
		TestResultCounter.PassCount++;
		//090612
		AmPostThreadMes(EQUIP_VIEW_TASK, WM_UPDATE_INFO_EQUIP, 0, TesterPort);
	}
	else if (0==Result)
	{
		TestResultCounter.FailCount++;
		//090612
		AmPostThreadMes(EQUIP_VIEW_TASK, WM_UPDATE_INFO_EQUIP, 0, TesterPort);
	}
	else if (-1==Result)
	{
		memset((void *)&TestResultCounter, 0, sizeof(TestResultCounter));
	}
	else if(2==Result)
	{
		if(TestResultCounter.FailCount)
		{
			TestResultCounter.FailCount--;
		}
		else
		{
		}		
	}
	else
	{

	}
	TestResultCounter.Total=TestResultCounter.PassCount + TestResultCounter.FailCount;
	if (TestResultCounter.Total)
	{
		TestResultCounter.YieldRate=((float)TestResultCounter.PassCount/TestResultCounter.Total)*100;
	}

	strTemp.Format(_T("%d"), TestResultCounter.PassCount);
	TestStat.SetItemText(PASSINDEX,1,strTemp);
	m_list.SetItemText(0,1,strTemp);//haibin.li 2011/11/12
	
	strTemp.Format(_T("%d"), TestResultCounter.FailCount);
	TestStat.SetItemText(FAILINDEX,1,strTemp);
	m_list.SetItemText(1,1,strTemp);//haibin.li 2011/11/12

	
	strTemp.Format(_T("%d"), TestResultCounter.Total);
	TestStat.SetItemText(TOTALINDEX,1,strTemp);
	m_list.SetItemText(2,1,strTemp);//haibin.li 2011/11/12


	strTemp.Format(_T("%.1f"), TestResultCounter.YieldRate);
	strTemp+=_T("%");
	TestStat.SetItemText(YEILDINDEX,1,strTemp);
	m_list.SetItemText(3,1,strTemp);//haibin.li 2011/11/12	


	if (0==Result||1==Result)//haibin.li 2011/12/5
	{
		dlgrtm->m_list_result.SetItemText(3,DetailLogDlg.TaskID,strTemp);//haibin.li 2011/12/5
		strTemp.Format(_T("%d"), TestResultCounter.PassCount);
		dlgrtm->m_list_result.SetItemText(0,DetailLogDlg.TaskID,strTemp);//haibin.li 2011/12/5
		strTemp.Format(_T("%d"), TestResultCounter.FailCount);
		dlgrtm->m_list_result.SetItemText(1,DetailLogDlg.TaskID,strTemp);//haibin.li 2011/12/5
		strTemp.Format(_T("%d"), TestResultCounter.Total);
		dlgrtm->m_list_result.SetItemText(2,DetailLogDlg.TaskID,strTemp);//haibin.li 2011/12/5		
	}
	
	return;
}

void CMainTestPanel::RepeatTest(void)
{
	//while(RepeatTime<gStationInfo.UiDisplay.RepeatTime)
	//{
	//	RepeatTime++;
	//	Sleep(2000);
	//	OnBnClickedBtnStartTest();
	//}
	//pTestPanel=this;//Maxwell 101015
	//pRepeatTestPanel[TestPort] = this;//Maxwell 101015
	hRepeat_Time =CreateThread( 
		NULL,                        // default security attributes 
		0,                           // use default stack size  
		Repeat_Time_Thread,                  // thread function 
		this,                // argument to thread function   //Maxwell 101015 from NULL to this
		0,                           // use default creation flags 
		&dwRepeat_TimeID/*(LPDWORD)this*/); 
}

//Maxwell 101015
DWORD WINAPI CMainTestPanel::Repeat_Time_Thread(LPVOID lpPara)
{
	CMainTestPanel *pRepeatTest = (CMainTestPanel*)(lpPara);
	//static int TestingPort = 1;
	if(pRepeatTest->RepeatTime<gStationInfo.UiDisplay.RepeatTime)
	{
		pRepeatTest->RepeatTime++;
		Sleep(1000);
		pRepeatTest->OnBnClickedBtnStartTest();
	}
	return 1;
}
//Maxwell 101015
//DWORD WINAPI Repeat_Time_Thread(LPVOID lpPara)
//{
//	
//	if(pTestPanel->RepeatTime<gStationInfo.UiDisplay.RepeatTime)
//	{
//		pTestPanel->RepeatTime++;
//		Sleep(2000);
//		pTestPanel->OnBnClickedBtnStartTest();
//	}
//	return 1;
//}
void CMainTestPanel::OnEditModify()
{
	// TODO: Add your command handler code here
	ShowTestResultStat(2);
}
BOOL CMainTestPanel::PreTranslateMessage(MSG* pMsg)   
{ 
	if(pMsg-> message==WM_LBUTTONDOWN) 
	{ 
		if(GetDlgItem(IDC_BIG_RESULT)-> m_hWnd==pMsg-> hwnd) 
		{
			CString str;
			GetDlgItemText(IDC_BTN_START_TEST,str);
			if(!_tcscmp(_T("TEST"), str)&&GetDlgItem(IDC_BTN_START_TEST)->IsWindowEnabled())
			{				
				OnBnClickedBtnStartTest();
			}
			pMsg-> hwnd=NULL;
		}	
		else
		{
			//TestStat.ShowWindow(TRUE);
			GetDlgItem(IDC_LIST1)->ShowWindow(TRUE);
		}		
	} 
	else if(pMsg-> message==WM_LBUTTONUP) 
	{ 
		//TestStat.ShowWindow(FALSE);
		GetDlgItem(IDC_LIST1)->ShowWindow(FALSE);
	} 
	
	return   CDialog::PreTranslateMessage(pMsg); 
}

void CMainTestPanel::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{//haibin.li 2011/11/11 for auto-draw list-control
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	NMLVCUSTOMDRAW* pNMCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	int nRow = (int)pNMCD->nmcd.dwItemSpec;	
	char szValue[20] = {0};
	if ( CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage )
	{
		*pResult |= CDRF_DODEFAULT;
		*pResult |= CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pNMCD->nmcd.dwDrawStage)
	{
		//pNMCD->clrTextBk = RGB(0xFF,0xFF,0xFF);
		*pResult= CDRF_NEWFONT; 
	    if(nRow==0)
		{
			pNMCD->clrText= RGB(0,255,0);
		}
		else if(nRow==1)
		{
			pNMCD->clrText= RGB(255,0x00,0x00);
		}		
		else if(nRow==3)
		{
			if(TestResultCounter.YieldRate<80.0)
			{
				pNMCD->clrText= RGB(255,0,0);
			}
			else
			{
				pNMCD->clrText= RGB(0,255,0);
			}
		}
		else if(nRow==4)
		{
			pNMCD->clrText= RGB(0,0,255);
		}	
		*pResult = CDRF_DODEFAULT;
	}
}
void CMainTestPanel::ShowSnMac(wchar_t widestr[500])
{
	CString strShow;
	CString strTemp;
	CString str;
	str=widestr;
	int i,j;
	i=str.Find(_T("SN["));
	if (i!=-1)
	{
		j=str.Find(']',i);
		strShow=_T("SN              :  ")+str.Mid(i+3,j-i-3)+_T("\n");
	}

	i=str.Find(_T("ETHERNETMAC["));
	if (i!=-1)
	{
		j=str.Find(']',i);
		strShow=strShow+_T("ETHMAC     :  ")+str.Mid(i+12,j-i-12)+_T("\n");
	}

	i=str.Find(_T("WIFIMAC0["));
	if (i!=-1)
	{
		j=str.Find(']',i);
		strShow=strShow+_T("WIFIMAC0  :  ")+str.Mid(i+9,j-i-9)+_T("\n");
	}

	i=str.Find(_T("WIFIMAC1["));
	if (i!=-1)
	{
		j=str.Find(']',i);
		strShow=strShow+_T("WIFIMAC1  :  ")+str.Mid(i+9,j-i-9);
	}
	SetDlgItemText(IDC_SN,strShow);
}