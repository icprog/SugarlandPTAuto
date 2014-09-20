#include "StdAfx.h"
#include "AmTesterIO.h"
#include "AmbitProTGlobalDefine.h"


CAmTesterIO::CAmTesterIO(void)
{
	dwRunEventThreadID=0;
	dwRevDataThreadID=0;
	RevOffset=0;
	ConnectFlag=0;
}

CAmTesterIO::~CAmTesterIO(void)
{


}



int CAmTesterIO::Start(unsigned int port)
{
	bool bSuccess = false;
	CString  m_strPort;//=_T("8003");
	m_strPort.Format(_T("%d"),port);


	// no smart addressing - we use connection oriented.
	SetSmartAddressing( false );
	SetServerState(true);
	
	ConnectFlag = bSuccess = CreateSocket( m_strPort, AF_INET, SOCK_STREAM, 0); // TCP

	//Create socket connect and receive thread.
	if (bSuccess && WatchComm())
	{
		//CString strServer, strAddr;
		//m_CurServer.GetLocalName( strServer.GetBuffer(256), 256);
		//strServer.ReleaseBuffer();
		//m_CurServer.GetLocalAddress( strAddr.GetBuffer(256), 256);
		//strAddr.ReleaseBuffer();
		//CString strMsg = _T("Server: ") + strServer;
		//strMsg += _T(", @Address: ") + strAddr;
		//strMsg += _T(" is running on port ") + m_strPort + CString("\r\n");
		//_tprintf(_T("%s\n"), strMsg);
	}

	return bSuccess;


}

void CAmTesterIO::Stop()
{

	StopComm();
	return;
}


void CAmTesterIO::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	LPBYTE pbuf=0;

	if (dwRevDataThreadID)
	{	
		if (dwCount>sizeof(RevBuf))//!!!socket receive buffer must = sizeof(RevBuf)
		{
			return;
		}
		memcpy_s((void *)(&RevBuf+RevOffset), sizeof(RevBuf)-RevOffset,lpBuffer, dwCount);
		RevOffset+=dwCount;

		if (RevOffset>=sizeof(RevBuf))
		{
			if (RevBuf.Len)
			{
				pbuf = (unsigned char *)malloc(RevBuf.Len);
				if (pbuf)
				{
					memset(pbuf, 0, RevBuf.Len);
					memcpy_s(pbuf, RevBuf.Len, RevBuf.Data, RevBuf.Len);
					Sleep(1);
					PostThreadMessage(dwRevDataThreadID,WM_TESTPERF_DATA,(unsigned int)pbuf, (unsigned long)RevBuf.Len);
				}				
			}
			RevOffset=0;
			memset((void *)&RevBuf, 0, sizeof(RevBuf));
		}
	}		

	return;
}

///////////////////////////////////////////////////////////////////////////////
// OnEvent
// Send message to parent window to indicate connection status
void CAmTesterIO::OnEvent(UINT uEvent, LPVOID lpvData)
{

	if (dwRunEventThreadID)
	{
		PostThreadMessage(dwRunEventThreadID,WM_TESTPERF_DATA,0,0);
	}
	return;
}


void CAmTesterIO::SetRevDataThreadID(unsigned int id)
{
	dwRevDataThreadID=id;
	return;
}


void CAmTesterIO::SetRevEventThreadID(unsigned int id)
{
	dwRunEventThreadID=id;
	return;
}

DWORD CAmTesterIO::AmSend(unsigned char *pBuf, DWORD dwSize)
{

	AM_SEND_BUF SendBuf;
	unsigned char SendTemp[4000];
	memset((void *)&SendBuf,0, sizeof(SendBuf));
	memset((void *)&SendTemp,0, sizeof(SendTemp));
	if (pBuf)
	{
		if (ConnectFlag)
		{
			if (IsOneClientConnect())
			{	
				SendBuf.Len=dwSize;
				memcpy_s(SendBuf.Data, sizeof(SendBuf.Data), pBuf, dwSize);
				memcpy_s(SendTemp,sizeof(SendTemp),(void*)&SendBuf, sizeof(SendBuf));
				return (WriteComm(SendTemp, sizeof(SendBuf), 1000));//time out 1 second
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 0;
}