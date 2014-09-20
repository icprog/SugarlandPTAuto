#pragma once
#include "tresultreport.h"
#include "atlstr.h"
class CTMMResultReport :
	public CTResultReport
{
public:
	CTMMResultReport(void);
public:
	~CTMMResultReport(void);
public:
	int OutputTMMLogFile(char* pRtn, int BufL);
	int WriteTMMLogFile(char *Pin);
    int FileNameInsert(char * pbuf);
    void FileNameInsertStart(void);

public:
    CString TMMHeadStr;
	CString TMMFileStr;
	CString TMMFileName;

public:
	int HeadContentInsert(char * pbuf);
public:
	void HeadInsertStart(void);

};
