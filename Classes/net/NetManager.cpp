#include "../net/NetManager.h"

#include <cstring> // strlen()
#include <iostream>

#include "../socket/socket.h" // libsocket
#include "cocos2d.h"

NetManager::NetManager()
{
		
}

NetManager::~NetManager()
{
	Close(m_iSocketFileDescriptor);
}

int NetManager::Init(std::string hostname, std::string service)
{
	printf("hostname: %s server: %s\n", hostname.c_str(), service.c_str());
	// Store hostname and service for reconnection attempts
	m_strHostname = hostname;
	m_strService = service;

	// Initialize connection to the raig server
	m_iSocketFileDescriptor = Connection(m_strHostname.c_str(), m_strService.c_str(), TYPE_CLIENT, SOCK_DGRAM);

	if (m_iSocketFileDescriptor == -1)
	{
		m_eState = CONNECTION_FAILED;
		CCLOG("InitConnection() connection failed sfd: %d", m_iSocketFileDescriptor);			
		return -1;
	}

	CCLOG("InitConnection() connection successful sfd: %d", m_iSocketFileDescriptor);
	std::cout << "Init() connection successful" << std::endl;

	m_eState = CONNECTED;
	SetNonBlocking(m_iSocketFileDescriptor);

	return m_iSocketFileDescriptor;
}

int NetManager::SendData(char* buffer)
{
	size_t size = strlen(buffer) + 1;
	int flags = 0;
	int bytesSents = 0;

	bytesSents = Send(m_iSocketFileDescriptor, buffer, size, flags);
	CCLOG("SendData() send buffer %s", buffer);
	ClearBuffer(buffer);
	return bytesSents;
}

int NetManager::ReadData(char* buffer, int size)
{
	int flags = 0;
	int bytesRecv = 0;
	
	ClearBuffer(buffer);

	bytesRecv = Recv(m_iSocketFileDescriptor, buffer, size, flags);
	if (strcmp(buffer, "0") != 0)
	{
		CCLOG("ReadData() recv buffer %s", buffer);
		CCLOG("ReadData() bytes recv %d", bytesRecv);
	}		

	// Server shutdown connection
	if (bytesRecv == 0 && m_eState == CONNECTED)
	{
		CCLOG("Server has shutdown the connection");
		m_eState = CONNECTION_FAILED;
		Close(m_iSocketFileDescriptor);
	}

	return bytesRecv;
}

void NetManager::ClearBuffer(char *buffer)
{
	sprintf(buffer, "%s", "0");
}

char *NetManager::TimeNow()
{
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return asctime(timeinfo);
}