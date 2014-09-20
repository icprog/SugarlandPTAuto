#ifndef TEST_ITEM_SCRIPT_H
#define TEST_ITEM_SCRIPT_H






class CTestItemScript
{
public:
	CTestItemScript(void);
public:
	~CTestItemScript(void);

public://for share value define
	struct 
	{
		int Flag;
		char PubBUF[5000];//Store global information 
	}GPubBUF;

	time_t SetHDSpinDownTime;//Check HD spin down time counter
	int SetHDSpindownFlag;
	int SetHDSpindownFailFlag;//defined for SetHDSpindown function to show whether the function has been runned and fail, added by Maxwell 2008/10/15

public:
	int WaitUUTUp(TEST_ITEM *pTI);//for detect the UUT boot-up
	int CheckUUTFWGM(TEST_ITEM *pTI);//Get F/W in between GM and PVT
	int SetUUTInfo(TEST_ITEM *pTI);
	int CheckUUTInfo(TEST_ITEM *pTI);
	int ReadUUTInfo(TEST_ITEM *pTI);
	int CheckUUTFW(TEST_ITEM *pTI);


	int CheckUutLed(TEST_ITEM *pTI);


	int SetUUTGmtTime(TEST_ITEM *pTI);
	int CheckUUTGmtTime(TEST_ITEM *pTI);

	int CheckUUTResetButton(TEST_ITEM *pTI);

	int CheckUUTUsb(TEST_ITEM *pTI);

	//link status check
	
	int CheckUUTLinkStatus(TEST_ITEM *pTI);//Talen 2011/10/28
	int LinkStatusParse(char *pPort, int LinkResult);

	int SetUUTStatus(TEST_ITEM *pTI);
	int CheckUUTStatus(TEST_ITEM *pTI);
	
	int CheckConnection(TEST_ITEM *pTI);
	int SetWifiAntenna(TEST_ITEM *pTI);
	int CheckWifiAntenna(TEST_ITEM *pTI);
	int CheckWifiTxThroughput(TEST_ITEM *pTI);
	int CheckWifiRxThroughput(TEST_ITEM *pTI);
	
	
	int CheckEthThroughput(TEST_ITEM *pTI);
	int CheckWiFiThroughPutInGolden(TEST_ITEM *pTI);


    int CheckLocalInfo(TEST_ITEM *pTI);
    int CheckDiagValue(TEST_ITEM *pTI);
	int CheckWanPortStatus(TEST_ITEM *pTI);
	int CheckAndSetDefault(TEST_ITEM *pTI);//Add by Maxwell 2008/01/19

    //For Time capsule test
    int CheckMLBBattery(TEST_ITEM *pTI);
	int CheckMLBBatteryUseVolt(TEST_ITEM *pTI);//Add by Maxwell 2008/10/14
	int CheckRadioStatus(TEST_ITEM *pTI);//Add by Maxwell 2008/11/13 for Check Radio Status
	int CheckHDDInformation(TEST_ITEM *pTI);
    int CheckHDDFormat(TEST_ITEM *pTI);
    int SetHDDFormat(TEST_ITEM *pTI);
    int SetHDSpinDown(TEST_ITEM *pTI);
    int CheckAndWaitHDSpinDown(TEST_ITEM *pTI);

	int CheckSystemModeStatus(TEST_ITEM *pTI);//Add by Maxwell 2008/12/26 for Check Mode Status
   	int CheckWifiRadioStatus(TEST_ITEM *pTI);//Add by Maxwell 2008/12/26 for Check Wifi Radio Status Use for 5G Radio

	int CheckTxRate5G(TEST_ITEM *pTI);//Add by Maxwell 2008/01/19 for Check Wifi Radio TXRate Use for 5G Radio
	int CheckTxRate2G(TEST_ITEM *pTI);//Add by Maxwell 2008/01/19 for Check Wifi Radio TXRate Use for 2G Radio

	int CheckSnInFile(TEST_ITEM *pTI);//Add by Maxwell 2008/05/08 for Check SN in local file

	int CheckFanSpin(TEST_ITEM *pTI);
    int CheckTemperatureSensor(TEST_ITEM *pTI);
	int CheckMlbTemperatureSensor(TEST_ITEM *pTI);
    int SetInstrumentStatus(TEST_ITEM *pTI);
    int RunDiagToolNoResult(TEST_ITEM *pTI);
	int RunDiagToolNoResultShow(TEST_ITEM *pTI);
	int OperatorIOInTesting(TEST_ITEM *pTI);
	int SetComTrans(TEST_ITEM *pTI);//627
	int CloseComTrans(TEST_ITEM *pTI);//627
	int OpenIperfInGolden(TEST_ITEM *pTI);//072509
	int AutoPowerOn(TEST_ITEM *pTI);//090901
	int AutoPowerOff(TEST_ITEM *pTI);//090901
	int AutoGoldenReboot(TEST_ITEM *pTI);//091005
	int CheckandSetWifiTXrate(TEST_ITEM *pTI);//091005
	int CheckSYSReboot(TEST_ITEM *pTI);//09/10/08
	int CheckHDD_SN(TEST_ITEM *pTI);//Add by Tony on 2009/12/17 for Get HD Serial Number
	int CheckWIFIMAC0(TEST_ITEM *pTI);//Add by Tony on 2009/12/23 for Send WIFIMAC0 on Sceen
	int CheckWIFIMAC1(TEST_ITEM *pTI);//Add by Tony on 2009/12/23 for Send WIFIMAC0 on Sceen
	int CheckSRCV(TEST_ITEM *pTI);//2010/02/14 for FW source version test
	int CheckminS(TEST_ITEM *pTI);//2010/03/18 for FW source version test
	int GenerateLabelInformation(TEST_ITEM *pTI);//Maxwell 100504 for M52 RMA on Apple site
	int CheckGOLDENRSSI2G(TEST_ITEM *pTI);//Maxwell100211
	int CheckGOLDENRSSI5G(TEST_ITEM *pTI);//Maxwell100211
	int CheckUUTRSSI(TEST_ITEM *pTI);//Talen 2011/10/31
	int ControlGoldenStatus(TEST_ITEM *pTI);//Maxwell 110212
	int SendGoldenMCSCMD(TEST_ITEM *pTI);//Maxwell 110212
	int CheckWifiNoiseFloor(TEST_ITEM *pTI);//Maxwell 110212
	int CheckUUTTXrate(TEST_ITEM *pTI);//Maxwell 110212
	int LogUUTCalData(TEST_ITEM *pTI);//Sam 110311
	int CheckK31TempSensor(TEST_ITEM *pTI);//Maxwell 20110326

	int RunIQProgram(TEST_ITEM *pTI);//Maxwell 20110326
	int DeletePathFile(TEST_ITEM *pTI);//Maxwell 20110326
	int CheckIQTestFinish(TEST_ITEM *pTI);//Maxwell 20110326
	int GenerateIQTestResultGRR(TEST_ITEM *pTI);//Maxwell 20110326
	int GetIQTestResult(TEST_ITEM *pTI);//Maxwell 20110326
	

    int BeginParaRun(ParallelPerfCallFunc pFunc,TEST_ITEM *pTI);

	int SwitchEthernet(TEST_ITEM *pTI);//Sam 110328
	int ClickUUTResetButton(TEST_ITEM *pTI);//Sam 110328
	int ClickUUTResetButtonNoCheck(TEST_ITEM *pTI);//Talen 2011/06/16
	int LEDCOLORTEST(TEST_ITEM *pTI);//Sam 110328
	int BlockPushFront(TEST_ITEM *pTI);//Sam 110328
	int BlockPullBack(TEST_ITEM *pTI);//Sam 110328
	int PanelPullUp(TEST_ITEM *pTI);//Sam 110328
	int PanelPushDown(TEST_ITEM *pTI);//Sam 110328

	int DownArtandCheck(TEST_ITEM *pTI);//Maxwell 20110328

	int SetGoldenConnect(TEST_ITEM *pTI);//Maxwell 20110329
	int SetAudioTest(TEST_ITEM *pTI);//Maxwell 20110329
	int WriteSNMACFile(TEST_ITEM *pTI);//Maxwell 20110330
	int CheckConnectionUsePing(TEST_ITEM *pTI);//add by Talen 2011/04/19
	int CtrlArtRun(TEST_ITEM *pTI);//add by Talen 2011/05/11
	int AutoCOMReboot(TEST_ITEM *pTI);//add by Talen 2011/05/12
	int CommitCaldataFlash(TEST_ITEM *pTI);//add by Talen 2011/05/18
	int AutoFixtureControl(TEST_ITEM *pTI);//Maxwell091127
	int AutoFixtureControlNoCheck(TEST_ITEM *pTI);//Talen 2011/06/16
	int SetUUTChannel(TEST_ITEM *pTI);//Talen 2011/05/25
	int GetArtVersion(TEST_ITEM *pTI);//Talen 2011/05/31
	int BlockControl(TEST_ITEM *pTI);//Talen 2011/06/14	
	int CheckLedCmd(TEST_ITEM *pTI);//haibin,li 2011/06/13
	int HiPotTest(TEST_ITEM *pTI);//haibin,li 2011/07/21
	int VisualInspection(TEST_ITEM *pTI);//haibin,li 2011/08/01
	int CheckLogoSN(TEST_ITEM *pTI);//haibin,li 2011/08/02
	int CheckLogoPosition(TEST_ITEM *pTI);//haibin,li 2011/08/02
	int CheckLogoDepth(TEST_ITEM *pTI);//haibin,li 2011/08/02
	int CheckLogoSkuMark(TEST_ITEM *pTI);//haibin,li 2011/08/02
	int CheckLogoSquareSize(TEST_ITEM *pTI);//haibin,li 2011/08/02
	int CheckSensorPosition(TEST_ITEM *pTI);//haibin,li 2011/08/23
	int CheckSKUTested(TEST_ITEM *pTI);//add by Talen 2011/06/23
	int CheckUUTDbugOff(TEST_ITEM *pTI);//add by Talen 2011/06/27
	int CheckUUTDbugOn(TEST_ITEM *pTI);//add by Talen 2011/06/29
	int CheckAndSetDbugOn(TEST_ITEM *pTI);//add by Talen 2011/06/27
	int CheckAndSetRadioOn(TEST_ITEM *pTI);//liu-chen 2011/06/27
	int WriteSNPNtoTxt(TEST_ITEM *pTI);//add by Talen 2011/07/04
	int CheckUUTch(TEST_ITEM *pTI);//Talen 2011/07/04
	int SetUUTCheckReturn(TEST_ITEM *pTI);//Talen 2011/07/07
	int RunProcessNoWait(TEST_ITEM *pTI);//Talen 2011/07/08
	int CheckProductWeight(TEST_ITEM *pTI);//add by Joyce 2011/07/08
	int CheckWanPortConnection(TEST_ITEM *pTI);//add by Talen 2011/08/15
	int CheckLedBehavior(TEST_ITEM *pTI);//add by Talen 2011/08/30
	int WaitForTest(TEST_ITEM *pTI);//add by Talen 2011/08/31
	int PerformPCLI(TEST_ITEM *pTI);//add by Talen 2011/08/31
	int KillProcess(TEST_ITEM *pTI);//add by Talen 2012/01/13
	int RunBatchFileAndWait(TEST_ITEM *pTI);//add by Talen 2012/01/18
	int PrintWipLabel(TEST_ITEM *pTI);//add by Liu-Chen for k31 Wip Label 2012/04/11

private:
	int GetSysDes();

};

#endif