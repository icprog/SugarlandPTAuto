// AmSfisDlg.cpp : implementation file
//
#include "stdafx.h"
#include "AmSfisDlg.h"
#include "AmbitProT.h"
#include <sys/timeb.h>//Talen 2011/10/24

#include "SFISDefine.h"
#include "Config.h"
#include "SfisDLL.h"
#include "AmParser.h"
#include "SfisLogicControl.h"
#include "AmThreadAdmin.h"
#include "AmLogRecord.h"
#include "VirtualSmo.h"
#include "MainTestPanel.h"
#include "DialogUser.h"

#pragma comment(lib,"SfisDLL.lib")
HANDLE TESTROBOT=NULL;   //sfis.dll init
HHOOK g_hKeyboard=NULL;  //hook
HANDLE hSendSmoMutex;
HANDLE hSmoLogMutex;//Talen 2011/10/10
HANDLE hROBOTSfisEvent=NULL; //Maxwell 010203
extern HANDLE hDlgMutex;//09.09

CAmSfisDlg *pSfisDlgDlg;
CSfisLogicControl SfisLogicControl;
extern CVirtualSmo *pVirtualSmo;
extern SFIS_VARIABLE s_variable;              //Store global information for all test
extern list <SFIS_TRANSMIT_ITEM> ReadList;    //for the recv data struct
extern list <SFIS_TRANSMIT_ITEM> SendList;    //for the send data struct
//come from the 

int _stdcall SFISCallback(int flag,char *p,unsigned int Len)
{
	Sleep(10);//add by Talen 2011/10/10
	char com_rev_buf[2048]="";
	char strTemp[2048]="";
	char pBuf[2048]="SMO->UI:";
	strcpy_s(strTemp,sizeof(strTemp),p);
	Len=(int)strlen(strTemp);
	strcpy_s(com_rev_buf,sizeof(com_rev_buf),strTemp); 
	if('\n'==com_rev_buf[0]) 
	{
		memset(com_rev_buf,0,sizeof(com_rev_buf));
	}
	if(flag==2)
	{
		if(Len>2&&strstr(com_rev_buf,"\r"))
		{
			strcat_s(pBuf,sizeof(pBuf),com_rev_buf); 
			pSfisDlgDlg->OnShowRecv(pBuf,2);
			memset(com_rev_buf,0,sizeof(com_rev_buf)); 
		}
	}
	if(flag==1)
	{
		if(Len>2&&strstr(com_rev_buf,"\r"))
		{
			if(strstr(com_rev_buf,"ERRO"))
			{
				strncpy_s(strstr(com_rev_buf,"\r"),sizeof(strstr(com_rev_buf,"\r")),"\0",1);
				strcat_s(pBuf,sizeof(pBuf),com_rev_buf); 
				pSfisDlgDlg->OnShowRecv(pBuf,1);
				if(pSfisDlgDlg->m_connFlag)
				{
					pSfisDlgDlg->OnShowRecv("SFIS ON",2);
				}
				pSfisDlgDlg->m_connFlag=0;
				gVariable.SMOConnFlag=1;
				SfisLogicControl.SendUIData(_T("SMOCONNFLAG=1;"),SFIS_LINK_STATUS);
				((CStatic*)pSfisDlgDlg->GetDlgItem(IDC_LINK_PICTURE))->SetIcon(AfxGetApp()->LoadIcon(IDI_PASS));
				pSfisDlgDlg->OnSFISAction(com_rev_buf,2);
			}
			if(strstr(com_rev_buf,"PASS"))
			{
				strncpy_s(strstr(com_rev_buf,"\r"),sizeof(strstr(com_rev_buf,"\r")),"\0",1);
				strcat_s(pBuf,sizeof(pBuf),com_rev_buf); 				
				pSfisDlgDlg->OnShowRecv(pBuf,1);
				pSfisDlgDlg->OnSFISAction(com_rev_buf,1);
				//Maxwell 010203
				SetEvent(hROBOTSfisEvent);
				//Maxwell 010203
			}
			memset(com_rev_buf,0,sizeof(com_rev_buf)); 
		}
	}
	return 0;
}

LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam) /////CURRENT THREAD HOOK
{

	static char CStrSend[512]="";
	CString theStr;
	static int i=0;
	
	if(VK_ADD==wParam)
	{
		return CallNextHookEx(g_hKeyboard,code,wParam,lParam);
	}

	if((VK_RETURN==wParam)&&(1==(lParam>>30&1)))
	{
		char buf[50]="";
		char strTemp[256]="UI->SMO:";
		//char strTemp[256]="";
		if(i>3)
		{
			strcat_s(strTemp,sizeof(strTemp),CStrSend);
			SfisLogicControl.StartTestTask(CStrSend);
			WaitForSingleObject(hSendSmoMutex,INFINITE);//0622
			//haibin.li 2012/2/9
			//SfisSend(TESTROBOT,CStrSend,buf,(int)strlen(CStrSend)+1,2);//send data to sfis 
			CString strTemp1=(CString)CStrSend;
			CString strTemp2=(CString)(s_variable.BarCode);
			strTemp1.MakeUpper();
			if ((strTemp1.Find((strTemp2+_T("1")))!=-1)||(strTemp1.Find((strTemp2+_T("2")))!=-1)||(strTemp1.Find((strTemp2+_T("3")))!=-1)||(strTemp1.Find((strTemp2+_T("4")))!=-1))
			{

			}
			else
			{
				SfisSend(TESTROBOT,CStrSend,buf,(int)strlen(CStrSend)+1,2);
				pSfisDlgDlg->OnShowRecv(strTemp,2);
			}			
			//haibin.li 2012/2/9
			Sleep(5);
			ReleaseMutex(hSendSmoMutex);
		}
		memset(CStrSend,'\0',sizeof(CStrSend));
		i=0;
		//haibin.li 2012/4/26 for virtual_smo dialog auto hide
		CString strText1,strText2,strText3,strText4;;
		if (pVirtualSmo->vSmoShowFlag)
		{
			pVirtualSmo->ShowWindow(SW_SHOW);
			pVirtualSmo->GetDlgItemText(IDC_VIR_SMO_ITEM1,strText1);
			pVirtualSmo->GetDlgItemText(IDC_VIR_SMO_ITEM2,strText2);
			pVirtualSmo->GetDlgItemText(IDC_VIR_SMO_ITEM3,strText3);
			pVirtualSmo->GetDlgItemText(IDC_VIR_SMO_ITEM4,strText4);

			if (!strText1.Compare(_T("")))
			{
				pVirtualSmo->GetDlgItem(IDC_VIR_SMO_ITEM1)->SetFocus();
			}
			else if (!strText2.Compare(_T("")))
			{
				pVirtualSmo->GetDlgItem(IDC_VIR_SMO_ITEM2)->SetFocus();
			}
			else if (!strText3.Compare(_T("")))
			{
				pVirtualSmo->GetDlgItem(IDC_VIR_SMO_ITEM3)->SetFocus();
			}
			else if (!strText4.Compare(_T("")))
			{
				pVirtualSmo->GetDlgItem(IDC_VIR_SMO_ITEM4)->SetFocus();
			}
			else
			{
				pVirtualSmo->GetDlgItem(IDC_VIR_SEND)->SetFocus();
			}
		}
		//haibin.li 2012/4/26 for virtual_smo dialog auto hide
		return 1;
	}
	else if(((48<=wParam&&wParam<=122)||(189==wParam)||(VK_SPACE==wParam)||(219==wParam)||(221==wParam))&&(1==(lParam>>30&1))) //add for APS
	{
		BYTE ks[256];
		GetKeyboardState(ks);
		WORD w;
		UINT scan;
		scan=0;
		ToAscii((UINT)wParam,scan,ks,&w,0);

		CStrSend[i++]=char(w);
		if(i>25)
		{
			memset(CStrSend,'\0',sizeof(CStrSend));
			i=0;
		}
		return 1;
	}
	else if((VK_BACK==wParam)&&(1==(lParam>>30&1)))
	{
		i--;
		if(i<0)
		{
			i=0;
			return CallNextHookEx(g_hKeyboard,code,wParam,lParam);
		}
		else
		{
			CStrSend[i]='\0';
			return 1;
		}
	}
	else if(27==wParam)
	{
		return 1;
	}
	else
	{
		return CallNextHookEx(g_hKeyboard,code,wParam,lParam);
	}
}

// CAmSfisDlg dialog

IMPLEMENT_DYNAMIC(CAmSfisDlg, CDialog)

CAmSfisDlg::CAmSfisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmSfisDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pSfisDlgDlg=this;
	m_connFlag=1;
	s_variable.sfisSwitchFlag=0;
	showRecvBuf=_T("");
	wmemset(SFISInformation,0,sizeof(SFISInformation)/2);
}

CAmSfisDlg::~CAmSfisDlg()
{
	if(hSendSmoMutex)
	{
		CloseHandle(hSendSmoMutex);
	}
	if(TESTROBOT)
	{
		SfisClose(TESTROBOT);
	}
	//add start by Talen 2011/10/10
	if(hSmoLogMutex)
	{
		CloseHandle(hSmoLogMutex);
	}
	//add end by Talen 2011/10/10
}

void CAmSfisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAmSfisDlg, CDialog)
	ON_BN_CLICKED(IDC_VIRT_OPEN, &CAmSfisDlg::OnBnClickedVirtOpen)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAmSfisDlg message handlers
//need do the SFIS message routing 
/*


                                    |----------------------|  <----------> TEST TASK 0
                                    |                      |  <----------> TEST TASK 1
    SFIS(Software DCT) <----------> |    SFIS DATA route   |  <----------> TEST TASK 2
           |                        |    (logic control)   |  <----------> TEST TASK 3
           |                        |                      |  <----------> TEST TASK 4
           |                        |----------------------|  <----------> ..........
           |                                    |
		   |									|
		SMO(Server)								|
												|
												|
                                    |----------------------|
                                    |                      |
                                    | SFIS DATA selector   |
                                    |    (interface)       |  
                                    |                      |
                                    |----------------------|
			                           |              |
									   |              |
									   |(Software)    |(Hardware)
									   |              |                   
									   |              |                 
									   |              |           
	                               -----------    ------------                         
		                           |  |  |  |     |  |  |  |  |                        
			                       1   2  3  4    1  2  3  4  5            
			                   
			                                                                         */
BOOL CAmSfisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	char name[20];
	GetLocalHostName((char*)&name);
	wchar_t tname[20]; 
	size_t cLen;
	mbstowcs_s(&cLen,tname,sizeof(tname)/2,name,sizeof(name));
	wmemset(s_variable.PCName,0,sizeof(s_variable.PCName)/2);
	wcscpy_s(s_variable.PCName,sizeof(s_variable.PCName)/2,tname);
	if(!AmbitLoadConfig())
	{
		return 0;
	}
	hSendSmoMutex = CreateMutex(NULL,NULL,_T("SendSmoMutex"));
	hSmoLogMutex = CreateMutex(NULL,NULL,_T("SmoLogMutex"));
	// Init sfis.dll
	if( SfisOpen(&TESTROBOT,SFISCallback)) 
	{   
		MessageBox(_T("Open server failed!\n"));
		return 0;
	}
	// Init the hook
	g_hKeyboard=SetWindowsHookEx(WH_KEYBOARD,KeyboardProc,NULL,GetCurrentThreadId());
	// open a thread which recv the data from the UI
	SfisLogicControl.StartSFISTask();
	//LINK STATUS
	ConnThread();
	((CStatic*)GetDlgItem(IDC_LINK_PICTURE))->SetIcon(AfxGetApp()->LoadIcon(IDI_FAIL));
	
	if(!s_variable.VirtSmoFlag)
	{
		GetDlgItem(IDC_VIRT_OPEN)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_VIRT_OPEN)->EnableWindow(true);
	}

	staFont.CreatePointFont(150, _T("Arial"));
	GetDlgItem(IDC_DCT)->MoveWindow(50,50,800,250);
	GetDlgItem(IDC_SFIS_SHOW)->MoveWindow(50,350,800,250);
	GetDlgItem(IDC_VIRT_OPEN)->MoveWindow(700,310,100,30);
	GetDlgItem(IDC_STATIC_SFIS)->MoveWindow(50,320,200,30);
	GetDlgItem(IDC_DCT)->SetFont(&staFont);
	GetDlgItem(IDC_SFIS_SHOW)->SetFont(&staFont);
	GetDlgItem(IDC_STATIC_SFIS)->SetFont(&staFont);
	GetDlgItem(IDC_STATIC_INDI)->SetFont(&staFont);
	return TRUE;  // return TRUE unless you set the focus to a control
}

int CAmSfisDlg::OnSFISAction(char* pRevStr,int flag)
{
	if(!pRevStr)
	{
		return 0;
	}
	char pTemp[2048]="";
	strcpy_s(pTemp,sizeof(pTemp),pRevStr);
	s_variable.sfisResult=FindResult(pTemp);
	size_t cLen;
	TCHAR TTempChar[2048]=_T("");
	mbstowcs_s(&cLen,TTempChar,sizeof(TTempChar)/2,pTemp,sizeof(pTemp));

	if(1==flag)//pass
	{
		if(wcsstr(TTempChar,s_variable.PN))
		{
			if(wcslen(TTempChar)!=s_variable.READ_LENGTH_VALID+4)
			{
				return 0;
			}
			list<SFIS_TRANSMIT_ITEM>::iterator Cy;
			for (Cy=ReadList.begin(); Cy!=ReadList.end();Cy++)
			{
				if((*Cy).readFlag)
				{
					TCHAR temp[128]=_T("");
					TCHAR *pToke=NULL;
					wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
					wcsncpy_s(temp,sizeof(temp)/2,TTempChar+(*Cy).readStartNum,(*Cy).readLength);
					pToke=wcsstr(temp,_T(" "));
					if(pToke)
					{
						temp[pToke-temp]=L'\0';
					}
					wcscpy_s((*Cy).Val,sizeof((*Cy).Val)/2,temp);
				}
			}
			OnSendMsgToUI(SFIS_START);
			return 1;
		}
		else if(wcsstr(TTempChar,s_variable.PCName))
		{
			size_t aa=wcslen(TTempChar);
			if(((int)wcslen(TTempChar)>s_variable.SEND_LENGTH_VALID+8)||((int)wcslen(TTempChar)<s_variable.SEND_LENGTH_VALID))
			{
				return 0;
			}
			else
			{
			}
			list<SFIS_TRANSMIT_ITEM>::iterator Cx;
			for (Cx=SendList.begin(); Cx!=SendList.end();Cx++)
			{
				if((*Cx).sendFlag)
				{
					TCHAR temp[128]=_T("");
					TCHAR *pToke=NULL;
					wmemset((*Cx).Val,0,sizeof((*Cx).Val)/2);
					wcsncpy_s(temp,sizeof(temp)/2,TTempChar+(*Cx).sendStartNum,(*Cx).sendLength);
					pToke=wcsstr(temp,_T(" "));
					if(pToke)
					{
						temp[pToke-temp]=L'\0';
					}
					wcscpy_s((*Cx).Val,sizeof((*Cx).Val)/2,temp);
				}
			}
			OnSendMsgToUI(SFIS_RESULT);
			return 1;
		}
		else
		{
			AfxMessageBox(_T("Please check PN name or PC name"));
		}
	}
	if(2==flag)//erro
	{
		if(wcsstr(TTempChar,s_variable.PCName))
		{
			if(((int)wcslen(TTempChar)>s_variable.SEND_LENGTH_VALID+8)||((int)wcslen(TTempChar)<s_variable.SEND_LENGTH_VALID))
			{
				return 0;
			}
			list<SFIS_TRANSMIT_ITEM>::iterator Cx;
			for (Cx=SendList.begin(); Cx!=SendList.end();Cx++)
			{
				if((*Cx).sendFlag)
				{
					TCHAR temp[128]=_T("");
					TCHAR *pToke=NULL;
					wmemset((*Cx).Val,0,sizeof((*Cx).Val)/2);
					wcsncpy_s(temp,sizeof(temp)/2,TTempChar+(*Cx).sendStartNum,(*Cx).sendLength);
					pToke=wcsstr(temp,_T(" "));
					if(pToke)
					{
						temp[pToke-temp]=L'\0';
					}
					wcscpy_s((*Cx).Val,sizeof((*Cx).Val)/2,temp);
				}
			}
			OnSendMsgToUI(SFIS_RESULT);
			//AfxMessageBox(_T("SN00_00 DCT Response NG..."));
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

///the function is not used
int CAmSfisDlg::OnPN(void)
{
	list<SFIS_TRANSMIT_ITEM>::iterator Cy;
	for (Cy=ReadList.begin(); Cy!=ReadList.end();Cy++)
	{
		if(!wcscmp((*Cy).Item,_T("PNNAME")))
		{
			if(wcsncmp((*Cy).Val,s_variable.PN,(int)wcslen(s_variable.PN)))
			{
				(*Cy).Val[0]='\0';
				AfxMessageBox(_T("PN in Tab file is different from PN in SFIS,please Check...\n請檢查機種名與SN是否一致"));
				return -1;
			}
			break;
		}
	}
	return 1;
}

int CAmSfisDlg::FindResult(char* strResult)
{
	int len=(int)strlen(strResult);
	if(12>=len||len>s_variable.SEND_LENGTH_VALID+8)
	{
		return 1;
	}	
	if(strstr(strResult,"PASS"))
	{
		return 2;
	}
	if(strstr(strResult,"ERRO"))
	{
		return 0;
	}
	return 1;
}

void CAmSfisDlg::OnSfisToPef(void)
{
	list<SFIS_TRANSMIT_ITEM>::iterator Cy;
	for (Cy=SendList.begin(); Cy!=SendList.end();Cy++)
	{
		if(!wcscmp((*Cy).Item,_T("PCNAME")))
		{
			wcscpy_s((*Cy).Val,sizeof((*Cy).Val)/2,s_variable.PCName);
			break;
		}
	}
	OnSendMsgToUI(1);
}

void CAmSfisDlg::OnSendMsgToUI(int flag)
{
	list<SFIS_TRANSMIT_ITEM>::iterator Cy;
	CString CStrTemp;
	if(SFIS_START==flag)
	{
		CStrTemp=_T("SFISSTART=");
		for (Cy=ReadList.begin(); Cy!=ReadList.end();Cy++)
		{
			CStrTemp+=(*Cy).Item;
			CStrTemp+=_T("[");
			CStrTemp+=(*Cy).Val;
			CStrTemp+=_T("]");
		}
		CStrTemp+=_T(";");
		TCHAR TTempStr[1024]= _T("");
		wcscpy_s(TTempStr,sizeof(TTempStr)/2,CStrTemp);

		if(0==s_variable.ModeOptionFlag)
		{
			SfisLogicControl.SendUIData(TTempStr,SFIS_START);
		}
		else
		{
			char pRecordData[128]="SCANNER=1;";
			size_t SizeLen=strlen(pRecordData);
			AmPostThreadMes(MAIN_CONTROL_TASK, WM_UPDATE_INFO_SCAN, (unsigned int)pRecordData, (unsigned int)SizeLen);
			//gVariable.UutScannerFlag=1;
			wcscpy_s(pSfisDlgDlg->SFISInformation,sizeof(pSfisDlgDlg->SFISInformation)/2,TTempStr);
		}
	}
	if(SFIS_RESULT==flag)
	{
		CStrTemp=_T("SFISRESULT=");
		for (Cy=SendList.begin(); Cy!=SendList.end();Cy++)
		{
			if(!wcscmp((*Cy).Item,_T("ERRORCODE"))&&(!wcscmp((*Cy).Val,_T("PASS"))))
			{
				wmemset((*Cy).Val,L'\0',sizeof((*Cy).Val)/2);
				continue; //based on the format ERRORCODE[] or no the ERRORCODE
			}
			if(!wcscmp((*Cy).Item,_T("ERRORCODE"))&&(!wcscmp((*Cy).Val,_T("ERRO"))))
			{
				wmemset((*Cy).Val,L'\0',sizeof((*Cy).Val)/2);
				AfxMessageBox(_T("SN00_00 SMO Response NG..."));
				return;
			}
			CStrTemp+=(*Cy).Item;
			CStrTemp+=_T("[");
			CStrTemp+=(*Cy).Val;
			CStrTemp+=_T("]");
		}
		CStrTemp+=_T(";");
		TCHAR TTempStr[1024]= _T("");
		wcscpy_s(TTempStr,sizeof(TTempStr)/2,CStrTemp);
		SfisLogicControl.SendUIData(TTempStr,SFIS_RESULT);
	}
}

void CAmSfisDlg::OnSend(wchar_t *RevBuf,int DataLen,int flag)
{
	int OTAflag=0;
	wchar_t strSendTemp[2048]=_T("");
	wchar_t strTemp[128]=_T("");
	list<SFIS_TRANSMIT_ITEM>::iterator Cy;
	CAmParser parse;
	parse.SetStartStopTag(_T("["),_T("]"));
	wchar_t TTempStr[2048];
	wcscpy_s(TTempStr,sizeof(TTempStr)/2,RevBuf);
	if(wcsstr(TTempStr,_T("PASS")))
	{
		for (Cy=SendList.begin(); Cy!=SendList.end();Cy++)
		{
			if(!wcscmp((*Cy).Item,_T("PCNAME")))
			{
				wmemset(strTemp,0,sizeof(strTemp)/2);
				WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,s_variable.PCName,_T("-"));
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
				continue;
			}
			wmemset(strTemp,0,sizeof(strTemp)/2);
			wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
			parse.ParserGetPara(TTempStr, (*Cy).Item,(*Cy).Val, sizeof((*Cy).Val)/2);
			if(*((*Cy).Val))
			{
				WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,(*Cy).Val,_T(" "));
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
				wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
			}
			//else
			//{
			//	WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,(*Cy).Val,_T("F"));
			//	wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
			//}
		}
	}
	else if(wcsstr(TTempStr,_T("FAIL")))
	{
		for (Cy=SendList.begin(); Cy!=SendList.end();Cy++)
		{
			if(!wcscmp((*Cy).Item,_T("PCNAME")))
			{
				wmemset(strTemp,0,sizeof(strTemp)/2);
				WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,s_variable.PCName,_T("-"));
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
				continue;
			}
			wmemset(strTemp,0,sizeof(strTemp)/2);
			wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
			parse.ParserGetPara(TTempStr, (*Cy).Item,(*Cy).Val, sizeof((*Cy).Val)/2);
			//add start by Talen 2011/08/08
			if(!wcscmp((*Cy).Item,_T("ERRORCODE")))
			{
				if(!wcsncmp((*Cy).Val,_T("NFTO"),4))
				{
					OTAflag=1;
				}
				else
				{
				}
			}
			//add end by Talen 2011/08/08
			if(!*((*Cy).Val))
			{
				WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,(*Cy).Val,_T("F"));
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
			}
			else
			{
				WAdjustLength((*Cy).sendLength,strTemp,sizeof(strTemp)/2,(*Cy).Val,_T(" "));
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,strTemp);
				wmemset((*Cy).Val,0,sizeof((*Cy).Val)/2);
			}
		}
		if(!gStationInfo.Server1Client.Enable)// Justin for X18 Robot SFIS different behave 2011/01/24
		{
			WaitForSingleObject(hDlgMutex,INFINITE);
			//add start by Talen 2011/08/12
			if(1==OTAflag)
			{
				if(IDOK==MessageBox(_T("To OTA！請移至OTA站"),  _T("Notice"),   MB_OK))
				{
				}
				//wcscat_s(strSendTemp,sizeof(strSendTemp)/2,_T("END"));//vincent090716 for DVT
			}
			//add end by Talen 2011/08/12  
			else if( MessageBox(_T("Re-Test?是否重測?"),  _T("ASk"),   MB_OK|MB_OKCANCEL)==IDOK)
			{
			}
			else
			{
				wcscat_s(strSendTemp,sizeof(strSendTemp)/2,_T("END"));//vincent090716 for DVT
			}
			
			ReleaseMutex(hDlgMutex);
		}
	}
	char buf[100];
	size_t i;
	char SendBuf[2048]="";
	wcstombs_s(&i,SendBuf,sizeof(SendBuf),strSendTemp,wcslen(strSendTemp));

//Modify 20101101 for Visual SFIS not send data to SMO
	WaitForSingleObject(hSendSmoMutex,INFINITE);//0622

	char ShowBuf[2048]="UI->SMO:";
	strcat_s(ShowBuf,sizeof(ShowBuf),SendBuf);
	OnShowRecv(ShowBuf,1);

	SfisSend(TESTROBOT,SendBuf,buf,sizeof(SendBuf),1);//flag--->1
	Sleep(1);
	
	ReleaseMutex(hSendSmoMutex);
	if(s_variable.VirtSmoFlag)
	{
		strcat_s(SendBuf,sizeof(SendBuf),"PASS");
		pSfisDlgDlg->OnSFISAction(SendBuf,1);
	}
}

void CAmSfisDlg::WAdjustLength(int nLength, wchar_t* pDesStr,int DesStrLength, wchar_t* pRevStr,wchar_t* fillstr)
{
	int len;
	len=(int)wcslen(pRevStr);
	if(len>nLength)
	{
		for(int i=nLength;i<len;i++)
		{
			pRevStr[i]=0;
		}
		wcscpy_s(pDesStr,DesStrLength,pRevStr);
		return;
	}
	wcscpy_s(pDesStr,DesStrLength,pRevStr);
	for(int i=0; i<nLength-len; i++)
	{
		wcscat_s(pDesStr,DesStrLength,fillstr);
	}
}

//////////////RECV THE DATA FROM THE UI FOR SEND TO SFIS
void CAmSfisDlg::ConnThread(void)
{
	HANDLE hConnThread;
	DWORD dwConnThreadID;
	hConnThread =CreateThread( 
		NULL,                        // default security attributes 
		0,                           // use default stack size  
		ConnThreadProcess,             // thread function 
		NULL,                        // argument to thread function 
		0,                           // use default creation flags 
		&dwConnThreadID);  
	if (hConnThread)
	{
		CloseHandle(hConnThread);
	}
}

DWORD WINAPI CAmSfisDlg::ConnThreadProcess(LPVOID lpPara)
{
	while (pSfisDlgDlg->m_connFlag)
	{		
		char buf[50]="";
		char strTemp[256]="hello";
		SfisSend(TESTROBOT,strTemp,buf,(int)strlen(strTemp)+1,1);
		Sleep(500);
	}
	return 1;
}

// show the send and recv data which get from the smo
void CAmSfisDlg::OnShowRecv(char *pBuf,int flag)
{
	CAmLogRecord LogRecord;
	char LogBuf[2048]="";
	/////////////////////////////////////////////////////////////show the time
	CTime t=CTime::GetCurrentTime();
	CString strTime = t.Format( "%H:%M:%S:" );
	CString mSecond;
	struct _timeb timebuff;
	_ftime(&timebuff);
	mSecond.Format(_T("%.3d "),timebuff.millitm);
	strTime+=mSecond;
	wchar_t wchTempBuf[256]=_T("");
	char TempBuf[256]="";
	size_t Length;
	wcscpy_s(wchTempBuf,sizeof(wchTempBuf)/2,strTime);
	wcstombs_s(&Length,TempBuf,sizeof(TempBuf),wchTempBuf,sizeof(wchTempBuf)/2);
	strcpy_s(LogBuf,sizeof(LogBuf),TempBuf);

	////////////////////////////////////////////////////////////

	char flagTemp[100]="";
	sprintf_s(flagTemp,sizeof(flagTemp),"flag=%d;",flag);
	strcat_s(LogBuf,sizeof(LogBuf),flagTemp);
	strcat_s(LogBuf,sizeof(LogBuf),pBuf);
	
	WaitForSingleObject(hSmoLogMutex,INFINITE);//add by talen 2011/10/10
	LogRecord.amprintf("%s",LogBuf);
	ReleaseMutex(hSmoLogMutex);//add by talen 2011/10/10
	
	if(flag==1)
	{
		//CString str;
		//MyMultiByteToWideChar(pBuf,str);
		size_t cLen,ulSize;
		char TempBuf[512]="";
		strcpy_s(TempBuf,sizeof(TempBuf),pBuf);
		ulSize=strlen(TempBuf);
		wchar_t wcBuf[512]=_T("");
		mbstowcs_s(&cLen, wcBuf, ulSize+1, TempBuf, ulSize);

		showRecvBuf+="\r\n";
		//showRecvBuf+=str;
		showRecvBuf+=wcBuf;
		GetDlgItem(IDC_SFIS_SHOW)->SetWindowText(showRecvBuf);
		if(showRecvBuf.GetLength()>10000)
		{
			showRecvBuf=_T("");
		}
	}
	if(flag==2)
	{
		CString str;		
		MyMultiByteToWideChar(pBuf,str);
		///////////////////////////////////////
		wchar_t wcBuf[1024]=_T("");		
		wcscpy_s(wcBuf,sizeof(wcBuf)/2,str);
		SfisLogicControl.SendUIData(wcBuf,4);
		///////////////////////////////////////
		showDctBuf+=str;
		showDctBuf+="\r\n";
		GetDlgItem(IDC_DCT)->SetWindowText(showDctBuf);
		int  i=((CEdit *)GetDlgItem(IDC_DCT))->GetLineCount();   
		((CEdit *)GetDlgItem(IDC_DCT))->LineScroll(i,0);
		if(showDctBuf.GetLength()>5000)
		{
			showDctBuf=_T("");
		}
	}	
}
int CAmSfisDlg::ParseUIData(char *pBuf, DWORD ulSize)
{
	char TempBuf[2000]="";
	char KeyBoardBuf[200]="";
	wchar_t wcBuf[2000];
	size_t cLen;
	CAmParser Parser;
	if (!pBuf)
	{
		return 0;
	}
	strcpy_s(TempBuf,sizeof(TempBuf),pBuf);
	mbstowcs_s(&cLen, wcBuf, ulSize+1, TempBuf, ulSize);
	OnSend(wcBuf,sizeof(wcBuf)/2,1);
	return 1;
}

void CAmSfisDlg::OnBnClickedVirtOpen()
{
	// TODO: Add your control notification handler code here
    CDialogUser dlguser;
	if (!userinfo.name.Compare(_T("")))
	{
		if (dlguser.DoModal()==IDCANCEL)
		{
			return;
		}
	}
	pSfisDlgDlg->m_connFlag=0;
	gVariable.SMOConnFlag=1;
	SfisLogicControl.SendUIData(_T("SMOCONNFLAG=1;"),SFIS_LINK_STATUS);
	SfisLogicControl.SendUIData(_T(" "),4);
	pVirtualSmo->vSmoShowFlag=1;//haibin.li 2012/4/26
	AmPostThreadMes(MAIN_VIEW_TASK, WM_VIRTUAL_SMO, 0, 0);
}

void CAmSfisDlg::OnClose()
{	
	CDialog::OnClose();
}

int CAmSfisDlg::OnSendDCT(char *pBuf)
{
	char CStrSendDCT[256]="";
	char buf[50]="";	
	strcpy_s(CStrSendDCT,sizeof(CStrSendDCT),pBuf);
	WaitForSingleObject(hSendSmoMutex,INFINITE);//0622
	SfisSend(TESTROBOT,CStrSendDCT,buf,(int)strlen(CStrSendDCT)+1,2);
	Sleep(5);
	ReleaseMutex(hSendSmoMutex);
	return 1;
}