/*

	Contains:	IO Message format define

	Version:	xxx 001 xxx

	Copyright:	(C) 2007 by Ambit Micro, Inc., all rights reserved.

    Technology:			M28

	Change History (most recent first):

	Create by Jeffrey Chou 2007/2/16

	Note: this is IO commucation between testperf and ambitprot module , it must be keep synchronization with ambitprot

*/


#ifndef A_TEST_IO_H
#define A_TEST_IO_H


//---------------------------------------------------------------------------------------------------
//IO control cmd value
enum CTL_TPERF_CMD
{
	T_START_TEST,// start test
	T_END_TEST,// test finish
	T_EXIT,// exit testperf
};
// IO report testperf status
enum TPERF_STS
{
	STS_ERROR,
};
//---------------------------------------------------------------------------------------------------










#endif