/*
Copyright (c) 2016 David Morton

https://github.com/damorton/libsocket.git
*/
#ifndef INCLUDES_SOCKETS_H_
#define INCLUDES_SOCKETS_H_

#ifdef WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#define SHUT_RD   SD_RECEIVE 
#define SHUT_WR   SD_SEND 
#define SHUT_RDWR SD_BOTH 
#pragma comment(lib,"ws2_32.lib")

typedef int socklen_t;  
//typedef int ssize_t;

#define close closesocket

#else
    
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

#include <stdio.h> // perror()
#include <sys/types.h>
#include <stdlib.h> // exit(),
#include <errno.h>

// Max buffer size used for the read buffer of file descriptors
#define MAX_BUF_SIZE 128
#define MAX_LISTEN_QUEUE_SIZE 1024

// Used to store addressing information and populated
// by the Address() wrapper function.
struct Address{
	struct sockaddr_in m_sAddress; // Address assembled here
	struct hostent * m_sHost_info; // Host information
	struct sockaddr_storage sender;
	socklen_t sendsize;
};

enum eAppType{
	TYPE_CLIENT,
	TYPE_SERVER
};

#ifdef __cplusplus
extern "C" {
#endif

int Socket(int family, int type, int protocol);

int  Connection(const char *address, const char *service, int type /* Client or Server */, int protocol /* UDP or TCP */);

int Accept(int iListenSocketFileDescriptor, struct Address *address);

void Connect(int socketFileDescriptor, const struct sockaddr* socketAddress, socklen_t socketSize);

int Select(int maxFileDescriptorsPlus1, fd_set *readFileDescriptorSet, fd_set *writeFileDescriptorSet, fd_set *exceptFileDescriptorSet, struct timeval *timeout);

SSIZE_T Read(int fileDescriptor, void *buffer, size_t numberOfBytes);

void Write(int fileDescriptor, void *buffer, size_t numberOfBytes);

void Shutdown(int fileDescriptor, int shutdownOption);

int Max(int x, int y);

void Bind(int socketFileDescriptor, const struct sockaddr* socketAddress, socklen_t socketSize);

void Listen(int socketFileDescriptor, int maxListenQSize);

void MultiplexIO(FILE* fp, int socketFileDescriptor);

int Send(int socketFileDescriptor, char *message, size_t size, int flags);

int SendTo(int socketFileDescriptor, char *message, size_t size, int flags, struct sockaddr *sender, socklen_t sendsize);

int Recv(int socketFileDescriptor, char *message, size_t size, int flags);

int ReceiveFrom(int socketFileDescriptor, char *message, int bufferSize, int flags, struct sockaddr *sender, socklen_t *sendsize);

int SetNonBlocking(int socketFileDescriptor);

int Close(int socketFileDescriptor);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDES_SOCKETS_H_ */
