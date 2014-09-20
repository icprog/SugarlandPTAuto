/*
	Contains:	Ambit pro test UI thread management function

	Version:	xxx 001 xxx

	Copyright:	(C) 2009 by Ambit Micro, Inc., all rights reserved.

    Technology:	**********

	Change History (most recent first):

	Create by Jeffrey Chou 2009/4/27

	2009/4/27 Create,  Jeffrey Chou

*/


#include "stdafx.h"
#include "AmThreadAdmin.h"
#include "AmbitProTGlobalDefine.h"


extern list<AM_TASK_INFO> gUITaskInfoList;//list for UI task information store



UINT AmPostThreadMes(UINT DesTask, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	list<AM_TASK_INFO>::iterator Cy;
	int Count=0;

	//gUITaskInfoList.empty();

	for (Cy=gUITaskInfoList.begin(); Cy!=gUITaskInfoList.end();Cy++)
	{	
		if ((*Cy).TaskID==DesTask)
		{
            PostThreadMessage((*Cy).uiTaskID, Msg, wParam, lParam);
			Count++;
			return 1;
		}
	}

	return 0;

}
void AmExitAllTask()
{

	AmPostThreadMes(SFIS_TASK, WM_EXIT, 0,0);
	list<AM_TASK_INFO>::iterator Cy;
	for (Cy=gUITaskInfoList.begin(); Cy!=gUITaskInfoList.end();Cy++)
	{	
        PostThreadMessage((*Cy).uiTaskID, WM_EXIT , 0, 0);
	}
	return;
}

void AmDelThreadMes(UINT TaskID)
{
	
	list<AM_TASK_INFO>::iterator Cy;
	
	for (Cy=gUITaskInfoList.begin(); Cy!=gUITaskInfoList.end();Cy++)
	{	
		if ((*Cy).TaskID==TaskID)
		{
			gUITaskInfoList.erase(Cy);
			break;
		}
	}

	return;
}
