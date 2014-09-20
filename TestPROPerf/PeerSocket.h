// PeerSocket.h: interface for the PeerSocket.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEERSOCKET_H__674A623F_4C93_4AE9_84A7_568658FA724E__INCLUDED_)
#define AFX_PEERSOCKET_H__674A623F_4C93_4AE9_84A7_568658FA724E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PEER_STREAM SOCK_STREAM
#define PEER_DGRAM  SOCK_DGRAM
#define PEER_RAW	SOCK_RAW

bool PeerCreateSocket(SOCKET *pNewSocket,int iSockType);
bool PeerBindSocket(SOCKET BindSocket,char *szHostAddr,int iHostPort);
bool PeerConnectSocket(SOCKET BindSocket,char *szHostAddr,int iHostPort);
bool PeerListenSocket(SOCKET ListenSocket);
bool PeerShutDownSocket(SOCKET nowSocket);
bool PeerCloseSocket(SOCKET nowSocket);
bool PeerSendData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,WSAEVENT hExitEvent,DWORD time=WSA_INFINITE);
bool PeerRecvData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,WSAEVENT hExitEvent,DWORD time=WSA_INFINITE);
bool PeerSendDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,WSAEVENT hExitEvent,DWORD time=WSA_INFINITE);
bool PeerRecvDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,WSAEVENT hExitEvent,DWORD time=WSA_INFINITE);
#endif // !defined(AFX_PEERSOCKET_H__674A623F_4C93_4AE9_84A7_568658FA724E__INCLUDED_)
