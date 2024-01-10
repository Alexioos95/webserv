/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2024/01/10 09:01:10 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//////////////////////////////
// Exception
const char	*Server::CreationException::what(void) const throw()
{ return (""); }

//////////////////////////////
// Constructors and Destructor
Server::Server(std::string server_name, std::string root, std::vector<int> ports) \
	: _server_name(server_name), _root(root)
{
	unsigned long	i;
	unsigned long	err;
	int				isocket;

	i = 0;
	err = 0;
	while (i < ports.size())
	{
		i++;
		isocket = socket(AF_INET, SOCK_STREAM, 0);
		if (isocket == -1)
		{
			std::cerr << "[!] Failed creating socket for " << this->_server_name << ":" << ports[i - 1];
			std::cerr << ": " << strerror(errno) << std::endl;
			err++;
			continue ;
		}
		ft_memset(this->_structsock.sin_zero, 0, sizeof(this->_structsock.sin_zero));
		this->_structsock.sin_family = AF_INET;
		this->_structsock.sin_addr.s_addr = htonl(INADDR_ANY);
		this->_structsock.sin_port = htons(ports[i - 1]);
		this->_addr = sizeof(this->_structsock);
		if (bind(isocket, reinterpret_cast<struct sockaddr *>(&this->_structsock), sizeof(this->_structsock)) == -1)
		{
			std::cerr << "[!] Failed binding socket for " << this->_server_name << " to port " << ports[i - 1];
			std::cerr << ": " << strerror(errno) << std::endl;
			err++;
			close(isocket);
			continue ;
		}
		if (setNonblockingFD(isocket) == 1)
		{
			std::cerr << "[!] Failed setting " << this->_server_name << ":" << ports[i - 1] << " to non-bloquant";
			std::cerr << ": " << strerror(errno) << std::endl;
			err++;
			close(isocket);
			continue ;
		}
		if (listen(isocket, SOMAXCONN) == -1)
		{
			std::cerr << "[!] Failed to make " << this->_server_name << " listening to port " << ports[i - 1];
			std::cerr << ": " << strerror(errno) << std::endl;
			err++;
			close(isocket);
			continue ;
		}
		this->_socket.push_back(isocket);
		this->_ports.push_back(ports[i - 1]);
	}
	if (err == ports.size())
	{
		std::cerr << "[!] server " << this->_server_name << " couldn't be created" << std::endl;
		throw (CreationException());
	}
	i = 0;
	std::cout << "[+] Succesfully created server " << this->_server_name << "\n";
	std::cout << "[*] Listening on port";
	if (this->_ports.size() > 1)
		std::cout << "s";
	std::cout << " ";
	while (i < this->_ports.size())
	{
		std::cout << this->_ports[i];
		if (i != this->_ports.size() - 1)
			std::cout << ", ";
		i++;
	}
	std::cout << std::endl;
}

Server::Server(Server const &rhs)
{
	this->_server_name = rhs._server_name;
	this->_root = rhs._root;
	this->_socket = rhs._socket;
	this->_ports = rhs._ports;
}

Server::~Server(void) {}

//////////////////////////////
// Overload
Server	&Server::operator=(Server const &rhs)
{
	this->_server_name = rhs._server_name;
	this->_root = rhs._root;
	this->_socket = rhs._socket;
	this->_ports = rhs._ports;
	return (*this);
}

//////////////////////////////
// Getters
std::string	Server::getName(void) const
{ return (this->_server_name); }

std::string	Server::getRoot(void) const
{ return (this->_root); }

std::vector<int>	&Server::getSocket(void)
{ return (this->_socket); }

std::vector<int>	&Server::getPorts(void)
{ return (this->_ports); }
