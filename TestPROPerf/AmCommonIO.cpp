
#include "stdafx.h"
#include "AmCommonIO.h"
#include "AClientSockets.h"
#include "AmbitTestSysDef.h"
#include "shlwapi.h"

char    DetailLogBuffer[128000]="";//Maxwell 11228

extern CLIENT_TYPE AmPROUI;

//Maxwell 090610
extern HANDLE hMsgSendMutex;

/**************************************************************************
* uiPrintf - print to the perl console
*
* This routine is the equivalent of printf.  It is used such that logging
* capabilities can be added.
*
* RETURNS: same as printf.  Number of characters printed
*/
long amprintf
    (
    const char * format,
    ...
    )
{
    va_list argList;
    long     retval = 0;
	char    buffer[3000];
    //char    buffer[4096];
	//Maxwell 090520
	SOC_BUF_SEND dataSend;
	memset(buffer , 0, sizeof(buffer));
	memset(dataSend.Data , 0, sizeof(dataSend.Data));

    /*if have logging turned on then can also write to a file if needed */

    /* get the arguement list */
    va_start(argList, format);

    /* using vprintf to perform the printing it is the same is printf, only
     * it takes a va_list or arguments
     */
    retval = vprintf(format, argList);
    fflush(stdout);

    /*if (logging) {
        
        fputs(buffer, logFile);
		fflush(logFile);
    }*/

	//if the GUI flag is be define,the message will be print to GUI process
#ifdef GUI_IO
	Sleep(1);
	//vsprintf_s(buffer, sizeof(buffer), format, argList);
	vsprintf_s(dataSend.Data, sizeof(dataSend.Data), format, argList);
	dataSend.Len = (int)(strlen(dataSend.Data));
	int m = (int)(sizeof(dataSend));
	WaitForSingleObject(hMsgSendMutex, INFINITE); 
	Sleep(1);
    AmbitSend(&AmPROUI,(char*)&dataSend,(int)(sizeof(dataSend)));

	//Maxwell 11228
	strcat_s(DetailLogBuffer,sizeof(DetailLogBuffer),dataSend.Data);
	//Maxwell 11228

	ReleaseMutex(hMsgSendMutex);
	//AmbitSend(&AmPROUI,buffer,(int)(strlen(buffer)));
#endif
    
	va_end(argList);    /* cleanup arg list */
    return(retval);
}

int LogPrint(int PrintMode)
{		
	
	switch(PrintMode)
	{
	case 1:		
		break;
	case 2:
		break;
	default:
		break;
	}
	
	return 1;

}

