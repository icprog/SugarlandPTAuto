/*

	Contains:	Ambit pro test UI global variable define

	Version:	xxx 001 xxx

	Copyright:	(C) 2009 by Ambit Micro, Inc., all rights reserved.

    Technology:	**********

	Change History (most recent first):

	Create by Jeffrey Chou 2009/4/23

	2009/4/23 Create,  Jeffrey Chou

*/
#ifndef AMBIT_PROT_GLOBAL_DEFINE_H
#define AMBIT_PROT_GLOBAL_DEFINE_H


#include "resource.h"       // main symbols
#include "MainControlPanelDlg.h"
#include "AmbitProTSysDefine.h"


//for task information management
typedef struct TaskManagemntInfoTag
{

	unsigned int TaskID;//ID for task
	HANDLE hTaskHandle;
	unsigned int uiTaskID;
	
}AM_TASK_INFO;


///////////////////////////////////////////////////////////////////
//Task name ID define
#define MAIN_CONTROL_TASK 0//Main control panel define
#define SFIS_TASK         1
#define MYDAS_TASK        2
#define MFG_MAMA_TASK     3
#define TEST_TASK_TYPE    2000//need dynamic define, offset value 2000 define 
#define MAIN_VIEW_TASK    5
#define EQUIP_VIEW_TASK   6 //090611
#define CLICK_SFIS_TASK   7//haibin.li 2011/11/09

///////////////////////////////////////////////////////////////////
//UI tester server port start range
#define UI_SERVER_PORT_START     10000
///////////////////////////////////////////////////////////////////
//Control message id define
#define WM_SWTO_TESTER_DLG       WM_USER + 1
#define WM_SWTO_MAIN_TEST_PANEL  WM_USER + 2
#define WM_SWTO_SFIS_DLG         WM_USER + 3
#define WM_SWTO_MYDAS_DLG        WM_USER + 4
#define WM_SWTO_CONF_DLG         WM_USER + 30//haibin.li 2011/11/07
#define WM_SWTO_DETAIL_DLG       WM_USER + 31//haibin.li 2011/11/07
#define WM_SWTO_RTM_DLG          WM_USER + 32//haibin.li 2011/11/11



//Tester control message id define
#define WM_TESTER_DATA           WM_USER + 5
#define WM_SFIS_DATA             WM_USER + 6
#define WM_MYDAS_LOG_DATA        WM_USER + 7
#define WM_MYDAS_RECORD_DATA     WM_USER + 8
#define WM_TESTPERF_DATA         WM_USER + 9
#define WM_EXIT                  WM_USER + 10
#define WM_UPDATE_INFO_STATION   WM_USER + 11//notify update main control panel station information
#define WM_UPDATE_INFO_SFIS      WM_USER + 12//notify update main control panel station information
#define WM_UPDATE_INFO_MYDAS     WM_USER + 13//notify update main control panel station information
#define WM_UPDATE_INFO_PRODUCT   WM_USER + 14//notify update main control panel station information
#define WM_DCT_DATA              WM_USER + 15
#define WM_UPDATE_INFO_EQUIP     WM_USER + 16//090611 notify update equipment use
#define WM_UPDATE_INFO_SCAN      WM_USER + 17//notify update main control panel uut scanner information
#define WM_MYDAS_OPEN            WM_USER + 18//notify to open PTSC 090618
#define WM_VIRTUAL_SMO           WM_USER + 19
#define WM_EQUIPMENT_OPEN        WM_USER + 20
#define WM_AUTOFIX_DATA      WM_USER + 21//Maxwell 1224 for auto-fixture
#define WM_MYDAS_MAIN_DATA		WM_USER + 22//Maxwell 100316 for MYDAS main information

#endif