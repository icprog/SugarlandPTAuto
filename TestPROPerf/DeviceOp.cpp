#include "StdAfx.h"
#include "DeviceOp.h"


#define SOC_IO_PORT 10265
#define WM_DEV_SRQ WM_USER + 619
CDeviceOp::CDeviceOp(void)
{
	SRQModeFlag=0;
	SRQTaskFlag=1;
	hSendEvent=NULL;
	hRevEvent=NULL;
	hExitEvent=NULL;
	RevID=NULL;
	pSRQCallback=NULL;
}

CDeviceOp::~CDeviceOp(void)
{
	if (SRQModeFlag)
	{
		DevSRQEnd();
	}
}



int CDeviceOp::DevWrite(char *Name,char *pcmd,UINT Len)
{

	if (SRQModeFlag)
	{
		return 0;
	}


	memset((void *)&cmd,0,sizeof(cmd));
	memset((void *)&RevCmd,0,sizeof(RevCmd));

	if (Name)
	{
		strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),Name);
	}
	if (pcmd)
	{
		strcpy_s((char *)cmd.Data,sizeof(cmd.Data),pcmd);
	}
	cmd.len=(UINT)strlen(pcmd);
	cmd.W_R_flag=MA_WRITE;
	cmd.OpMode=0;//index query mode

	return (DevIO());
}

int CDeviceOp::DevRead(char *Name,char *pcmd,UINT Len)
{
	UINT ActRead=0;

	if (SRQModeFlag)
	{
		return 0;
	}


	memset((void *)&cmd,0,sizeof(cmd));
	memset((void *)&RevCmd,0,sizeof(RevCmd));

	if (Name)
	{
		strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),Name);
	}
	if (pcmd)
	{
		strcpy_s((char *)cmd.Data,sizeof(cmd.Data),pcmd);
	}
	cmd.len=(UINT)strlen(pcmd);
	cmd.W_R_flag=MA_READ;
	cmd.OpMode=0;//index query mode

	if (Len>=sizeof(RevCmd.Data))
	{
        ActRead=sizeof(RevCmd.Data);
	}
	else
	{
        ActRead=Len;
	}

	if (DevIO())
	{
        memcpy_s(pcmd,Len,RevCmd.Data,ActRead);  
	}
    return 1;

}


int CDeviceOp::DevIO()
{

	DWORD retLen;

	hSendEvent=WSACreateEvent();
	hRevEvent=WSACreateEvent();
	hExitEvent=WSACreateEvent();

	if((hSendEvent==WSA_INVALID_EVENT)||(hRevEvent==WSA_INVALID_EVENT)||(hExitEvent==WSA_INVALID_EVENT))
	{
		return 0;
	}
	
	if(!PeerCreateSocket(&s,PEER_STREAM))
	{
		WSACloseEvent(hSendEvent);
		WSACloseEvent(hRevEvent);
		WSACloseEvent(hExitEvent);
		return 0;
	}
	if(!PeerConnectSocket(s,"127.0.0.1",SOC_IO_PORT))
	{
		WSACloseEvent(hSendEvent);
		WSACloseEvent(hRevEvent);
		WSACloseEvent(hExitEvent);
		closesocket(s);
		return 0;
	}

	if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
	{
		WSACloseEvent(hSendEvent);
		WSACloseEvent(hRevEvent);
		WSACloseEvent(hExitEvent);
		closesocket(s);
		return 0;
	}

	if(!PeerRecvDataS(s,(char*)&RevCmd,sizeof(RevCmd),&retLen,hRevEvent,hExitEvent))
	{
		WSACloseEvent(hSendEvent);
		WSACloseEvent(hRevEvent);
		WSACloseEvent(hExitEvent);
		closesocket(s);
		return 0;
	}
	WSACloseEvent(hSendEvent);
	WSACloseEvent(hRevEvent);
	WSACloseEvent(hExitEvent);
	closesocket(s);
	return 1;
}
//-----------------------------------------------------------------------------------
//Sevice requiements query mode operation mode
/////////////////////////////////
//服务线程
/////////////////////////////////
UINT WINAPI SRQRev(PVOID pParam)
{

	DWORD retLen;
	CDeviceOp *pDOP=(CDeviceOp *)pParam;
	unsigned char *pBuf=NULL;
	UINT SizeBuf=0;
	DEV_OP_CMD RevCmd;
  

	while (pDOP->SRQTaskFlag)
	{
		if(!PeerRecvDataS(pDOP->s,(char*)&(RevCmd),sizeof(RevCmd),&retLen,pDOP->hRevEvent,pDOP->hExitEvent))
		{
			return 0;
		}
		if (pDOP->RevID)
		{
			SizeBuf=sizeof(RevCmd.Data)*sizeof(unsigned char);
			pBuf=(unsigned char *)malloc(SizeBuf);
			if (pBuf)
			{
				memcpy_s(pBuf,SizeBuf,&(RevCmd.Data),SizeBuf);
			}
			PostThreadMessage(pDOP->RevID,WM_DEV_SRQ,(UINT)pBuf,(UINT)SizeBuf);
		}
	}
	return 1;
}
int CDeviceOp::DevSRQOnRev(unsigned char *p,UINT Len)
{
	
	if (pSRQCallback)
	{
		(*pSRQCallback)(p,Len);
	}

	return 1;
}
UINT WINAPI SRQRevCallbackThread(LPVOID lpPara)
{

	MSG msg;
	unsigned char RevBuf[MA_DATA_LEN];
	UINT DataLen=0;
	CDeviceOp *pDOP=(CDeviceOp *)lpPara;

	while (1)
	{
		if (!WaitMessage()) 
		{
			return 1;
		}	
		memset(RevBuf,0,sizeof(RevBuf));
		while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
		{			
			if (WM_QUIT==msg.message)
			{
				goto Exit;
			}

			memcpy_s(RevBuf, sizeof(RevBuf),(unsigned char *)msg.wParam,msg.lParam);	
			pDOP->DevSRQOnRev(RevBuf,msg.lParam);		

			if ((unsigned char *)msg.wParam)
			{
				free((unsigned char *)msg.wParam);
			}
		}	  	
	}
Exit:
	return 1;
}



int CDeviceOp::DevSRQWrite(char *Name,char *pcmd,UINT Len)
{
	DWORD retLen;
	memset((void *)&cmd,0,sizeof(cmd));
	memset((void *)&RevCmd,0,sizeof(RevCmd));

	if (Name)
	{
		strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),Name);
	}
	if (pcmd)
	{
		strcpy_s((char *)cmd.Data,sizeof(cmd.Data),pcmd);
	}

    cmd.len=Len;
	cmd.OpMode=1;//index the operation mode is SRQ mode
	cmd.ExitFlag=1;

	if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
	{
		return 0;
	}

	return 1;
}


int CDeviceOp::DevSRQRegCallback(SRQCallback pSRQCallback)
{
	this->pSRQCallback=pSRQCallback;
	return 1;
}
int CDeviceOp::DevSRQSetOnRev(DWORD RevID)
{

	//this->RevID=RevID;
	return 1;

}
int CDeviceOp::DevSRQStart(char *pDevName)
{
	HANDLE hThread=NULL;
	unsigned int dwThreadID=NULL;

	HANDLE hccThread=NULL;
	unsigned int dwccThreadID=NULL;


	hSendEvent=WSACreateEvent();
	hRevEvent=WSACreateEvent();
	hExitEvent=WSACreateEvent();

	if((hRevEvent==WSA_INVALID_EVENT)||(hExitEvent==WSA_INVALID_EVENT))
	{
		return 0;
	}


	if(!PeerCreateSocket(&s,PEER_STREAM))
	{
		DevSRQEnd();
		return 0;
	}
	if(!PeerConnectSocket(s,"127.0.0.1",SOC_IO_PORT))
	{
		return 0;
	}

	DevSRQWrite(pDevName,"Connect requirement",strlen("Connect requirement"));	
	
	hThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		SRQRev,
		this,
		0,
		&dwThreadID 
		);

	if (hThread)
	{

	}
	else
	{
		return 0;
	}


	hccThread=(HANDLE)_beginthreadex( 
		NULL,
		0,
		SRQRevCallbackThread,
		this,
		0,
		&dwccThreadID 
		);

	if (hccThread)
	{
        RevID=dwccThreadID;
	}
	else
	{
		return 0;
	}
	Sleep(100);

	SRQModeFlag=1;
	return 1;
}
void CDeviceOp::DevSRQStop(char *Name)
{

	DWORD retLen;
	memset((void *)&cmd,0,sizeof(cmd));

	if (Name)
	{
		strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),Name);
	}

	cmd.OpMode=0;//index the operation mode is SRQ mode
	cmd.ExitFlag=0;
	if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
	{
		DevSRQEnd();
		return;
	}
	
	DevSRQEnd();
	return;
}
void CDeviceOp::DevSRQEnd(void)
{
	if (s)
	{
		closesocket(s);
		s=0;
	}
	PostThreadMessage(RevID,WM_QUIT,0,0);
	SRQTaskFlag=0;
	if (hSendEvent)
	{
		WSACloseEvent(hSendEvent);
		hSendEvent=NULL;
	}
	if (hRevEvent)
	{
		WSACloseEvent(hRevEvent);
		hRevEvent=NULL;
	}
	if (hExitEvent)
	{
		WSACloseEvent(hExitEvent);
		hExitEvent=NULL;
	}
	SRQModeFlag=0;
	return;
}
