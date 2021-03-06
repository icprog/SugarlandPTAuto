/********************************************************************
created:	2007/02/06
filename: 	..\TestPROPerf\TestPROPerf\AAPTest.cpp
file path:	..\TestPROPerf\TestPROPerf
file base:	TestPROPerf
file ext:	cpp
author:		Jeffrey

purpose:	do the act test in process function definition

*********************************************************************/
#include "stdafx.h"
#include "TestItemScript.h"
#include "com_class.h" //627
#include "AmbitExeCtrl.h"
#include "Shlwapi.h"//Talen 2011/09/01 for PathFileExists()

//extern UUT_INFO gUUTInfo;
//extern SFISSW SFISSWStu;
extern _asyn_com gComDev;//627

extern TEST_INPUT_INFO gTI;//test input information
//CInstrumentCTRL gFqc;// GPIB instrument 
extern list<TEST_ITEM> TItemList;//Talen 2011/11/01

CTestItemScript tiScript;

int ParseTestItem(const char *p1, const char *p2)
{
	size_t len;
	len=strlen(p1);
	if (strlen(p2)>len)
	{
		len=strlen(p2);
	}
	return (strncmp(p1,p2,len));
}






int __stdcall ItemTest(TEST_ITEM *pTI)
{
	int retFlag=0;
	if(!ParseTestItem(pTI->ID,"WAIT_UUT_UP"))
	{
        retFlag=tiScript.WaitUUTUp(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_FW"))
	{
		retFlag=tiScript.CheckUUTFW(pTI);  
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_FW_TIME"))
	{
		retFlag=tiScript.CheckUUTFWGM(pTI);  
	}

	//add start by Talen 2011/10/28
	else if(!ParseTestItem(pTI->ID,"SET_UUT_INFO"))
	{
		retFlag=tiScript.SetUUTInfo(pTI);
	}
	//add end by Talen 2011/10/28

	//add start by Talen 2011/10/28
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_INFO"))
	{
		retFlag=tiScript.CheckUUTInfo(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"READ_UUT_INFO"))
	{
		retFlag=tiScript.ReadUUTInfo(pTI);
	}
	//add end by Talen 2011/10/28

	//else if(!ParseTestItem(pTI->ID,"SET_UUT_GMT_TIME"))
	//{
	//	retFlag=tiScript.SetUUTGmtTime(pTI);
	//}
	//else if(!ParseTestItem(pTI->ID,"CHECK_UUT_GMT_TIME"))
	//{
	//	retFlag=tiScript.CheckUUTGmtTime(pTI);
	//}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_USB_PORT"))
	{
		retFlag=tiScript.CheckUUTUsb(pTI);
	}
	//add start by Talen 2011/10/28
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_LINK"))
	{
		retFlag=tiScript.CheckUUTLinkStatus(pTI);
	}
	//add end by Talen 2011/10/28

	else if(!ParseTestItem(pTI->ID,"SET_UUT_STATUS"))
	{
		retFlag=tiScript.SetUUTStatus(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_ETH_THROUGHPUT"))
	{
		retFlag=tiScript.CheckEthThroughput(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_WIFI_TPINGOLDEN"))//Maxwell 090728
	{
		retFlag=tiScript.CheckWiFiThroughPutInGolden(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_TH_CONNECTION"))
	{
		retFlag=tiScript.CheckConnection(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"SET_UUT_WIFI_ANTENNA"))
	{
		retFlag=tiScript.SetWifiAntenna(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_WIFI_ANTENNA"))
	{
		retFlag=tiScript.CheckWifiAntenna(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_THROUGHPUT_TX"))
	{
		retFlag=tiScript.CheckWifiTxThroughput(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_THROUGHPUT_RX"))
	{
		retFlag=tiScript.CheckWifiRxThroughput(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_LOCAL"))
	{
		retFlag=tiScript.CheckLocalInfo(pTI);
	}	
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_LED"))
	{
		retFlag=tiScript.CheckUutLed(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_RESET_BUTTON"))
	{
		retFlag=tiScript.CheckUUTResetButton(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CLICK_RESET_BUTTON"))
	{
		retFlag=tiScript.ClickUUTResetButton(pTI);
	}
	//Talen 2011/06/16
	else if(!ParseTestItem(pTI->ID,"CLICK_RESET_BUTTON_NOCHECK"))
	{
		retFlag=tiScript.ClickUUTResetButtonNoCheck(pTI);
	}
	//Talen 2011/06/16

	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_DIAG_MODE"))
	{
		retFlag=tiScript.CheckDiagValue(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_WAN_PORT_VALUE"))
	{
		retFlag=tiScript.CheckWanPortStatus(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"SET_INSTRUMENT_CMD"))
	{
		retFlag=tiScript.SetInstrumentStatus(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_FAN_SPIN"))
	{
		retFlag=tiScript.CheckFanSpin(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_MLB_TEMP_SENSOR"))
	{
		retFlag=tiScript.CheckMlbTemperatureSensor(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_TEMP_SENSOR"))
	{
		retFlag=tiScript.CheckTemperatureSensor(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"SET_UUT_HDD_FORMAT"))
	{
		retFlag=tiScript.SetHDDFormat(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_HDD_FORMAT"))
	{
		retFlag=tiScript.CheckHDDFormat(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_HDD_INFOMATION"))
	{
		retFlag=tiScript.CheckHDDInformation(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_MLB_BATTERY"))
	{
		retFlag=tiScript.CheckMLBBattery(pTI);
	}
//Add start by Maxwell 2008/10/14
	else if(!ParseTestItem(pTI->ID,"CHECK_MLB_BATTERY_VOLT"))
	{
		retFlag=tiScript.CheckMLBBatteryUseVolt(pTI);
	}
//Add end by Maxwell 2008/10/14
	else if(!ParseTestItem(pTI->ID,"RUN_DIAG_TOOL_NR"))
	{
		retFlag=tiScript.RunDiagToolNoResult(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"OPERATOR_IO_DIAG"))
	{
		retFlag=tiScript.OperatorIOInTesting(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"RUN_DIAG_TOOL_NR_SHOW"))
	{
		retFlag=tiScript.RunDiagToolNoResultShow(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"SET_HD_SPIN_DOWN"))
	{
		retFlag=tiScript.SetHDSpinDown(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_HD_SPIN_DOWN"))
	{
		retFlag=tiScript.CheckAndWaitHDSpinDown(pTI);
	}
//Add start by Maxwell 2008/11/13 only one radio
	else if(!ParseTestItem(pTI->ID,"CHECK_RADIO_STATUS"))
	{
		retFlag=tiScript.CheckRadioStatus(pTI);
	}
//Add end by Maxwell 2008/11/13

//Add start by Maxwell 2008/12/26
	else if(!ParseTestItem(pTI->ID,"CHECK_MODE_STATUS"))
	{
		retFlag=tiScript.CheckSystemModeStatus(pTI);
	}
//Add end by Maxwell 2008/11/26

	else if(!ParseTestItem(pTI->ID,"CHECK_AND_SET_TXRATE"))
	{
		retFlag=tiScript.CheckandSetWifiTXrate(pTI);
	}

	else if(!ParseTestItem(pTI->ID,"CHECK_SYS_REBOOT"))
	{
		retFlag=tiScript.CheckSYSReboot(pTI);
	}

//Add start by Talen 2011/11/01
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_STATUS"))
	{
		retFlag=tiScript.CheckWifiRadioStatus(pTI);
	}
//Add end by Talen 2011/11/01
//Add start by Maxwell 2008/01/19
	else if(!ParseTestItem(pTI->ID,"CHECK_AND_SET_DEFAULT"))
	{
		retFlag=tiScript.CheckAndSetDefault(pTI);
	}
//Add end by Maxwell 2008/01/19	
//Add start by Maxwell 2008/01/19
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_TXRATE_5G"))
	{
		retFlag=tiScript.CheckTxRate5G(pTI);
	}
//Add end by Maxwell 2008/01/19
//Add start by Maxwell 2008/01/19
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_TXRATE_2G"))
	{
		retFlag=tiScript.CheckTxRate2G(pTI);
	}
//Add end by Maxwell 2008/01/19	

//Add start by Maxwell 2008/01/21
	else if(!ParseTestItem(pTI->ID,"CHECK_SN_INFILE"))
	{
		retFlag=tiScript.CheckSnInFile(pTI);
	}
	//else if(!ParseTestItem(pTI->ID,"CHECK_SRCV_VERSION"))
	//{
	//	retFlag=tiScript.CheckSRCV(pTI);
	//}
	else if(!ParseTestItem(pTI->ID,"CHECK_minS_VERSION"))
	{
		retFlag=tiScript.CheckminS(pTI);
	}
//Add end by Maxwell 2008/01/21	
	//627
	else if(!ParseTestItem(pTI->ID,"SET_WIFI_PORT"))
	{
		retFlag=tiScript.SetComTrans(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CLOSE_WIFI_PORT"))
	{
		retFlag=tiScript.CloseComTrans(pTI);
	}
	//627
	//072509
	else if(!ParseTestItem(pTI->ID,"OPEN_IPERF_SERVER"))
	{
		retFlag=tiScript.OpenIperfInGolden(pTI);
	}
	//072509
	//090901
	else if(!ParseTestItem(pTI->ID,"AUTO_POWER_ON"))
	{
		retFlag=tiScript.AutoPowerOn(pTI);
	}
	//090901
	//090901
	else if(!ParseTestItem(pTI->ID,"AUTO_POWER_OFF"))
	{
		retFlag=tiScript.AutoPowerOff(pTI);
	}
	//091005
	else if(!ParseTestItem(pTI->ID,"AUTO_GOLDEN_REBOOT"))
	{
		retFlag=tiScript.AutoGoldenReboot(pTI);
	}
	//091005

	//091216 Add start by Tony for K30 check HDSN
	else if(!ParseTestItem(pTI->ID,"CHECK_HDD_SN"))
	{
		retFlag=tiScript.CheckHDD_SN(pTI);
	}
	//091216 Add end by Tony for K30 check HDSN

	//091223 Add start by Tony for K30A Send WIFIMAC0 to SFIS on Screen
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_MAC0"))
	{
		retFlag=tiScript.CheckWIFIMAC0(pTI);
	}
	//091223 Add end by Tony for K30A Send WIFIMAC0 to SFIS on Screen

	//091223 Add start by Tony for K30A Send WIFIMAC1 to SFIS on Screen
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_MAC1"))
	{
		retFlag=tiScript.CheckWIFIMAC1(pTI);
	}
	//091223 Add end by Tony for K30A Send WIFIMAC1 to SFIS on Screen

	else if(!ParseTestItem(pTI->ID,"GENERATE_LABEL_INFOR"))
	{
		retFlag=tiScript.GenerateLabelInformation(pTI);
	}

	//Add start by Maxwell 2010/02/11	
	else if(!ParseTestItem(pTI->ID,"CHECK_AP_RSSI"))
	{
		retFlag=tiScript.CheckUUTRSSI(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_GOLDEN_RSSI_2G"))
	{
		retFlag=tiScript.CheckGOLDENRSSI2G(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_GOLDEN_RSSI_5G"))
	{
		retFlag=tiScript.CheckGOLDENRSSI5G(pTI);
	}
//Add end by Maxwell 2010/02/11	

//Maxwell 110212
	else if(!ParseTestItem(pTI->ID,"CONTROL_GOLDEN_STATUS"))
	{
		retFlag=tiScript.ControlGoldenStatus(pTI);
	}
//Maxwell 110212
	//Maxwell 110212
	else if(!ParseTestItem(pTI->ID,"SET_GOLDEN_MCS"))
	{
		retFlag=tiScript.SendGoldenMCSCMD(pTI);
	}
	//Maxwell 110212
		//Maxwell 110212
	else if(!ParseTestItem(pTI->ID,"CHECK_WIFI_NOISE"))
	{
		retFlag=tiScript.CheckWifiNoiseFloor(pTI);
	}
	//Maxwell 110212
		//Maxwell 110212
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_TXRATE"))
	{
		retFlag=tiScript.CheckUUTTXrate(pTI);
	}
	//Maxwell 110212

	//Sam 110311
	else if(!ParseTestItem(pTI->ID,"LOG_CAL_DATA"))
	{
		retFlag=tiScript.LogUUTCalData(pTI);
	}
	//Sam 110311
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"CHECK_K31_TEMP"))
	{
		retFlag=tiScript.CheckK31TempSensor(pTI);
	}
	//Maxwell 20110326
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"RUN_IQ_PRO"))
	{
		retFlag=tiScript.RunIQProgram(pTI);
	}
	//Maxwell 20110326
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"DElETE_PATH_FILE"))
	{
		retFlag=tiScript.DeletePathFile(pTI);
	}
	//Maxwell 20110326
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"CHECK_IQ_FINISH"))
	{
		retFlag=tiScript.CheckIQTestFinish(pTI);
	}
	//Maxwell 20110326
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"GENE_IQ_TGRR"))
	{
		retFlag=tiScript.GenerateIQTestResultGRR(pTI);
	}	
	//Maxwell 20110326
	//Maxwell 20110326
	else if(!ParseTestItem(pTI->ID,"GET_IQ_TRESULT"))
	{
		retFlag=tiScript.GetIQTestResult(pTI);
	}	
	//Maxwell 20110326

	//Talen 2011/05/31
	else if(!ParseTestItem(pTI->ID,"GET_ART_VERSION"))
	{
		retFlag=tiScript.GetArtVersion(pTI);
	}
	//Talen 2011/05/31

	//Sam 20110328
	else if(!ParseTestItem(pTI->ID,"ETHERNET_SWITCH"))
	{
		retFlag=tiScript.SwitchEthernet(pTI);
	}	
	//Sam 20110328

	//Sam 20110328
	else if(!ParseTestItem(pTI->ID,"BLOCK_PUSH_FRONT"))
	{
		retFlag=tiScript.BlockPushFront(pTI);
	}	
	//Sam 20110328

	//Sam 20110328
	else if(!ParseTestItem(pTI->ID,"BLOCK_PULL_BACK"))
	{
		retFlag=tiScript.BlockPullBack(pTI);
	}	
	//Sam 20110328

	//Sam 20110328
	else if(!ParseTestItem(pTI->ID,"PANEL_PUSH_DOWN"))
	{
		retFlag=tiScript.PanelPushDown(pTI);
	}	
	//Sam 20110328

	//Sam 20110328
	else if(!ParseTestItem(pTI->ID,"PANEL_PULL_UP"))
	{
		retFlag=tiScript.PanelPullUp(pTI);
	}	
	//Sam 20110328
	//Maxwell 20110328
	else if(!ParseTestItem(pTI->ID,"DOWN_ART_CHECK"))
	{
		retFlag=tiScript.DownArtandCheck(pTI);
	}
	//Maxwell 20110328
	//Maxwell 20110329
	else if(!ParseTestItem(pTI->ID,"SET_GOLDEN_CONNECT"))
	{
		retFlag=tiScript.SetGoldenConnect(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"SET_AUDIO_TEST"))
	{
		retFlag=tiScript.SetAudioTest(pTI);
	}
	//Maxwell 20110329
	//Maxwell 20110330
	else if(!ParseTestItem(pTI->ID,"WRITE_IQ_SNMAC"))
	{
		retFlag=tiScript.WriteSNMACFile(pTI);
	}
	//Maxwell 20110330
	//Talen 2011/04/19
	else if(!ParseTestItem(pTI->ID,"CHECK_CONNECTION"))
	{
		retFlag=tiScript.CheckConnectionUsePing(pTI);
	}
	//Talen 2011/04/19
	//Talen 2011/05/11
	else if(!ParseTestItem(pTI->ID,"CONTROL_ART_RUN"))
	{
		retFlag=tiScript.CtrlArtRun(pTI);
	}
	//Talen 2011/05/11
	
	//Talen 2011/05/12
	else if(!ParseTestItem(pTI->ID,"AUTO_COM_REBOOT"))
	{
		retFlag=tiScript.AutoCOMReboot(pTI);
	}
	//Talen 2011/05/12
	//Talen 2011/05/18
	else if(!ParseTestItem(pTI->ID,"COMMIT_CALDATA_FLASH"))
	{
		retFlag=tiScript.CommitCaldataFlash(pTI);
	}
	//Talen 2011/05/18
	//Talen 2011/05/23
	else if(!ParseTestItem(pTI->ID,"AUTO_FIXTURE_CONTROL"))//20091127
	{
		retFlag=tiScript.AutoFixtureControl(pTI);
	}
	//Talen 2011/05/23
	//Talen 2011/06/16
	else if(!ParseTestItem(pTI->ID,"AUTO_FIXTURE_CONTROL_NOCHECK"))//20091127
	{
		retFlag=tiScript.AutoFixtureControlNoCheck(pTI);
	}
	//Talen 2011/06/16
	//Talen 2011/05/25
	else if(!ParseTestItem(pTI->ID,"SET_UUT_CHANNEL"))
	{
		retFlag=tiScript.SetUUTChannel(pTI);
	}
	//Talen 2011/05/25
	//Talen 2011/06/14
	else if(!ParseTestItem(pTI->ID,"DOUBLE_FIXTURE_CONTROL"))
	{
		retFlag=tiScript.BlockControl(pTI);
	}	
	//Talen 2011/06/14

	//haibin,li 06.13
	//else if(!ParseTestItem(pTI->ID,"CHECK_LED_CMD"))
	//{
		//retFlag=tiScript.CheckLedCmd(pTI);
	//}
    //haibin,li 06.13

	//haibin,li 07.21
	else if(!ParseTestItem(pTI->ID,"HI_POT_TEST"))
	{
		retFlag=tiScript.HiPotTest(pTI);
	}
	//haibin,li 07.21

	//haibin,li 08.01
	else if(!ParseTestItem(pTI->ID,"VISUAL_INSPECTION"))
	{
		retFlag=tiScript.VisualInspection(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_LOGO_SN"))
	{
		retFlag=tiScript.CheckLogoSN(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_LOGO_POSITION"))
	{
		retFlag=tiScript.CheckLogoPosition(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_LOGO_DEPTH"))
	{
		retFlag=tiScript.CheckLogoDepth(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_LOGO_SKU_MARK"))
	{
		retFlag=tiScript.CheckLogoSkuMark(pTI);
	}
	else if(!ParseTestItem(pTI->ID,"CHECK_LOGO_SQUARE_SIZE"))
	{
		retFlag=tiScript.CheckLogoSquareSize(pTI);
	}

	//haibin,li 08.01

	//haibin,li 08.23
	else if(!ParseTestItem(pTI->ID,"CHECK_SENSOR_POSITION"))
	{
		retFlag=tiScript.CheckSensorPosition(pTI);
	}	
	//haibin,li 08.23

	//Talen 2011/06/23
	else if(!ParseTestItem(pTI->ID,"CHECK_SKU_TESTER"))
	{
	    retFlag=tiScript.CheckSKUTested(pTI);
	}
	//Talen 2011/06/23

	//Talen 2011/06/27
	else if(!ParseTestItem(pTI->ID,"CHECK_DBUG_OFF"))
	{
	    retFlag=tiScript.CheckUUTDbugOff(pTI);
	}
	//Talen 2011/06/27
	
	//Talen 2011/06/29
	else if(!ParseTestItem(pTI->ID,"CHECK_DBUG_ON"))
	{
	    retFlag=tiScript.CheckUUTDbugOn(pTI);
	}
	//Talen 2011/06/29

	//Talen 2011/06/27
	else if(!ParseTestItem(pTI->ID,"CHECK_AND_SET_DBUG_ON"))
	{
	    retFlag=tiScript.CheckAndSetDbugOn(pTI);
	}
	//Talen 2011/06/27
	
	//Talen 2011/06/27
	else if(!ParseTestItem(pTI->ID,"CHECK_AND_SET_RADIO_ON"))
	{
	    retFlag=tiScript.CheckAndSetRadioOn(pTI);
	}
	//Talen 2011/06/27

	//Talen 2011/07/04
	else if(!ParseTestItem(pTI->ID,"WRITE_SNPN_TO_FOBA"))
	{
		retFlag=tiScript.WriteSNPNtoTxt(pTI);
	}
	//Talen 2011/07/04

	//Talen 2011/07/04
	else if(!ParseTestItem(pTI->ID,"CHECK_UUT_CH"))
	{
		retFlag=tiScript.CheckUUTch(pTI);
	}
	//Talen 2011/07/04

	//Talen 2011/07/04
	else if(!ParseTestItem(pTI->ID,"SET_UUT_CHECK_RETURN"))
	{
		retFlag=tiScript.SetUUTCheckReturn(pTI);
	}
	//Talen 2011/07/04

	//Talen 2011/07/08
	else if(!ParseTestItem(pTI->ID,"RUN_PROCESS_NOWAIT"))
	{
		retFlag=tiScript.RunProcessNoWait(pTI);
	}
	//Talen 2011/07/08

	else if(!ParseTestItem(pTI->ID,"CHECK_PRODUCTWEIGHT"))
	{
         retFlag=tiScript.CheckProductWeight(pTI);
	}
	//Talen 2011/08/15
	else if(!ParseTestItem(pTI->ID,"CHECK_WANPORT_CONNECTION"))
	{
         retFlag=tiScript.CheckWanPortConnection(pTI);
	}
	//Talen 2011/08/15
	//Talen 2011/08/31
	else if(!ParseTestItem(pTI->ID,"WAIT_FOR_TEST"))
	{
         retFlag=tiScript.WaitForTest(pTI);
	}
	//Talen 2011/08/31
	//Talen 2011/08/30
	else if(!ParseTestItem(pTI->ID,"CHECK_LED_BEHAVIOR"))
	{
         retFlag=tiScript.CheckLedBehavior(pTI);
	}
	//Talen 2011/08/30

	//Talen 2011/09/10
	else if(!ParseTestItem(pTI->ID,"PERFORM_PCLI"))
	{
         retFlag=tiScript.PerformPCLI(pTI);
	}
	//Talen 2011/09/10

	//Talen 2012/01/13
	else if(!ParseTestItem(pTI->ID,"STOP_PROCESS"))
	{
		retFlag=tiScript.KillProcess(pTI);
	}
	//Talen 2012/01/13
	
	//Talen 2012/01/18
	else if(!ParseTestItem(pTI->ID,"RUN_BATFILE_AND_WAIT"))
	{
		retFlag=tiScript.RunBatchFileAndWait(pTI);
	}
	//Talen 2012/01/18
	else if(!ParseTestItem(pTI->ID,"PRINT_LABEL"))
	{
         retFlag=tiScript.PrintWipLabel(pTI);
	}//add by Liu-Chen for k31 Wip Label 2012/04/11
	else
	{
  
	}
	return retFlag;
}


unsigned int _stdcall SRQRevCom(LPVOID lpPara)
{

	MSG msg;
	char cCmdBuf[512] = "";
	char RevBuf[1024];
	UINT DataLen=0;

	while (1)
	{
		if (!WaitMessage()) 
		{
			return 1;
		}	
		memset(RevBuf,0,sizeof(RevBuf));
		while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
		{
			
			memcpy_s(RevBuf,sizeof(RevBuf),(unsigned char *)msg.wParam,msg.lParam);
			if ((unsigned char *)msg.wParam)
			{
				free((unsigned char *)msg.wParam);
			}
			printf("COM --%s\n",RevBuf);

		}	  	
	}
	return 1;
}


int _stdcall SRQRevComCl(unsigned char *p,UINT Len)
{
	
	printf("%s\n",p);
	return 1;

}


int CCDInitial()
{
	char InsBuf[100]="iImageT.exe";
	if (atoi(gTI.TestStaInfo.CCDAutoFlag))
	{
        return (RunOneProcess(InsBuf));
	}
	return 1;
}


int K31printInitial()
{
	int tt=0;
	int i=20;
	char InsBuf[100]="Print_wip.exe";
	if (atoi(gTI.TestStaInfo.PrintLabelFlag))
	{
        tt=RunOneProcessMinisize(InsBuf);
		if(0==tt)
		{
			return 0;
		}
		HWND ToolWin=NULL;
		while(i)
		{
			ToolWin = ::FindWindow(NULL,"Print_wip");
			if(ToolWin)
			{
				break;			
			}
			Sleep(100);
			i--;
		}
		if(0==i)
		{
			amprintf("can not find window: LabelReprint;\n");
		}
		SetWindowPos(ToolWin,HWND_BOTTOM,0,0,100,38,SWP_NOMOVE|/*SWP_NOSIZE|*/SWP_NOACTIVATE);
		::ShowWindow(ToolWin,SW_SHOWMINNOACTIVE);
	}
	return 1;
}
//add by Liu-Chen for k31 Wip Label 2012/04/11




//haibin 2011/08/01
int VitestInitial()
{
	char InsBuf[100]="vitest.exe";
    KillTargetProcess("vitest.exe");
	if (atoi(gTI.TestStaInfo.VitestFlag))
	{
        return (RunOneProcess(InsBuf));
	}
	return 1;
}
//haibin 2011/08/01



int InstrumentInitial()
{
	char InsBuf[100]="MeasurementAuto.exe";
	if (atoi(gTI.TestStaInfo.MEAutoFlag))
	{
        return (RunOneProcess(InsBuf));
	}
	return 1;
}

void InstrumentUnInitial()
{
	HWND killWin=NULL;
	int f=1;
	while (f)
	{
		killWin=NULL;
		killWin = ::FindWindow(NULL,"Untitled - MeasurementAuto");
		if (killWin)
		{
			f=1;
			::SendMessage(killWin,WM_CLOSE,NULL,NULL);
			
		}
		else
		{
			f=0;
		}
		Sleep(500);
	}
	return;
}

void CCDUnInitial()
{
	HWND killWin=NULL;
	int f=1;
	while (f)
	{
		killWin=NULL;
		killWin = ::FindWindow(NULL,"Image Pro");
		if (killWin)
		{
			f=1;
			::SendMessage(killWin,WM_CLOSE,NULL,NULL);
			
		}
		else
		{
			f=0;
		}
		Sleep(500);
	}
	return;
}


void K31printUnInitial()
{
	HWND killWin=NULL;
	int f=1;
	while (f)
	{
		killWin=NULL;
		killWin = ::FindWindow(NULL,"Print_wip");
		if (killWin)
		{
			FILE *pp=NULL;
			f=1;
			::SendMessage(killWin,WM_CLOSE,NULL,NULL);
			
		}
		else
		{
			f=0;
		}
		Sleep(500);
	}
	return;
}
//add by Liu-Chen for k31 Wip Label 2012/04/11



//haibin 2011/08/01
void VitestUnInitial()
{	
	KillTargetProcess("vitest.exe");
	KillTargetProcess("VITEST.EXE");
	return;
}
//haibin 2011/08/01

//Add start by Maxwell 2008/12/15 for open&close iperf in FT1
int IperfInitial()
{
	/*char IperfBuf1[100]=gTI.TestStaInfo.IperfBat1;
	char IperfBuf2[100]=gTI.TestStaInfo.IperfBat2;*/
	if (1==atoi(gTI.TestStaInfo.IperfAutoFlag))
	{
		runShellExecute(gTI.TestStaInfo.IperfBat1);
	}
	else if (2==atoi(gTI.TestStaInfo.IperfAutoFlag))
	{
        runShellExecute(gTI.TestStaInfo.IperfBat1);
		runShellExecute(gTI.TestStaInfo.IperfBat2);
	}
	return 1;
}

//Add start by Maxwell 2009/07/28 for set the MCS of Golden in FT1
int THGoldenInitial()
{
	char cmd[500]="";
	char cStop[2]={0x03};
	char revbuf[2048]=""; 
	int ComTime=50;

	if (1==atoi(gTI.TestStaInfo.GoldenSetMCSFlag))
	{
//Set MCS of Golden 2G
		strcpy_s(cmd,sizeof(cmd),gTI.TestStaInfo.GoldenCmd2G);

		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate);
		//len = com.read(strtem, sizeof(strtem)); 

		int m = strlen(cmd);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write %s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write %s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		gComDev.read(revbuf,(int)sizeof(revbuf));
		strcat_s(cmd,sizeof(cmd),"\n");
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write %s;\n",cmd);
			//return 0;
		}
		Sleep(500);
		gComDev.read(revbuf,(int)sizeof(revbuf));

		amprintf("Com read %s;\n",revbuf);
		if(strstr(revbuf,"outputs"))
		{
			amprintf("The MCS of 2G set success!\n");
		}
		else
		{
			amprintf("The MCS of 2G set fail!\n");
			gComDev.close();
			return 0;
		}

		//Set MCS of Golden 5G
		strcpy_s(cmd,sizeof(cmd),gTI.TestStaInfo.GoldenCmd5G);

		if(gComDev.is_open())
		{
			gComDev.close();
		}
		if(!gComDev.open(gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate);
		//len = com.read(strtem, sizeof(strtem)); 

		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write %s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write %s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		gComDev.read(revbuf,(int)sizeof(revbuf));
		strcat_s(cmd,sizeof(cmd),"\n");
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write %s;\n",cmd);
			//return 0;
		}
		Sleep(500);
		gComDev.read(revbuf,(int)sizeof(revbuf));

		//amprintf("Com read %s;\n",revbuf);
		if(strstr(revbuf,"outputs"))
		{
			amprintf("The MCS of 5G set success!\n");
		}
		else
		{
			amprintf("The MCS of 5G set fail!\n");
			gComDev.close();
			return 0;
		}

		Sleep(1000);
		amprintf("MCS index of Golden sever set pass;\n");
		gComDev.close();
	}
	
	return 1;
}


//Add start by Maxwell 2009/10/05 for reboot UUT auto
int THGoldenReboot()
{
	char cmd[500]="";
	char cmdReboot[500]="reboot";
	char cStop[2]={0x03};
	char revbuf[2048]=""; 
	int ComTime=50;

	if (1==atoi(gTI.TestStaInfo.GoldenRebootFlag))
	{
/////////-----------------------------------------
		if(gComDev.is_open())
		{
			gComDev.close();
		}

		if(!gComDev.open(gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",gTI.TestStaInfo.Golden2GCom,gTI.TestStaInfo.GoldenComTransRate);
		//len = com.read(strtem, sizeof(strtem)); 

		int m = strlen(cmd);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write  in 2.4G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		gComDev.read(revbuf,(int)sizeof(revbuf));
		strcat_s(cmdReboot,sizeof(cmdReboot),"\n");
		strcat_s(cmd,sizeof(cmd),"\n");

		if(!gComDev.write(cmdReboot, (int)strlen(cmdReboot)))
		{
			amprintf("Com command write  in 2.4G Golden%s;\n",cmdReboot);
			//return 0;
		}

/////////-----------------------------------------
		if(gComDev.is_open())
		{
			gComDev.close();
		}

		if(!gComDev.open(gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate))
		{
			amprintf("Open ComPort %d TransRate at %d fail!;\n",gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate);
			return 0;
		}
		amprintf("Open ComPort %d TransRate at %d pass;\n",gTI.TestStaInfo.Golden5GCom,gTI.TestStaInfo.GoldenComTransRate);
		//len = com.read(strtem, sizeof(strtem)); 

		m = strlen(cmd);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		if(!gComDev.write(cStop, 1))
		{
			amprintf("Com command write in 5G Golden%s;\n",cStop);
			//return 0;
		}
		Sleep(100);
		gComDev.read(revbuf,(int)sizeof(revbuf));
		/*strcat_s(cmdReboot,sizeof(cmdReboot),"\n");
		strcat_s(cmd,sizeof(cmd),"\n");*/

		if(!gComDev.write(cmdReboot, (int)strlen(cmdReboot)))
		{
			amprintf("Com command write in 5G Golden%s;\n",cmdReboot);
			//return 0;
		}

		if(gComDev.is_open())
		{
			gComDev.close();
		}

		Sleep(150000);
	}
	
	return 1;
}

void IperfUnInitial()
{
	HWND killWin=NULL;
	int f=1;

	while (f)
	{
		killWin=NULL;
		killWin = ::FindWindow(NULL,"Iperf");
		if (killWin)
		{
			f=1;
			::SendMessage(killWin,WM_CLOSE,NULL,NULL);
			
		}
		else
		{
			f=0;
		}
		Sleep(500);
	}
	return;
}
//Add end by Maxwell 2008/12/15 for open&close iperf in FT1

int UUTIOInitial()
{   
//	gUUTCom.open(1,115200);
	return 1;
}

int DeleteAFile(char *FileName)
{
	if(!PathFileExists(FileName))
	{
		return 1;
	}
	else
	{
		if(!DeleteFile(FileName))
		{
			amprintf("Delete %s fail\n",FileName);
			return 0;
		}
		else
		{

		}
	}
	return 1;
}

int MultiInitial()
{   
	char FileName[512]="";
	memset(FileName,0,sizeof(FileName));
	sprintf_s(FileName,sizeof(FileName),"%s\\testing.txt",gTI.TestStaInfo.LocalFilePath);
	if(!DeleteAFile(FileName))
	{
		return 0;
	}

	return 1;
}

void UUTIOUnInitial()
{
//	gUUTCom.close();
	printf("IO Uninitial process.\n");
	return;
}


void IQUnInitial()//Maxwell 20110326
{
	HWND killWin=NULL;
	HWND killWin1=NULL;
	int f=1;
	int f1=1;

	while (f)
	{
		killWin=NULL;
		killWin = ::FindWindow(NULL,"IQ");
		if (killWin)
		{
			f=1;
			::SendMessage(killWin,WM_CLOSE,NULL,NULL);
		}
		else
		{
			f=0;
		}

		Sleep(500);
	}

	while (f1)
	{

		killWin1=NULL;
		killWin1 = ::FindWindow(NULL,"CMD");
		if (killWin1)
		{
			f1=1;
			::SendMessage(killWin1,WM_CLOSE,NULL,NULL);
		}
		else
		{
			f1=0;
		}
		Sleep(500);
	}


	return;
}

//Add start by Maxwell 20110326 for call NCArt auto
int RunNCart()
{
	IQUnInitial(); //Close cmd and IQ.BAT 

	if (1==atoi(gTI.TestStaInfo.NCArtFlag))
	{
		runShellExecute(gTI.TestStaInfo.NartCmd);
		//runShellExecute(gTI.TestStaInfo.CartCmd);
	}

	else if (2==atoi(gTI.TestStaInfo.NCArtFlag))
	{
		runShellExecute(gTI.TestStaInfo.NartCmd);
		runShellExecute(gTI.TestStaInfo.CartCmd);
	}
	else
	{
		//return 0;
	}
	return 1;

}
//Add start by Maxwell 20110326 for call NCArt auto

// Add by liu-chen for K31 PT2 "manufactruring_flow" and "path_loss" down load.
int BinFolderInitial()
{
	char srcFile[256]="";
	char desFile[256]="";


	sprintf_s(srcFile,sizeof(srcFile),"%s\\%s",gTI.TestStaInfo.LocalFilePath,"path_loss.csv");

	sprintf_s(desFile,sizeof(desFile),"%s\\Bin\\%s",gTI.TestStaInfo.LocalFilePath,"path_loss.csv"); 

	if(!CopyFile(srcFile,desFile,FALSE))
	{
		amprintf("Failed to copy path_loss.csv;");
		return 0;
	}


	sprintf_s(srcFile,sizeof(srcFile),"%s\\%s",gTI.TestStaInfo.LocalFilePath,"manufacturing_flow_K31_PT2_flow.txt");

	sprintf_s(desFile,sizeof(desFile),"%s\\Bin\\%s",gTI.TestStaInfo.LocalFilePath,"manufacturing_flow_K31_PT2_flow.txt"); 

	if(!CopyFile(srcFile,desFile,FALSE))
	{
		amprintf("Failed to copy manufacturing_flow_K31_PT2_flow.txt;");
		return 0;
	}

	return 1;
}

int sendConfigToUI()
{
	list<TEST_ITEM>::iterator Cy;
	char sendData[2032]="";
	char temp[512]="";
	strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[START]");
	amprintf("%s;\n",sendData);

	int i=0;
	int count=0;

	i=1;
	strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)//FLAG=4
	{
		if(TFLAG_INI_RUN==(*Cy).Flag)
		{
			(*Cy).index=i;
			i++;
			count++;
			memset(temp,0,sizeof(temp));
			sprintf_s(temp,sizeof(temp),"%s|",(*Cy).Name);
			strcat_s(sendData,sizeof(sendData),temp);
			if(count>=20)
			{
				strcat_s(sendData,sizeof(sendData),"]");
				amprintf("%s;\n",sendData);
				count=0;
				strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
			}
		}
	}

	if(0!=count)
	{
		strcat_s(sendData,sizeof(sendData),"]");
		amprintf("%s;\n",sendData);
	}

	count=0;
	strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");


	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)//FLAG=1,2,3
	{
		if(TFLAG_FAIL_STOP==(*Cy).Flag || TFLAG_FAIL_CONTINUE==(*Cy).Flag || TFLAG_FAIL_PAUSE==(*Cy).Flag)
		{
			(*Cy).index=i;
			i++;
			count++;
			memset(temp,0,sizeof(temp));
			sprintf_s(temp,sizeof(temp),"%s|",(*Cy).Name);
			strcat_s(sendData,sizeof(sendData),temp);
			if(count>=20)
			{
				strcat_s(sendData,sizeof(sendData),"]");
				amprintf("%s;\n",sendData);
				count=0;
				strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
			}
		}
	}

	if(0!=count)
	{
		strcat_s(sendData,sizeof(sendData),"]");
		amprintf("%s;\n",sendData);
	}

	count=0;
	strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");


	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)//FLAG=5
	{
		if(TFLAG_END_RUN==(*Cy).Flag)
		{
			(*Cy).index=i;
			i++;
			count++;
			memset(temp,0,sizeof(temp));
			sprintf_s(temp,sizeof(temp),"%s|",(*Cy).Name);
			strcat_s(sendData,sizeof(sendData),temp);
			if(count>=20)
			{
				strcat_s(sendData,sizeof(sendData),"]");
				amprintf("%s;\n",sendData);
				count=0;
				strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
			}
		}
	}

	if(0!=count)
	{
		strcat_s(sendData,sizeof(sendData),"]");
		amprintf("%s;\n",sendData);
	}
	count=0;
	strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
	

	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)//FLAG=6
	{
		if(TFLAG_TERMINATE_RUN==(*Cy).Flag)
		{
			(*Cy).index=i;
			i++;
			count++;
			memset(temp,0,sizeof(temp));
			sprintf_s(temp,sizeof(temp),"%s|",(*Cy).Name);
			strcat_s(sendData,sizeof(sendData),temp);
			if(count>=20)
			{
				strcat_s(sendData,sizeof(sendData),"]");
				amprintf("%s;\n",sendData);
				count=0;
				strcpy_s(sendData,sizeof(sendData),"CONFIG=ITEM[");
			}
		}
	}

	if(0!=count)
	{
		strcat_s(sendData,sizeof(sendData),"]");
		amprintf("%s;\n",sendData);
	}

	amprintf("CONFIG=ITEM[END];\n");
	return 1;

}