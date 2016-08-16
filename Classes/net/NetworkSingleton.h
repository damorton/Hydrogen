#ifndef NET_NET_MANAGER_H_
#define NET_NET_MANAGER_H_

#include <string> // string

class NetManager {
public:

	enum State{
		CONNECTED,
		CONNECTION_FAILED,
	};

	NetManager();
	~NetManager();

	int Init(std::string hostname, std::string service);

	State GetState(){ return m_eState; }

	int SendData(char* buffer);

	int ReadData(char* buffer, int size);

private:

	void ClearBuffer(char *buffer);

	char *TimeNow();

	int m_iSocketFileDescriptor;

	State m_eState;

	std::string m_strHostname;
	std::string m_strService;
};

#endif