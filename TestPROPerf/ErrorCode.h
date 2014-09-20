/*******************************************************************************************

	Contains:	IO Message format define

	Version:	1.10.100.001

	Copyright:	(C) 2007 by Ambit Micro, Inc., all rights reserved.

    Technology:			M52

	Change History (most recent first):


	Create by CPE2 RD 08/07/2007

	Note:
**********************************************************************************************/

#define TM00   "TM00\tSet UUT to Test Mode." 
#define DW00   "DW00\tDetect Wireless Card Fail." 
#define WM00   "WM00\tWrite WAN MAC Fail."
#define WS00   "WS00\tWrite SN Fail" 
#define RM00   "RM00\tRead WAN MAC Default MACID" 
#define RM01   "RM01\tRead WAN MAC NOT MATCH" 
#define RM02   "RM02\tRead WAN MAC fail - WAN MAC is ODD NUMBER"
#define RS00   "RS00\tRead SN EMPTY" 
#define RS01   "RS01\tRead SN NOT MATCH" 
#define RW00   "RW00\tRead WiFi MAC NOT MATCH"  



#define SL00   "SL00\tYELLOW SYSTEM LED DOES NOT LIGHT." 
#define SL10   "SL10\tYELLOW SYSTEM LED RGB VALUE IS OUT OF SPEC."                                        
#define SL01   "SL01\tGREEN SYSTEM LED DOES NOT LIGHT." 
#define SL11   "SL11\tGREEN SYSTEM LED RGB VALUE IS OUT OF SPEC."                                                 
#define SL02   "SL02\tBLUE SYSTEM LED DOES NOT LIGHT." 
#define SL12   "SL12\tBLUE SYSTEM LED RGB VALUE IS OUT OF SPEC."   
#define SL04   "SL04\tSYSTEM LED DOES NOT LIGHT OFF." 


#define EL00   "EL00\tWAN LED DOES NOT LIGHT." 
#define EL10   "EL10\tWAN LED RGB VALUE IS OUT OF SPEC."                                                            
#define EL03   "EL03\tLAN3 LED DOES NOT LIGHT."
#define EL13   "EL13\tLAN3 LED RGB VALUE IS OUT OF SPEC."                                                               
#define EL02   "EL02\tLAN2 LED DOES NOT LIGHT."
#define EL12   "EL12\tLAN2 LED RGB VALUE IS OUT OF SPEC."                                                               
#define EL01   "EL01\tLAN1 LED DOES NOT LIGHT."
#define EL11   "EL11\tLAN1 LED RGB VALUE IS OUT OF SPEC."                                                               
#define EL04   "EL04\tETHERNET LEDS do NOT LIGHT."                    //for PT ONLY      


#define FW00  "FW00\tCan not get FW version." 
#define FW01  "FW01\tFW version not correct."    

#define PL01  "PL01\tUUT DOES NOT REPLY PING BY REQUEST TIME ON LAN1."
#define PL02  "PL02\tUUT DOES NOT REPLY PING BY REQUEST TIME ON LAN2"
#define PL03  "PL03\tUUT DOES NOT REPLY PING BY REQUEST TIME ON LAN3."
#define PL00  "PL00\tUUT DOES NOT REPLY PING BY REQUEST TIME ON WAN."
#define PU00  "PU00\tUUT DOES NOT REPLY PING BY REQUEST TIME." 
#define PW55  "PW55\t(PWAC) UUT DOES NOT REPLY PING BY REQUEST TIME ON ANTENNA A+C." 
#define PW56  "PW56\t(PWBC) UUT DOES NOT REPLY PING BY REQUEST TIME ON ANTENNA B+C." 


#define WT15   "WT15\tWIRELESS (A+C)THROUGHPUT IS UNDER SPEC " //"WT15 WIRELESS (A+C)THROUGHPUT IS UNDER SPEC @ MCSx ,CHxx." 
#define WT16   "WT16\tWIRELESS (b+C)THROUGHPUT IS UNDER SPEC"  //"WT16 WIRELESS (b+C)THROUGHPUT IS UNDER SPEC @ MCSx ,CHxx"
#define LT02   "LT02\tLAN1 TO LAN2 TX THROUGHPUT IS UNDER SPEC ." 
#define LR02   "LR02\tLAN1 TO LAN2 RX THROUGHPUT IS UNDER SPEC ."   //Modify by Wind for easy understand
#define LT03   "LT03\tLAN1 TO LAN3 TX THROUGHPUT IS UNDER SPEC ." 
#define LR03   "LR03\tLAN1 TO LAN3 RX THROUGHPUT IS UNDER SPEC ."  //Modify by Wind for easy understand
#define LT00   "LT00\tLAN1 TO WAN TX THROUGHPUT IS UNDER SPEC ." 
#define LR00   "LR00\tLAN1 to WAN RX THROUGHPUT IS UNDER SPEC ."   //Modify by Wind for easy understand

#define BU00   "BU00\tUUT IS NOT DISCOVERED BY BONJOUR ." 
#define BU01   "BU01\tUUT IS NOT DISCOVERED BY BONJOUR AFTER QUICKLY REBOOT" 

#define LS04   "LS04\tWan Port linkstatus=0."  
#define LS14   "LS14\tWan Port linkstatus not equal to the expected value."                                               
#define LS01   "LS01\tLan Port 1 linkstatus=0." 
#define LS11   "LS11\tLan Port 1 linkstatus not equal to the expected value."                                                
#define LS02   "LS02\tLan Port 2 linkstatus=0." 
#define LS12   "LS12\tLan Port 2 linkstatus not equal to the expected value."                                                
#define LS03   "LS03\tLan Port 3 linkstatus=0." 
#define LS13   "LS13\tLan Port 3 linkstatus not equal to the expected value."
#define DC00   "DC00\tWRITE LOCALE EMPTY"
#define DC01   "DC01\tLOCALE NOT MATCH"


#define US00	"US00\tINTERNAL USB CONTROLLER CAN NOT BE FOUND." 
#define US01	"US01\tHIGH SPEED DEVICE CAN NOT BE FOUND." 
#define US11	"US11\tFOUND UNEXPECTED HIGH SPEED USB DEVICE ."                                                         
#define US02    "US02\tFULL SPEED DEVICE CAN NOT BE FOUND." 
#define US12    "US12\tFOUND UNEXPECTED FULL SPEED USB DEVICE ."                                                           
#define US03    "US03\tLOW SPEED DEVICE CAN NOT BE FOUND." 
#define US13    "US13\tFOUND UNEXPECTED LOW SPEED USB DEVICE ."                                                          


#define SA05   "SA05\tSwitch Antenna A+C  Fail. raRX return VALUE NOT EQUAL TO 5 ." 
#define SA06   "SA06\tSwitch Antenna b+C  Fail. raRX return VALUE NOT EQUAL TO 6 ." 


#define RB00   "RB00\tRESET BUTTON NO RESPONSE." 

#define OF00   "OF00\tCan not Open Fixture ." 
#define CF00   "CF00\tCan not Close Fixture." 

#define RT00   "RT00\tTHE DIFFERENCE BETWEEN TIME READ FROM UUD AND TIME SERVER IS LARGER THAN 10 SEC."  
#define RT01   "RT01\tTHE DIFFERENCE BETWEEN DATE READ FROM UUD AND TIME SERVER IS LARGER THAN 24H."  
#define RT02   "RT02\tRead PC Local Time Error."   //Add for get local time error
#define DG00   "DG00\tFLASH CAN NOT BE PROGRAMMED"


#define TS00   "TS00\tEXTERNAL TEMP SENSOR return VALUE < 15." 
#define TS01   "TS01\tMLB TEMP SENSOR CHIP return VALUE < 15." 
#define FS00   "FS00\tFAN SPIN SPEED return VALUE OUT OF SPEC." 
#define HD00   "HD00\tNO HD DETECTED." 
#define HD01   "HD01\tHD MODEL IS NOT EXPECTED."                                                      
#define HD02   "HD02\tHD FW VERSION IS NOT EXPECTED." 
#define HD03   "HD03\tHD Capacity IS NOT EXPEXTED." 
#define HD04   "HD04\tHD No Wiping."       //Add by Tony on 2009/12/17 for K30 RMA.
#define FD00   "FD00\tFORMATED HD SIZE OUT OF SPEC." 


#define BT00   "BT00\tBATTERY TEST OUT OF SPEC." 
#define AP00   "AP00\tSET GET ACP PROPERTY ERROR"