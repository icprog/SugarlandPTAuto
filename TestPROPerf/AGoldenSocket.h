//Used for K31 Golden control
#ifndef A_GOLDEN_SOCKET_H
#define  A_GOLDEN_SOCKET_H

#include "stdafx.h"

#include <winsock2.h>

typedef struct GoldenRevThreadParaTag
{
	SOCKET Socket;
	char *pData;
	size_t iLength;
	HANDLE hRevEvent;
	int *ConnectFlag;//Talen 2011/07/27
}GOLD_REV_TH_PARA;

typedef struct GoldenClientType
{
	SOCKET     SocketType;
	char IPAddr[32];
	UINT uPort;
	char    AmSkcInfo[64];
	HANDLE hRevEvent;
	char *pgRevBufType;
	DWORD dwRevThreadID;
	int iRevBufTypeLen;
	int iConnectSerFlag;
	char RecvInfo[1024]; //Maxwell 101223

}GOLD_CLIENT_TYPE;

//Maxwell 090520
typedef struct GoldenSendRevBuffTag
{
	DWORD Len;
	char Data[3000];
}GOLD_SOC_BUF_SEND,GOLD_SOC_BUF_RECV;
//Maxwell 090520

BOOL GoldenInitSocket();
int GoldenGetLocalHostName(char* sHostName, int Len);
int GoldenGetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3);
int GoldenSelectProtocols(DWORD dwSetFlags,
					DWORD dwNotSetFlags,
					LPWSAPROTOCOL_INFO lpProtocolBuffer,
					LPDWORD lpdwBufferLength,
					WSAPROTOCOL_INFO *pProtocol);
int GoldenSend(SOCKET Socket,char *szSend);
int GoldenRecv(SOCKET Socket,char *szRecv,int iRecvLen);
void Goldenmemblast(void* dest,void* src,DWORD count);
unsigned int _stdcall GoldenReceiveThreadProc(LPVOID lpParameter);
//int LoadWinsock(char *szIP, char *RevBuf, int BufLen);
int GoldenLoadWinsock(GOLD_CLIENT_TYPE *pClientType);
int GoldenOpenClient(char *pIP, char *pcRevBuf, int BufLen);
int GoldenAmbitSend(GOLD_CLIENT_TYPE *pClientType, char *pcSendBuf, int SendBufLen);
void GoldenAmbitCloseClient();
int GoldenReadResponseFromOneSoc(GOLD_CLIENT_TYPE *pClientType);
int ReStartGoldenServerConnect(GOLD_CLIENT_TYPE *Server,char *IP,char *PORT,char *connectInfo);

#endif