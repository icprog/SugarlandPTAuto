
#include "stdafx.h"
#include "FixtureOperation.h"

_asyn_com cFixtureComDev;

static int COM_PORT = 2;
static int COM_RATE = 9600;




int closeFixture()
{
	char CmdBuf[128]="";						
	if(!cFixtureComDev.open(COM_PORT, COM_RATE))
	{
		return FAILED_MODE.OpenFailed;
	}
	Sleep(50);

	char cmd[500]="";
	strcpy_s(cmd,sizeof(cmd),"close#013");
	
	char revbuf[8192]="";

	if( cFixtureComDev.write(cmd, (int)strlen(cmd)) == 0)//write data failed
	{
		cFixtureComDev.close();
		return FAILED_MODE.WriteFailed;
	}

	Sleep(500);
	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	int revbufLen = 0;
	while((revbufLen==0) && (i<100))
	{
		cFixtureComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(100 == i)
	{
		cFixtureComDev.close();
		return FAILED_MODE.RecTimeOut;
	}

	if (strstr(revbuf,"Tester ready#013"))
	{
		cFixtureComDev.close();
		return FAILED_MODE._PASS_;
	}
	else
	{		
		cFixtureComDev.close();
		return FAILED_MODE.ResponseError;
	}
}

int openFixture()
{
	char CmdBuf[128]="";						
	if(!cFixtureComDev.open(COM_PORT, COM_RATE))
	{
		return FAILED_MODE.OpenFailed;
	}
	Sleep(50);

	char cmd[500]="";
	strcpy_s(cmd,sizeof(cmd),"open#013");
	
	char revbuf[8192]="";

	if( cFixtureComDev.write(cmd, (int)strlen(cmd)) == 0)//write data failed
	{
		cFixtureComDev.close();
		return FAILED_MODE.WriteFailed;

	}

	Sleep(500);
	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	int revbufLen = 0;
	while((revbufLen==0) && (i<100))
	{
		cFixtureComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(100 == i)
	{
		cFixtureComDev.close();
		return FAILED_MODE.RecTimeOut;
	}

	if (strstr(revbuf,"Tester reset#013"))
	{
		cFixtureComDev.close();
		return FAILED_MODE._PASS_;
	}
	else
	{
		cFixtureComDev.close();
		return FAILED_MODE.ResponseError;
	}
}