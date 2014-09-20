/*

	Contains:	Ambit pro test UI data structure define

	Version:	xxx 002 xxx

	Copyright:	(C) 2007 by Ambit Micro, Inc., all rights reserved.

    Technology:			TestproT test platform

	Change History (most recent first):

	Create by Jeffrey Chou 2007/2/16

    2007/2/19 add the AmbitProTConfigTag data structure, Jeffrey Chou
	2009/4/22 Modify for fit new AmbitProT, Jeffrey Chou
	2009/5/11 Redefine support unicode, Jeffrey Chou

*/

#ifndef AMBIT_PROT_SYS_DEFINE_H
#define AMBIT_PROT_SYS_DEFINE_H

//UUT test statistic information
typedef struct UUTTestCounterTag
{
	unsigned int PassCount;
	unsigned int FailCount;
	unsigned int Total;
	float        YieldRate;
}UUT_TEST_COUNTER;


// UUT informations about product line
 typedef struct UUTInfoTag
 {
	 wchar_t UUTName[512];
	 wchar_t UUTPN[128];
	 wchar_t UUTMO[256];
	 wchar_t UUTSN[256];
	 UUT_TEST_COUNTER UUTCounter;
 }UUT_INFO;


 // Tester station informations
 typedef struct TestStationInfoTag
 {
	 wchar_t Station_Name[256];
	 wchar_t TestStaSN[128];
	 wchar_t TestStaID[128];
	 wchar_t TestProgramVersion[256];
	 wchar_t UIVersion[256];
 }TEST_STATION_INFO;


 //Test PC information statics
 typedef struct PcInformationTag
 {
	 wchar_t HostName[256];
	 wchar_t IPAddress[20];
 }PC_INFO;


 //Operation information
 typedef struct OperatorInformationTag
 {
	 wchar_t EmployeID[100];

 }OPER_INFO;

//SFIS information 
 typedef struct SfisInformationTag
 {
	 int SfisON_OFF;
	 wchar_t ServerIP[20];
	 wchar_t SfisStationName[50];

 }SFIS_INFO;

 //MYDAS connection information
 typedef struct PtsInformationTag
 {

	 int PtsSwitch;//indes PTS ON or OFF
	 wchar_t PtsServerIP[20];
	 wchar_t PtsTitleVer[128];//Maxwell 100311
	 unsigned int RecordNum;
	 int ConnectStatus;
	 wchar_t RecordData[8096];//Maxwell 100316
	 wchar_t ErrorData[1024];//Maxwell 100310 Error information
	 wchar_t MainInforData[1024];//Maxwell 100316 Main information
	 wchar_t *pLogData;

 }PTS_INFO;

//define mfg management system connecting status
 typedef struct MfgManagementInfoTag
 {
	 int MfgManaON_OFF;
	 wchar_t MfgManaServerIP[20];

 }MFG_MANA_INFO;

 //define UI display behavior
 typedef struct UIDisplayTag
 {
	 int StandByTime;
	 int TPtimeout;
	 int SfisTimeout;
	 int RepeatTime;
 }UI_DISPLAY;


 //define multi-task parameter
 typedef struct MultiTaskTag
 {
	 int Number;
	 wchar_t ControlMethod[100];
	 wchar_t UIServerIP[50];
	 int PortStart;
 }MULTI_TASK_SETTING;

  //define Server1 parameter
 typedef struct Server1Parameter
 {
	 int Enable;//Maxwell 100311
	 int Port;
	 wchar_t IP[100];
	 wchar_t ClientNumber[50];
 }SERVER1_CLIENT_SETTING;


  // Equipment usage info
 typedef struct EqipInfoTag
 {
	 wchar_t EqipStation[20];//Equipment usage log path
	 wchar_t EqipLine[20];//Equipment usage log path
	wchar_t EqipServerIP[20];//Equipment usage log IP
	wchar_t EqipLogPath[200];//Equipment usage log path
	wchar_t EqipOperPath[200];//Equipment usage operation log path
	int EqipXmlFlag;
    int EqipCtlFlag;// 1 is control and 0 is not control 
	int EqipEnableFlag;//1 is enable and 0 is not enable
	float EqipAlarmRate;//Equipment alarm rate
 }EQIP_INFO;

 //All system information define
 typedef struct AmProTInfoTag
 {
	 UUT_INFO uut;
	 TEST_STATION_INFO TestStation;
	 PC_INFO PcInfo;
	 SFIS_INFO SfisInfo;
	 OPER_INFO Operator;
	 PTS_INFO PtsInfo;
	 MFG_MANA_INFO MfgManaInfo;
	 UI_DISPLAY UiDisplay;
	 MULTI_TASK_SETTING MultiTaskSetting;
	 EQIP_INFO EquipInfo;
	 SERVER1_CLIENT_SETTING Server1Client;
 }AM_PROT_INFO;



// data format define, use in right-list show the Item detail information
typedef struct TestItemTag
{
	int ItemNum;
	int ID;// index the item show sequence
	wchar_t Name[1024];
	wchar_t Val[512];
	wchar_t Spec[512];
	wchar_t Result[20];
    wchar_t Description[512];
	wchar_t ItemTestTime[32];
}TEST_ITEM;


 // SFIC system info
 typedef struct SFICInfoTag
 {
    int SFICSwitchFlag;// 1 is on and 0 is off ,2 is virtual on
	int SFICMode;//sfic mode index, 0 index sfic os off, if val > 0, index the sfic mode num
	int SFICComNum;// index the sfic use com number.
	wchar_t SMOData[500];//SMO DATA from DCT
	wchar_t TesterData[500];//from tester

 }SFIC_INFO;



 //Test result
 typedef struct TestResultInfoTag
 {
	 int result;
	 wchar_t Content[128];
	 wchar_t ErrorCode[512];

 }TEST_RESULT;

  typedef struct g_G_variable
 {
	 int StandByTime;
	 int Show_All_MSG;
	 int Repeat_Time_Flag;
	 char UIVer[64];
	 int ShowTime;
	 char PN[32];
	 char gDirectory[256];
	 int TPtimeout;
	 int SMOConnFlag;
	 char TESerPNStaPath[512];
	 char TEquipPath[512];
	 char PDBS_IP[128];
	 int PDBS_FLAG;
     int  SendtoPDBSFlag;
	 int PDBS_item_sum;//add by maxwell 2008/09/23
	 int EQUIP_CONTROL_FLAG;//Add by Maxwell 2008/11/11
	 float EQUIP_ALARM_RATE;//Add by Maxwell 2008/11/11
	 int UutScannerFlag;
	 int virtualflag;//haibin.li 2011/12/5
 }G_VARIABLE;

   //090611
 typedef struct EquipmentInfoTag
{
	wchar_t FixCountName[100];
	wchar_t EquipmentName[100];
	int Usedtime;
	//char DeviceInfo[1000];
	int Maxusetime;

}EQUIP_USE; //090611

//haibin.li 2011/11/09
 typedef struct UserInformation
 {
	 CString name;
	 CString password;
 }USER_INFO;
 //haibin.li 2011/11/09

 //haibin.li 2011/12/1
 typedef struct Utilization
 {
	 long TestTime;
	 long TotalTime;
	 double Uti;
 }UI_UTI;
 //haibin.li 2011/12/1

 //haibin.li 2012/1/18
 typedef struct Priority
 {
	 CString UserId[100];
	 int num;
 }USER_PRIO;
 //haibin.li 2012/1/18

#endif