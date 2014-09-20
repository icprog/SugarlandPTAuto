#include "StdAfx.h"
#include "CommInterfaceBase.h"

CCommInterfaceBase::CCommInterfaceBase(void)
{
	pRecvMsgProcessor=NULL;
}

CCommInterfaceBase::~CCommInterfaceBase(void)
{
}


int CCommInterfaceBase::CommConnect(int argc,...)
{
	return 0;
}
int CCommInterfaceBase::CommDisconnect()
{
	return 0;
}
int CCommInterfaceBase::CommRead(char* buff,int nLen)
{

	return 0;
}
int CCommInterfaceBase::CommWrite(char* buff,int nLen)
{
	return 0;
}

int CCommInterfaceBase::CommRegisterRecvCallBack(RecvMsgCallback pCallback)
{
	pRecvMsgProcessor=pCallback;
	return 0;
}