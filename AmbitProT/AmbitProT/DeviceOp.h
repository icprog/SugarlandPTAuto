#pragma once

#include "PeerSocket.h"
#include "PTSC.h"
//#include "..\PTS\DeviceDefine.h"
#include "DeviceDefine.h"
#define DEV_IO_REMOTE
#define MAX_SEND 100

class CDeviceOp
{
public:
	CDeviceOp(INIT_INFO *pInit);
public:
	~CDeviceOp(void);

public:
	WSAEVENT hExitEvent;
public:
	DEV_OP_CMD cmd;
	DEV_OP_CMD RevCmd;
public:
	int DevSRQWrite(char *pcmd,UINT Len);
	int SetWriteData(char *p,unsigned int Len,unsigned int Seq);
	int DevSend(void);
	SRQCallback pSRQCallback;
    int DevSRQRegCallback(SRQCallback pSRQCallback);
	int SRQModeFlag;	
	DWORD RevID;
	int DevSRQOnRev(char *p,UINT Len);
	SOCKET s;
	WSAEVENT hSendEvent;
	WSAEVENT hRevEvent;
	HANDLE IsLinkResponseEvent;
public:
	int DevSRQStart(void);
	void DevSRQStop(void);
	int IsLinkToPts(void);
public:
	void DevSRQEnd(void);
	int SRQTaskFlag;
private:	
	INIT_INFO DevInfo;
	typedef struct SendDataTag
	{
		char *pSendData;
		unsigned int Len;
	}SEND_DATA;
	SEND_DATA SendData[MAX_SEND];
};
