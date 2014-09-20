//Used for K31 Golden control
#include "stdafx.h"
#include "AGoldenSocket.h"
#include <winsock2.h>
#include "ControlLogic.h"//Talen 2011/06/07

#define SETFLAGS     XP1_GUARANTEED_DELIVERY|XP1_GUARANTEED_ORDER
#define NOTSETFLAGS      XP1_CONNECTIONLESS

static BYTE m_ip[4];
struct sockaddr_in Goldserver;

extern GOLD_CLIENT_TYPE GoldenServer;
extern GOLD_CLIENT_TYPE GoldenServer1;//Talen 2011/06/27
extern CControlLogic CLogic;//Talen 2011/06/07

// Initial the socket communication 
BOOL GoldenInitSocket()
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
	   return FALSE;
	}
	
	//get local host informations and print it
	GetLocalHostName(name, sizeof(name));

	GetIpAddress(name,&m_ip[0],&m_ip[1],&m_ip[2],&m_ip[3]);
    sprintf_s(IpAddress, sizeof(IpAddress), "%d.%d.%d.%d", m_ip[0], m_ip[1], m_ip[2], m_ip[3]);

	return TRUE;
}

//get host name
int GoldenGetLocalHostName(char* sHostName, int Len)	
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
	
	return 0;
}

//get host IP Informations
int GoldenGetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3)
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
int GoldenOpenClient(char *pIP, char *pcRevBuf, int BufLen)
{
	
	return 0;
}
//////////////////////////////////////////////////////////////////////////
// load the socket and connect the server
int GoldenLoadWinsock(GOLD_CLIENT_TYPE *pClientType)
{
	printf("GoldenLoadWinsock;\r\n");//Talen 2011/07/28
	//amprintf("GoldenLoadWinsock;\n");//Talen 2011/07/28
	DWORD				dwLen;
	int					nRet,nZero;
	LPBYTE				pBuf;
	WSAPROTOCOL_INFO	Protocol;
	char				szString[81];

	SOCKET sClientTypeTemp;
	HANDLE				hRevThread;
	unsigned		    dwRevThreadId;

	struct sockaddr_in Goldserver;
	GOLD_REV_TH_PARA * RevThreadPara=(GOLD_REV_TH_PARA*)malloc(sizeof(GOLD_REV_TH_PARA));
	
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
	
	Goldserver.sin_family = AF_INET;
	Goldserver.sin_port = htons(pClientType->uPort);
	Goldserver.sin_addr.s_addr = inet_addr(pClientType->IPAddr);
	if (connect(sClientTypeTemp,(struct sockaddr *)&Goldserver,sizeof(Goldserver)) == SOCKET_ERROR)
	{
		printf("GoldenServerConnect (connect) fail ;\r\n");//Talen 2011/07/28
		memset(szString,'\0',sizeof(szString));
		sprintf_s(szString,sizeof(szString),"Connect() failed: %d",WSAGetLastError());
		return 1;
	}
	else// if connect the server is successful, create the rev thread
	{
		printf("GoldenServerConnect (connect) pass ;\r\n");//Talen 2011/07/28
		pClientType->SocketType = sClientTypeTemp;	
		RevThreadPara->Socket = pClientType->SocketType;
		//RevThreadPara.pData = pClientType->pgRevBufType;
		RevThreadPara->pData = pClientType->RecvInfo;
		//add start by talen for debug
		//char aa[10]="abcde";
		//RevThreadPara.pData=aa;
		//add end by talen for debug
		RevThreadPara->iLength = pClientType->iRevBufTypeLen;
		//RevThreadPara.hRevEvent = pClientType->hRevEvent;//remove by Talen 2011/07/27
        pClientType->iConnectSerFlag=1;//set connect flag to false
		RevThreadPara->ConnectFlag=&(pClientType->iConnectSerFlag);

		//Maxwell 101223

		pClientType->hRevEvent = CreateEvent( 
			NULL,   // default security attributes
			TRUE,  // auto-reset event object
			FALSE,  // initial state is nonsignaled
			NULL);  // unnamed object
		
		if (pClientType->hRevEvent == NULL) 
		{ 
			//amprintf("CreateEvent failed (%d)\n", GetLastError());
		}
		//Maxwell 101223
		RevThreadPara->hRevEvent = pClientType->hRevEvent;//Talen 2011/07/27
		hRevThread=(HANDLE)_beginthreadex( 
			NULL,
			0,
			GoldenReceiveThreadProc,
			RevThreadPara,
			0,
			&dwRevThreadId 
			);

		if (hRevThread)
		{
			//close the thread handle 
			CloseHandle(hRevThread);
		}
	}
	printf("GoldenLoadWinsock pass;\r\n");//Talen 2011/07/28
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//
void GoldenAmbitCloseClient()
{
	//terminates use of the WS2_32.DLL
	WSACleanup();
	return;
}
//////////////////////////////////////////////////////////////////////////
//
int GoldenSend(SOCKET Socket,char *szSend)
{
	int		iSent = 0;
	return iSent;
}
////////////////////////////////////////////////////////////////////////////

//////int GoldenAmbitSend(GOLD_CLIENT_TYPE *pClientType, char *pcSendBuf, int SendBufLen)
//////{
//////	int		iSent;
//////	//fSending = TRUE;
//////	//Maxwell 090520
//////
//////	if (!pClientType->iConnectSerFlag) 
//////	{
//////       amprintf("ERROR! The Connect is not linked.\n");
//////
//////	   CloseHandle(hGoldenEvent);
//////	   if(!CLogic.StartGoldenServerConnect())
//////	   {
//////		   amprintf("ERROR! The Connect relinked fail.\n");
//////		   return 0;
//////	   }
//////	}
//////	//memset(pClientType->RecvInfo,0,sizeof(pClientType->RecvInfo));//Maxwell 101223
//////	iSent = send(pClientType->SocketType,pcSendBuf,SendBufLen,0);
//////
//////	amprintf("iSent:%d;\n",iSent);
//////	if(SOCKET_ERROR==iSent)//Talen 2011/06/23
//////	{
//////		return 0;
//////	}
//////	return iSent;
//////}



int GoldenAmbitSend(GOLD_CLIENT_TYPE *pClientType, char *pcSendBuf, int SendBufLen)
{
	int		iSent;
	//fSending = TRUE;
	//Maxwell 090520

	if (!pClientType->iConnectSerFlag) 
	{
       printf("ERROR! The Connect is not linked.\n");	
       return 0;
	}

	//memset(pClientType->RecvInfo,0,sizeof(pClientType->RecvInfo));//Maxwell 101223
	iSent = send(pClientType->SocketType,pcSendBuf,SendBufLen,0);
	if(SOCKET_ERROR==iSent)//Talen 2011/07/27
	{
		return 0;
	}
	return iSent;
}

////////////////////////////////////////////////////////////////////////////

unsigned int _stdcall GoldenReceiveThreadProc(LPVOID lpParameter)
{

	SOCKET	MySocket;
	FD_SET	SocketSet;
	struct	timeval	timeout;
	char	szMessage[1024]=""; //Maxwell 091222
	char	RecvMessage[1024]="";//Maxwell 091222
	char    RecvSFIS[128]="";//Maxwell 091222
	DWORD	iRecv;
	DWORD	iRet;

	int timeCount = 0;
	unsigned char *pDataBUF = NULL;

	GOLD_REV_TH_PARA* pp=(GOLD_REV_TH_PARA*)lpParameter;
	
	// 分析参数
	//MySocket = GoldenServer.SocketType;
	//MySocket = ((GOLD_REV_TH_PARA*)lpParameter)->Socket;
	MySocket = pp->Socket;
	amprintf("MySocket=%d;\n",MySocket);//Talen 2011/07/28
	
	// 设置超时值
	timeout.tv_sec = 0;		// 秒
	timeout.tv_usec = 0;	// 微秒
	
	// 设置Socket集合
	SocketSet.fd_count = 1;
	SocketSet.fd_array[1] = MySocket;

	
	//GoldenServer.iConnectSerFlag=1;//set connect flag to true
	*(pp->ConnectFlag)=1;//set connect flag to true
	printf("%d\n",*(pp->ConnectFlag));
	printf("%s\n",(pp->pData));
	memset((pp->pData),'a',1023);
	int iOffset = 0;   //Maxwell 090521
	SOC_BUF_RECV DataRecv;     //Maxwell 090521
	int DataRecvLen = sizeof(DataRecv);
	int MsgRecvLen = sizeof(RecvMessage);
	memset(szMessage,'\0',sizeof(szMessage));   //Maxwell 090521
	memset(&DataRecv,'\0',sizeof(DataRecv));
	memset(RecvSFIS,'\0',sizeof(RecvSFIS));

	GOLD_CLIENT_TYPE temp;

	// 轮询sockets
	while(TRUE)
	{
		memset(szMessage,'\0',sizeof(szMessage));   //Maxwell 090521
		memset(RecvMessage,'\0',sizeof(RecvMessage));
		memset(RecvSFIS,'\0',sizeof(RecvSFIS));
		// 等候发送过来的数据直到超时
		iRet = select(0,&SocketSet,NULL,NULL,&timeout);
		if (iRet != 0)
		{
			//if(SOCKET_ERROR==iRet)//add by Talen 2011/11/11
			//{
			//	break;
			//}
			// 初始化缓冲
			//memset(szMessage,'\0',sizeof(szMessage));     //Maxwell 090521
			// 阻塞方式调用recv()
			//iRecv = recv(MySocket,szMessage,3004,0);    //Maxwell 090521
			iRecv = recv(MySocket,szMessage,1024,0);    //Maxwell 091222
			if(SOCKET_ERROR==iRecv)//add by Talen 2011/11/11
			{
			   break;
			}
			memcpy_s(RecvMessage,sizeof(RecvMessage),szMessage,iRecv);
			

////////////////// if the server is nothing recv, the time count is start
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
				   if (iRecv > 1024) 
				   {
					   break;
				   }
				   
				   else
				   {

				   }

					if(strstr(RecvMessage,"SET_GOLDEN_RETURN")||strstr(RecvMessage,"AUDIO_TEST_RESULT"))
					{
						memset(pp->pData,0,sizeof(temp.RecvInfo));
						memcpy_s(pp->pData,sizeof(temp.RecvInfo),RecvMessage,iRecv);
						SetEvent(pp->hRevEvent);
					}
					else if(strstr(RecvMessage,"Open"))
					{
						
					}
					else
					{

					}
			}
		}////////////////
	}
	amprintf("disconnect.\n");
	
	//GoldenServer.iConnectSerFlag=0;//set connect flag to false
	*(pp->ConnectFlag)=0;
	if(NULL!=pp)
	{
		free(pp);
	}
	closesocket(MySocket);
	return 0;
}


int GoldenSelectProtocols(DWORD dwSetFlags,DWORD dwNotSetFlags,LPWSAPROTOCOL_INFO lpProtocolBuffer,LPDWORD lpdwBufferLength,WSAPROTOCOL_INFO *pProtocol)
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

void Goldenmemblast(void* dest,void* src,DWORD count)
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



int ReStartGoldenServerConnect(GOLD_CLIENT_TYPE *Server,char *IP,char *PORT,char *connectInfo)
{
	closesocket(Server->SocketType);
	amprintf("close socket;\n");

	CloseHandle(Server->hRevEvent);
	amprintf("close Server.hRevEvent;\n");

	if(!CLogic.StartGoldenServerConnect(Server,IP,PORT,connectInfo))
	{
		amprintf("ERROR! The Connect relinked fail.\n");
		return 0;
	}
	Sleep(1000);
	return 1;
}