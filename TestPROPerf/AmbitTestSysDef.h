
#ifndef AMBIT_TEST_SYS_DEF_H
#define AMBIT_TEST_SYS_DEF_H
#include "ATestIO.h"

/*the version define rule:
Version: A.B.C.D
A. main version number (Apple - 1, Netgear - 2, Thomson - 3)
B. large version number(PT - 10, FT - 20, RC - 30)
C.version number(intial - 101) when little version increase 10,update the version number
D.little version number ()   when modify the code more then 10 then modify it.
tag. a - demo, b - demo range,NULL - release 
for example
Apple project    1.20.101.0
Netgear project  2.30.101.0
Cisco            3.30.101.0
									*/
/*
#define SOFTWARE_VERSION	"3.10.101.0"
#define SOFTWARE_CONTROL ".6"     //For 
#define SOFTWARE_BIG_VERSION "1."   //For FT2
#define STATION_TYPE "FT2"// For FT2*/


//*
//#define SOFTWARE_VERSION	"3.10.102.19"////Sam 20110316
//#define SOFTWARE_VERSION	"3.10.102.23"	//LiYongjun 20110318
//#define SOFTWARE_VERSION	"3.10.102.24"	//Talen 20110615(Pre-EVT)
//#define SOFTWARE_VERSION	"3.10.102.25"	//Talen 20110702(EVT)
//#define SOFTWARE_VERSION	"3.10.102.26"  //haibin 08.03
//#define SOFTWARE_VERSION	"3.10.102.27"  //Talen 2012/02/16
//#define SOFTWARE_VERSION	"3.10.102.28"  //FSB build,Talen 2012/02/20(+delta time between check local temp first and second time)
//#define SOFTWARE_VERSION	"3.10.102.29"  
	/*Talen 2012/03/08(+Retry status[0--pass and no retry;1--pass and retry 1 time;2--pass and retry 2 times;3--retry fail];
																+send SN info to Audio test server;
																+change erro code to TO99 if no return from Audio test server;
																+click log;
																+erase cal data before copy art to DUT;
																+recode bonjour time;
																+recode every item test time;*/
//#define SOFTWARE_VERSION	"3.10.102.30"  //Talen 2012/03/13(+record retry item count(reset button,link status,audio,USB))
//#define SOFTWARE_VERSION	"3.10.102.31"	//Talen (+check SoundloopBackX version)
//#define SOFTWARE_VERSION	"3.10.102.32"	//Talen (delete something about retry)
#define SOFTWARE_VERSION	"3.10.102.33"	//Liu-Chen (add Wip Lable print on station Hi-Pot)


#define SOFTWARE_CONTROL ".10"	
#define SOFTWARE_BIG_VERSION "10." 
#define STATION_TYPE "ATE"
#define UUT_NAME "APPLE"

#define AM_UI_IP "127.0.0.1"// use com with UI
#define AM_UI_PORT  10000    // use com with UI
#define IMAGE_PRO_PORT 12638// use com with ImagePro
#define SFISUseNum 10    //090613 for sfis use number in test item




// UUT informations 
typedef struct UUTInfoTag
{
	char UUTName[512];
	char UUTMO[256];
	char UUTSN[256];
	char UUTMAC1[256];
	char UUTMAC2[256];
	char UUTMAC3[256];
	char HDAPPLESN[256];//add by Talen 2011/05/19
	char TestTime[64];
	char TestTimeCost[64];
	char TestDate[64];
	char MsgRes[256];	
	char UUTPN[128];
	char LocalTemp1[64];
	char RemoteTemp1[64];
	char RemoteTemp2[64];
	char RemoteTemp3[64];
	int TotalResultFlag;//Talen 2011/08/30
	int LedBlink;
//	int retryItemCount;
}UUT_INFO;

//haibin 08.03 for visual inspection on logo
typedef struct LOGOInformation
{
	char OutBuf[512];	
	double X_OFFSET;
	double Y_OFFSET;
	double ROTATE_ANGLE;
	double AVG_HIST;
	double CCORR;
	double TOTAL_AREA;
	double MAX_AREA;
	double MAX_PERIMETER;
	double SQUARE_WIDTH;
	double SQUARE_HEIGHT;
	char LogoSN[48];
}LOGO_INFO;
//haibin 08.03

//090613 Used for test items in test
typedef struct SFISUseTag
{
	//test item ID define
	char Item[128];// index the item show sequence
	char Data[512];
}SFIS_USE;
//090613

//Maxwell 20110329
typedef struct GoldenServerStruct
{
	int connectFlag;
	char connectIP[128];
	char connectPORT[128];
}GOLDEN_SERVER_PARA;
//Maxwell 20110329

//Maxwell 20110329
typedef struct ControlBoardStruct
{
	int ConnectFlag;
	int LedShowFlag;
}CON_BOARD_PARA;
//Maxwell 20110329

// Tester station informations
typedef struct TestStationInfoTag
{
	char TestStaName[256];
	int  TestStaSN;
	int  TestStaID;
	int  Golden5GCom; //Add by Maxwell 2009/07/28
	int  Golden2GCom; //Add by Maxwell 2009/07/28
	int	 GoldenComTransRate;//Add by Maxwell 2009/07/28
	char TestProgramVersion[256];
	char UIVersion[100];
	char ManufactureLine[50];
	char Sector[50];
	char Post[50];
	char Area[50];
	char TesterPCName[100];
	char TesterPCIP[50];
	char LogFileType[50];
	char Operator[50];
	char MEAutoFlag[10];
	char CCDAutoFlag[10];//Add by Maxwell 2009/10/27
	char VitestFlag[10];//haibin 2011/08/01
	char IperfAutoFlag[10];//Add by Maxwell 2008/12/15
	char GoldenSetMCSFlag[10];//Add by Maxwell 2009/07/28
	char GoldenRebootFlag[10];//Add by Maxwell 2009/10/06
	char IperfBat1[100];//Add by Maxwell 2008/12/16
	char IperfBat2[100];//Add by Maxwell 2008/12/16
	char GoldenCmd5G[100];//Add by Maxwell 2009/07/28
	char GoldenCmd2G[100];//Add by Maxwell 2009/07/28
	char TestLogPath[128];//Add by Maxwell 2011/02/14
	char PrintLabelFlag[10];//add by Liu-Chen for k31 Wip Label 2012/04/11
	
	char NCArtFlag[10];//Maxwell 20110326
	char NartCmd[100];//Add by Maxwell 20110326
	char CartCmd[100];//Add by Maxwell 20110326
	char LocalFilePath[256]; //Add by Maxwell 20110326
	int GoldenSelect; //Justin 0124 Golden select; //Add by Maxwell 20110326
	float GoldenSelectionMAX;//Add by Maxwell 20110326
	float GoldenSelectionMIN;//Add by Maxwell 20110326

	int errLimitTimes;
	int indexSelected[200];//Talen 2011/11/01
	int fixID;
	char errorCode[8];
	int errorCodeCount;
	char opID[16];
//	int retryNoticeFlag;//Talen 2012/03/13
 

	GOLDEN_SERVER_PARA Golden_Connect_Para;//Maxwell 20110329
	GOLDEN_SERVER_PARA Golden_Connect1_Para;//Talen 2011/06/27
	CON_BOARD_PARA Control_Board_Para;//Maxwell 20110329

}TEST_STATION_INFO;

// Tester station informations
typedef struct ResMsgUITag
{
	char MsgRes[256];
	HANDLE hEvent;
}RES_MSG_UI;

// Tester station informations
typedef struct TestInputDataTag
{
	TEST_STATION_INFO TestStaInfo;
	UUT_INFO UUTInfo;
	RES_MSG_UI UiResMsg;
	LOGO_INFO LOGOInfo;//haibin 08.03
}TEST_INPUT_INFO;


// data format define, use in rightlist show the Item detail information
typedef struct TestItemTag
{
	//test item ID define
	int  Num;// index the item show sequence
	int LoopRunNum;//Maxwell 110310
	char Name[512];
	char ID[30];
	int  Flag;//0 not test, 1 test (stop on fail) 2 test(fail and continue) 3 test (fail and push)
	int  RtyFlag;//retry flag for set 0=not retry,>1=retry time.default=0;not retry
	int  Parallel;//parallel run test flag
	char RetryJumpID[30];//for retry jump ID index
	char RetryAfterJumpID[30];//for return to test item atter jump, add by maxxell 2008/10/13
	int  RetryJumpCount;
	int  RebootCount;//Add by maxxell 2009/10/05
	int  RetryItemCount;
	int  ItemProperty;
	int  RebootTime;//Add by maxxell 2009/10/05
	
	int index;//Talen 2011/11/01
	int selected;//Talen 2011/11/01

	//test parameter define
	//for UUT control cmd
	char UUTCmd[500];//cmd for control UUT
	//090702
	char UUTCmdAdd[100][500];//cmd for control UUT
	//090702
	char CmdPerfTime[100];//wait time for cmd perform 
	
	//for test tool 
	char DiagCmd[500];//cmd for test tool 
	char DiagPerfTime[100];//wait time for cmd perform 
	
	
	//for instrument control
	char InsCmd[500];//cmd for instrument control
    char InsCtrlID[50];//for instrument control utility ID 
    char InsName[80];//Name of the instrument
	char InsCmdTime[100];//time wait op finished
	int ComPort;//627 define for com port
	int ComPort_1;//1005 define for com port 2
	int TransRate;//627 define for TransRate


	int Port_1;
	int Port_2;

	int LED_PORT_1;
	int LED_PORT_2;
	
	int RGBI_R_SPEC_UP;
	int RGBI_R_SPEC_DOWN;
	int RGBI_G_SPEC_UP;
	int RGBI_G_SPEC_DOWN;
	int RGBI_B_SPEC_UP;
	int RGBI_B_SPEC_DOWN;
	int RGBI_I_SPEC_UP;
	int RGBI_I_SPEC_DOWN;
	double RGBI_W_SPEC_UP;
	double RGBI_W_SPEC_DOWN;
	//haibin 08.01
	double CCORR_UP;
    double CCORR_DOWN;
	long SocketPort;
	char SocketIp[100];
	double X_OFFSET_UP;
    double X_OFFSET_DOWN;
    double Y_OFFSET_UP;
    double Y_OFFSET_DOWN;
    double ROTATE_ANGLE_UP;
    double ROTATE_ANGLE_DOWN;
    double AVG_HIST_UP;
    double AVG_HIST_DOWN;
    
    double TOTAL_AREA_UP;
    double TOTAL_AREA_DOWN;
    double MAX_AREA_UP;
    double MAX_AREA_DOWN;
    double MAX_PERIMETER_UP;
    double MAX_PERIMETER_DOWN;   
    double SQUARE_WIDTH_UP;
    double SQUARE_WIDTH_DOWN;
    double SQUARE_HEIGHT_UP;
    double SQUARE_HEIGHT_DOWN;
	//haibin 08.01



	//spec define
	char Spec[512];
	char MultiSpec[20][512];//Maxwell 090928
	char Spec_1[512];
	char UpLimit[200];
	char DownLimit[200];

	//result define
	char ResultType[512];
	int  ResultFlag;
	char Result[2048];//Maxwell 20110328
	char ErrorCode[128];//for error code define
    char ErrorDes[4096];//for error code descript //Maxwell 20110328
	//char ErrorCode_1[512];//for error code define
    //char ErrorDes_1[512];//for error code descript
	char SFISData[512];//Maxwell 090609

	char MultiResult[20][512];//Maxwell 090928

	//090613 Add for STB use
	SFIS_USE SFISDataTest[SFISUseNum];

	int ResultNumber; //Maxwell 110225 for define numbers of "," in the test result

	//for test logic control,no parse
	HANDLE hEndEvent;//multi event wait
	int CmpFlag;
}TEST_ITEM;



typedef int (_stdcall *ParallelPerfCallFunc)(TEST_ITEM *pTI);
typedef struct ParallelParaTag
{
	ParallelPerfCallFunc pFunc;
	TEST_ITEM *pTI;
}PARALLEL_PARA;


 typedef struct SFISSW
 {
	int SFISMode;
	char SSFISMode[20];	//add by LiYongjun 2011/03/18 for SFIS status use string
 }SFISSW;

// start test function will be use it
typedef struct AStartTestTag
{
   UUT_INFO UUTInfo;
}A_START_TEST_PARA;

//end test function will be use it
typedef struct AStopTestTag
{

    int TestGerRes;
	char ErrorCode[100];

}A_STOP_TEST_PARA;

// record the start and stop test infomations
 typedef struct ASSTestParaTag
 {
       A_START_TEST_PARA StartPARA;
       A_STOP_TEST_PARA StopPARA;
 }ASS_TEST_PARA;

// when entire system initilaztion, this config infomation will be dynamic load to GUI
typedef struct AmPROTLConfigTag
{
	char TestStaName[50];
	char TSysSoftVersion[50];
	char TestStaSN[128];
	char TestStaID[128];
	char UUTName[20];
	char FM_Version[50];
	char UUT_MODEL[50];	
}AM_PROTL_CONFIG;



// system config 
typedef struct SystemConfig
{	
	char REMOTE_IP[100];
	char LOG[100];
    char DUT_CMD_LIST[100];
	char STATION_NAME[100];
	char FM_Version[100];
	char Record_path[100];	
	char PowerTime[100];
	char LOWER_Retry[2];
	char ComPort[3];
}SystemConfig;


// add by jeffrey 070225
typedef struct RunTestParaTag
{
	int CMDCode;
}RUN_TEST_PARA;


// add by jeffrey 070321
typedef struct MainTestThParaTag
{
	DWORD MTestThreadID;
	HANDLE hMTestThread;
}MAIN_TEST_PARA;

enum TEST_FLAG_DEFINE
{
	TFLAG_SKIP,          
	TFLAG_FAIL_STOP,     
	TFLAG_FAIL_CONTINUE,
	TFLAG_FAIL_PAUSE,
	TFLAG_INI_RUN,//Run flag,the item will be perform when test initial
	TFLAG_END_RUN,//the item will be perform when all item be clear
	TFLAG_TERMINATE_RUN,//The item will be perform when test end,after show PASS/FAIL
};
enum TEST_RESULT_FLAG_DEFINE
{
	TEST_FAIL=0,
	TEST_PASS,
	TEST_ERROR,
	NO_TEST,
};

typedef struct SFISItemTag
{
	//test item ID define
	char Item[128];// index the item show sequence
	char Data[512];
}SFIS_ITEM;
#endif
