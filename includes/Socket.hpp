#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

class Socket
{
public:
	explicit Socket(int p,const std::string&	addr);

	~Socket();

	Socket &operator=(const Socket &obj);

	void binding();

	void listening(int bl) const;

	void communicate(std::string msg);

	void setPort(int p);
	int getPort() const;

	void setServerFd(int s);
	int getServerFd() const;


	struct sockaddr_in address;
	unsigned int addrlen;

private:
	int	epfd;
	int port;
	int server_fd;
	int response_fd;
};

#endif
