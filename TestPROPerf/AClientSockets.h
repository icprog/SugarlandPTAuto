/********************************************************************
created:	2006/11/27
created:	11:10:2006   10:56
filename: 	..\ambitclient\aclientsocket.h
file path:	..\ambitclient
file base:	aclientsocket
file ext:	h
author:		Jeffrey

purpose:	it is sockets client interface to eth 
*********************************************************************/
#ifndef A_CLIENT_SOCKET_H
#define  A_CLIENT_SOCKET_H

#include "stdafx.h"

#include <winsock2.h>


#define WM_GUIIO WM_USER + 1214


typedef struct RevThreadParaTag
{
	SOCKET Socket;
	char *pData;
	size_t iLength;
	HANDLE hRevEvent;
}REV_TH_PARA;

typedef struct ClientType
{
	SOCKET     SocketType;
	char IPAddr[20];
	UINT uPort;
	char    AmSkcInfo[50];
	HANDLE hRevEvent;
	char *pgRevBufType;
	DWORD dwRevThreadID;
	int iRevBufTypeLen;
	int iConnectSerFlag;

}CLIENT_TYPE;

//Maxwell 090520
typedef struct SendRevBuffTag
{
	DWORD Len;
	char Data[3000];
}SOC_BUF_SEND,SOC_BUF_RECV;
//Maxwell 090520

BOOL InitSocket();
int GetLocalHostName(char* sHostName, int Len);
int GetIpAddress(char *sHostName, BYTE *f0,BYTE *f1,BYTE *f2,BYTE *f3);
int SelectProtocols(DWORD dwSetFlags,
					DWORD dwNotSetFlags,
					LPWSAPROTOCOL_INFO lpProtocolBuffer,
					LPDWORD lpdwBufferLength,
					WSAPROTOCOL_INFO *pProtocol);
int Send(SOCKET Socket,char *szSend);
int Recv(SOCKET Socket,char *szRecv,int iRecvLen);
void memblast(void* dest,void* src,DWORD count);
unsigned int _stdcall ReceiveThreadProc(LPVOID lpParameter);
//int LoadWinsock(char *szIP, char *RevBuf, int BufLen);
int LoadWinsock(CLIENT_TYPE *pClientType);
int OpenClient(char *pIP, char *pcRevBuf, int BufLen);
int AmbitSend(CLIENT_TYPE *pClientType, char *pcSendBuf, int SendBufLen);
void AmbitCloseClient();
int ReadResponseFromOneSoc(CLIENT_TYPE *pClientType);

#endif