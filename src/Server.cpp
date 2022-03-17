#include "Server.hpp"
#include "Request.hpp"
#include "parser_utils.hpp"
#include <vector>

Server::Server() : port(80), fd(), bodySize(3000), s()
{
	std::cout << "Server constructor" << std::endl;
}

void Server::launch()
{
	std::string te;
	size_t size;
	std::vector<unsigned char> body;
	std::string ans;

	while (true)
	{
		char *buf = new char [this->bodySize];
		this->fd = accept(s->getServerFd(), (struct sockaddr *) &s->address,
						  (socklen_t *) &s->addrlen);
		if (this->fd == -1)
		{
			close(this->fd);
			delete[] buf;
			exit(1);
		}
		size = recv(this->fd, buf, this->bodySize, 0);
		if (size == 0)
		{
			close(this->fd);
			delete[] buf;
			exit(1);
		}
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
}

Server::~Server() { std::cout << "destructed" << std::endl; }

void Server::createSocket()
{
	this->s = new Socket(this->port);
	this->s->binding();
	this->s->listening(10);
}

void Server::setPort(int p) { this->port = p; }
int Server::getPort() const { return (this->port); }

void Server::setBody(int b) { this->bodySize = b; }
int Server::getBody() const { return this->bodySize; }

void Server::addRoute(const Route &r) { routesList.push_back(r); }
std::vector<Route> Server::getRoutes() { return this->routesList; }
