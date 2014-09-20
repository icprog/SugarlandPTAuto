#include "StdAfx.h"
#include "CenTime.h"

CCenTime::CCenTime(void)
{
}

CCenTime::~CCenTime(void)
{
}
void CCenTime::GenCurrentTime(char *timebuf, int Len)
{

    char tmpbuf[128];
	if(!timebuf) 
	{
		return;
	}
    // Set time zone from TZ environment variable. If TZ is not set,
    // the operating system is queried to obtain the default value 
    // for the variable. 
    //
    _tzset();

    // Display operating system-style date and time. 
    _strtime_s( tmpbuf, 128 );
	strcpy_s(timebuf, Len, tmpbuf);

	return;
}
void CCenTime::TimeStartCount()
{

    Start_t=clock();
	return;
}
void CCenTime::GetElapseTime(char *prRtn,int Len)
{
	clock_t Duration=0;
	int Hours=0;
	int Min=0;
	int Seconds=0;
	int MSeconds=0;
	int Temp=0;
	char TimeStrBuf[100];

	Current_t=clock();
	Duration=Current_t-Start_t;
	
	if (Duration<0)
	{
		Duration=(2147483648-Start_t+Current_t);
	}

	Hours=Duration/(60*60*CLOCKS_PER_SEC);
	Temp=Duration%(60*60*CLOCKS_PER_SEC);
	Min=Temp/(60*CLOCKS_PER_SEC);	
	Temp=Temp%(60*CLOCKS_PER_SEC);
	Seconds=Temp/CLOCKS_PER_SEC;
	MSeconds=Temp%CLOCKS_PER_SEC;

	sprintf_s(TimeStrBuf,sizeof(TimeStrBuf),"%.2d:%.2d:%.2d.%.3d",Hours,Min,Seconds,MSeconds);
	strcpy_s(prRtn,Len,TimeStrBuf);
	return;
}
void CCenTime::GetDate(char *timebuf, int Len)
{

    char tmpbuf[128];
	if(!timebuf) 
	{
		return;
	}
    // Set time zone from TZ environment variable. If TZ is not set,
    // the operating system is queried to obtain the default value 
    // for the variable. 
    //
    _tzset();

    // Display operating system-style date and time. 
    _strdate_s( tmpbuf, 128 );

	tmpbuf[8]=tmpbuf[6];
	tmpbuf[9]=tmpbuf[7];
	tmpbuf[6]='2';
	tmpbuf[7]='0';
	tmpbuf[10]=0;

	strcpy_s(timebuf, Len, tmpbuf);

	return;
}

void CCenTime::DataFormatC(char *timebuf, int Len, char *pIn)
{

    char Otmpbuf[128];
	char Ntmpbuf[128];
	
	if((!timebuf) || (!pIn)) 
	{
		return;
	}

	// 06/17/2008
    // Display operating system-style date and time. 
    strcpy_s(Otmpbuf, sizeof(Otmpbuf),pIn);

	Ntmpbuf[0]=Otmpbuf[6];
	Ntmpbuf[1]=Otmpbuf[7];
	Ntmpbuf[2]=Otmpbuf[8];
	Ntmpbuf[3]=Otmpbuf[9];
	Ntmpbuf[4]=Otmpbuf[0];
	Ntmpbuf[5]=Otmpbuf[1];
	Ntmpbuf[6]=Otmpbuf[3];
	Ntmpbuf[7]=Otmpbuf[4];
	Ntmpbuf[8]=0;
	strcpy_s(timebuf, Len, Ntmpbuf);

	return;
}

void CCenTime::CostTimeConvert(char *timebuf,int timebufLen,char *pIn)
{
	char Ntmpbuf[128];
	
	if((!timebuf) || (!pIn)) 
	{
		return;
	}
	memset(Ntmpbuf,0,sizeof(Ntmpbuf));
	strcpy_s(Ntmpbuf,sizeof(Ntmpbuf),pIn);
	char *pToke=Ntmpbuf;
	float secondCount=0;

	int count=0;
	for(count=0;count<3;count++)
	{
		secondCount=secondCount*60+atof(pToke);
		pToke=pToke+3;
	}

	memset(timebuf,0,timebufLen);
	sprintf_s(timebuf,timebufLen,"%.3f",secondCount);

	return;
}