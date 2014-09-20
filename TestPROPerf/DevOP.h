// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DEVOP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DEVOP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DEVOP_EXPORTS
#define DEVOP_API extern "C" __declspec(dllexport)
#else
#define DEVOP_API extern "C" __declspec(dllimport)
#endif


#define QUERY_MODE 0
#define SRQ_MODE   1
typedef int (_stdcall *SRQCallback)(unsigned char *p,UINT Len);



//----------------------------------------------------------
//libraly initialization and Uninitialization
DEVOP_API int OpenDev(void** pDeviceOP,char *pMAName="DEVICE", int iMode=QUERY_MODE, char *pIP="127.0.0.1",int port=10265);
DEVOP_API void CloseDev(void *pDeviceOP);
DEVOP_API int DevQuery(void *pDeviceOP,char *pcmd,char *pRtn, UINT Len);

//-----------------------------------------------------------
//Query mode operation 
DEVOP_API int DevWrite(void *pDeviceOP,char *pcmd,UINT Len);
DEVOP_API int DevRead(void *pDeviceOP,char *pcmd,UINT Len);


//-----------------------------------------------------------
//SRQ mode operation
DEVOP_API int DevSRQWrite(void *pDeviceOP,char *pcmd,UINT Len);
DEVOP_API int DevSRQRegCallback(void *pDeviceOP,SRQCallback pSRQCallback);
