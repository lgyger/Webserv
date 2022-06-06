#include "../includes/Socket.hpp"

Socket::Socket(const Server &sv)
	: serverList(), address(), port(sv.getPort()), response_fd()
{
	std::cout << "Socket constructor" << std::endl;
	int val = 1;
	serverList.push_back(sv);
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = htonl(INADDR_ANY);
	this->address.sin_port = htons(port);
	this->addrlen = sizeof(address);
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &val,
			   sizeof(val));
	fcntl(this->server_fd, F_SETFL, O_NONBLOCK);
	if (this->server_fd == -1)
	{
		std::cerr << "Error creating the socket" << std::endl;
		exit(1);
	}
	binding();
}

void Socket::binding()
{
	if (bind(this->server_fd, (struct sockaddr *) &address, sizeof(address)) ==
		-1)
	{
		std::cerr << "Error binding the socket" << std::endl << this->server_fd;
		exit(1);
	}
}

void Socket::listening(int bl) const
{
	listen(this->server_fd, bl);
	std::cout << "Server is listening on port " << this->port << std::endl;
}

Socket::~Socket()
{
	if (this->server_fd != -1 && this->server_fd != 0) close(this->server_fd);

	std::cout << "Socket destructed" << std::endl;
}

Socket &Socket::operator=(const Socket &obj)
{
	this->address = obj.address;
	this->server_fd = obj.server_fd;
	this->response_fd = obj.response_fd;
	this->port = obj.port;
	this->addrlen = obj.addrlen;

	return (*this);
}

int Socket::getPort() const { return (this->port); }

void Socket::launch()
{
	std::cout << "Launched socket!" << std::endl;
	std::string te;

	this->fd =
		accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);

	if (this->fd == -1) return;

	std::string buff = readSocket();
	std::cout << buff << std::endl;

	Request req(buff, this->port);

	for (std::vector<Server>::iterator i = serverList.begin();
		 i != serverList.end(); ++i)
	{
		if (i->getServerName() == req.getServerName())
		{
			i->launch(req, this->fd);
			return;
		}
	}
	std::cout << "Called default route: " << req.getServerName() << std::endl;
	defaultServer.launch(req, this->fd);
}

void npolling(struct pollfd *fds, int size)
{
	int status;
	fds->revents = 0;
	do {
		status = poll(fds, size, 1);
	} while (status == 0 && fds->events != fds->revents);
}

std::string Socket::readSocket() const
{
	std::string te;
	struct pollfd fds;
	fds.fd = this->fd;
	fds.events = POLLIN;
	npolling(&fds, 1);
	char *buf = new char[3000];
	if (!buf) { std::exit(1); }
	for (int in = 0; in != 3000; in++) buf[in] = 0;
	int size = recv(this->fd, buf, 3000, 0);
	if (size <= 0)
	{
		delete[] buf;
		return (std::string());
	}
	te.resize(size);
	te = buf;
	delete[] buf;
	return (te);
}
int Socket::getServerFd() const { return server_fd; }
