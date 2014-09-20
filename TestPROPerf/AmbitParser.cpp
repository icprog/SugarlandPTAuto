#include "stdafx.h"
#include "AmbitParser.h"



extern AM_PROTL_CONFIG gTestStaInfo;
extern SFISSW SFIS;
extern TEST_INPUT_INFO gTI;//Talen 2011/11/01
extern list<TEST_ITEM> TItemList;//Talen 2011/11/01

char *Rule[100] = {"1",
"2"
};

//////////////////////////////////////////////////////////////////////////
// 
void CmdLineParseParameters(int argc, _TCHAR* argv[])
{

	int i;
    char *pTok = NULL;
	if (argc > 1)
	{
		for (i = 1;i < argc; i++)
		{
            if (((argv[i][0] == '-') || (argv[i][0] == '/')) &&
                (argv[i][1] != 0) && (argv[i][2] == 0)) 
			{
                switch(tolower(argv[i][1])) 
				{
					case 'c':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iChanNum = atoi(argv[++i]);
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
					case 'b':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iBandWidth = atoi(argv[++i]);
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
					case 's':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iSideBand = atoi(argv[++i]);
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
					case 'm':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iRateS = atoi(argv[++i]);
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
                    case 'l':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iStream0Cable = atof(argv[++i]);							   
								//pTok = NULL;
							//	pTok = strstr(argv[i],"#");
								//iStream1Cable = atof(pTok+1);
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
                    case 'g':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//g_pLogFile = argv[++i];
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
                    case 'i':
                        if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
                                //strcpy(g_szSSID, argv[++i]);
								//g_pLogFile = argv[++i];
                                break;
                            }
                        }
                        Usage(argv[0]);
                        break;
                    case 't':
						if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
                                //g_bStopOnFail = atoi(argv[++i]);
                                break;
                            }
                        }
                        break;
					case 'p':
						if (argv[i + 1])
						{
							if (argv[i+1][0] != '-')
							{                               
								//iPwrInitialFlag = atoi(argv[++i]);
                                break;
                            }
                        }
                        break;

					case 'f':
						if (argv[i+1])
						{
							if (argv[i+1][0] != '-')
							{
								//iTestSequence = atoi(argv[++i]);
                                break;
                            }
                        }
                        break;

						
                    default:
                        Usage(argv[0]);
                        break;
                }
            }
            else
                Usage(argv[0]);
        }
    }
}
void Usage(_TCHAR* ProgName) 
{
    printf("\nProgram . Version %s.\n", SOFTWARE_VERSION);
    printf("Copyright(C) Ambit Micro system.\n");
	
    printf("\n%s [-c TestChannel] [-l stream0cableloss#stream1cableloss] [-i SSID]  [-g Log] [-t StopOnFail]\n\n", ProgName);
    printf("  TestChannel    2G/5G Channel Number 2G(1-14)  5G(36-216)");
    printf("  stream0cableloss#stream1cableloss   ant 0 cable loss value#ant 1 cable loss value");
    printf("  SSID              SSID Wlan NIC to join.  (default: )\n");
    printf("  Log               Log file name.  (default: NULL)\n");
    printf("  StopOnFail        Stop on Failed.  (default)\n");
	
	printf("\r\nHit Ctrl-C to terminate\n");
    return;
}
/*this function is copy some character from string to other case one spec character was happen*/
int CopyCaseStr(char *pDst, int DstLen, const char *pSrc, char caseCH)
{

	const char *pTok=0;

	if ((!pDst)||(!pSrc))
	{
		return 0;
	}

	pTok=strchr(pSrc, caseCH);
	if (!pTok)
	{
		*pDst=0;
		return 0;
	}

	return(!(strncpy_s(pDst, DstLen,pSrc, (pTok-pSrc))));
}
int ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen)
{
	char TagBUFF[128] = "";
	char AnalyseBUFF[512] = "";
	char ItemID[100]="";
	char *pToken = NULL;
	char *pFlagToken = NULL;
	char *pConStart = NULL;
	char *PConEnd = NULL;

	if((NULL==pDesBUFF)||(NULL==pTagBuff)||(NULL==pRtnConBUFF))
	{
		return 0;
	}
	if ((RtnConBUFFLen <= 0) || (RtnConBUFFLen > 512))
	{
		return 0;
	}
	
	
	strcpy_s(AnalyseBUFF, sizeof(AnalyseBUFF), pDesBUFF);
	strcpy_s(TagBUFF, sizeof(TagBUFF), pTagBuff);

	pFlagToken = NULL;
	pFlagToken=strstr(AnalyseBUFF,TagBUFF);
	if (!pFlagToken)
	{
        return 0;
	}

	if (!CopyCaseStr(ItemID, sizeof(ItemID),pFlagToken,'='))
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}

	if (strlen(ItemID)!=strlen(TagBUFF))
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}


	if (!strncmp(ItemID, TagBUFF, strlen(TagBUFF)))
	{
		pToken = strstr(pFlagToken, "="); 
		if (pToken)
		{
			pConStart = pToken;
			pConStart += 1;  
		}
		else
		{
			return 0;
		}
		pToken = strstr(pToken, ";"); 
		if (pToken)
		{
			PConEnd = pToken;
			*PConEnd = NULL;
		}
		else
		{
			return 0;
		}
		if (pConStart && PConEnd)
		{
			strcpy_s(pRtnConBUFF, RtnConBUFFLen, pConStart);
		}	
		return 1;
	}
	else
	{
		strcpy_s(pRtnConBUFF, RtnConBUFFLen, "");
		return 0;
	}

}


int AmbitLoadConfig()
{
		
	// load station type information
	strcpy_s(gTestStaInfo.TestStaName, sizeof(gTestStaInfo.TestStaName), STATION_TYPE);
    // load the version
	strcpy_s(gTestStaInfo.TSysSoftVersion, sizeof(gTestStaInfo.TSysSoftVersion), SOFTWARE_VERSION);
	// load UUT name
	strcpy_s(gTestStaInfo.UUTName, sizeof(gTestStaInfo.UUTName), UUT_NAME);
	
	return TRUE;
};


int ParserConfigItem(char *Srcbuf)
{
	int i=0;
	char *Toke=Srcbuf;
	for(i=0;i<200;i++)
	{
		gTI.TestStaInfo.indexSelected[i]=0;
	}
	i=0;
	while(Toke && i<200)
	{
		gTI.TestStaInfo.indexSelected[i]=atoi(Toke);
		if(0==gTI.TestStaInfo.indexSelected[i])
		{
			break;
		}
		i++;
		Toke=strstr(Toke,",");
		if(!Toke)
		{
			break;
		}
		Toke++;
	}
	if(i>=200)
	{
		amprintf("MSGBOX=CONTENT[There have more than 200 items to test!!!];\n");
	}
	return 1;
}

int Setambitconfig()
{
	int i;
	list<TEST_ITEM>::iterator Cy;
	for (Cy=TItemList.begin(); Cy!=TItemList.end();Cy++)//FLAG=5
	{
		(*Cy).selected=0;
		for(i=0;i<200&&gTI.TestStaInfo.indexSelected[i]!=0;i++)
		{
			if((*Cy).index==gTI.TestStaInfo.indexSelected[i])
			{
				(*Cy).selected=1;
				break;
			}
		}
	}
	return 1;
}