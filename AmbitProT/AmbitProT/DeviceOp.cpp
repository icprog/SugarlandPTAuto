#include "StdAfx.h"
#include "DeviceOp.h"

#define SOC_IO_PORT 11565
#define WM_DEV_SRQ WM_USER + 619
CDeviceOp::CDeviceOp(INIT_INFO *pInit)
{
	SRQModeFlag=0;
	SRQTaskFlag=1;
	hSendEvent=NULL;
	hRevEvent=NULL;
	hExitEvent=NULL;
	RevID=NULL;
	pSRQCallback=NULL;	
	WSADATA	wsd;
	WSAStartup(0x202,&wsd);
	memcpy_s((void *)&DevInfo,sizeof(INIT_INFO), (void*)pInit, sizeof(INIT_INFO));
	memset((void *)&SendData,0,sizeof(SendData));
}

CDeviceOp::~CDeviceOp(void)
{
	if (SRQModeFlag)
	{
		DevSRQEnd();
	}
	WSACleanup();
}

UINT WINAPI SRQRev(PVOID pParam)
{
	DWORD retLen;
	CDeviceOp *pDOP=(CDeviceOp *)pParam;
	unsigned char *pBuf=NULL;
	UINT SizeBuf=0;
	DEV_OP_CMD RevCmd;  

	while (pDOP->SRQTaskFlag)
	{
		memset((void *)&RevCmd,0,sizeof(RevCmd));
		if(!PeerRecvDataS(pDOP->s,(char*)&(RevCmd),sizeof(RevCmd),&retLen,pDOP->hRevEvent,pDOP->hExitEvent))
		{
			return 0;
		}

		if ((LINK_RESPONSE+1)==RevCmd.W_R_flag)
		{
			SetEvent(pDOP->IsLinkResponseEvent);
			continue;
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
int CDeviceOp::DevSRQOnRev(char *p,UINT Len)
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
			pDOP->DevSRQOnRev((char *)RevBuf,msg.lParam);		

			if ((unsigned char *)msg.wParam)
			{
				free((unsigned char *)msg.wParam);
			}
		}	  	
	}
Exit:
	return 1;
}

int CDeviceOp::DevSend(void)
{
	int Index=0;
	unsigned int TTSize=0;
	unsigned int ActCpy=0;
	unsigned int CpyOffset=0;
	char *pSendBuf=0;

	for (Index=0;Index<MAX_SEND;Index++)
	{
		TTSize+=SendData[Index].Len;
	}

	if (!TTSize)
	{
		for (Index=0;Index<MAX_SEND;Index++)
		{
			if (SendData[Index].pSendData)
			{
				free(SendData[Index].pSendData);
				SendData[Index].pSendData=0;
				SendData[Index].Len=0;
			}
		}
		return 0;
	}

	pSendBuf=(char *)malloc(sizeof(char)*TTSize);
	if (pSendBuf)
	{
		for (Index=0;Index<MAX_SEND;Index++)
		{
			if (SendData[Index].pSendData)
			{
				memcpy_s(pSendBuf+CpyOffset,(TTSize-CpyOffset),SendData[Index].pSendData,SendData[Index].Len);
				CpyOffset+=SendData[Index].Len;
				free(SendData[Index].pSendData);
				SendData[Index].pSendData=0;
				SendData[Index].Len=0;
			}
		}
	}
	else
	{
		memset((void *)&SendData,0,sizeof(SendData));
		return 0;
	}

	if (!DevSRQWrite(pSendBuf,TTSize))
	{
		if (pSendBuf)
		{
			free(pSendBuf);
		}
		memset((void *)&SendData,0,sizeof(SendData));
		return 0;
	}

	if (pSendBuf)
	{
		free(pSendBuf);
	}
	memset((void *)&SendData,0,sizeof(SendData));
	return 1;
}

int CDeviceOp::DevSRQWrite(char *pcmd,unsigned int Len)
{
	DWORD retLen=0;
	UINT SendPkt=0;
	UINT LastLeft=0;
	UINT SeqPkt=0;
	UINT ActSend=0;

    SendPkt=Len/MA_DATA_LEN;
	LastLeft=Len%MA_DATA_LEN;

	memset((void *)&cmd,0,sizeof(cmd));
	memset((void *)&RevCmd,0,sizeof(RevCmd));

	strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),DevInfo.ComName);
	strcpy_s(cmd.DevVersion,sizeof(cmd.DevVersion),DevInfo.DaigVersion);
	strcpy_s(cmd.Product,sizeof(cmd.Product),DevInfo.Product);
	strcpy_s(cmd.TestSta,sizeof(cmd.TestSta),DevInfo.TestStaName);
	strcpy_s(cmd.PN,sizeof(cmd.PN),DevInfo.ProductPN);
	strcpy_s(cmd.TitleVersion,sizeof(cmd.TitleVersion),DevInfo.TitleVersion);//Maxwell set title version

	cmd.TotalLen=Len;
    //cmd.len=Len;
	cmd.OpMode=1;//index the operation mode is SRQ mode
	cmd.ExitFlag=1;

	if (SendPkt<1)
	{
		if (LastLeft)
		{
			if (pcmd)
			{
				memcpy_s((char *)cmd.Data,sizeof(cmd.Data),pcmd,LastLeft);
				cmd.PackSeq=SeqPkt;
				cmd.len=LastLeft;
			}
			if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
			{
				return 0;
			}
			ActSend=retLen;
		}
	}
	else
	{
		for (SeqPkt=0;SeqPkt<SendPkt;SeqPkt++)
		{
			if (pcmd)
			{
				memcpy_s((char *)cmd.Data,sizeof(cmd.Data),(pcmd+(SeqPkt*MA_DATA_LEN)),MA_DATA_LEN);
				cmd.PackSeq=SeqPkt;
				cmd.len=MA_DATA_LEN;
			}
			if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
			{
				return 0;
			}
			ActSend+=retLen;
		}
		if (LastLeft)
		{
			if (pcmd)
			{
				memcpy_s((char *)cmd.Data,sizeof(cmd.Data),(pcmd+(SeqPkt*MA_DATA_LEN)),LastLeft);
				cmd.PackSeq=SeqPkt;
				cmd.len=LastLeft;
			}
			if(!PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent))
			{
				return 0;
			}
			ActSend+=retLen;
		}
	}
	
	return ActSend;
}

int CDeviceOp::SetWriteData(char *p,unsigned int Len,unsigned int Seq)
{
	if (!p)
	{
		return 0;
	}

	if (0!=(SendData[Seq].pSendData))
	{
		return 0;
	}
	
	if (0!=(SendData[Seq].Len))
	{
		return 0;
	}
	
	unsigned int SizeD=Len*sizeof(char)+1;
	char *pTemp=(char *)malloc(SizeD);
	if (!pTemp)
	{
		return 0;
	}
	memset(pTemp,0,SizeD);
    memcpy_s(pTemp,SizeD,p,Len);
	SendData[Seq].pSendData=pTemp;
    SendData[Seq].Len=SizeD;

	return 1;
}
int CDeviceOp::DevSRQRegCallback(SRQCallback pSRQCallback)
{
	this->pSRQCallback=pSRQCallback;
	return 1;
}

int CDeviceOp::DevSRQStart(void)
{
	HANDLE hThread=NULL;
	unsigned int dwThreadID=NULL;
	HANDLE hccThread=NULL;
	unsigned int dwccThreadID=NULL;	

	IsLinkResponseEvent=CreateEvent( 
        NULL,         // default security attributes
        FALSE,         // auto-reset event
        FALSE,         // initial state is unsignaled
        NULL  // object name
        ); 

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
	if(!PeerConnectSocket(s,DevInfo.Ip,SOC_IO_PORT))
	{
		return 0;
	}

	DevSRQWrite("Connect requirement",strlen("Connect requirement"));	
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
void CDeviceOp::DevSRQStop(void)
{
	DWORD retLen;
	memset((void *)&cmd,0,sizeof(cmd));
	strcpy_s(cmd.DeviceName,sizeof(cmd.DeviceName),DevInfo.ComName);
	cmd.OpMode=0;//index the operation mode is SRQ mode
	cmd.ExitFlag=0;
	Sleep(1000);//Sleep wait for PTS service exit	
	PeerSendDataS(s,(char*)&cmd,sizeof(cmd),&retLen,hSendEvent,hExitEvent);
	Sleep(100);
	DevSRQEnd();
	return;
}
void CDeviceOp::DevSRQEnd(void)
{
	int Index=0;
	if (s)
	{
		closesocket(s);
		s=0;
	}
	for (Index=0;Index<MAX_SEND;Index++)
	{
		if (SendData[Index].pSendData)
		{
			free(SendData[Index].pSendData);
			SendData[Index].pSendData=0;
			SendData[Index].Len=0;
		}
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
int CDeviceOp::IsLinkToPts(void)
{
	DWORD retLen;
    DEV_OP_CMD LinkTestCmd;
	DWORD dwWaitResult=0;
	memset((void *)&LinkTestCmd,0,sizeof(LinkTestCmd));
	strcpy_s(LinkTestCmd.DeviceName,sizeof(LinkTestCmd.DeviceName),DevInfo.ComName);

	LinkTestCmd.OpMode=SRQ_MODE;
	LinkTestCmd.ExitFlag=1;//Not Exit packet
	LinkTestCmd.W_R_flag=LINK_RESPONSE;

	PeerSendDataS(s,(char*)&LinkTestCmd,sizeof(LinkTestCmd),&retLen,hSendEvent,hExitEvent);
	
	dwWaitResult = WaitForSingleObject(IsLinkResponseEvent, 5000L);//wait for 5 second
	if (WAIT_TIMEOUT== dwWaitResult)
	{
         return 0; 
	}
	return 1;
}