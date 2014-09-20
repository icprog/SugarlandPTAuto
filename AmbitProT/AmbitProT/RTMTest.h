// RTMTest.h : main header file for the RTMTest DLL
//


//------------------------------------------------------------------------
//
//MFC DLL.The DLL is mainly send data to RTM system.The interface function is SendRTM.
//
//------------------------------------------------------------------------

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "winsock2.h " 
#pragma comment(lib,"WS2_32.LIB ")


//------------------------------------------------------------------------
//  Send data to RTM function.Parameter str is the data from UI,and the data
//  will be packed in XML file in WriteXml function,and then send in this function.
//  Parameter strItem is the Item name you want add.Parameter is the Item value
//  corresponding to the Item you add.
//------------------------------------------------------------------------
int SendRTM(char *str,int TaskID,char *strItem,char *strValue);

//------------------------------------------------------------------------
//  Put the string str to XML file function.The function uses class library
//  to make XML file.Parameter strItem is the Item name you want add.Parameter is
//  the Item value corresponding to the Item you add.
//------------------------------------------------------------------------
int WriteXml(char *str,int TaskID,char *strItem,char *strValue);

//------------------------------------------------------------------------
//  Get substring function.The function will return string from pStart in
//  str to pEnd in str.The function mainly used in WriteXml file to get key 
//  data and then make node or element of XML file.
//------------------------------------------------------------------------
CString GetString(char *str,char pStart,char pEnd);

//------------------------------------------------------------------------
//  Get file version function.The function uses class library to get software
//  version,such as acp.exe and son.
//------------------------------------------------------------------------
CString FileVersion(CString str);

//------------------------------------------------------------------------
//  Calculate YieldRate of the latest 20 piece boards.
//------------------------------------------------------------------------
void YieldRate(int TaskID);

//------------------------------------------------------------------------
//  If one or more error-code occurs more than 3 times,it will send error-warning
//  to RTM system.Parameter str is failed error-code.
//------------------------------------------------------------------------
CString ErrorCodeWarning(CString str,int TaskID);

//------------------------------------------------------------------------
//  The function mainly accounts total utilization of test PC.If the quotation is 
//  closed,it will write test time to the current directory file.Parameter InitialFlag
//  1 for Open Quotation read the 
//------------------------------------------------------------------------
void WriteTime(int InitialFlag,long *testT,long *testT1,long *totalT);

//------------------------------------------------------------------------
//  Change wide-char data to multi-byte data.The function is similar as
//  WideCharToMulitByte();
//------------------------------------------------------------------------
void MyWideCharToMultiByte(WCHAR* wchars,CHAR* schars,int scharsLen);


//------------------------------------------------------------------------
//  struct FileVer,accounts the software for Dns-sd,ACP,ARP and so on.
//------------------------------------------------------------------------
typedef struct FileVer
{
	CString UIVer;
	CString TestProPerfVer;
	CString DnsVer;
	CString AcpVer;
	CString ArpVer;
}FILE_VER;

//------------------------------------------------------------------------
//  struct TestTimeCount,accounts passed-time,failed-time and YieldRate.
//------------------------------------------------------------------------
typedef struct TestTimeCount
{
	int PassedTimes;
	int FailedTimes;
	int TotalTimes;	
	double YieldRate;
	bool status[21];
}TEST_TIME;

//------------------------------------------------------------------------
//  struct ErrorCode,used for account the error-code that continually occur
//  more than three times.
//------------------------------------------------------------------------
typedef struct ErrorCode
{
	CString Error;
	int number;
}ERR_CODE;

