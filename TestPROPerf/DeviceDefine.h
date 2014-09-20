#ifndef DEVICE_DEFINE_H
#define DEVICE_DEFINE_H
#define DEV_IO_REMOTE
#ifndef DEV_IO_REMOTE
#include "com_class.h"
#include "InstrumentCTRL.h"

typedef struct DeviceParaTag
{
	char DeviceName[100];//record the device name
	char OpFlag[5];
	char DevVersion[100];//device hardware version and software version
	char Address[50];//device ID address
	char Interface[50];//communication interface
	char Speed[50];//communication port speed
	char PreSet[500];//default parameter set for Device

	HANDLE hDevMutex;
	_asyn_com *pComDev;
	CInstrumentCTRL *pGPIBDev;

}DEVICE_PARA;
#endif

#define MA_READ  2
#define MA_WRITE 1
#define MA_DATA_LEN 1000
typedef struct DevOpCMDTag
{   	
	char DeviceName[100];//record the device name 	
	char Interface[50];//communication interface
	char Address[50];//device ID address
	char DevVersion[100];//device hardware version and software version
	int  W_R_flag;//data operation mode, 1 index we write something and 2 index we read something form IO port.
	unsigned char Data[MA_DATA_LEN];//carry the data
	unsigned int len;//transfer data length,the value must be < 1000 byte
	int  OpMode;//index the operation mode 0: query mode 1:SRQ(Service Requirement Query)Mode 
	int  ExitFlag;//we use this flag in SRQ mode, when this flag set to 0,the sevice thread will be exit.
}DEV_OP_CMD;



#endif