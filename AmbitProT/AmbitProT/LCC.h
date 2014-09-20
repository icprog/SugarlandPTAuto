// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PTSC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PTSC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef LCC_H
#define LCC_H
#ifdef LCC_EXPORTS
#define LCC_API  extern "C" __declspec(dllexport)
#else
#define LCC_API  extern "C" __declspec(dllimport)
#endif

typedef struct InitInfoTag
{
	char Ip[100];//PTS server IP address
	char ComName[100];//test computer name
	char DaigVersion[100];//test program version
	char Product[100];//UUT product name, ex:M52,K10
	char TestStaName[100];//Test station name,ex,FT1,PT4
	char ProductPN[100];//test UUT part number,ex,U01H465.00
}INIT_INFO;

//Set a connection to PTS,if operation successful return 1,else return 0
LCC_API int LCC_Connect(void **pInstance,INIT_INFO *pInit);

//Disconnect with PTS,no return value
LCC_API void LCC_Disconnect(void *pInstance);

//Get the link status with PTS,call this function before send data to PTS
//If connection availble,return value 1,else return 0
LCC_API int LCC_GetLinkStatus(void *pInstance);

//Set record data to the send buffer, if operation successful return 1,else return 0
//input:char *p data buffer,unsigned int len:data size,unsigned int Seq:data sequence,shoud be 0,1,2....
LCC_API int LCC_SetSendData(void *pInstance,char *p, unsigned int len,unsigned int Seq);

//perform actual send operate after you set the data buffer
//if operation successful return 1,else return 0
LCC_API int LCC_Send(void *pInstance);

//Callback function formart define
typedef int (__stdcall *SRQCallback)(char *p,unsigned int Len);
//Regist call back function,when PTSC recive the data from PTS
//if operation successful return 1,else return 0
LCC_API int LCC_RegOnRev(void *pInstance,SRQCallback pSRQCallback);
#endif