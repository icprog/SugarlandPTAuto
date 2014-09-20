// PTSC.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PTSC.h"
#include "DeviceOp.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

int ConnectFlag=0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	int ty=0;
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ty=0;break;
	case DLL_THREAD_ATTACH:
		ty=0;break;
	case DLL_THREAD_DETACH:
		ty=0;break;
	case DLL_PROCESS_DETACH:
		ty=0;break;
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

int PTSC_Connect(void **pInstance,INIT_INFO *pInit)
{
	if (!pInit)
	{
		*pInstance=NULL;
		return 0;
	}

	CDeviceOp *pInstance_Dev = new CDeviceOp(pInit);
	(*pInstance) = pInstance_Dev;	

	if  (pInstance_Dev->DevSRQStart())
	{
		ConnectFlag=1;	
	}
	else
	{
		return 0;
	}

	return 1;
}

void PTSC_Disconnect(void *pInstance)
{
    CDeviceOp *pInstance_Dev = (CDeviceOp *)pInstance;
	pInstance_Dev->DevSRQStop();
	delete pInstance_Dev;	
	return;
}

int PTSC_GetLinkStatus(void *pInstance)
{
    CDeviceOp *pInstance_Dev = (CDeviceOp *)pInstance;
	return (pInstance_Dev->IsLinkToPts());
}

//
int PTSC_SetSendData(void *pInstance,char *p, unsigned int len,unsigned int Seq)
{
	CDeviceOp *pInstance_Dev = (CDeviceOp *)pInstance;
	return (pInstance_Dev->SetWriteData(p,len,Seq));
}
//
int PTSC_Send(void *pInstance)
{
	CDeviceOp *pInstance_Dev = (CDeviceOp *)pInstance;
	return (pInstance_Dev->DevSend());
}

int PTSC_RegOnRev(void *pInstance,SRQCallback pSRQCallback)
{
	CDeviceOp *pInstance_Dev = (CDeviceOp *)pInstance;
	return (pInstance_Dev->DevSRQRegCallback(pSRQCallback));
}