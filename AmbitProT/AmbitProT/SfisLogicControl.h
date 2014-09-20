#pragma once

#define SWITCH_NUM 50

class CSfisLogicControl
{
public:
	CSfisLogicControl(void);
public:
	~CSfisLogicControl(void);
public:
	int StartSFISTask(void);
	int SendUIData(wchar_t *pBuf,int flag);
	//char SwitchControlBuf[50][200];
	int GetTestTaskID(char *pBuf,int SizeLen);
	int StartTestTask(char *pBuf);
	void ClearSfisBuf(int flag);
	void ClearTestBuf(char *pBuf);
};
