// AmbitProTView.cpp : implementation of the CAmbitProTView class
//

#include "stdafx.h"
#include "AmbitProT.h"

#include "AmbitProTDoc.h"
#include "AmbitProTView.h"

#include "MainTestPanel.h"
#include "TesterDlg.h"
#include "AmSfisDlg.h"
#include "AmMydasDlg.h"
#include "AmThreadAdmin.h"
#include "ProcessControl.h"
#include "VirtualSmo.h"
#include "MainTestPanel.h"
#include "com_class.h"

#include "DialogMain.h"//haibin.li 2011/11/07
#include "DialogConf.h"//haibin.li 2011/11/07
#include "DetailLogDlg.h"//haibin.li 2011/11/07
#include "DialogRtm.h"//haibin.li 2011/11/11

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MAXMUM_DLG_NUMBER 100
CMainTestPanel *gpMainTestPanel[MAXMUM_DLG_NUMBER];//for main test panel dialog global define
CTesterDlg *pTesterDlg;//for system configuration and information display, global define
CAmMydasDlg *pAmMydasDlg;//for MYDAS system
CAmSfisDlg *pAmSfisDlg;// for sfis system
CVirtualSmo *pCVirtualSmoDlg;// for virtual smo system
CDialogMain *pDlgMain;//haibin.li 2011/11/07
CDialogConf *pDlgConf;//haibin.li 2011/11/07
CDetailLogDlg *pDlgDetail;//haibin.li 2011/11/07
CDialogRtm *pDlgRtm;//haibin.li 2011/11/11
extern CListCtrl *pLog;//haibin.li 2011/11/07
extern CListCtrl *pTestItemSummary;//haibin.li 2011/11/07
extern CEditShow *pLogDisplay;//haibin.li 2011/11/07
extern CListCtrl *pLogTwo;//haibin.li 2011/11/14
extern CListCtrl *pTestItemSummaryTwo;//haibin.li 2011/11/08
extern CEditShow *pLogDisplayTwo;//haibin.li 2011/11/07

extern CMainTestPanel *pTestPanel;
//Global windows define
extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store
UINT WINAPI MainViewTask(LPVOID lpPara);


// CAmbitProTView

IMPLEMENT_DYNCREATE(CAmbitProTView, CListView)

BEGIN_MESSAGE_MAP(CAmbitProTView, CListView)
	ON_WM_STYLECHANGED()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_COUNT, &CAmbitProTView::OnViewCount)
END_MESSAGE_MAP()

// CAmbitProTView construction/destruction

CAmbitProTView::CAmbitProTView()
{
	// TODO: add construction code here

}

CAmbitProTView::~CAmbitProTView()
{
	if (pDlgMain)
	{
		delete pDlgMain;
	}
}

BOOL CAmbitProTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

void CAmbitProTView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();    

	CString str;
	
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
	
	if (0<gStationInfo.MultiTaskSetting.Number&&gStationInfo.MultiTaskSetting.Number<=50)
	{
		AmCreateMTestDlg(gStationInfo.MultiTaskSetting.Number);//input tester number, maximum number support 100
	}
	else
	{   str.Format(_T("Can not support %d test number, RANGE(1-50)"), gStationInfo.MultiTaskSetting.Number);
		MessageBox(str,_T("MULTI TASK ERROR 02"));
	}

	AmCreateTesterDlg();
	AmCreateSfisDlg();
    AmCreateMydasDlg();
	AmCreateVirtualSmoDlg();
	AmCreateMainDlg();//haibin.li 2011/11/07
	AmCreateConfDlg();//haibin.li 2011/11/07
	AmCreateDetailDlg();//haibin.li 2011/11/07
	AmCreateRtmDlg();//haibin.li 2011/11/11
}


// CAmbitProTView diagnostics

#ifdef _DEBUG
void CAmbitProTView::AssertValid() const
{
	CListView::AssertValid();
}

void CAmbitProTView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAmbitProTDoc* CAmbitProTView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAmbitProTDoc)));
	return (CAmbitProTDoc*)m_pDocument;
}
#endif //_DEBUG


// CAmbitProTView message handlers
void CAmbitProTView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window	
	CListView::OnStyleChanged(nStyleType,lpStyleStruct);	
}

int CAmbitProTView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Main test dialog create
	CProcessControl ProcessControl;
	while(ProcessControl.KillTargetProcess(_T("TestPROPerf.exe"))) 
	{
		Sleep(20);//Maxwell 101011
	}
	MainViewTaskStart();
	
	return 0;
}

void CAmbitProTView::OnDestroy()
{
	CListView::OnDestroy();

	AmExitAllTask();//Exit all control task

	//wait closing.
	Sleep(1500);//Wait all task exit

    //haibin.li 2011/11/07
	if (pLog)
	{
		delete pLog;
	}
	if (pTestItemSummary)
	{
		delete pTestItemSummary;
	}
    if (pLogDisplay)
    {
		delete pLogDisplay;
    }
	if (pLogTwo)
	{
		delete pLogTwo;
	}
	if (pTestItemSummaryTwo)
	{
		delete pTestItemSummaryTwo;
	}
	if (pLogDisplayTwo)
	{
		delete pLogDisplayTwo;
	}
	KillTimer(ICON_TIMER);//haibin.li 2011/11/09
	AmDestroyMTestDlg();
	AmDestroyTesterDlg();
	AmDestroyMydasDlg();
	AmDestroySfisDlg();
	AmDestroyVirtualSmoDlg();
	AmDestroyMainDlg();//haibin.li 2011/11/07
	AmDestroyConfDlg();//haibin.li 2011/11/07
	AmDestroyDetailDlg();//haibin.li 2011/11/07
	AmDestroyRtmDlg();//haibin.li 2011/11/11
}

int CalculateRow(int wa[3],int row,int m)
{
	int r;
	wa[0]=row;
	r=m%wa[0];
	if(!r)
	{
		wa[1]=m/wa[0];
		wa[2]=0;
	}
	else if(!(r=(m+1)%wa[0]))
	{
		wa[1]=(m+1)/wa[0];
		wa[2]=1;
	}
	else if(!(r=(m+2)%wa[0]))
	{
		wa[1]=(m+2)/wa[0];
		wa[2]=2;
	}
	else if(!(r=(m+3)%wa[0]))
	{
		wa[1]=(m+3)/wa[0];
		wa[2]=3;
	}
		else if(!(r=(m+4)%wa[0]))
	{
		wa[1]=(m+4)/wa[0];
		wa[2]=4;
	}
	else if(!(r=(m+5)%wa[0]))
	{
		wa[1]=(m+5)/wa[0];
		wa[2]=5;
	}
	else if(!(r=(m+6)%wa[0]))
	{
		wa[1]=(m+6)/wa[0];
		wa[2]=6;
	}
		else if(!(r=(m+7)%wa[0]))
	{
		wa[1]=(m+7)/wa[0];
		wa[2]=7;
	}
	else if(!(r=(m+8)%wa[0]))
	{
		wa[1]=(m+8)/wa[0];
		wa[2]=8;
	}
	else if(!(r=(m+9)%wa[0]))
	{
		wa[1]=(m+9)/wa[0];
		wa[2]=9;
	}
	return 1;
}

int CalculateLineAndRow(int *px,int *py,int m)//px是列數，py是行數
{
	int i;
	int wa[3];
	double temp;
	if(m<1||m>100)
	{
		//printf("the number is illegal,please input the number:1~100");
		return 0;
	}
	temp=floor(sqrt(m*1.0));
	if(temp*temp<m)
		temp=temp+1;
	for(i=1;i<=m;i++)
	{
		if(fabs(i-temp)<0.000001)
		{
			CalculateRow(wa,i,m);
			break;
		}
	}
	*px=wa[0];
	*py=wa[1];
	return 1;
}

int CAmbitProTView::AmCreateTesterDlg()
{
	pTesterDlg = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pTesterDlg = (CTesterDlg *)new CTesterDlg;
	if (pTesterDlg)
	{
		pTesterDlg->Create(IDD_TESTER_DLG, this);
		pTesterDlg->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		pTesterDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}

	return 1;
}

int CAmbitProTView::AmDestroyTesterDlg()
{
	if (pTesterDlg)
	{
		pTesterDlg->DestroyWindow();
		delete pTesterDlg;
		pTesterDlg=NULL;
	}

	return 1;
}

int CAmbitProTView::AmCreateMydasDlg()
{
	pAmMydasDlg = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pAmMydasDlg = (CAmMydasDlg *)new CAmMydasDlg;
	if (pAmMydasDlg)
	{
		pAmMydasDlg->Create(IDD_MYDAS_DLG, this);
		pAmMydasDlg->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}

	return 1;
}

int CAmbitProTView::AmDestroyMydasDlg()
{
	if (pAmMydasDlg)
	{
		pAmMydasDlg->DestroyWindow();
		delete pAmMydasDlg;
		pAmMydasDlg=NULL;
	}

	return 1;
}

int CAmbitProTView::AmCreateSfisDlg()
{
	pAmSfisDlg = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pAmSfisDlg = (CAmSfisDlg *)new CAmSfisDlg;
	if (pAmSfisDlg)
	{
		pAmSfisDlg->Create(IDD_SFIS_DLG, this);
		pAmSfisDlg->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}

	return 1;
}

int CAmbitProTView::AmDestroySfisDlg()
{
	if (pAmSfisDlg)
	{
		pAmSfisDlg->DestroyWindow();
		delete pAmSfisDlg;
		pAmSfisDlg=NULL;
	}

	return 1;
}

int CAmbitProTView::AmCreateVirtualSmoDlg()
{
	pCVirtualSmoDlg = NULL;
	CRect ViewRect(0,0,400,250);	
	GetClientRect(ViewRect);
	pCVirtualSmoDlg = (CVirtualSmo *)new CVirtualSmo;
	if (pCVirtualSmoDlg)
	{
		pCVirtualSmoDlg->Create(IDD_VIR_SMO, this);
		//pCVirtualSmoDlg->SetWindowPos(NULL, ViewRect.top+50, ViewRect.left+100, ViewRect.top+400, ViewRect.left+250, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		pCVirtualSmoDlg->CenterWindow();
		pCVirtualSmoDlg->ActivateTopParent();
		pCVirtualSmoDlg->EnsureTopLevelParent();
		pCVirtualSmoDlg->SetFocus();
		pCVirtualSmoDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}

	return 1;
}

int CAmbitProTView::AmDestroyVirtualSmoDlg()
{
	if (pCVirtualSmoDlg)
	{
		pCVirtualSmoDlg->DestroyWindow();
		delete pCVirtualSmoDlg;
		pCVirtualSmoDlg=NULL;
	}

	return 1;
}

int CAmbitProTView::AmCreateMTestDlg(UINT Number)
{
	UINT DlgCount=0;
	CMainTestPanel* pTemp=NULL;

	memset(gpMainTestPanel, 0, sizeof(gpMainTestPanel));	
		
	//Make dlg layout
	CRect ViewRect;	
	GetClientRect(ViewRect);

	LONG HSize=0;
	LONG VSize=0;

	int xWindows=0;
	int yWindows=0;

	if (!CalculateLineAndRow(&xWindows,&yWindows,Number))
	{
		return 0;
	}//xWindows is column,yWindows is row,Number is task number.	

	//add start by Haibin.Li for three task
	if(Number==3)
	{
		xWindows=3;
		yWindows=1;
	}
	//add end by Haibin.Li

	HSize = ViewRect.right/xWindows;//
	VSize = ViewRect.bottom/yWindows;//

	int HCounter=0;
	int VCounter=0;

	for (VCounter=0;VCounter<yWindows;VCounter++)
	{
		for (HCounter=0;HCounter<xWindows;HCounter++)
		{
			pTemp=NULL;
			pTemp = (CMainTestPanel* )new CMainTestPanel;

			Sleep(50);

			if (pTemp)
			{
				gpMainTestPanel[DlgCount]=pTemp;
				gpMainTestPanel[DlgCount]->Create(IDD_MAIN_TEST_PANEL, this);

				gpMainTestPanel[DlgCount]->SetWindowPos(NULL,HSize*HCounter+20,VSize*VCounter,HSize-1,VSize-1,SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
				gpMainTestPanel[DlgCount]->ShowWindow(SW_SHOW);
				
				if ((DlgCount++)>=Number-1)
				{
					goto Eixt;//if create complete, exit create task
				}
			}
		}
	}

Eixt:

	return 0;
}
//haibin.li 2011/11/07
int CAmbitProTView::AmCreateMainDlg()
{
	pDlgMain = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pDlgMain = (CDialogMain *)new CDialogMain;
	if (pDlgMain)
	{
		pDlgMain->Create(IDD_DIALOG_MAIN, this);
		pDlgMain->SetWindowPos(NULL, ViewRect.top, ViewRect.left, ViewRect.left+20, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
		pDlgMain->ShowWindow(SW_SHOW);
	}
	else
	{
		return 0;
	}

	return 1;
}

int CAmbitProTView::AmDestroyMainDlg()
{
	if (pDlgMain)
	{
		pDlgMain->DestroyWindow();
		delete pDlgMain;
		pDlgMain=NULL;
	}
	return 1;
}
int CAmbitProTView::AmCreateConfDlg()
{
	pDlgConf = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pDlgConf = (CDialogConf *)new CDialogConf;
	if (pDlgConf)
	{
		pDlgConf->Create(IDD_DIALOG_CONF, this);
		pDlgConf->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);
		pDlgConf->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}
	return 1;
}

int CAmbitProTView::AmDestroyConfDlg()
{
	if (pDlgConf)
	{
		pDlgConf->DestroyWindow();
		delete pDlgConf;
		pDlgConf=NULL;
	}
	return 1;
}
int CAmbitProTView::AmCreateDetailDlg()
{
	pDlgDetail = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pDlgDetail = (CDetailLogDlg *)new CDetailLogDlg;
	if (pDlgDetail)
	{
		pDlgDetail->Create(IDD_DETAIL_LOG_DLG, this);
		pDlgDetail->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);
		pDlgDetail->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}
	return 1;
}

int CAmbitProTView::AmDestroyDetailDlg()
{
	if (pDlgDetail)
	{
		pDlgDetail->DestroyWindow();
		delete pDlgDetail;
		pDlgDetail=NULL;
	}
	return 1;
}
int CAmbitProTView::AmCreateRtmDlg()
{
	pDlgRtm = NULL;
	CRect ViewRect(0,0,500,700);	
	GetClientRect(ViewRect);
	pDlgRtm = (CDialogRtm *)new CDialogRtm;
	if (pDlgRtm)
	{
		pDlgRtm->Create(IDD_DIALOG_RTM, this);
		pDlgRtm->SetWindowPos(NULL, ViewRect.top+20, ViewRect.left, ViewRect.right, ViewRect.bottom, SWP_NOZORDER|SWP_NOACTIVATE);
		pDlgRtm->ShowWindow(SW_HIDE);
	}
	else
	{
		return 0;
	}
	return 1;
}

int CAmbitProTView::AmDestroyRtmDlg()
{
	if (pDlgRtm)
	{
		pDlgRtm->DestroyWindow();
		delete pDlgRtm;
		pDlgRtm=NULL;
	}
	return 1;
}
void CAmbitProTView::ShowConfDlg()
{
	SetMTestDlgShow(SW_HIDE);
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}
	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}
	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}
	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}
	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}
	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_SHOW);
	}
	return;
}
void CAmbitProTView::ShowDetailDlg()
{
	SetMTestDlgShow(SW_HIDE);
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}
	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}
	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}
	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}
	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}
	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_SHOW);
	}
	return;
}

//haibin.li 2011/11/07
void CAmbitProTView::SetMTestDlgShow(int Bev)
{

	UINT DlgCount=0;
	for (DlgCount=0; DlgCount<MAXMUM_DLG_NUMBER; DlgCount++)
	{
		if (gpMainTestPanel[DlgCount])
		{
			gpMainTestPanel[DlgCount]->ShowWindow(Bev);
		}
	}
	return;
}

void CAmbitProTView::ShowTesterDlg()
{
	SetMTestDlgShow(SW_HIDE);
	
	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}

	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}

	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}

	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}

	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}

	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_SHOW);
	}

	return;
}
void CAmbitProTView::ShowMTestDlg()
{	
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}

	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}

	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}

	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}

	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}

	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}

	SetMTestDlgShow(SW_SHOW);

	return;
}

void CAmbitProTView::ShowSfisDlg()
{
	SetMTestDlgShow(SW_HIDE);
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}

	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}
	
	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}

	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}

	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}

	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_SHOW);
	}

	return;
}
void CAmbitProTView::ShowMydasDlg()
{
	SetMTestDlgShow(SW_HIDE);
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}

	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}

	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}
	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}

	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_HIDE);
	}

	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_SHOW);
	}

	return;
}

void CAmbitProTView::ShowRtmDlg()
{
	SetMTestDlgShow(SW_HIDE);
	if (pTesterDlg)
	{
		pTesterDlg->ShowWindow(SW_HIDE);
	}

	if (pAmMydasDlg)
	{
		pAmMydasDlg->ShowWindow(SW_HIDE);
	}

	if (pAmSfisDlg)
	{
		pAmSfisDlg->ShowWindow(SW_HIDE);
	}

	if (pDlgConf)//haibin.li 2011/11/07
	{
		pDlgConf->ShowWindow(SW_HIDE);
	}
	if (pDlgDetail)//haibin.li 2011/11/07
	{
		pDlgDetail->ShowWindow(SW_HIDE);
	}

	if (pDlgRtm)//haibin.li 2011/11/11
	{
		pDlgRtm->ShowWindow(SW_SHOW);
	}

	return;
}

void CAmbitProTView::AmDestroyMTestDlg(void)
{
	UINT DlgCount=0;
	for (DlgCount=0; DlgCount<MAXMUM_DLG_NUMBER; DlgCount++)
	{
		if (gpMainTestPanel[DlgCount])
		{
			//gpMainTestPanel[DlgCount]->PostExitMessage();
			//Sleep(1000);//Delay 1 second wait for thread work finish.
			gpMainTestPanel[DlgCount]->DestroyWindow();
			delete gpMainTestPanel[DlgCount];
		}
	}

	return;
}

BOOL CAmbitProTView::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CListView::DestroyWindow();
}

void CAmbitProTView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	
	/*//Make dlg layout
	CRect ViewRect, DlgRect;
	UINT DlgCount=0;
	GetClientRect(ViewRect);
	//this->getwin

	LONG HSize=0;
	LONG VSize=0;

	//LONG TempHSize=0;
	//LONG TempVSize=0;

	int xWindows=0;
	int yWindows=0;

	if (!CalculateLineAndRow(&xWindows,&yWindows,10))
	{
		//return 0;
	}//px是列數，py是行數
	
	//TempHSize = (ViewRect.right -ViewRect.left);
	//TempVSize = (ViewRect.bottom-ViewRect.top);

	//HSize = (830)/xWindows;//
	//VSize = (685)/yWindows;//
	HSize = ViewRect.right/xWindows;//
	VSize = ViewRect.bottom/yWindows;//
	int HCounter=0;
	int VCounter=0;

	for (HCounter=0;HCounter<xWindows;HCounter++)
	{
		for (VCounter=0;VCounter<yWindows;VCounter++)
		{

			//gpMainTestPanel[DlgCount]->MoveWindow(HSize*HCounter,VSize*VCounter,HSize-1,VSize-1);

				//gpMainTestPanel[DlgCount]->SetWindowPos(NULL,HSize*HCounter,VSize*VCounter,HSize-1,VSize-1,SWP_NOZORDER|SWP_NOACTIVATE);// set dialog size
				//gpMainTestPanel[DlgCount]->ShowWindow(SW_SHOW);
			//DlgCount++;
		}
	}*/
}
int CAmbitProTView::MainViewTaskStart(void)
{

	AM_TASK_INFO TaskTemp;

	TaskTemp.hTaskHandle=(HANDLE)_beginthreadex( 
		NULL,
		0,
		MainViewTask,
		this,
		0,
		&TaskTemp.uiTaskID 
		);

	if (TaskTemp.hTaskHandle)
	{
		TaskTemp.TaskID=MAIN_VIEW_TASK;
        gUITaskInfoList.push_back(TaskTemp);//Insert the task information to management list
	}
	else
	{
		return 0;
	}	
	
	return 1;
}


/////////////////////////////////////////////////////////////////////////
//For total management all UI information and control logic
//It distribute UI control message for all sub dialog.
UINT WINAPI MainViewTask(LPVOID lpPara)
{

	CAmbitProTView *pMView=(CAmbitProTView *)lpPara;
	MSG msg;
	int i=0;


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
			case WM_SWTO_TESTER_DLG:
				pMView->ShowTesterDlg();
				break;
			case WM_SWTO_MAIN_TEST_PANEL:
				pMView->ShowMTestDlg();
				break;
			case WM_SWTO_SFIS_DLG:
				pMView->ShowSfisDlg();
				break;
			case WM_SWTO_MYDAS_DLG:
				pMView->ShowMydasDlg();
				break;
			case WM_VIRTUAL_SMO:
				pCVirtualSmoDlg->ShowWindow(SW_SHOW);
				break;
			case WM_EXIT:
				goto ExitLabel;
				break;
			case WM_SWTO_CONF_DLG://haibin.li 2011/11/07
				pMView->ShowConfDlg();
				break;
			case WM_SWTO_DETAIL_DLG://haibin.li 2011/11/07
				pMView->ShowDetailDlg();
				break;
			case WM_SWTO_RTM_DLG://haibin.li 2011/11/11
				pMView->ShowRtmDlg();
				break;
			default:
				;
			}
		}
	}
ExitLabel:

	return 0;
}
void CAmbitProTView::OnViewCount()
{
	// TODO: Add your command handler code here
	pTestPanel->ShowTestResultStat(2);
}