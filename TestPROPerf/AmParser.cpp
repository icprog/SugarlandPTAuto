#include "StdAfx.h"
#include "AmParser.h"

CAmParser::CAmParser(void)
{

#ifdef _UNICODE




	wcscpy_s(StartTag, sizeof(StartTag)/2, _T("="));
	wcscpy_s(StopTag, sizeof(StopTag)/2, _T(";"));





#else
	strcpy_s(StartTag, sizeof(StartTag), "=");
	strcpy_s(StopTag, sizeof(StopTag), ";");
#endif

}

CAmParser::~CAmParser(void)
{


}

#ifdef _UNICODE



void CAmParser::SetStartStopTag(wchar_t *pStart, wchar_t *pStop)
{

	if (!pStart || !pStop)
	{
		return;
	}
	wcscpy_s(StartTag, sizeof(StartTag)/2, pStart);
	wcscpy_s(StopTag, sizeof(StopTag)/2, pStop);

	return;
}

/*this function is copy some character from string to other case one spec character was happen*/
int CAmParser::CopyCaseStr(wchar_t *pDst, int DstLen, const wchar_t *pSrc, wchar_t *caseCH)
{

	const wchar_t *pTok=0;

	if ((!pDst)||(!pSrc))
	{
		return 0;
	}

	pTok=wcsstr(pSrc, caseCH);
	if (!pTok)
	{
		*pDst=0;
		return 0;
	}

	return(!(wcsncpy_s(pDst, DstLen, pSrc, (pTok-pSrc))));
}
int CAmParser::ParserGetPara(wchar_t *pDesBUFF, wchar_t *pTagBuff, wchar_t *pRtnConBUFF, UINT RtnConBUFFLen)
{
	wchar_t TagBUFF[128] = _T("");
	wchar_t AnalyseBUFF[512] = _T("");
	wchar_t ItemID[100]=_T("");
	wchar_t *pToken = NULL;
	wchar_t *pFlagToken = NULL;
	wchar_t *pConStart = NULL;
	wchar_t *PConEnd = NULL;

	if((NULL==pDesBUFF)||(NULL==pTagBuff)||(NULL==pRtnConBUFF))
	{
		return 0;
	}
	if ((RtnConBUFFLen <= 0) || (RtnConBUFFLen > 2048))
	{
		return 0;
	}
	
	
	wcscpy_s(AnalyseBUFF, sizeof(AnalyseBUFF)/2, pDesBUFF);
	wcscpy_s(TagBUFF, sizeof(TagBUFF)/2, pTagBuff);

	pFlagToken = NULL;
	pFlagToken=wcsstr(AnalyseBUFF,TagBUFF);
	if (!pFlagToken)
	{
        return 0;
	}

	if (!CopyCaseStr(ItemID, sizeof(ItemID)/2,pFlagToken,StartTag))
	{
		wcscpy_s(pRtnConBUFF, RtnConBUFFLen,_T(""));
		return 0;
	}

	if (wcslen(ItemID)!=wcslen(TagBUFF))
	{
		wcscpy_s(pRtnConBUFF, RtnConBUFFLen, _T(""));
		return 0;
	}


	if (!wcsncmp(ItemID, TagBUFF, wcslen(TagBUFF)))
	{
		pToken = wcsstr(pFlagToken, StartTag); 
		if (pToken)
		{
			pConStart = pToken;
			pConStart += 1;  
		}
		else
		{
			return 0;
		}
		pToken = wcsstr(pToken, StopTag); 
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
			wcscpy_s(pRtnConBUFF, RtnConBUFFLen, pConStart);
		}	
		return 1;
	}
	else
	{
		wcscpy_s(pRtnConBUFF, RtnConBUFFLen, _T(""));
		return 0;
	}

}



#else

void CAmParser::SetStartStopTag(char *pStart, char *pStop)
{

	if (!pStart || !pStop)
	{
		return;
	}
	strcpy_s(StartTag, sizeof(StartTag), pStart);
	strcpy_s(StopTag, sizeof(StopTag), pStop);

	return;
}

/*this function is copy some character from string to other case one spec character was happen*/
int CAmParser::CopyCaseStr(char *pDst, int DstLen, const char *pSrc, char *caseCH)
{

	const char *pTok=0;

	if ((!pDst)||(!pSrc))
	{
		return 0;
	}

	pTok=strstr(pSrc, caseCH);
	if (!pTok)
	{
		*pDst=0;
		return 0;
	}

	return(!(strncpy_s(pDst, DstLen,pSrc, (pTok-pSrc))));
}
int CAmParser::ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen)
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

#ifdef OneWords
	pFlagToken=strstr(AnalyseBUFF,TagBUFF);
	if (!pFlagToken)
	{
        return 0;
	}

	if (!CopyCaseStr(ItemID, sizeof(ItemID),pFlagToken,StartTag))
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
		pToken = strstr(pFlagToken, StartTag); 
		if (pToken)
		{
			pConStart = pToken;
			pConStart += 1;  
		}
		else
		{
			return 0;
		}
		pToken = strstr(pToken, StopTag); 
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
#else
	pFlagToken = AnalyseBUFF;
	pFlagToken=strstr(pFlagToken,TagBUFF);
	while(pFlagToken)
	{
		if (!CopyCaseStr(ItemID, sizeof(ItemID),pFlagToken,StartTag))
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
			pToken = strstr(pFlagToken, StartTag); 
			if (pToken)
			{
				pConStart = pToken;
				pConStart += 1;  
			}
			else
			{
				return 0;
			}
			pToken = strstr(pToken, StopTag); 
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
#endif
}

#endif


