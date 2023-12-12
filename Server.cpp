/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2023/12/12 12:25:35 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Exceptions
const char	*Server::FailSocketException::what(void) const throw()
{ return ("Couldn't create the server's socket: "); }

const char	*Server::FailBindException::what(void) const throw()
{ return ("Couldn't bind the server's socket: "); }

const char	*Server::FailListenException::what(void) const throw()
{ return ("Couldn't listen on the server's socket: "); }

const char	*Server::FailAcceptingClientException::what(void) const throw()
{ return ("Couldn't accept client's connection on the server's socket: "); }

const char	*Server::ClientFailReadException::what(void) const throw()
{ return ("Client failed reading on the server's socket: "); }

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

Server::~Server(void) { }

// Functions
int	Server::getSocket(void) const
{ return (this->_socket); }

void	Server::run(void)
{
	Client cl;

	while (1)
	{
		if (listen(this->_socket, this->_backlog) == -1)
			throw (FailListenException());
		cl.setFD(accept(this->_socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), reinterpret_cast<socklen_t *>(&this->_addr)));
		if (cl.getFD() == -1)
			throw (FailAcceptingClientException());
		char	buffer[4096] = {0};
		if (read(cl.getFD(), buffer, 4096) == -1)
			throw (ClientFailReadException());
		std::cout << buffer << std::endl;
		std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello world!\n");
		write(cl.getFD(), hello.c_str(), strlen(hello.c_str()));
		close(cl.getFD());
	}
}
