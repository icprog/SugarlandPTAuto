#ifndef CONTROL_LOGIC_DEFINE_H
#define CONTROL_LOGIC_DEFINE_H

int TestFail(int testmode);
void SendTitle();
void TestResultInitial();

int GetTime();
int CheckVerion();

UUT_INFO gUUTInfo;
SFISSW SFISSWStu;



MAIN_TEST_PARA gMainTestPara;
CLIENT_TYPE AmPROUI;


int temptime =0;
int cnt_phys;
// global value define
AM_PROTL_CONFIG gTestStaInfo;


//int TestPerfWorkRun();


#endif