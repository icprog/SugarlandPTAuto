#ifndef PTSC_H
#define PTSC_H
typedef struct InitInfoTag
{
	char Ip[100];//PTS server IP address
	char ComName[100];//test computer name
	char DaigVersion[100];//test program version
	char TitleVersion[100];//Maxwell 100311
	char Product[100];//UUT product name, ex:M52,K10
	char TestStaName[100];//Test station name,ex,FT1,PT4
	char ProductPN[100];//test UUT part number,ex,U01H465.00
}INIT_INFO;

//Set a connection to PTS,if operation successful return 1,else return 0
int PTSC_Connect(void **pInstance,INIT_INFO *pInit);

//Disconnect with PTS,no return value
void PTSC_Disconnect(void *pInstance);

//Get the link status with PTS,call this function before send data to PTS
//If connection available,return value 1,else return 0
int PTSC_GetLinkStatus(void *pInstance);

//Set record data to the send buffer, if operation successful return 1,else return 0
//input:char *p data buffer,unsigned int len:data size,unsigned int Seq:data sequence,shoud be 0,1,2....
int PTSC_SetSendData(void *pInstance,char *p, unsigned int len,unsigned int Seq);

//perform actual send operate after you set the data buffer
//if operation successful return 1,else return 0
int PTSC_Send(void *pInstance);

//Callback function formart define
typedef int (__stdcall *SRQCallback)(char *p,unsigned int Len);
//Regist call back function,when PTSC recive the data from PTS
//if operation successful return 1,else return 0
int PTSC_RegOnRev(void *pInstance,SRQCallback pSRQCallback);

#endif