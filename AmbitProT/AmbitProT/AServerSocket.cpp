
#include "stdafx.h"

#include <winsock2.h>
#include "AmLogRecord.h"

#define NO_FLAGS_SET 0

#define PORT (u_short) 6000
#define MAXBUFLEN 256


CAmLogRecord RobotRecord;

INT ServerSocket(VOID)
{
	WSADATA Data;
	SOCKADDR_IN serverSockAddr;
	SOCKADDR_IN clientSockAddr;
	SOCKET serverSocketID;
	SOCKET clientSocketID;
	int addrLen=sizeof(SOCKADDR_IN);
	int status;
	int numrcv;
	char buffer[MAXBUFLEN];

	/* initialize the Windows Socket DLL */
	status=WSAStartup(MAKEWORD(2, 2), &Data);
	if (status != 0)
	{
		RobotRecord.amprintf("ERROR: WSAStartup unsuccessful %d;\n", GetLastError());
	}
		
	/* zero the sockaddr_in structure */
	memset(&serverSockAddr, 0, sizeof(serverSockAddr));
	/* specify the port portion of the address */
	serverSockAddr.sin_port=htons(PORT);
	/* specify the address family as Internet */
	serverSockAddr.sin_family=AF_INET;
	/* specify that the address does not matter */
	serverSockAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	/* create a socket */
	serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketID == INVALID_SOCKET)
	{
		RobotRecord.amprintf("ERROR:Server socket unsuccessful %d;\n", GetLastError());
	}

	int nZero = 1;
	setsockopt(serverSocketID, SOL_SOCKET, SO_RCVBUF, (char *)&nZero, sizeof(nZero));

	/* associate the socket with the address */
	status=bind(serverSocketID,(LPSOCKADDR) &serverSockAddr,sizeof(serverSockAddr));
	if (status == SOCKET_ERROR)
	{
		RobotRecord.amprintf("ERROR:socket bind unsuccessful %d;\n", GetLastError());
	}

	/* allow the socket to take connections */
	status=listen(serverSocketID, 1);
	if (status == SOCKET_ERROR)
	{
		RobotRecord.amprintf("ERROR:socket listen unsuccessful %d;\n", GetLastError());
	}

	/* accept the connection request when one is received */
	clientSocketID = accept(serverSocketID,(LPSOCKADDR) &clientSockAddr,&addrLen);

	while(true)
	{
		numrcv = recv(clientSocketID, buffer, MAXBUFLEN, NO_FLAGS_SET);
		if (numrcv == SOCKET_ERROR)
		{
			RobotRecord.amprintf("Connection terminated.%d;\n", GetLastError());
			status=closesocket(clientSocketID);
			if (status == SOCKET_ERROR)
			{
				RobotRecord.amprintf("ERROR: closesocket unsuccessful %d;\n", GetLastError());
			}
			return 1; 
		}

		if(strstr(buffer,"ASK"))//receiv reboot "ASK" 
		{
			//no DUT, response IDLE
			send(clientSocketID, "IDLE\\r\n", sizeof("IDLE\\r\n"), 0);

			//test result is PASS, response PASS
			//open fixture
			send(clientSocketID, "PASS\\r\n", sizeof("PASS\\r\n"), 0);

			//test result is FAIL, response FAIL
			//open fixture
			send(clientSocketID, "FAIL\\r\n", sizeof("FAIL\\r\n"), 0);

			//in testing, response RUN
			send(clientSocketID, "RUN\\r\n", sizeof("RUN\\r\n"), 0);

			//if some error, response ERROR
			send(clientSocketID, "ERROR\\r\n", sizeof("ERROR\\r\n"), 0);

		}
		else if(strstr(buffer, "START"))//format : SN(7):START\r\n
		{
			//get SN

			//send to SFIS

			//get SFIS Result and set status

		}
		else if(strstr(buffer, "GETOK"))
		{
			//respone "IDLE"
		}


	} 
}