#ifndef DEVICE_DEFINE_H
#define DEVICE_DEFINE_H

#ifndef DEV_IO_REMOTE
#define WM_SRQ_QUIT WM_USER + 619

typedef struct DeviceParaTag
{
	char DeviceName[100];//record the device name
	char OpFlag[5];
	char DevVersion[100];//device hardware version and software version
	char TitleVersion[100];//Maxwell
	char Address[50];//device ID address
	char Interface[50];//communication interface
	char Speed[50];//communication port speed
	char PreSet[500];//default parameter set for Device

	HANDLE hDevMutex;
}DEVICE_PARA;
#endif

#define MA_READ  2
#define MA_WRITE 1
#define MA_DATA_LEN 1000
#define QUERY_MODE 0
#define SRQ_MODE 1
#define LINK_RESPONSE 2

typedef int (__stdcall *SRQCallback)(char *p,unsigned int Len);

typedef struct DevOpCMDTag
{   	
	char DeviceName[50];//record the device name 		
	char DevVersion[30];//device hardware version and software version
	char TitleVersion[32];//Title version add Maxwell 100223
	char Product[20];//UUT product name, ex:M52,K10
	char TestSta[20];//Test station name,ex,FT1,PT4
	char PN[20];//test UUT part number,ex,U01H465.00
	int  W_R_flag;//data operation mode, 1 index we write something and 2 index we read something form IO port.
	unsigned int PackSeq;//DATA packet sequence
	unsigned int TotalLen;//total data packet size 
	unsigned char Data[MA_DATA_LEN];//carry the data
	unsigned int len;//transfer data length,the value must be < 1000 byte
	int  OpMode;//index the operation mode 0: query mode 1:SRQ(Service Requirement Query)Mode 
	int  ExitFlag;//we use this flag in SRQ mode, when this flag set to 0,the sevice thread will be exit.
}DEV_OP_CMD;

#endif