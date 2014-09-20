#ifndef AMBIT_PARSER_H
#define AMBIT_PARSER_H

#include "AmbitTestSysDef.h"


#define FILE_PASER_BUF_L 500


#define ITEM_NAME  "ITEM_NAME"
#define TEST_FLAG  "TEST_FLAG"
#define SPEC_MAX  "SPEC_MAX"
#define SPEC_MIN  "SPEC_MIN"



typedef struct AmCmdParseInfoStructTag
{
   
//   char Rule[100];
//   char ResultBUF[128];

}AM_CMD_PARSE_INFO_STRUCT;



void CmdLineParseParameters(int argc, _TCHAR* argv[]);
void Usage(_TCHAR* ProgName);
int ConfigFileParse(char *pfile_name);
int AmbitLoadConfig();
int ParserGetPara(char *pDesBUFF, char *pTagBuff, char *pRtnConBUFF, UINT RtnConBUFFLen);
int ReadConfigFile(char *pFileName);
/*this function is copy some character from string to other case one spec character was happen*/
int CopyCaseStr(char *pDst, int DstLen, const char *pSrc, char caseCH);
int ParserConfigItem(char *Srcbuf);
int Setambitconfig();
#endif