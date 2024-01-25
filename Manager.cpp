/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2024/01/10 08:51:22 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"
#include "Client.hpp"

//////////////////////////////
// Exceptions
const char	*Manager::SelectException::what(void) const throw()
{ return ("Select: "); }

//////////////////////////////
// Constructors and Destructor
Manager::Manager(void) { }

Manager::~Manager(void)
{
	std::map<int, int>::iterator	it;

	it = this->_sockets.begin();
	while (it != this->_sockets.end())
	{
		close((*it).second);
		it++;
	}
}

//////////////////////////////
// Getters
std::map<int, int>	&Manager::getSockets(void)
{ return (this->_sockets); }

//////////////////////////////
// Functions
void	Manager::run(void)
{
	// v A enlever, c'est juste pour test. v
	std::vector<int>	vec;
	vec.push_back(8080);
	vec.push_back(8081);
	vec.push_back(8082);
	vec.push_back(8083);
	vec.push_back(8084);
	vec.push_back(8085);
	vec.push_back(8086);
	std::vector<int>	vec2;
	vec2.push_back(8090);
	vec2.push_back(8091);
	vec2.push_back(8092);
	Server	tmp("test.com", "./qr///////////////", vec, this->_sockets, 99999);
	Server	tmp2("netpractice.net", "./np///", vec2, this->_sockets, 99999);
	this->_servs.push_back(tmp);
	this->_servs.push_back(tmp2);
	std::cout << std::endl;
	// ^ A enlever, c'est juste pour test. ^

	while (1)
	{
		struct timeval t;

		t.tv_sec = 1;
		t.tv_usec = 0;
		if (g_sigint == true)
		{
			this->shutdown();
			throw (SigintException());
		}
		this->manageFDSets();
		if (select(FD_SETSIZE, &this->_rset, &this->_wset, &this->_errset, &t) == -1)
		{
			std::cerr << "\n[!] Critical error on select: " << strerror(errno) << ". Shutting down the program..." << std::endl;
			this->shutdown();
			throw (SigintException());
		}
		else
		{
			this->managePorts();
			this->manageClients();
			this->manageTimeout();
		}
	}
}

void	Manager::shutdown(void)
{
	std::vector<Client>::iterator	it;
	std::map<int, int>::iterator	ite;
	std::vector<int>				sock;

	it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		close((*it).getFD());
		it++;
	}
	ite = this->_sockets.begin();
	while (ite != this->_sockets.end())
	{
		close((*ite).second);
		ite++;
	}
}

void	Manager::manageFDSets(void)
{
	std::map<int, int>::iterator	it;
	std::vector<Client>::iterator	ite;

	FD_ZERO(&this->_rset);
	FD_ZERO(&this->_wset);
	FD_ZERO(&this->_errset);
	it = this->_sockets.begin();
	while (it != this->_sockets.end())
	{
		FD_SET((*it).second, &this->_rset);
		FD_SET((*it).second, &this->_errset);
		it++;
	}
	ite = this->_clients.begin();
	while (ite != this->_clients.end())
	{
		if ((*ite).toRead())
			FD_SET((*ite).getFD(), &this->_rset);
		else
			FD_SET((*ite).getFD(), &this->_wset);
		FD_SET((*ite).getFD(), &this->_errset);
		ite++;
	}
}

void	Manager::managePorts(void)
{
	std::map<int, int>::iterator	itport;
	std::map<int, int>::iterator	tmp;
	std::vector<Server>::iterator	itserv;
	std::vector<int>				servport;
	std::vector<int>::iterator		itservport;
	struct sockaddr_in				ssock;
	int								port;
	int								fdsock;
	int								clfd;

	itport = this->_sockets.begin();
	while (itport != this->_sockets.end())
	{
		port = (*itport).first;
		fdsock = (*itport).second;
		tmp = itport;
		if (FD_ISSET(fdsock, &this->_errset))
		{
			std::cerr << "[!] A critical error occured while listening on port " << port;
			std::cerr << ". Closing the connection..." << "\n" << std::endl;
			close(fdsock);
			itport--;
			this->_sockets.erase(tmp);
			itserv = this->_servs.begin();
			while (itserv != this->_servs.end())
			{
				servport = (*itserv).getPorts();
				itservport = servport.begin();
				while (itservport != servport.end())
				{
					if (port == (*itservport))
						itservport = servport.erase(itservport) - 1;
					itservport++;
				}
				if (servport.empty())
				{
					std::cout << "[-] " << (*itserv).getName() << " doesn't have any port to listen to anymore. ";
					std::cout << "Closing the server..." << "\n" << std::endl;
					itserv = this->_servs.erase(itserv) - 1;
				}
				if (this->_servs.empty())
				{
					std::cout << "[-] " << "No more server are running. ";
					std::cout << "Shutting down the program..." << "\n" << std::endl;
					this->shutdown();
				}
				itserv++;
			}
		}
		else if (FD_ISSET(fdsock, &this->_rset))
		{
			ft_memset(ssock.sin_zero, 0, sizeof(ssock.sin_zero));
			clfd = accept(fdsock, 0, 0);
			// v A enlever ? v
			// clfd = accept(fdsock, reinterpret_cast<struct sockaddr *>(&ssock), reinterpret_cast<socklen_t *>(sizeof(ssock)));
			// ^ A enlever ? ^
			if (clfd == -1)
			{
				std::cerr << "[!] Failed to accept a new client on port " << port << ":" << strerror(errno) << "\n" << std::endl;
				errno = 0;
			}
			if (fcntl(clfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
			{
				std::cerr << "[!] Failed setting the fd of a new client on port " << port << " to non-bloquant";
				std::cerr << ": " << strerror(errno) << ". Closing the connection...\n" << std::endl;
				errno = 0;
				close(clfd);
			}
			try
			{
				Client	cl(this, clfd, port);
				this->_clients.push_back(cl);
				std::cout << "[+] Accepted new client on port " << port << ". Gave him fd " << clfd << "\n" << std::endl;
			}
			catch (const std::exception& e)
			{
				std::cerr << "[!] Failed creating new client on port " << port << ": " << e.what() << '\n' << std::endl;
				close(clfd);
			}
		}
		itport++;
	}
}

void	Manager::manageClients(void)
{
	std::vector<Client>::iterator	it;
	int								fd;
	int								bytes;
	std::string						status;
	std::string						rdstatus;
	std::vector<char>				response;

	it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		fd = (*it).getFD();
		if (FD_ISSET(fd, &this->_errset))
		{
			std::cerr << "[-] An error occured with a client (fd " << fd << ") on port " << (*it).getPort();
			std::cerr << ". Closing the connection...\n" << std::endl;
			close(fd);
			it = this->_clients.erase(it) - 1;
		}
		else if (FD_ISSET(fd, &this->_rset))
		{
			bytes = (*it).readRequest();
			if (bytes < 0)
			{
				std::cerr << "[-] An error occured when reading the request of a client (fd " << fd << ") on port " << (*it).getPort();
				std::cerr << ". Closing the connection...\n" << std::endl;
				close(fd);
				it = this->_clients.erase(it) - 1;
			}
		}
		else if (FD_ISSET(fd, &this->_wset))
		{
			bytes = (*it).writeResponse();
			if (bytes == 0)
			{
				if (!(*it).keepAlive())
				{
					close(fd);
					std::cout << "Closed it\n";
					it = this->_clients.erase(it) - 1;
				}
			}
			else if (bytes < 0)
			{
				std::cerr << "[-] An error occured when sending the response to a client (fd " << fd << ") on port " << (*it).getPort();
				std::cerr << ". Closing the connection...\n" << std::endl;
				close(fd);
				it = this->_clients.erase(it) - 1;
			}
		}
		it++;
	}
}

std::vector<Server>::iterator	Manager::searchServ(std::string name, int port)
{
	std::vector<Server>::iterator							servit;
	std::vector<int>										ports;
	std::vector<int>::iterator								portit;
	std::vector<std::vector<Server>::iterator>				res;
	std::vector<std::vector<Server>::iterator>::iterator	resit;

	servit = this->_servs.begin();
	while (servit != this->_servs.end())
	{
		ports = (*servit).getPorts();
		portit = ports.begin();
		while (portit != ports.end())
		{
			if ((*portit) == port)
				res.push_back((servit));
			portit++;
		}
		servit++;
	}
	resit = res.begin();
	while (resit != res.end())
	{
		if ((*(*resit)).getName() == name)
			return ((*resit));
		resit++;
	}
	return (*res.begin());
}

void	Manager::manageTimeout(void)
{
	std::vector<Client>::iterator	it;
	int								fd;

	it = this->_clients.begin();
	this->_timer = std::time(0);
	while (it != this->_clients.end())
	{
		if ((*it).getTimer() - this->_timer < -9)
		{
			fd = (*it).getFD();
			std::cout << "[-] A client (fd " << fd << ") timed out. ";
			std::cout << "Closing the connection...\n" << std::endl;
			close(fd);
			it = this->_clients.erase(it) - 1;
		}
		it++;
	}
}
