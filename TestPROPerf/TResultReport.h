#pragma once

#include <list>
#include "stdarg.h"





using namespace std;


typedef struct MaTestResultTag
{
	int ItemID;
	char ItemName[1024];
	char TestResultDes[2048];//Maxwell 20110328
	unsigned int Len;
}MA_TEST_RESULT;



class CTResultReport
{
public:
	CTResultReport(void);
public:
	~CTResultReport(void);

public:
	list<MA_TEST_RESULT> TestResultList;
public:
	long InputResult(char *pItemName,const char * format,...);
	int OutputAllResult(char *Outbuf, int Len, char *Titlebuf, int titleLen);
	int OutputTitle(char *Outbuf, int Len);
	void SetUUTModelName(char *pName);
	void SetStationName(char *pSName);
	int OutputFormatFile(char *pFileName);
	int OutputLogFile(char *pFileName,char *pTitle,char *pData);
	int OutputTimeLogFile(char *pFileName,char *pData);//Maxwell 090702
	void ClearBUF();

	int LogDataNF(char* sn,char *datetime,char* filename,char* pData);//Sam 110310
	int CTResultReport::LogDataCAL(char* sn,char *datetime,char* filename,char* pData);//Sam 110311

	int makeItemTitle(char *outBuf, char *inPut, int rty);


private:
	int ItemID;
	MA_TEST_RESULT TRTemp;
	char StationName[100];
	char UUTModelName[100];

};
