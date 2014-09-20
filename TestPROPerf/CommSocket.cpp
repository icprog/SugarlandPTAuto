#include "StdAfx.h"
#include "CommSocket.h"
#include <winsock2.h>

#define SETFLAGS     XP1_GUARANTEED_DELIVERY|XP1_GUARANTEED_ORDER
#define NOTSETFLAGS      XP1_CONNECTIONLESS

#define MA_DATA_LEN 5120
#define RECV_MAX_LEN 5120

CCommSocketClient::CCommSocketClient(void)
{
	InitSocket();
	iConnectSerFlag = 0;
	
	hReadEvent = CreateEvent( 
		NULL,         // default security attributes
		FALSE,         // Auto-reset event
		FALSE,         // initial state is unsignaled
		NULL  // object name
		); 


	if (hReadEvent == NULL) 
	{
		printf("CreateMutex error: %d\n", GetLastError());
		//return 0;
	}
	memset(RecvBuf,0,sizeof(RecvBuf));

	

}

CCommSocketClient::~CCommSocketClient(void)
{
	CloseHandle(hReadEvent);
}

int CCommSocketClient::CommConnect(int argc,...)
{

	va_list argList;
	/* get the arguement list */
    va_start(argList, argc);


	char *ip = va_arg( argList, char* );
	printf("%s\n",ip);


	int port = va_arg( argList, int);
	printf("%d\n",port);

	int open_recv_thread=1;
	open_recv_thread=va_arg( argList, int);
	if(open_recv_thread!=1)
	{
		open_recv_thread=0;
	}

	va_end(argList);    /* cleanup arg list */
	int ret=OpenClient(ip, port,open_recv_thread,RecvBuf);

	return ret;
}
int CCommSocketClient::CommDisconnect()
{
	shutdown(sClient, SD_BOTH);
    return ( 0 == closesocket( sClient ));
	//WSACleanup();
	return 0;
}
int CCommSocketClient::CommRead(char* buff,int nLen)
{
	int recv=Recv(sClient,buff,nLen);
	//int ret=CommWaitReadMsg(buff,nLen);
	//if(ret==-1) return -1;
	recv=strlen(buff);
	return recv;
}
int CCommSocketClient::CommWrite(char* buff,int nLen)
{
	Send(sClient,buff,nLen);
	return 0;
}


int  CCommSocketClient::CommWaitReadMsg(char* buff,int nLen)
{
	DWORD dwWaitResult; 

	// Request ownership of mutex.
	dwWaitResult = WaitForSingleObject( 
		hReadEvent,   // handle to mutex
		50000L);   // fifty-second time-out interval

	switch (dwWaitResult) 
	{
		// The thread got mutex ownership.
		case WAIT_OBJECT_0: 
			strcpy(buff,RecvBuf);
			//memset(RecvBuf,0,5120);
			break;
		// Cannot get mutex ownership due to time-out.
		case WAIT_TIMEOUT: 
			return -1; 
		// Got ownership of the abandoned mutex object.
		case WAIT_ABANDONED: 
			return 0; 
		default:break;
	}
	return 0;
}

// Initial the socket communication 
bool CCommSocketClient::InitSocket()
{
	
	// Winsock initialization
	WSADATA				wsd;
	char				szString[255];
	char				name[20];
	
	// load Winsock2.2 version
	if (WSAStartup(0x202,&wsd) != 0)
	{
		
	   sprintf(szString,"Failed to load Winsock!");
#ifdef _CONSOLE
       printf("Client, %s\n", szString);
#else
       MessageBox(NULL,"Failed to load Winsock!","Client",MB_OK);
#endif		
	   return FALSE;
	}
	
	//get local host informations and print it
	GetLocalHostName(name);

	printf("HostName:%s       ", name);
	
	GetIpAddress(name,&m_ip[0],&m_ip[1],&m_ip[2],&m_ip[3]);
	
	
	printf("IP Address: %d.%d.%d.%d \n", m_ip[0], m_ip[1], m_ip[2], m_ip[3]);
	
	
	return TRUE;
	
}

//get host name
int CCommSocketClient::GetLocalHostName(char* sHostName)	
{
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//generate error
		strcpy(sHostName,"Can not get the host name.");
		return GetLastError();
	}
	strcpy(sHostName,szHostName);
	return 0;
}

//get host IP Informations
int CCommSocketClient::GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3)
{
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName);
	if(lpHostEnt==NULL)
	{
		//generate error
		f0=f1=f2=f3=0;
		return GetLastError();
	}
	//get ip
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		*f0=inAddr.S_un.S_un_b.s_b1;
		*f1=inAddr.S_un.S_un_b.s_b2;
		*f2=inAddr.S_un.S_un_b.s_b3;
		*f3=inAddr.S_un.S_un_b.s_b4;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//start the client
int CCommSocketClient::OpenClient(char *pIP, int port,int open_recv_thread,char *pcRevBuf)
{
	
    if (LoadWinsock(pIP,port,open_recv_thread,pcRevBuf) == 0)
	{
	   iConnectSerFlag = TRUE;
	   	return 1;
	}
    else
	{
	   iConnectSerFlag = FALSE;
	   	return 0;
	}


}
//////////////////////////////////////////////////////////////////////////
// load the socket and connect the server
int CCommSocketClient::LoadWinsock(char *szIP,int port,int open_recv_thread,char *RevBuf)
{
	WSADATA				wsd;
	DWORD				dwLen;
	int					nRet,nZero;
	LPBYTE				pBuf;
	WSAPROTOCOL_INFO	Protocol;
	char				szString[81];
//	char				szMessage[81];
	
	HANDLE				hRevThread;
	DWORD				dwRevThreadId;
	
   
	
	if (WSAStartup(0x202,&wsd) != 0)
	{
	
		//MessageBox(NULL,"hehe","Client Socket Error",MB_OK);
		
		printf("Client Socket Error!\n");
		
		return 1;
	}
	
	dwLen = 0;
	nRet = WSAEnumProtocols(NULL,NULL,&dwLen);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAENOBUFS)
		return 1;
	}
	
	pBuf = (unsigned char *)malloc(dwLen);
	
	nRet = SelectProtocols(SETFLAGS,NOTSETFLAGS,(LPWSAPROTOCOL_INFO)pBuf,&dwLen,&Protocol);
	
	free(pBuf);
	
	sClient = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,SOCK_STREAM);
	if (sClient == INVALID_SOCKET)
		return 1;
	
	nZero = 0;
	setsockopt(sClient,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));
	
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(szIP);
	
	if (connect(sClient,(struct sockaddr *)&server,sizeof(server)) == SOCKET_ERROR)
	{
		memset(szString,'\0',sizeof(szString));
		sprintf(szString,"Connect() failed: %d",WSAGetLastError());
		//MessageBox(NULL,szString,"Client Socket Error",MB_OK);
		printf("Client socket error. %s \n", szString);
		return 1;
	}

	
	else if(open_recv_thread)// if connect the server is successful, create the rev thread
	{

		RevThreadPara.Socket = sClient;
	    RevThreadPara.pData = RevBuf;
		RevThreadPara.iLength = sizeof(RevBuf);
		RevThreadPara.hReadEvent=hReadEvent;
		RevThreadPara.pRecvCallback = pRecvMsgProcessor;
		

		
		hRevThread = CreateThread(NULL,0,RecvThreadProc,&RevThreadPara,0,&dwRevThreadId);
		if (hRevThread)
		{
			//close the thread handle 
			CloseHandle(hRevThread);
		}
		
	}
	
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
int CCommSocketClient::Send(SOCKET Socket,char *szSend,int nLen)
{
	int		iSent;
	//fSending = TRUE;
	if (!iConnectSerFlag) 
	{
       printf("ERROR! The Connect is not linked.\n");	
       return 0;
	}
	iSent = send(Socket,szSend,strlen(szSend),0);
	return iSent;
	
}
//////////////////////////////////////////////////////////////////////////


int CCommSocketClient::Recv(SOCKET Socket,char *szRecv,int iRecvLen)
{
	int		iRecv;
	//fReceiving = TRUE;
	iRecv = recv(Socket,szRecv,iRecvLen,0);
	
	if (iRecv == SOCKET_ERROR || iRecv == 0)
		MessageBox(NULL,"NO DATA or SOCKET ERROR","Client",MB_OK);
	szRecv[iRecv] = '\0';
	//fReceiving = FALSE;
	return iRecv;
}
//////////////////////////////////////////////////////////////////////////
//
DWORD WINAPI RecvThreadProc(LPVOID lpParameter)
{
	WSAOVERLAPPED	olRecv;
	WSAEVENT	gheventOlSock;
	WSAEVENT	eventArray[2];
	
	WSABUF		buffRecv;
	DWORD		dwRet,dwNumBytes,dwFlags;
	int			nWSAError;
	DWORD		iNumBytes = 0;
	
	char		szError[81];

	
    RECV_TH_PARA *pRev_para;

    pRev_para = (RECV_TH_PARA *)lpParameter;

	gheventOlSock = WSACreateEvent();
	eventArray[0] = gheventOlSock;
	
	ZeroMemory(&olRecv,sizeof(WSAOVERLAPPED));
	olRecv.hEvent= gheventOlSock;
	buffRecv.len = pRev_para->iLength;
	buffRecv.buf = pRev_para->pData;

	HANDLE hReadEvent=pRev_para->hReadEvent;
	
	//fReceiving = TRUE;
	char temp[MA_DATA_LEN];
	char msg[MA_DATA_LEN];

	memset(temp,0,MA_DATA_LEN);
	memset(msg,0,MA_DATA_LEN);
	buffRecv.len = MA_DATA_LEN;
	buffRecv.buf = temp;

	int total_recv=0;
	while (TRUE)
	{
		dwFlags = 0;
		dwNumBytes = 0;
		if ((dwRet = WSARecv(pRev_para->Socket,&buffRecv,1,&dwNumBytes,&dwFlags,&olRecv,NULL)) == SOCKET_ERROR) 
		{
			nWSAError = WSAGetLastError();
			if (nWSAError != ERROR_IO_PENDING)
			{
				sprintf(szError,"WSARecv failed with error %d\n",nWSAError);
				//MessageBox(NULL,szError,"Client",MB_OK);
				printf("Client:%s\n", szError);
				return SOCKET_ERROR;
			}
		}
		
		if (WSAWaitForMultipleEvents(1,eventArray,FALSE,WSA_INFINITE,FALSE) == WSA_WAIT_FAILED) 
		{
			sprintf(szError,"WSAWaitForMultipleEvents failed %d\n", WSAGetLastError());
			//MessageBox(NULL,szError,"Client",MB_OK);
			printf("Client:%s\n", szError);
		} 
		WSAResetEvent(eventArray[0]);

		
		if (WSAGetOverlappedResult(pRev_para->Socket,&olRecv,&dwNumBytes,FALSE,&dwFlags) == TRUE)
		{
			if (dwNumBytes == 0)
				break;
		} 
		else 
		{
			sprintf(szError,"WSAGetOverlappedResult failed with error %d\n", WSAGetLastError());
			MessageBox(NULL,szError,"Client",MB_OK);
			printf("Client:%s\n", szError);
			return SOCKET_ERROR;
		}
		

		
	    *(buffRecv.buf+dwNumBytes) = NULL;
		printf("Receive Message: %s\n", buffRecv.buf);


		
		total_recv+=dwNumBytes;
		if(total_recv<=RECV_MAX_LEN)
		{
			memcpy(msg+total_recv-dwNumBytes,buffRecv.buf,dwNumBytes);
		}
		else
		{
			printf("Buffer too big.\n");
			break;
		}
		//if(buffRecv.buf[dwNumBytes-1]=='\n')
		{
			msg[total_recv]='\0';
			printf("Recv Data:%s",msg);
			memset(pRev_para->pData,0,5120);
			memcpy(pRev_para->pData,msg,total_recv);
			total_recv=0;
			msg[0]='\0';
			if(pRev_para->pRecvCallback!=NULL)
			{
				pRev_para->pRecvCallback(pRev_para->pData,total_recv);
			}
			SetEvent(hReadEvent);
			
		}
		

		
		/*
		if(strstr(buffRecv.buf,"TEST"))
		{
			send(pRev_para->Socket,"OK\r\n",strlen("OK\r\n"),0);
			
			Sleep(1000);
		}
		*/
		

		// because we are not use the buf with continue mode, mark this code segment.
		//buffRecv.len -= dwNumBytes;
		//iNumBytes = iNumBytes + dwNumBytes;
		//if (buffRecv.len == 0) 
			//break;
		//else 
			//buffRecv.buf += dwNumBytes;
        
		


	}
//	fReceiving = FALSE;
	return iNumBytes;
}


int CCommSocketClient::SelectProtocols(DWORD dwSetFlags,DWORD dwNotSetFlags,LPWSAPROTOCOL_INFO lpProtocolBuffer,LPDWORD lpdwBufferLength,WSAPROTOCOL_INFO *pProtocol)
{
	LPBYTE				pBuf;
	LPWSAPROTOCOL_INFO	pInfo;
	DWORD				dwNeededLen;
	LPWSAPROTOCOL_INFO	pRetInfo;
	DWORD				dwRetLen;
	int					nCount;
	int					nMatchCount;
	int					nRet;

	dwNeededLen = 0;
	nRet = WSAEnumProtocols(NULL, NULL, &dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAENOBUFS)
			return SOCKET_ERROR;
	}

	pBuf = (unsigned char *)malloc(dwNeededLen);
	if (pBuf == NULL)
	{
		WSASetLastError(WSAENOBUFS);
		return SOCKET_ERROR;
	}

	nRet = WSAEnumProtocols(NULL,(LPWSAPROTOCOL_INFO)pBuf,&dwNeededLen);
	if (nRet == SOCKET_ERROR)
	{
		free(pBuf);
		return SOCKET_ERROR;
	}
	#define REJECTSET(f) \
	    ((dwSetFlags & f) && !(pInfo->dwServiceFlags1 & f))
	#define REJECTNOTSET(f) \
	    ((dwNotSetFlags &f) && (pInfo->dwServiceFlags1 & f))
	#define REJECTEDBY(f) (REJECTSET(f) || REJECTNOTSET(f))

	pInfo = (LPWSAPROTOCOL_INFO)pBuf;	
	pRetInfo = lpProtocolBuffer;
	dwRetLen = 0;
	nMatchCount = 0;
	for(nCount = 0; nCount < nRet; nCount++)
	{
		while(1)
		{
			if (REJECTEDBY(XP1_CONNECTIONLESS))
				break;
			if (REJECTEDBY(XP1_GUARANTEED_DELIVERY))
				break;
			if (REJECTEDBY(XP1_GUARANTEED_ORDER))
				break;
			if (REJECTEDBY(XP1_MESSAGE_ORIENTED))
				break;
			if (REJECTEDBY(XP1_PSEUDO_STREAM))
				break;
			if (REJECTEDBY(XP1_GRACEFUL_CLOSE))
				break;
			if (REJECTEDBY(XP1_EXPEDITED_DATA))
				break;
			if (REJECTEDBY(XP1_CONNECT_DATA))
				break;
			if (REJECTEDBY(XP1_DISCONNECT_DATA))
				break;
			if (REJECTEDBY(XP1_SUPPORT_BROADCAST)) 
				break;
			if (REJECTEDBY(XP1_SUPPORT_MULTIPOINT))
				break;
			if (REJECTEDBY(XP1_MULTIPOINT_DATA_PLANE))
				break;
			if (REJECTEDBY(XP1_QOS_SUPPORTED))
				break;
			if (REJECTEDBY(XP1_UNI_SEND))
				break;
			if (REJECTEDBY(XP1_UNI_RECV))
				break;
			if (REJECTEDBY(XP1_IFS_HANDLES))
				break;
			if (REJECTEDBY(XP1_PARTIAL_MESSAGE))
				break;

			dwRetLen += sizeof(WSAPROTOCOL_INFO);
			if (dwRetLen > *lpdwBufferLength)
			{
				WSASetLastError(WSAENOBUFS);
				*lpdwBufferLength = dwNeededLen;
				free(pBuf);
				return SOCKET_ERROR;
			}
			nMatchCount++;
			memblast(pRetInfo,pInfo,sizeof(WSAPROTOCOL_INFO));
			if (strcmp(pInfo->szProtocol,"MSAFD Tcpip [TCP/IP]") == 0)
				memblast(pProtocol,pInfo, sizeof(WSAPROTOCOL_INFO));

			pRetInfo++;
			break;
		}
		pInfo++;
	}
	free(pBuf);
	*lpdwBufferLength = dwRetLen;
	return(nMatchCount);
}

void CCommSocketClient::memblast(void* dest,void* src,DWORD count)
{
	DWORD	iCount;

	__asm
	{
		MOV		ECX,count
		SHR		ECX,2
		SHL		ECX,2
		MOV		iCount,ECX
		MOV		ESI,src	
		MOV		EDI,dest
		MOV		ECX,iCount
		SHR		ECX,2	
		REP		MOVSD	

		MOV		ECX,count
		MOV		EAX,iCount
		SUB		ECX,EAX

		JZ		Exit
		MOV		ESI,src	
		ADD		ESI,EAX	
		MOV		EDI,dest
		ADD		EDI,EAX
		REP		MOVSB	
		Exit:
	}
}
