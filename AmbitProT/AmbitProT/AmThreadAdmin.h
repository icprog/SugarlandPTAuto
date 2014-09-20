/*
	Contains:	Ambit pro test UI thread management function define

	Version:	xxx 001 xxx

	Copyright:	(C) 2009 by Ambit Micro, Inc., all rights reserved.

    Technology:	**********

	Change History (most recent first):

	Create by Jeffrey Chou 2009/4/27

	2009/4/27 Create,  Jeffrey Chou

*/

#ifndef AM_THREAD_ADMIN_H
#define AM_THREAD_ADMIN_H
//#include "AmbitProTGlobalDefine.h"


UINT AmPostThreadMes(UINT DesTask, UINT Msg, WPARAM wParam, LPARAM lParam);
void AmDelThreadMes(UINT TaskID);
void AmExitAllTask();



#endif
