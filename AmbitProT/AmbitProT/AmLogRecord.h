#pragma once

class CAmLogRecord
{
public:
	CAmLogRecord(void);
public:
	~CAmLogRecord(void);

public:
	long amprintf(const char * format,...);
};
