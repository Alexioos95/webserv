/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2024/01/23 14:24:41 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//////////////////////////////
// Constructors and Destructor
Server::Server(std::string name, std::string root, std::vector<int> ports, std::map<int, int> &m, int bodymax) \
	: _name(name), _root(root), _bodymax(bodymax)
{
	size_t						i;
	size_t						err;
	std::vector<int>::iterator	it;

	i = 0;
	err = 0;
	it = ports.begin();
	while (it != ports.end())
	{
		if (!this->bindPort(m, (*it)))
			err++;
		else
			this->_ports.push_back((*it));
		it++;
	}
	if (err == ports.size())
	{
		std::cerr << "[!] server " << name << " couldn't be created" << std::endl;
		throw (CreationException());
	}
	std::cout << "[+] Succesfully created server " << name << "\n";
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
	this->_name = rhs._name;
	this->_root = rhs._root;
	this->_ports = rhs._ports;
}

Server::~Server(void) {}

//////////////////////////////
// Overloads
Server	&Server::operator=(Server const &rhs)
{
	this->_name = rhs._name;
	this->_root = rhs._root;
	this->_ports = rhs._ports;
	return (*this);
}

//////////////////////////////
// Getters
std::string	Server::getName(void) const
{ return (this->_name); }

std::string	Server::getRoot(void) const
{ return (this->_root); }

std::vector<int>	&Server::getPorts(void)
{ return (this->_ports); }

int	Server::getBodyMax(void) const
{ return (this->_bodymax); }

//////////////////////////////
// Functions
bool	Server::bindPort(std::map<int, int> &m, int port)
{
	std::map<int, int>::iterator	it;
	int								fdsock;
	struct sockaddr_in				ssock;

	it = m.begin();
	while (it != m.end())
	{
		if ((*it).first == port)
			return (true);
		it++;
	}
	fdsock = socket(AF_INET, SOCK_STREAM, 0);
	if (fdsock == -1)
	{
		std::cerr << "[!] Failed creating socket for " << port << " for " << this->_name;
		std::cerr << ": " << strerror(errno) << std::endl;
		return (false);
	}
	ft_memset(ssock.sin_zero, 0, sizeof(ssock.sin_zero));
	ssock.sin_family = AF_INET;
	ssock.sin_addr.s_addr = htonl(INADDR_ANY);
	ssock.sin_port = htons(port);
	if (bind(fdsock, reinterpret_cast<struct sockaddr *>(&ssock), sizeof(ssock)) == -1)
	{
		std::cerr << "[!] Failed binding socket to port " << port << " for " << this->_name << ": " << strerror(errno) << std::endl;
		close(fdsock);
		return (false);
	}
	if (fcntl(fdsock, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		std::cerr << "[!] Failed setting socket of port " << port << " for " << this->_name << " to non-bloquant";
		std::cerr << ": " << strerror(errno) << std::endl;
		close(fdsock);
		return (false);
	}
	if (listen(fdsock, SOMAXCONN) == -1)
	{
		std::cerr << "[!] Failed to listen on port " << port << " for " << this->_name;
		std::cerr << ": " << strerror(errno) << std::endl;
		close(fdsock);
		return (false);
	}
	m.insert(std::pair<int, int>(port, fdsock));
	return (true);
}
