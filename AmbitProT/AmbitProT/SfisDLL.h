
#ifdef SFISDLL_EXPORTS
#define SFIS_API extern "C" __declspec(dllexport)
#else
#define SFIS_API extern "C" __declspec(dllimport)
#endif


/*----------------------------------------------------------------------
Call back function. Print the data received from sfis. 
The parameter 'flag' represents the port from which the data is received.
-----'1': from COM port
     '2': from keyboard port
The data received is stored in parameter 'p'.
The parameter 'Len' represents the length of data received.
------------------------------------------------------------------------*/
typedef int (_stdcall *SRQSFISCallback)(int flag,char *p,unsigned int Len);


/*--------------------------------------------------------------------------
Open server functon. Open the server communicating with sfis.
The parameter 'pDeviceOP' point to the address of a object of class 'Sfis' packing the server communicating with sfis.
The parameter ' pSRQCallback' is the call back function defined above.
-------------------------------------------------------------------------*/
SFIS_API int SfisOpen(void** pDeviceOP,SRQSFISCallback pSRQCallback);


/*--------------------------------------------------------------------------
Close server function. Close the server.
The parameter 'pDeviceOP' point to a object of class 'Sfis' packing the server communicating with sfis.
--------------------------------------------------------------------------*/
SFIS_API void SfisClose(void *pDeviceOP);


/*------------------------------------------------------------------------
Send data function. Send data to Sfis.
The parameter 'pDeviceOP' point to a object of class 'Sfis' packing the server communicating with sfis.
The data need to send is stored in the parameter 'pcmd'.
The error code returned is stored in 'pRtn'.
The parameter 'Len' represents the length of data need to send.
The parameter 'flag' represents the port from which the data is sent.
-----'1': from COM port
     '2': from keyboard port
---------------------------------------------------------------------------*/
SFIS_API int SfisSend(void *pDeviceOP,char *pcmd,char *pRtn, unsigned int  Len,int flag);

