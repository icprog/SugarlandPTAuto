#pragma once

#include "PeerSocket.h"
#include "DeviceDefine.h"



typedef int (_stdcall *SRQCallback)(unsigned char *p,UINT Len);




class CDeviceOp
{
public:
	CDeviceOp(void);
public:
	~CDeviceOp(void);


public:
	WSAEVENT hExitEvent;
	int DevIO();
public:
	int DevWrite(char *Name,char *pcmd,UINT Len);
	int DevRead(char *Name,char *pcmd,UINT Len);
	DEV_OP_CMD cmd;
	DEV_OP_CMD RevCmd;
public:
	int DevSRQWrite(char *Name,char *pcmd,UINT Len);
	
	SRQCallback pSRQCallback;
    int DevSRQRegCallback(SRQCallback pSRQCallback);
	int SRQModeFlag;
	
	DWORD RevID;
	int DevSRQSetOnRev(DWORD RevID);
	int DevSRQOnRev(unsigned char *p,UINT Len);
	SOCKET s;
	WSAEVENT hSendEvent;
	WSAEVENT hRevEvent;
public:
	int DevSRQStart(char *pDevName);
	void DevSRQStop(char *Name="NoDefine");
public:
	void DevSRQEnd(void);
	int SRQTaskFlag;
};
