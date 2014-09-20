#pragma once

#define REMOTE_TESTER 0
#define LOCAL_TESTER  1



class CTesterControl
{
public:
	CTesterControl(void);
public:
	~CTesterControl(void);


public:
	int StartTeser(CString ModuleName, int TesterType);
public:
	int ForceTesterExit(CString ModuleName, int TesterType);
};
