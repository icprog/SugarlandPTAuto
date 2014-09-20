#include "StdAfx.h"
#include "TesterControl.h"
#include "ProcessControl.h"


CTesterControl::CTesterControl(void)
{



}

CTesterControl::~CTesterControl(void)
{


}

int CTesterControl::StartTeser(CString ModuleName, int TesterType=LOCAL_TESTER)
{

	if (LOCAL_TESTER==TesterType)
	{
         CProcessControl PCtrl;
		// PCtrl.KillTargetProcess(_T("TestPROPerf.exe"));

		 Sleep(50);
		 return (PCtrl.StartProcessInLocal(ModuleName));

	}
	else if (REMOTE_TESTER==TesterType)
	{




	}
	else
	{



		return 0;
	}


	return 1;
}

int CTesterControl::ForceTesterExit(CString ModuleName, int TesterType)
{

    CProcessControl PCtrl;
	return (PCtrl.KillTargetProcess(ModuleName));
;
}