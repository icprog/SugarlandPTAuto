#include "stdafx.h"
#include "com_class.h"

struct _FAILED_MODE
{
	int OpenFailed;
	int WriteFailed;
	int RecTimeOut;
	int ResponseError;
	int _PASS_;
}FAILED_MODE;

int openFixture(void);
int closeFixture(void);