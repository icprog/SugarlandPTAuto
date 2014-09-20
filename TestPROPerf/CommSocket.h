#pragma once
#include "comminterfacebase.h"
#include <winsock2.h>





typedef struct RecvThreadParaTag
{
	SOCKET Socket;
	char *pData;
	DWORD iLength;
	HANDLE hReadEvent;
	RecvMsgCallback pRecvCallback;
}RECV_TH_PARA;


#define MTLC_SOCKET_DATA_MAXLEN 3000
typedef struct MTLCPACKETTag
{
	DWORD Len;
	unsigned char Data[MTLC_SOCKET_DATA_MAXLEN];
}MTLC_SOCKET_PACKET;


DWORD WINAPI RecvThreadProc(LPVOID lpParameter);

class CCommSocketClient :
	public CCommInterfaceBase
{
public:
	CCommSocketClient(void);
public:
	~CCommSocketClient(void);


public:
	int CommConnect(int argc,...);
	int CommDisconnect();
	int CommRead(char* buff,int nLen);
	int CommWrite(char* buff,int nLen);

	int CommWaitReadMsg(char* buff,int nLen);


	

	
public:
	//SOCKET sClientSocket;
	BYTE m_ip[4];
	SOCKET sClient;
	struct sockaddr_in server;
	RECV_TH_PARA        RevThreadPara;
	int iConnectSerFlag;
	char RecvBuf[5120];
	MTLC_SOCKET_PACKET RecvPACKET;


public:
	HANDLE hReadEvent;

private:
	bool InitSocket();
	int GetLocalHostName(char* sHostName);
	int GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3);
	int SelectProtocols(DWORD dwSetFlags,
					DWORD dwNotSetFlags,
					LPWSAPROTOCOL_INFO lpProtocolBuffer,
					LPDWORD lpdwBufferLength,
					WSAPROTOCOL_INFO *pProtocol);
	int Send(SOCKET Socket,char *szSend,int nLen);
	int Recv(SOCKET Socket,char *szRecv,int iRecvLen);
	void memblast(void* dest,void* src,DWORD count);
	int LoadWinsock(char *szIP,int port,int open_recv_thread,char *RevBuf);
	int OpenClient(char *pIP, int port,int open_recv_thread,char *pcRevBuf);
};
