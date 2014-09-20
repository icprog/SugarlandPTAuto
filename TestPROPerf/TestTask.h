
#ifndef TETS_TASK_H
#define TETS_TASK_H








unsigned int _stdcall MainTestThread(LPVOID lpParameter);
int TestTaskPrepare();
int RunTest();
int PerfTest(list<TEST_ITEM>::iterator *pCy);
int ReportTestResult(list<TEST_ITEM>::iterator Cy);
int TestInitial();
void TestUninitial();
int AcceptCMD(char *pCmd,UINT DataLen);
int RunTest();
int AnaTheData(char *pBuf);
int SendInfRequrementToUI();
int SendLogTileToUI();
int SendIDInfoToUI(void);
void TestReportReset();
UINT WINAPI AppParaFunc(PVOID pParam);
int BeginItemParaRun(ParallelPerfCallFunc pFunc,TEST_ITEM *pTI);
int SetSFISData(char *pITEMID,char *pData,int Index=-1);
void ClearSFISData(char *pITEMID);
int AfterTestWork();

#endif