#pragma once
#include "socketcomm.h"

typedef struct SendBufTag
{
	DWORD Len;
	unsigned char Data[3000];
}AM_SEND_BUF,AM_REV_BUF;



class CAmTesterIO :
	public CSocketComm
{
public:
	CAmTesterIO(void);
public:
	~CAmTesterIO(void);


private:
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);
	unsigned int dwRunEventThreadID;//Thread ID for process socket event
    unsigned int dwRevDataThreadID;//Thread id for rev data id

	AM_REV_BUF RevBuf;
	DWORD RevOffset;
	int ConnectFlag;


public:
	void SetRevDataThreadID(unsigned int id);
	void SetRevEventThreadID(unsigned int id);
	int Start(unsigned int port);
    void Stop();
	DWORD AmSend(unsigned char *pBuf, DWORD dwSize);



};
