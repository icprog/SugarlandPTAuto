#pragma once


typedef int (_stdcall *RecvMsgCallback)(char *msg,int Len);
class CCommInterfaceBase
{
public:
	CCommInterfaceBase(void);
public:
	~CCommInterfaceBase(void);

public:
	RecvMsgCallback pRecvMsgProcessor;

public:
	virtual int CommConnect(int argc,...);
	virtual int CommDisconnect();
	virtual int CommRead(char* buff,int nLen);
	virtual int CommWrite(char* buff,int nLen);

	int CommRegisterRecvCallBack(RecvMsgCallback pCallback);
};
