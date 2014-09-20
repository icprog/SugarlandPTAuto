#ifndef CCEN_TIME_H
#define CCEN_TIME_H
#include <time.h>
class CCenTime
{
public:
	CCenTime(void);
public:
	~CCenTime(void);
public:
	void TimeStartCount();
public:
	void GenCurrentTime(char *timebuf, int Len);
	void GetElapseTime(char *prRtn,int Len);
    void GetDate(char *timebuf, int Len);
	void DataFormatC(char *timebuf, int Len, char *pIn);
	void CostTimeConvert(char *timebuf,int timebufLen,char *pIn);//Talen 2012/03/23
private:
	clock_t Start_t,Current_t;
};
#endif