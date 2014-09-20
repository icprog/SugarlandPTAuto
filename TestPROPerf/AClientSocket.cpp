/********************************************************************
	created:	2006/11/27
	created:	27:11:2006   11:19
	filename: 	..\ambitsockets\ambitclient\aclientsocket.cpp
	file path:	..\ambitsockets\ambitclient
	file base:	aclientsocket
	file ext:	cpp
	author:		Jeffrey
	
	purpose:  The function define file, the client socket interface 	
*********************************************************************/

#include "stdafx.h"
#include "AClientSockets.h"
#include <winsock2.h>
#include "AmbitTestSysDef.h"

#define SETFLAGS     XP1_GUARANTEED_DELIVERY|XP1_GUARANTEED_ORDER
#define NOTSETFLAGS      XP1_CONNECTIONLESS

 
//UINT port=DEFAULT_PORT;
static BYTE m_ip[4];
struct sockaddr_in server;

REV_TH_PARA        RevThreadPara;

extern HANDLE ghRevEvent; 
extern CLIENT_TYPE AmPROUI;
extern TEST_INPUT_INFO gTI;//test input information

// Initial the socket communication 
BOOL InitSocket()
{
	
	// Winsock initialization
	WSADATA				wsd;
	char				szString[255];
	char IpAddress[50];
	char				name[20];
	
	// load Winsock2.2 version
	if (WSAStartup(0x202,&wsd) != 0)
	{
		
	   sprintf_s(szString, "%s", "Load Winsock   Fail");
#ifdef _CONSOLE
       amprintf("Client, %s;\n", szString);
#else
       MessageBox(NULL,"Failed to load Winsock!","Client",MB_OK);
#endif		
	   return FALSE;
	}
	
	//get local host informations and print it
	GetLocalHostName(name, sizeof(name));

	
	//printf("HostName:%s       ", name);
	GetIpAddress(name,&m_ip[0],&m_ip[1],&m_ip[2],&m_ip[3]);
    sprintf_s(IpAddress, sizeof(IpAddress), "%d.%d.%d.%d", m_ip[0], m_ip[1], m_ip[2], m_ip[3]);
	strcpy_s(gTI.TestStaInfo.TesterPCIP,sizeof(gTI.TestStaInfo.TesterPCIP),IpAddress);	
	return TRUE;
	
}

//get host name
int GetLocalHostName(char* sHostName, int Len)	
{
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//generate error
		strcpy_s(sHostName,Len,"Can not get the host name.");
		return GetLastError();
	}
	strcpy_s(sHostName,Len,szHostName);
	strcpy_s(gTI.TestStaInfo.TesterPCName,sizeof(gTI.TestStaInfo.TesterPCName),sHostName);

	return 0;
}

//get host IP Informations
int GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3)
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
int OpenClient(char *pIP, char *pcRevBuf, int BufLen)
{
	
    /*if (LoadWinsock(pIP, pcRevBuf, BufLen) == 0)
	{
	   giConnectSerFlag = TRUE;
	}
    else 
	{
	   giConnectSerFlag = FALSE;
	}*/

	return 0;
}
//////////////////////////////////////////////////////////////////////////
// load the socket and connect the server
//int LoadWinsock(char *szIP, char *RevBuf, int BufLen)
int LoadWinsock(CLIENT_TYPE *pClientType)
{

	DWORD				dwLen;
	int					nRet,nZero;
	LPBYTE				pBuf;
	WSAPROTOCOL_INFO	Protocol;
	char				szString[81];

	SOCKET sClientTypeTemp;
	HANDLE				hRevThread;
	unsigned		    dwRevThreadId;
	
  	
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
	
	sClientTypeTemp = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_IP,NULL,0,SOCK_STREAM);
	
	if (sClientTypeTemp == INVALID_SOCKET)
		return 1;
	
	nZero = 0;
	setsockopt(sClientTypeTemp,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));
	

	server.sin_family = AF_INET;
	server.sin_port = htons(pClientType->uPort);
	server.sin_addr.s_addr = inet_addr(pClientType->IPAddr);
	
	if (connect(sClientTypeTemp,(struct sockaddr *)&server,sizeof(server)) == SOCKET_ERROR)
	{
		memset(szString,'\0',sizeof(szString));
		sprintf_s(szString,sizeof(szString),"Connect() failed: %d",WSAGetLastError());
		//MessageBox(NULL,szString,"Client Socket Error",MB_OK);
		amprintf("Client socket error. %s; \n", szString);
		return 1;
	}
	else// if connect the server is successful, create the rev thread
	{

		//RevThreadPara.Socket = sClient;
		//RevThreadPara.pData = RevBuf;
		//RevThreadPara.iLength = BufLen;
		pClientType->SocketType = sClientTypeTemp;	
		RevThreadPara.Socket = pClientType->SocketType;
		RevThreadPara.pData = pClientType->pgRevBufType;
		RevThreadPara.iLength = pClientType->iRevBufTypeLen;
		RevThreadPara.hRevEvent = pClientType->hRevEvent;
        AmPROUI.iConnectSerFlag=1;//set connect flag to false

		hRevThread=(HANDLE)_beginthreadex( 
			NULL,
			0,
			ReceiveThreadProc,
			&RevThreadPara,
			0,
			&dwRevThreadId 
			);

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
void AmbitCloseClient()
{
	//terminates use of the WS2_32.DLL
	WSACleanup();
	return;
}
//////////////////////////////////////////////////////////////////////////
//
int Send(SOCKET Socket,char *szSend)
{
	int		iSent = 0;
	//fSending = TRUE;
	//if (!giConnectSerFlag) 
	//{
      // amprintf("ERROR! The Connect is not linked.;\n");	
       //return 0;
	//}
	//iSent = send(Socket,szSend,(int)strlen(szSend),0);
	return iSent;
	
}
////////////////////////////////////////////////////////////////////////////

int AmbitSend(CLIENT_TYPE *pClientType, char *pcSendBuf, int SendBufLen)
{
	int		iSent;
	//fSending = TRUE;
	//Maxwell 090520

	if (!pClientType->iConnectSerFlag) 
	{
       printf("ERROR! The Connect is not linked.\n");	
       return 0;
	}

	iSent = send(pClientType->SocketType,pcSendBuf,SendBufLen,0);
	return iSent;
}

////////////////////////////////////////////////////////////////////////////

unsigned int _stdcall ReceiveThreadProc(LPVOID lpParameter)
{

	SOCKET	MySocket;
	FD_SET	SocketSet;
	struct	timeval	timeout;
	//char	szMessage[1024];    //Maxwell 090521
	char	szMessage[3004];
	char	RecvMessage[3004];
	DWORD	iRecv;
	DWORD	iRet;

	int timeCount = 0;
	unsigned char *pDataBUF = NULL;
	

	// 分析参数
	MySocket = AmPROUI.SocketType;

	// 设置超时值
	timeout.tv_sec = 0;		// 秒
	timeout.tv_usec = 0;	// 微秒
	
	// 设置Socket集合
	SocketSet.fd_count = 1;
	SocketSet.fd_array[1] = MySocket;
	

	AmPROUI.iConnectSerFlag=1;//set connect flag to true

	int iOffset = 0;   //Maxwell 090521
	SOC_BUF_RECV DataRecv;     //Maxwell 090521
	int DataRecvLen = sizeof(DataRecv);
	int MsgRecvLen = sizeof(RecvMessage);
	memset(szMessage,'\0',sizeof(szMessage));   //Maxwell 090521
	memset(&DataRecv,'\0',sizeof(DataRecv));
	memset(RecvMessage,'\0',sizeof(RecvMessage));


	// 轮询sockets
	while(TRUE)
	{  
		// 等候发送过来的数据直到超时
		iRet = select(0,&SocketSet,NULL,NULL,&timeout);
		if (iRet != 0)
		{
			// 初始化缓冲
			//memset(szMessage,'\0',sizeof(szMessage));     //Maxwell 090521
			// 阻塞方式调用recv()
			iRecv = recv(MySocket,szMessage,3004,0);    //Maxwell 090521
			//iRecv = recv(MySocket,(char*)(&DataRecv),3004,0);	
			if(MsgRecvLen-iOffset>iRecv)
			{
				memcpy_s(RecvMessage+iOffset,MsgRecvLen-iOffset,szMessage,iRecv);
				iOffset +=  iRecv;
			}
			else
			{
				memcpy_s(RecvMessage+iOffset,MsgRecvLen-iOffset,szMessage,MsgRecvLen-iOffset);
				iOffset +=  MsgRecvLen-iOffset;
			}
			
			//memcpy_s(((&DataRecv)+iOffset),DataRecvLen-iOffset,szMessage,sizeof(szMessage));
			//iOffset +=  iRecv;
			if ((iOffset <= DataRecvLen) && (iRecv >= 0) )
			{
				//Maxwell 090521
				
				if(DataRecvLen!=iOffset)
				{
					Sleep(1);
					continue;
				}
				else
				{
					memcpy_s(&DataRecv,DataRecvLen,RecvMessage,sizeof(RecvMessage));
					memset(szMessage,'\0',sizeof(szMessage));   //Maxwell 090521
					memset(RecvMessage,'\0',sizeof(RecvMessage));
					iOffset = 0;
					DataRecv.Data[DataRecvLen] = '\0';
				}        
			}
			else
			{
				break;
			}
					
			// if the server is nothing recv, the time count is start
			if (0 == iRecv) 
			{
               timeCount++;
			   if (timeCount > 200) 
			   {
				   timeCount = 0;
				   break;
			   }
			}
			else
			{
                   timeCount = 0;
				   if (iRecv > 3004) 
				   {
					   break;
				   }

				   if(DataRecv.Len)
				   {
					   //pDataBUF=(char *)malloc(sizeof(char)*(DataRecvLen+1));
					   pDataBUF = (unsigned char *)malloc(DataRecv.Len);
					   if (pDataBUF)
					   {
						   memset(pDataBUF,'\0',DataRecv.Len);
						   memcpy_s(pDataBUF, DataRecv.Len,DataRecv.Data,DataRecv.Len);
						   PostThreadMessage(AmPROUI.dwRevThreadID, WM_GUIIO, (UINT)pDataBUF,(UINT)(DataRecv.Len));
						   memset(&DataRecv,'\0',sizeof(DataRecv));
					   }      
				   }
			}
		}
	}
	printf("disconnect.\n");
	
	AmPROUI.iConnectSerFlag=0;//set connect flag to false
	
	closesocket(MySocket);
	return 0;
}




#ifdef jeffrey
DWORD WINAPI ImageProRevThreadProc(LPVOID lpParameter)
{
	WSAOVERLAPPED	olRecv;
	WSAEVENT	gheventOlSock;
	WSAEVENT	eventArray[2];
	
	WSABUF		buffRecv;
	DWORD		dwRet,dwNumBytes,dwFlags;
	int			nWSAError;
	DWORD		iNumBytes = 0;
	
	char		szError[81];

	
    REV_TH_PARA *pRev_para;

    pRev_para = (REV_TH_PARA *)lpParameter;

	gheventOlSock = WSACreateEvent();
	eventArray[0] = gheventOlSock;
	
	ZeroMemory(&olRecv,sizeof(WSAOVERLAPPED));
	olRecv.hEvent= gheventOlSock;
	
	
	buffRecv.len = sizeof(gImageRevBuf);
	buffRecv.buf = gImageRevBuf;
	

	printf("soc IMAGE thread: %d\n", ImageProSoc.SocketType);
	//fReceiving = TRUE;
	while (TRUE)
	{
		dwFlags = 0;
		dwNumBytes = 0;
		
		if ((dwRet = WSARecv(ImageProSoc.SocketType, &buffRecv,1,&dwNumBytes,&dwFlags,&olRecv,NULL)) == SOCKET_ERROR) 
		{
			nWSAError = WSAGetLastError();
			if (nWSAError != ERROR_IO_PENDING)
			{
				//sprintf(szError,"WSARecv failed with error %d\n",nWSAError);
				sprintf_s(szError, sizeof(szError), "WSARecv failed with error %d\n",nWSAError);
				//MessageBox(NULL,szError,"Client",MB_OK);
				amprintf("Client: %s;\n", szError);
				return SOCKET_ERROR;
			}
		}
		
		if (WSAWaitForMultipleEvents(1,eventArray,FALSE,WSA_INFINITE,FALSE) == WSA_WAIT_FAILED) 
		{
			sprintf_s(szError, sizeof(szError), "WSAWaitForMultipleEvents failed %d\n", WSAGetLastError());
			//MessageBox(NULL,szError,"Client",MB_OK);
			amprintf("Client: %s;\n", szError);
		} 
		WSAResetEvent(eventArray[0]);
		if (WSAGetOverlappedResult(pRev_para->Socket,&olRecv,&dwNumBytes,FALSE,&dwFlags) == TRUE)
		{
			if (dwNumBytes == 0)
				break;
		} 
		else 
		{
			sprintf_s(szError,sizeof(szError),"WSAGetOverlappedResult failed with error %d\n", WSAGetLastError());
			//MessageBox(NULL,szError,"Client",MB_OK);
			amprintf("Client: %s;\n", szError);
			return SOCKET_ERROR;
		}

	    *(buffRecv.buf+dwNumBytes) = NULL;
		//amprintf("Recive Message: %s;\n", buffRecv.buf);
		if (!SetEvent(ImageProSoc.hRevEvent)) 
		{ 
			// Deal with error.
		} 
		else
		{

		}

		

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
#endif

int SelectProtocols(DWORD dwSetFlags,DWORD dwNotSetFlags,LPWSAPROTOCOL_INFO lpProtocolBuffer,LPDWORD lpdwBufferLength,WSAPROTOCOL_INFO *pProtocol)
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
			
			if (strcmp((char *)pInfo->szProtocol,"MSAFD Tcpip [TCP/IP]") == 0)
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

void memblast(void* dest,void* src,DWORD count)
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
