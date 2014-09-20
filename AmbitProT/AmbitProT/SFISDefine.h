
 typedef struct Sfis_G_variable
 {
	 TCHAR PCName[20];
	 TCHAR PN[32];
	 int READ_LENGTH_VALID;
	 int SEND_LENGTH_VALID;
	 int sfisResult;
	 char gDirectory[256];
	 int sfisSwitchFlag;//the number of uut that wait for be test
	 wchar_t BarCode[20];
	 int ModeOptionFlag;
	 int ReadItemNum;
	 int VirtSmoFlag;

 }SFIS_VARIABLE;

typedef struct SFISTransmitTag
{
	TCHAR Item[128];
	TCHAR Val[128];
	int readFlag;
	int readStartNum;
	int readLength;
	int sendFlag;
	int sendStartNum;
	int sendLength;
}SFIS_TRANSMIT_ITEM;
