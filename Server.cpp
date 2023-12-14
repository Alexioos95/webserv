/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2023/12/14 11:38:41 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Exceptions
const char	*Server::FailSocketException::what(void) const throw()
{ return ("Couldn't create the server's socket: "); }

const char	*Server::FailBindException::what(void) const throw()
{ return ("Couldn't bind the server's socket: "); }

const char	*Server::FailFcntlException::what(void) const throw()
{ return ("Couldn't set the non-blocking mode to the file descriptor"); }

const char	*Server::FailListenException::what(void) const throw()
{ return ("Couldn't listen on the server's socket: "); }

const char	*Server::FailAcceptingClientException::what(void) const throw()
{ return ("Couldn't accept client's connection on the server's socket: "); }

const char	*Server::ClientFailReadException::what(void) const throw()
{ return ("Client failed reading on the server's socket: "); }

const char	*Server::FailSelectException::what(void) const throw()
{ return ("Failed to use select() to see which client needs to communicate: "); }


// Constructors and Destructor
Server::Server(void) : _port(8080), _backlog(9999)
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET is connection via IP.
	// SOCK_STREAM is (second) connection, via TCP.
	// 0 is the protocol of the socket (always 0 for TCP/IP sockets).
	if (this->_socket == -1)
		throw (FailSocketException());
	// Parameter the struct for bind
	ft_memset(this->_structsock.sin_zero, 0, sizeof(this->_structsock.sin_zero));
	this->_structsock.sin_family = AF_INET;
	this->_structsock.sin_addr.s_addr = htonl(INADDR_ANY); // Convert int to host's IP. INADDR_ANY for auto set.
	this->_structsock.sin_port = htons(this->_port); // Convert int to port for the connection.
	this->_addr = sizeof(this->_structsock);
	if (bind(this->_socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), sizeof(this->_structsock)) == -1)
		throw (FailBindException());
}

Server::Server(int const port) : _port(port), _backlog(9999)
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket == -1)
		throw (FailSocketException());
	ft_memset(this->_structsock.sin_zero, 0, sizeof(this->_structsock.sin_zero));
	this->_structsock.sin_family = AF_INET;
	this->_structsock.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_structsock.sin_port = htons(this->_port);
	this->_addr = sizeof(this->_structsock);
	if (bind(this->_socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), sizeof(this->_structsock)) == -1)
		throw (FailBindException());
	this->setNonblockingFD(this->_socket);
}

Server::~Server(void)
{
	if (this->_socket >= 0)
		close(this->_socket);
}

// Functions
int	Server::getSocket(void) const
{ return (this->_socket); }

void	Server::setFDSet(void)
{
	std::vector<Client>::iterator	it;

	it = this->_clients.begin();
	// Reset the set of fd to listen
	FD_ZERO(&this->_rset);
	FD_ZERO(&this->_wset);
	FD_ZERO(&this->_errset);
	// Bind the set to the socket
	FD_SET(this->_socket, &this->_rset);
	FD_SET(this->_socket, &this->_errset);
	// Add fd of clients to their set
	while (it != this->_clients.end())
	{
		if ((*it).getSentbytes() < (*it).getTotalbytes())
			FD_SET((*it).getFD(), &this->_wset);
		else
			FD_SET((*it).getFD(), &this->_rset);
		FD_SET((*it).getFD(), &this->_errset);
		it++;
	}

}

void	Server::setNonblockingFD(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw (FailFcntlException());
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw (FailFcntlException());
}

void	Server::run(void)
{
	Client	cl;
	int		toread;

	while (1)
	{
		try
		{
			// Waiting for clients
			if (listen(this->_socket, this->_backlog) == -1)
				throw (FailListenException());
			// Initialize the set of fd
			this->setFDSet();
			// Select(), to see which client needs
			toread = select(FD_SETSIZE, &this->_rset, &this->_wset, &this->_errset, NULL);
			if (toread == -1)
				throw (FailSelectException());
			// Accept his connection
			cl.setFD(accept(this->_socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), reinterpret_cast<socklen_t *>(&this->_addr)));
			if (cl.getFD() == -1)
				throw (FailAcceptingClientException());
			// Set his fd to non-blocking mode
			this->setNonblockingFD(cl.getFD()); // Blocks from reading multiple times in a short interval... ?
			// Adding the client to the vector
			this->_clients.push_back(cl);
			// Read
			if (read(cl.getFD(), cl.getBuffer(), 4096) == -1)
				throw (ClientFailReadException());
			// Print
			std::cout << cl.getBuffer() << std::endl;
			// Answer
			std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello world!\n");
			write(cl.getFD(), hello.c_str(), strlen(hello.c_str()));
			// Close his fd
			if (cl.getFD() >= 0)
				close(cl.getFD());
		}
		catch (Server::FailSelectException &e)
		{
			std::cout << "Error: " << e.what() << strerror(errno) << std::endl;
			return ;
		}
		catch (std::exception &e)
		{
			std::cout << "Error: " << e.what();
			if (errno != 0)
				std::cout << strerror(errno);
			std::cout << std::endl;
		}
	}
}
