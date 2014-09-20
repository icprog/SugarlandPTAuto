#ifndef AM_COMMON_IO_H
#define AM_COMMON_IO_H



#define GUI_IO
#include "AClientSockets.h"
#include   <stdio.h>
#include   <dos.h>
// 
long amprintf
    (
    const char * format,
    ...
    );

int LogPrint(int PrintMode);   //print test result to log file

#endif