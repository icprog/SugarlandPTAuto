#pragma once

class CAmParser
{
public:
	CAmParser(void);
public:
	~CAmParser(void);


#ifdef _UNICODE
public:
	int ParserGetPara(wchar_t *pDesBUFF, wchar_t *pTagBuff, wchar_t *pRtnConBUFF, UINT RtnConBUFFLen);
	/*this function is copy some character from string to other case one spec character was happen*/
	int CopyCaseStr(wchar_t *pDst, int DstLen, const wchar_t *pSrc,  wchar_t *caseCH);
	void SetStartStopTag(wchar_t *pStart, wchar_t *pStop);


private:
	wchar_t StartTag[50];
	wchar_t StopTag[50];


#else
public:
	int ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen);
	/*this function is copy some character from string to other case one spec character was happen*/
	int CopyCaseStr(char *pDst, int DstLen, const char *pSrc,  char *caseCH);
	void SetStartStopTag(char *pStart, char *pStop);
	int ParserGetParaMulti(char *pDesBUFF, char *pTagBuff);//Maxwell for parser more than one tag


private:
	char StartTag[50];
	char StopTag[50];

#endif



};
