#include "Server.hpp"
#include "Request.hpp"
#include "parser_utils.hpp"
#include <vector>
#include <poll.h>
Server::Server() : port(80), fd(), bodySize(3000)
{
}

void Server::launch()
{
	std::string te;
	size_t size;
	std::vector<unsigned char> body;
	std::string ans;
	struct pollfd fds;
	int status;
	fds.events = POLLPRI | POLLOUT;
	do
	{
		for (size_t i = 0; i != s.size(); i++)
		{
			this->fd = accept(s[i].getServerFd(), (struct sockaddr *) &s[i].address,
						  (socklen_t *) &s[i].addrlen);
			fds.fd = this->fd;
		}
		status = poll(&fds, 1,1000);
		if (status == -1)
		{
			close (this->fd);
			close (s[0].getServerFd());
			exit(1);
		}
		std::cout << status << std::endl;
		if (this->fd == -1)
		{
			close(this->fd);
			exit(1);
		}
		for (int i = 0; i != status; i++)
		{
			char *buf = new char [this->bodySize];
			for (int in = 0; in != this->bodySize; in++)
				buf[in] = 0;
			size = recv(this->fd, buf, this->bodySize, 0);
			if (size == 0)
			{
				close(this->fd);
				delete[] buf;
				exit(1);
			}
			std::cout << buf << std::endl;
			te.resize(size);
			buf[size] = 0;
			te = buf;
			std::cout << te << std::endl;
			std::string serverName =
			"localhost"; /* TODO: fix hardcoded serverName */
			Request req(te, serverName, this->port);
			Response r(getRoutes(), req);
			ans = r.getRequest();
			send(this->fd, ans.c_str(), ans.size(), 0);
			body = r.get_body();
			size = r.get_size();
			send(this->fd, (char *) &body[0], size, 0);
			close(this->fd);
			delete [] buf;
		}
	} while (status != 0);
}

Server::~Server() { std::cout << "destructed" << std::endl; }

void Server::createSocket()
{
	this->s.insert(this->s.begin(), Socket(this->port));
	this->s[0].binding();
	this->s[0].listening(10);
}

void Server::setPort(int p) { this->port = p; }
int Server::getPort() const { return (this->port); }

void Server::setBody(int b) { this->bodySize = b; }
int Server::getBody() const { return this->bodySize; }

void Server::addRoute(const Route &r) { routesList.push_back(r); }
std::vector<Route> Server::getRoutes() { return this->routesList; }
