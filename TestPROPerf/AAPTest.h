/********************************************************************
created:	2007/02/06
filename: 	..\TestPROPerf\TestPROPerf\AAPTest.h
file path:	..\TestPROPerf\TestPROPerf
file base:	TestPROPerf
file ext:	h
author:		Jeffrey

purpose:	do the act test in process
*********************************************************************/
#ifndef A_AP_TEST_H
#define A_AP_TEST_H



int __stdcall ItemTest(TEST_ITEM *pTI);
int ParseTestItem(const char *p1, const char *p2);
int InstrumentInitial();
int IperfInitial();//Add by Maxwell 2008/12/15 for open iperf in FT1
int THGoldenInitial();//Add by Maxwell 2009/07/28 for set MCS of Golden in FT1
int THGoldenReboot();//Add by Maxwell 2009/10/05 for reboot Golden in FT1
int UUTIOInitial();
int DeleteAFile(char *FileName);//add by Talen 2011/09/05
int MultiInitial();//add by Talen 2011/09/01
int CCDInitial();
int VitestInitial();//haibin 2011/08.01
void VitestUnInitial();//haibin 2011/08.01
void InstrumentUnInitial();
void IperfUnInitial();//Add by Maxwell 2008/12/15 for close iperf in FT1
void UUTIOUnInitial();
void CCDUnInitial();
void IQUnInitial();//Maxwell 20110326
int RunNCart();;//Add by Maxwell 20110326 for run Nart&Cart

int BinFolderInitial();  //Add by liu-chen for K31 PT2 "manufactruring_flow" and "path_loss" down load.
int sendConfigToUI();//Talen 2011/11/01
int K31printInitial();//add by Liu-Chen for k31 Wip Label 2012/04/11
void K31printUnInitial();//add by Liu-Chen for k31 Wip Label 2012/04/11

#endif