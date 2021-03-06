#include "StdAfx.h"
#include "ScriptSupport.h"
#include "CommSocket.h"
#include "com_class.h" //627

extern CCommSocketClient DeviceAT100Client;
extern _asyn_com gComDev;//627
extern TEST_INPUT_INFO gTI;//test input information
extern HANDLE hOperateComMutex;//Talen 2011/09/01
extern int fix_id;
extern list<TEST_ITEM> TItemList;//Talen 2011/09/03
// use parse the xml file 
//add by jeffrey 09/21 
int ParseProperyResult(char *SeachStr, char *Rtn, int RtnLenth, char *KeyWord, char *InType)
{
    char *AnaBufTemp=NULL;

	size_t AnaSize=0;

	char *pToke = NULL;
	char *pTokeK = NULL;
	char *pTokeEnd = NULL;
	char TypeEnd[128] = "";
	int iCopyIndex = 0;
	int iFindSQIndex = 0;
	int iFindNum = 0;
	char *pAToke[512];
	char *Type = NULL;
	char SeachKey[100]="";

	if (!SeachStr || !Rtn || !KeyWord || !InType)
	{
		return 0;
	}

	if(!strstr(SeachStr,KeyWord))
	{
		return 0;
	}
	else
	{
		char *pok = strstr(SeachStr,KeyWord);
			if(!strstr(pok,InType))
			{
				return 0;
			}
			else
			{

			}
	}
    
    Type = InType;
	//prepare toke end str
	while (*Type)
	{
		TypeEnd[iCopyIndex++] = *Type++;
		if (1 == iCopyIndex)
		{
            TypeEnd[iCopyIndex++] = '/';
		}
	}
	
	
	ZeroMemory(pAToke, sizeof(pAToke));
    
	AnaSize=strlen(SeachStr);

	AnaBufTemp=new char[AnaSize+1];
	if (!AnaBufTemp)
	{
		return 0;
	}
	ZeroMemory(AnaBufTemp, AnaSize+1);
	strcpy_s(AnaBufTemp, AnaSize+1, SeachStr);
	pToke = strstr(AnaBufTemp, KeyWord);
	
	if (NULL == pToke)
	{
		if (AnaBufTemp)
		{
			delete AnaBufTemp;
		}
		return 0;
	}

	while (pToke)
	{
         
		pToke = strstr(pToke, KeyWord);
		if (pToke)
		{
			for (int i=0; i<100;i++)
			{
				if ('<'==*(pToke+i))
				{
					break;
				}
				SeachKey[i]=*(pToke+i);
			}
					
			if (!strncmp(SeachKey,KeyWord,strlen(SeachKey)))
			{
			    pAToke[iFindSQIndex++] = pToke;
			}
			pToke +=strlen(KeyWord);
		}
	
	}

	iFindSQIndex = 0;
	while (pToke = pAToke[iFindSQIndex++])
	{

		if (pToke)
		{
			pToke = strstr(pToke, InType);
			pTokeEnd = strstr(pToke, TypeEnd);
			if (pToke)
			{
				pToke = pToke + strlen(InType);
				for (iCopyIndex = 0;iCopyIndex<RtnLenth;iCopyIndex++)
				{
					*Rtn++ = *pToke++;
					if (pTokeEnd == pToke)
					{
						*Rtn++ =',';
						iFindNum++;
						break;
					}
				}
			}
		}
		else
		{
			continue;
		}
	}
	*Rtn = 0;
	if (AnaBufTemp)
	{
		delete AnaBufTemp;
	}
	return iFindNum;
}
int GetXmlIndexValue(char *pBuf, int iIndex, int *pVal)
{
	int val = 0;
	char *pToke = 0;
	int LoopIndex = 0;

	char Buf[256]="";

	if (!pVal)
	{
		return 0;
	}
	strcpy_s(Buf, sizeof(Buf), pBuf);

	if (0 == iIndex)
	{	
		val = atoi(Buf);
	}

	for (LoopIndex = 0; LoopIndex < iIndex; LoopIndex++)
	{

		if (0 == LoopIndex)
		{
			pToke = strstr(Buf,",");
			pToke +=1;
			val = atoi(pToke);
		}
		else
		{
			pToke = strstr(pToke,",");
			if (pToke)
			{
				val = atoi(pToke+1);
			}
			else
			{
                return 0;
			}

		}
	}
	
	*pVal = val;
	return 1;
}
int GetXmlIndexStr(char *pBuf, int iIndex, char Tag, char *pVal, int RtnSize)
{
	char *pval = 0;
	char *pToke = 0;
	int LoopIndex = 0;

	char Buf[500]="";

	if (!pVal)
	{
		return 0;
	}
	strcpy_s(Buf, sizeof(Buf), pBuf);
	pToke=Buf;
	for (LoopIndex = 0;LoopIndex < iIndex; LoopIndex++)
	{
		pToke = strchr(pToke,Tag);
		if (pToke)
		{
			if (0 == LoopIndex)
			{	
				strSTEDcpy(pVal, RtnSize,Buf,0xff,Tag);
			}
			if (0 != LoopIndex)
			{	
				strSTEDcpy(pVal, RtnSize,pToke+1,0xff,Tag);
				pToke+=1;
			}
		}
	}
	return 1;
}

int HconverD(char* pHStr,int hstrlength)
{
    int num=0;
	for(int i=0;i<hstrlength;i++)
	{
		int j=SinglecharConver(pHStr+i);
		int k=(int)pow(16.0,hstrlength-i-1);
		num+=j*k;			
	}
	return num;

}

int SinglecharConver(char* pch)
{
	char ch=*pch;
	
	if('a'==ch||'A'==ch)
	{
		return 10;
	}
	else if('b'==ch||'B'==ch)
	{
		return 11;
	}
	else if('c'==ch||'C'==ch)
	{
		return 12;
	}
	else if('d'==ch||'D'==ch)
	{
		return 13;
	}
	else if('e'==ch||'E'==ch)
	{
		return 14;
	}
	else if('f'==ch||'F'==ch)
	{
		return 15;
	}
	else
	{
		return atoi(&ch);

	}
}
unsigned char HEXConvert (unsigned char ucHEX)
{
	unsigned char A = 'A', a='a',zero = '0';
	
	if (ucHEX >= a) return (ucHEX-a+10);
	if (ucHEX >= A) return (ucHEX-A+10);
	if (ucHEX >= zero) return (ucHEX-zero);

	return 0;
}
int StrHEXConvert(unsigned char *pDes, char *psrc, int iCovertNum)
{

	int i = 0;
	int j = 0;
	unsigned char ByteTemp1 = 0;
	unsigned char ByteTemp2 = 0;
	unsigned char ByteTemp = 0;
	if (!pDes || !psrc)
	{
		return 0;
	}
	for (i = 0,j = 0; i < iCovertNum; i+=2, j++)
	{
        ByteTemp1 = HEXConvert (*(psrc + i));
		ByteTemp2 = HEXConvert (*(psrc + i + 1));
		ByteTemp = ByteTemp1 << 4 | ByteTemp2;
		*(pDes + j) = ByteTemp;
	}
	return i;

}



int RunMultAcp(char* IP,char* pCommand,char* DesBuf,int BufLen,int Run_Type/*1Get 2 Set 3 no wait*/)
{
	char OutBuf[2048];
	char AnaTemp[2048];
	char pTokeTemp[1024] ;
	DWORD dwExitCode;
	char *pToke = NULL;
	char RunBuf[128]="";

	memset(OutBuf,0,sizeof(OutBuf));
	memset(AnaTemp,0,sizeof(AnaTemp));
	memset(pTokeTemp,0,sizeof(pTokeTemp));
	//strncpy_s(configstu.testinfo.Custom_SN,sizeof(configstu.testinfo.Custom_SN),gUUTInfo.UUTSN,strlen(gUUTInfo.UUTSN));
    sprintf_s(RunBuf,sizeof(RunBuf),"acp.exe -a %s -p public %s",IP,pCommand);
	amprintf( "ACP:  %s;\n", RunBuf );

	if(RA_GET==Run_Type)
	{
		if (!RunExeFileLocal(RunBuf, DesBuf,BufLen, &dwExitCode))
		{
			//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",FW00);
			return 0;		
		}
	}
	else if(RA_SET==Run_Type)
	{
		if (!RunExeFileNoOutput(RunBuf, DesBuf,BufLen, &dwExitCode))
		{
			//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",FW00);
			return 0;		
		}

	}
	else if(RA_NWAIT==Run_Type)
	{
		if (!RunExeFileNoWait(RunBuf, DesBuf,BufLen, &dwExitCode))
		{
			//sprintf_s(configstu.testinfo.Error_Code,sizeof(configstu.testinfo.Error_Code),"%s",FW00);
			return 0;		
		}
	}
	
	return 1;
}
int GetsubStrInStr(char*DesBuf,int BufLen,char*SourceBuf,char*pStartStr,char*pEndStr)
{
	char *pStart=NULL;
	char *pEnd=NULL;
	//if ((!DesBuf)||(!DesBuf))
	if ((!DesBuf)||(!SourceBuf))
	{
		return 0;
	}
	pStart=strstr(SourceBuf,pStartStr);
	if(!pStart)
	{
		return 0;
	}

	pEndStr=strstr(pStart+strlen(pStartStr),pEndStr);
	if(!pStart||!pEndStr)
	{
        return 0;
	}
	if((pEndStr-pStart-strlen(pStartStr))>=(unsigned)BufLen)
	{
		return 0;
	}
	strncpy_s(DesBuf,BufLen,pStart+strlen(pStartStr),pEndStr-pStart-strlen(pStartStr));
	return 1;	
}

int strSTEDcpy(char *pDes, int DesSize, const char *pSrc,const char start, const char end)
{

	char *pToke=NULL;
	char *pTempDes=NULL;
	char Temp[1024]="";
	int i=0;
	if ((!pDes)||(!pSrc))
	{
		return 0;
	}
	pTempDes=pDes;
	strcpy_s(Temp, sizeof(Temp), pSrc);
	pToke=strchr(Temp, start);
	if (pToke)
	{
		pToke+=1;
		for (i=0;i<1024;i++)
		{
			if ((end==*(pToke+i))||(0==*(pToke+i)))
			{
				*(pTempDes+i)=0;
				break;
			}
			*(pTempDes+i)=*(pToke+i);
		}
	}
	else
	{
		for (i=0;i<1024;i++)
		{
			if ((end==*(Temp+i))||(0==*(Temp+i)))
			{
				*(pTempDes+i)=0;
				break;
			}
			*(pTempDes+i)=*(Temp+i);
		}	
	}

	return 1;
}

int strAllCmp(const char *p1, const char *p2)
{
	size_t len;
	len=strlen(p1);
	if (strlen(p2)>len)
	{
		len=strlen(p2);
	}
	return (strncmp(p1,p2,len));
}

bool AddquteToMACAddress(char* newMACAddress,int lenMACAddress,char* pMAC)
{
	if(!pMAC)
	{
		return false;
	}
	if(12!=strlen(pMAC))
	{
		return false;
	}
	for(int i=0;i<6;i++)
	{
		strncat_s(newMACAddress,lenMACAddress,pMAC+i*2,2);
		strcat_s(newMACAddress,lenMACAddress,":");
	}
	*(newMACAddress+17)=0;
	return true;	

}
void DateFormatC(char *buf, char chr,char aftchar)
{
	char *pTok=0;
	pTok=buf;
	while (pTok)
	{
		pTok=strchr(pTok,chr);
		if (pTok)
		{
			*pTok=aftchar;
		}
	}
}

int DateTimeFormat(char *date, char* time,char *datetime)
{
	char year[16]="";
	char month[16]="";
	char day[16]="";
	char hour[16]="";
	char minute[16]="";
	char second[16]="";
	int flag_1=0;
	int flag_2=0;
	if(date)
	{
		char * pos_1=strstr(date,"/");
		if(pos_1)
		{
			memcpy(month,date,pos_1-date);
			if(*(pos_1+1)!='\0')
			{
				char * pos_2=strstr(pos_1+1,"/");
				if(pos_2)
				{
					memcpy(day,pos_1+1,pos_2-pos_1-1);
					memcpy(year,pos_2+1,date+strlen(date)-pos_2);
					flag_1=1;
				}
			}	
		}
	}
	if(time)
	{
		char *pos_1=strstr(time,":");
		if(pos_1)
		{
			memcpy(hour,time,pos_1-time);
			if(*(pos_1+1)!='\0')
			{
				char * pos_2=strstr(pos_1+1,":");
				if(pos_2)
				{
					memcpy(minute,pos_1+1,pos_2-pos_1-1);
					memcpy(second,pos_2+1,time+strlen(time)-pos_2);
					flag_2=1;

				}
			}	
		}
	}
	if(flag_1&&flag_2)
	{
		memset(datetime,0,sizeof(datetime));
		sprintf(datetime,"%s%s%s%s%s%s",year,month,day,hour,minute,second);
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;
}

int GetCurrentCh(char* Buf,int &nCh2G,int &nCh5G)
{
	char * pos1=strstr(Buf,"raCh");
	if(pos1)
	{
		char* pos2=strstr(pos1,"<integer>");
		if(pos2)
		{
			nCh2G=atoi(pos2+strlen("<integer>"));
			char * pos3=strstr(pos2,"raCh");
			if(pos3)
			{
				char * pos4=strstr(pos3,"raCh");
				if(pos4)
				{
					char* pos5=strstr(pos4,"<integer>");
					if(pos5)
					{
						nCh5G=atoi(pos5+strlen("<integer>"));
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 0;
}


int PullUpPanel1(int port)//port 9
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
			
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int PullUpPanel2(int port)//port 13
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int PushDownPanel1(int port)//port 9
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int PushDownPanel2(int port)//port 13
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;

}

int SetResetButton1(int m_time,int port)//port 2
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(m_time);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(50);
		return 1;
	}
	return 0;

}

int SetResetButton2(int m_time,int port)//port 6
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(m_time);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(50);
		return 1;
	}
	return 0;

}

int PushFrontBlock1(int port)//port 1
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;

}

int PushFrontBlock2(int port)//port 5
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int PullBackBlock1(int port)//port 1
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int PullBackBlock2(int port)//port 5
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",port);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;

}
/*********************************************************************************************/
//Add by Sam 2011/3/30
int LEDShowPassStatus(void)//yellow led port 5 greed led port 9  red led port 13
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",5);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		//sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",9);
		//DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		//Sleep(5);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",13);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);

		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",9);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int LEDShowFailStatus(void)//yellow led port 5 greed led port 9  red led port 13
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",5);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",9);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		//sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",13);
		//DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		//Sleep(5);

		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",13);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}

int LEDShowTestingStatus(void)//yellow led port 5 greed led port 9  red led port 13
{
	if(DeviceAT100Client.iConnectSerFlag)
	{
		char cmd[64]="";
		//sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",5);
		//DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		//Sleep(5);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",9);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		sprintf(cmd,"RELAY_CTL_OFF:%d;\r\n",13);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);

		sprintf(cmd,"RELAY_CTL_ON:%d;\r\n",5);
		DeviceAT100Client.CommWrite(cmd,strlen(cmd));
		Sleep(10);
		return 1;
	}
	return 0;
}
/*********************************************************************************************/


int ParseLEDDetect(int *spec,int led_port,char *result)
{
	sprintf_s(result,200,"%d,%d,%d,%d",-999999,-999999,-999999,-999999);
	char temp_buff[5120]="";
	sprintf(temp_buff,"LEDDETECT_CTL_SELECT:%d;\r\n",led_port);
	DeviceAT100Client.CommWrite(temp_buff,strlen(temp_buff));
	amprintf(temp_buff);

	Sleep(50);
	char CmdBuf[200]="";
	char ResBuf[200]="";
	DeviceAT100Client.CommRead(ResBuf,sizeof(ResBuf));
	Sleep(50);

	sprintf_s(CmdBuf,200,"LEDDETECT_CTL_RGB:;");
	strcpy_s(ResBuf,200,"");
	DeviceAT100Client.CommWrite(CmdBuf,strlen(CmdBuf));
	Sleep(100);
	DeviceAT100Client.CommRead(ResBuf,sizeof(ResBuf));
	amprintf("LEDDETECT");
	amprintf(ResBuf);
    Sleep(5);

	int r_value=0;
	int g_value=0;
	int b_value=0;
	int i_value=0;


	char *pos=strstr(ResBuf,"R:");
	if(pos)
	{
		r_value=atoi(pos+2);
	}

	pos=NULL;
	pos=strstr(ResBuf,"G:");
	if(pos)
	{
		g_value=atoi(pos+2);
	}

	pos=NULL;
	pos=strstr(ResBuf,"B:");
	if(pos)
	{
		b_value=atoi(pos+2);
	}

	pos=NULL;
	pos=strstr(ResBuf,"I:");
	if(pos)
	{
		i_value=atoi(pos+2);
	}


	//char result[200]="";
	//sprintf_s(result,200,"R:%d G:%d B:%d I:%d",r_value,g_value,b_value,i_value);
	//sprintf_s(result,200,"%d,%d,%d,%d,",r_value,g_value,b_value,i_value);
	//amprintf(result);


	if(i_value<=0)
	{
		i_value=0;
		r_value=0;
		g_value=0;
		b_value=0;

	}
	else
	{
		r_value=255*r_value/i_value;
		g_value=255*g_value/i_value;
		b_value=255*b_value/i_value;
		if(r_value>255)
		{
			r_value=255;
		}
		if(g_value>255)
		{
			g_value=255;
		}
		if(b_value>255)
		{
			b_value=255;
		}
	}

	sprintf_s(result,200,"%d,%d,%d,%d",r_value,g_value,b_value,i_value);
	amprintf(result);



	if((i_value<*spec)||(i_value>*(spec+1)))
	{
		return 0;
	}
	if((r_value<*(spec+2))||(r_value>*(spec+3)))
	{
		return 0;
	}
	if((g_value<*(spec+4))||(g_value>*(spec+5)))
	{
		return 0;
	}
	if((b_value<*(spec+6))||(b_value>*(spec+7)))
	{
		return 0;
	}
	return 1;
}
////////////////unsigned int _stdcall ThreadForDownArt(LPVOID lpPara)
////////////////{
////////////////	HWND ToolWin=NULL;
////////////////	char exitcmd[10]="exit\r";
////////////////	int i=0;
////////////////	int count=300;
////////////////	int sendsuccess = 100000;
////////////////	LRESULT result;
////////////////
////////////////	//Sleep(5000);
////////////////	while(!ToolWin&&count--)
////////////////	{
////////////////		amprintf("%d\n",count);
////////////////		//Sleep(5000);
////////////////		ToolWin=NULL;
////////////////		//ToolWin = ::FindWindow(NULL,"WinSCP");
////////////////		
////////////////		ToolWin = ::FindWindowEx(NULL,NULL,NULL,"WinSCP");
////////////////
////////////////		amprintf("%d;\n",ToolWin);
////////////////		amprintf("::FindWindow after;\n");
////////////////		if(ToolWin)
////////////////		{
////////////////
////////////////			//::SetWindowPos(ToolWin,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE||SWP_SHOWWINDOW);
////////////////			//Sleep(10);
////////////////			//::SetWindowPos(ToolWin,HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE||SWP_SHOWWINDOW);
////////////////			//amprintf("SetWindowPos;\n");
////////////////			//Sleep(10);
////////////////			////::SendMessage(ToolWin,WM_ACTIVATE,NULL,NULL);
////////////////			////Sleep(10);
////////////////			////::SendMessage(ToolWin,WM_SETFOCUS,NULL,NULL);
////////////////			////Sleep(10);
////////////////			//SetFocus(ToolWin);
////////////////			//amprintf("SetFocus;\n");
////////////////			//::SetActiveWindow(ToolWin);
////////////////			//amprintf("SetActiveWindow;\n");
////////////////			//Sleep(10);
////////////////			//::SetForegroundWindow(ToolWin);
////////////////			//::SendMessage(ToolWin,WM_CLOSE,NULL,NULL);
////////////////
////////////////
////////////////			//Sleep(3000);
////////////////			amprintf("find WinSCP window;\n");
////////////////			//SendMessageCallback(ToolWin,WM_CHAR,'U',1,NULL,NULL);
////////////////			//::PostMessage(ToolWin,WM_CHAR,'U',1);
////////////////			result = SendMessageTimeout(ToolWin,WM_CHAR,'U',1,SMTO_NORMAL,2000,NULL);
////////////////			//::SendMessage(ToolWin,WM_CHAR,'U',1);
////////////////			amprintf("result=%d;\n",result);
////////////////			amprintf("send \"U\";\n");
////////////////			Sleep(50);
////////////////			SendMessageCallback(ToolWin,WM_CHAR,'\r',1,NULL,NULL);
////////////////			//SendMessageTimeout(ToolWin,WM_CHAR,'\r',1,SMTO_NORMAL,2000,NULL);
////////////////			//::PostMessage(ToolWin,WM_CHAR,'\r',1);
////////////////			//::SendMessage(ToolWin,WM_CHAR,'\r',1);
////////////////			amprintf("send \\r;\n");
////////////////			Sleep(50);
////////////////			i=0;
////////////////			while(exitcmd[i]!='\0')
////////////////			{
////////////////				SendMessageCallback(ToolWin,WM_CHAR,exitcmd[i],1,NULL,NULL);
////////////////				amprintf("%c\n",exitcmd[i]);
////////////////				//::SendMessage(ToolWin,WM_CHAR,exitcmd[i],1);
////////////////				i++;
////////////////				Sleep(50);
////////////////			}
////////////////			amprintf("send exit;\n");
////////////////			Sleep(100);
////////////////			if(::FindWindow(NULL,"WinSCP"))
////////////////			{
////////////////				if(0!=result)
////////////////				{
////////////////					break;
////////////////				}
////////////////				if(0 == sendsuccess)
////////////////				{
////////////////					break;
////////////////				}
////////////////				sendsuccess--;
////////////////				ToolWin=NULL;
////////////////				continue;
////////////////			}
////////////////			break;
////////////////		}
////////////////		Sleep(100);
////////////////	}
////////////////
////////////////	if(0>=count)
////////////////	{
////////////////		amprintf("can not find WinSCP window;\n");
////////////////		return 0;
////////////////	}
////////////////	amprintf("XXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
////////////////	return 1;
////////////////}


unsigned int _stdcall ThreadForDownArt(LPVOID lpPara)
{
	HWND ToolWin=NULL;
	char exitcmd[10]="exit\r";
	int i=0;
	int count=200;
	int sendsuccess = 200;
	LRESULT result;

	while(!ToolWin&&count--)
	{
		ToolWin=NULL;
		ToolWin = ::FindWindow(NULL,"WinSCP");
		
		amprintf("%d;\n",ToolWin);
		amprintf("::FindWindow after;\n");
		if(ToolWin)
		{

			result = SendMessageTimeout(ToolWin,WM_CHAR,'U',1,SMTO_NORMAL,2000,NULL);
			//::SendMessage(ToolWin,WM_CHAR,'U',1);
			amprintf("result=%d;\n",result);
			amprintf("send \"U\";\n");
			Sleep(50);
			//result *= SendMessageTimeout(ToolWin,WM_CHAR,'\r',1,SMTO_NORMAL,1000,NULL);
			SendMessageCallback(ToolWin,WM_CHAR,'\r',1,NULL,NULL);
			Sleep(50);
			//result *= SendMessageTimeout(ToolWin,WM_CHAR,'Y',1,SMTO_NORMAL,1000,NULL);
			SendMessageCallback(ToolWin,WM_CHAR,'Y',1,NULL,NULL);
			Sleep(50);
			//result *= SendMessageTimeout(ToolWin,WM_CHAR,'\r',1,SMTO_NORMAL,1000,NULL);
			SendMessageCallback(ToolWin,WM_CHAR,'\r',1,NULL,NULL);
			//SendMessageTimeout(ToolWin,WM_CHAR,'\r',1,SMTO_NORMAL,2000,NULL);
			//::PostMessage(ToolWin,WM_CHAR,'\r',1);
			//::SendMessage(ToolWin,WM_CHAR,'\r',1);
			amprintf("send \\r;\n");
			Sleep(50);
			i=0;
			while(exitcmd[i]!='\0')
			{
				//result *= SendMessageTimeout(ToolWin,WM_CHAR,exitcmd[i],1,SMTO_NORMAL,1000,NULL);
				SendMessageCallback(ToolWin,WM_CHAR,exitcmd[i],1,NULL,NULL);
				amprintf("%c\n",exitcmd[i]);
				//::SendMessage(ToolWin,WM_CHAR,exitcmd[i],1);
				i++;
				Sleep(50);
			}
			amprintf("send exit;\n");
			Sleep(100);
			if(::FindWindow(NULL,"WinSCP"))
			{
				if(0!=result)
				{
					break;
				}
				if(0 == sendsuccess)
				{
					break;
				}
				sendsuccess--;
				continue;
			}
			break;
		}
		Sleep(100);
	}

	if(0>=count||0>=sendsuccess)
	{
		amprintf("can not find WinSCP window;\n");
		return 0;
	}
	amprintf("XXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
	return 1;
}


int MoveBlock(int ComPort,int TransRate,char *Cmd,char *Timeout,char *Spec)
{
	char cmd[500]="";
	char revbuf[512]=""; 
	int revbufLen = 0;
	int ComTime=50;
	DWORD dwWaitResult;
	int errcode;

	strcpy_s(cmd,sizeof(cmd),Cmd);

	WaitForSingleObject(hOperateComMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(ComPort,TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",ComPort,TransRate);
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",ComPort,TransRate);

	////size_t m = strlen(cmd);
	////
	////strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd, (int)strlen(cmd)))
	{
		amprintf("Com command write %s;\n",cmd);
		//return 0;
	}
	else
	{
		gComDev.close();
		amprintf("Com command %s write fail;\n",cmd);
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	Sleep(200);
	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	revbufLen = 0;
	while(revbufLen==0&&i<atoi(Timeout))
	{
		gComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(i==atoi(Timeout))
	{
		amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
		//gComDev.close();
		//return 0;
	}

	amprintf("Auto fixture reply: %s!\n",revbuf);
	if (strlen(revbuf)&&strstr(Spec,revbuf))
	{
		amprintf("Right Command Reply !\n");
	}
	////if (revbuf[0]==pTI->Spec[0])
	////{
	////	amprintf("Right Command Reply !\n");
	////}
	else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
	{
		amprintf("SENSOR=RESPONSE[%s];",revbuf);
		Sleep(99999999);
	}
	else
	{
		amprintf("Wrong Command Reply !\n");
		
		if(!gComDev.write(cmd, (int)strlen(cmd)))
		{
			amprintf("Com command write %s;\n",cmd);
			//return 0;
		}
		else
		{
			gComDev.close();
			amprintf("Com command %s write fail;\n",cmd);
			amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
			amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			ResetEvent(gTI.UiResMsg.hEvent);
			ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
			KillTargetProcess("AmbitProT.exe");
			_exit(errcode);
			return 0;
		}
		Sleep(200);
		memset(revbuf,'\0',sizeof(revbuf));
		int i = 0;
		revbufLen = 0;
		while(revbufLen==0&&i<atoi(Timeout))
		{
			gComDev.read(revbuf,(int)sizeof(revbuf));
			revbufLen = (int)strlen(revbuf);
			Sleep(100);
			i++;
		}
		if(i==atoi(Timeout))
		{
			amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
			gComDev.close();
			//return 0;
		}

		amprintf("Auto fixture reply: %s!\n",revbuf);
		if (strlen(revbuf)&&strstr(Spec,revbuf))
		{
			amprintf("Right Command Reply !\n");
		}
		////if (revbuf[0]==pTI->Spec[0])
		////{
		////	amprintf("Right Command Reply !\n");
		////}
		else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
		{
			amprintf("SENSOR=RESPONSE[%s];",revbuf);
			Sleep(99999999);
		}
		else
		{
			amprintf("Wrong Command Reply !\n");

			if(!gComDev.write(cmd, (int)strlen(cmd)))
			{
				amprintf("Com command write %s;\n",cmd);
				//return 0;
			}
			else
			{
				gComDev.close();
				amprintf("Com command %s write fail;\n",cmd);
				amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
				amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
				dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
				ResetEvent(gTI.UiResMsg.hEvent);
				ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
				KillTargetProcess("AmbitProT.exe");
				_exit(errcode);
				return 0;
			}
			Sleep(200);
			memset(revbuf,'\0',sizeof(revbuf));
			int i = 0;
			revbufLen = 0;
			while(revbufLen==0&&i<atoi(Timeout))
			{
				gComDev.read(revbuf,(int)sizeof(revbuf));
				revbufLen = (int)strlen(revbuf);
				Sleep(100);
				i++;
			}
			if(i==atoi(Timeout))
			{
				amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
				gComDev.close();
				//return 0;
			}

			amprintf("Auto fixture reply: %s!\n",revbuf);
			if (strlen(revbuf)&&strstr(Spec,revbuf))
			{
				amprintf("Right Command Reply !\n");
			}
			////if (revbuf[0]==pTI->Spec[0])
			////{
			////	amprintf("Right Command Reply !\n");
			////}
			else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
			{
				amprintf("SENSOR=RESPONSE[%s];",revbuf);
				Sleep(99999999);
			}
			else
			{
				amprintf("Wrong Command Reply !\n");
				//gComDev.close();
				//return 0;		
			}
			//gComDev.close();
			//return 0;		
		}
	}

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
	return 1;

}

//add start by Talen 2011/06/15
int PushPullResetButton(int ComPort,int TransRate,char *Cmd,char *Timeout,char *PauseTime)
{
	list<TEST_ITEM>::iterator Cy;
	char revbuf[512]=""; 
	int revbufLen = 0;
	int ComTime=50;
	int errcode;
	DWORD dwWaitResult;
	

	char cmd1[512]="";
	char cmd2[512]="";

	char spec1[100]="";
	char spec2[100]="";

	char *pp=NULL;

	strcpy_s(cmd1,sizeof(cmd1),Cmd);

	strcpy(cmd2,cmd1);
	pp=strstr(cmd2,"A");
	strncpy(pp,"B",1);
	//strcpy_s(pp,1,"B");

	strcpy(spec1,cmd1);
	pp=strstr(spec1,"A");
	strncpy(pp,"G",1);

	strcpy(spec2,cmd1);
	pp=strstr(spec2,"A");
	strncpy(pp,"H",1);


	WaitForSingleObject(hOperateComMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(ComPort,TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",ComPort,TransRate);

		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",ComPort,TransRate);


	////size_t m = strlen(cmd);
	////
	////strcat_s(cmd,sizeof(cmd),"\r\n");

//**************************************************  Push Out Reset Button  *************************************
	Sleep(50);
	if(!gComDev.write(cmd1, (int)strlen(cmd1)))
	{
		amprintf("Com command write %s;\n",cmd1);
		//return 0;
	}
	else
	{
		gComDev.close();
		amprintf("Com command %s write fail;\n",cmd1);
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	Sleep(100);
	memset(revbuf,'\0',sizeof(revbuf));
	int i = 0;
	revbufLen = 0;
	while(revbufLen==0&&i<atoi(Timeout))
	{
		gComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(i==atoi(Timeout))
	{
		amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
	}

	amprintf("Auto fixture reply: %s!\n",revbuf);
	if (strlen(revbuf)&&strstr(spec1,revbuf))
	{
		amprintf("Right Command Reply !\n");
	}
	else
	{
		amprintf("Wrong Command Reply !\n");	
	}

	Sleep(atoi(PauseTime));

	if(!gComDev.write(cmd2, (int)strlen(cmd2)))
	{
		amprintf("Com command write %s;\n",cmd2);
		//return 0;
	}
	else
	{
		gComDev.close();
		amprintf("Com command %s write fail;\n",cmd2);
		amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
		amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
		dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
		ResetEvent(gTI.UiResMsg.hEvent);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		KillTargetProcess("AmbitProT.exe");
		_exit(errcode);
		return 0;
	}
	Sleep(100);
	memset(revbuf,'\0',sizeof(revbuf));
	i = 0;
	revbufLen = 0;
	while(revbufLen==0&&i<atoi(Timeout))
	{
		gComDev.read(revbuf,(int)sizeof(revbuf));
		revbufLen = (int)strlen(revbuf);
		Sleep(100);
		i++;
	}
	if(i==atoi(Timeout))
	{
		amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
	}

	amprintf("Auto fixture reply: %s!\n",revbuf);
	if (strlen(revbuf)&&strstr(spec2,revbuf))
	{
		amprintf("Right Command Reply !\n");
	}
	else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
	{
		amprintf("SENSOR=RESPONSE[%s];",revbuf);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		Sleep(99999999);
	}
	else
	{
		amprintf("Wrong Command Reply !\n");
		
		for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
		{			
			if(!strcmp((*Cy).ID,"CHECK_RESET_BUTTON"))
			{
				if(1==(*Cy).ResultFlag)
				{
					ReleaseMutex(hOperateComMutex);
					return 0;					
				}
				break;
			}
		}

		if(!gComDev.write(cmd1, (int)strlen(cmd1)))
		{
			amprintf("Com command write %s;\n",cmd1);
			//return 0;
		}
		else
		{
			gComDev.close();
			amprintf("Com command %s write fail;\n",cmd1);
			amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
			amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			ResetEvent(gTI.UiResMsg.hEvent);
			ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
			KillTargetProcess("AmbitProT.exe");
			_exit(errcode);
			return 0;
		}
		Sleep(100);
		memset(revbuf,'\0',sizeof(revbuf));
		int i = 0;
		revbufLen = 0;
		while(revbufLen==0&&i<atoi(Timeout))
		{
			gComDev.read(revbuf,(int)sizeof(revbuf));
			revbufLen = (int)strlen(revbuf);
			Sleep(100);
			i++;
		}
		if(i==atoi(Timeout))
		{
			amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
		}

		amprintf("Auto fixture reply: %s!\n",revbuf);
		if (strstr(spec1,revbuf))
		{
			amprintf("Right Command Reply !\n");
		}
		else
		{
			amprintf("Wrong Command Reply !\n");	
		}

		Sleep(atoi(PauseTime));


		if(!gComDev.write(cmd2, (int)strlen(cmd2)))
		{
			amprintf("Com command write %s;\n",cmd2);
			//return 0;
		}
		else
		{
			gComDev.close();
			amprintf("Com command %s write fail;\n",cmd2);
			amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
			amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
			dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
			ResetEvent(gTI.UiResMsg.hEvent);
			ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
			KillTargetProcess("AmbitProT.exe");
			_exit(errcode);
			return 0;
		}
		Sleep(100);
		memset(revbuf,'\0',sizeof(revbuf));
		i = 0;
		revbufLen = 0;
		while(revbufLen==0&&i<atoi(Timeout))
		{
			gComDev.read(revbuf,(int)sizeof(revbuf));
			revbufLen = (int)strlen(revbuf);
			Sleep(100);
			i++;
		}
		if(i==atoi(Timeout))
		{
			amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
		}

		amprintf("Auto fixture reply: %s!\n",revbuf);
		if (strlen(revbuf)&&strstr(spec2,revbuf))
		{
			amprintf("Right Command Reply !\n");
		}
		else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
		{
			amprintf("SENSOR=RESPONSE[%s];",revbuf);
			ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
			Sleep(99999999);
		}
		else
		{
			amprintf("Wrong Command Reply !\n");

			for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)
			{			
				if(!strcmp((*Cy).ID,"CHECK_RESET_BUTTON"))
				{
					if(1==(*Cy).ResultFlag)
					{
						ReleaseMutex(hOperateComMutex);
						return 0;					
					}
					break;
				}
			}

			if(!gComDev.write(cmd1, (int)strlen(cmd1)))
			{
				amprintf("Com command write %s;\n",cmd1);
				//return 0;
			}
			else
			{
				gComDev.close();
				amprintf("Com command %s write fail;\n",cmd1);
				amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
				amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
				dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
				ResetEvent(gTI.UiResMsg.hEvent);
				ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
				KillTargetProcess("AmbitProT.exe");
				_exit(errcode);
				return 0;
			}
			Sleep(100);
			memset(revbuf,'\0',sizeof(revbuf));
			int i = 0;
			revbufLen = 0;
			while(revbufLen==0&&i<atoi(Timeout))
			{
				gComDev.read(revbuf,(int)sizeof(revbuf));
				revbufLen = (int)strlen(revbuf);
				Sleep(100);
				i++;
			}
			if(i==atoi(Timeout))
			{
				amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
			}

			amprintf("Auto fixture reply: %s!\n",revbuf);
			if (strstr(spec1,revbuf))
			{
				amprintf("Right Command Reply !\n");
			}
			else
			{
				amprintf("Wrong Command Reply !\n");	
			}

			Sleep(atoi(PauseTime));

			if(!gComDev.write(cmd2, (int)strlen(cmd2)))
			{
				amprintf("Com command write %s;\n",cmd2);
				//return 0;
			}
			else
			{
				gComDev.close();
				amprintf("Com command %s write fail;\n",cmd2);
				amprintf("FIXTURE_ID=CONTENT[%d];\n",fix_id);
				amprintf("MSGBOX=CONTENT[Please Find TE check com port!];\n");
				dwWaitResult= WaitForSingleObject(gTI.UiResMsg.hEvent,INFINITE);
				ResetEvent(gTI.UiResMsg.hEvent);
				ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
				KillTargetProcess("AmbitProT.exe");
				_exit(errcode);
				return 0;
			}
			Sleep(100);
			memset(revbuf,'\0',sizeof(revbuf));
			i = 0;
			revbufLen = 0;
			while(revbufLen==0&&i<atoi(Timeout))
			{
				gComDev.read(revbuf,(int)sizeof(revbuf));
				revbufLen = (int)strlen(revbuf);
				Sleep(100);
				i++;
			}
			if(i==atoi(Timeout))
			{
				amprintf("Waited %d microseconds from sending command to fixture, time out!\n",100*atoi(Timeout));
			}

			amprintf("Auto fixture reply: %s!\n",revbuf);
			if (strlen(revbuf)&&strstr(spec2,revbuf))
			{
				amprintf("Right Command Reply !\n");
			}
			else if(!strstr(revbuf,"E0")&&strstr(revbuf,"E"))
			{
				amprintf("SENSOR=RESPONSE[%s];",revbuf);
				ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
				Sleep(99999999);
			}
			else
			{
				amprintf("Wrong Command Reply !\n");

				gComDev.close();
				ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
				return 0;		
			}
		}
	}


	if(gComDev.is_open())
	{
		gComDev.close();
	}
	ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01

	return 1;
}
//add end by Talen 2011/06/15


//add start by Talen 2011/06/16
int PushPullResetButtonNoCheck(int ComPort,int TransRate,char *Cmd,char *Timeout,char *PauseTime)
{
	char revbuf[512]=""; 
	int revbufLen = 0;
	int ComTime=50;

	char cmd1[512]="";
	char cmd2[512]="";

	char *pp=NULL;

	strcpy_s(cmd1,sizeof(cmd1),Cmd);

	strcpy(cmd2,cmd1);
	pp=strstr(cmd2,"A");
	strncpy(pp,"B",1);
	//strcpy_s(pp,1,"B");

	WaitForSingleObject(hOperateComMutex, INFINITE); //add by Talen 2011/09/01
	Sleep(1);
	if(gComDev.is_open())
	{
		gComDev.close();
	}
	if(!gComDev.open(ComPort,TransRate))
	{
		amprintf("Open ComPort %d TransRate at %d fail!;\n",ComPort,TransRate);
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}
	amprintf("Open ComPort %d TransRate at %d pass;\n",ComPort,TransRate);

	////size_t m = strlen(cmd);
	////
	////strcat_s(cmd,sizeof(cmd),"\r\n");
	Sleep(50);
	if(!gComDev.write(cmd1, (int)strlen(cmd1)))
	{
		amprintf("Com command write %s;\n",cmd1);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd1);
		gComDev.close();
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}
	Sleep(100);	
//**********************************************************************
	Sleep(atoi(PauseTime));

//**********************************************************************

	if(!gComDev.write(cmd2, (int)strlen(cmd2)))
	{
		amprintf("Com command write %s;\n",cmd2);
		//return 0;
	}
	else
	{
		amprintf("Com command %s write fail;\n",cmd2);
		gComDev.close();
		ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
		return 0;
	}
	Sleep(100);
	
//**********************************************************************

	if(gComDev.is_open())
	{
		gComDev.close();
	}
	ReleaseMutex(hOperateComMutex);//add by Talen 2011/09/01
	return 1;

}
//add end by Talen 2011/06/16