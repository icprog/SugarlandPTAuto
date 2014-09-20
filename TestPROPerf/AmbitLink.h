/********************************************************************
created:	2007/02/06
filename: 	..\TestPROPerf\TestPROPerf\AmbitLink.h
file path:	..\TestPROPerf\TestPROPerf
file base:	TestPROPerf
file ext:	h
author:		Jeffrey

purpose:	verify the network link status
*********************************************************************/
#ifndef AMBIT_LINK_H
#define AMBIT_LINK_H


int VerifyLink(char *pDesIP, int PingTime);
int WaitBootUp(char *pDesIP, int PingTime);
int BonjourLink(char *pCmd, int TimeOut);
int VerifyLinkUsePing(char *pDesIP, int PingTime);//add by Talen 2011/04/19





#endif