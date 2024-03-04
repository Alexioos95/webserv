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
// Constructors and Destructor
Manager::Manager(void)
{ this->_timeout.tv_sec = 1; this->_timeout.tv_usec = 0; }

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

Server	Manager::getServ(std::string name, int port)
{
	std::vector<int>										ports;
	std::vector<std::vector<Server>::iterator>				res;
	std::vector<Server>::iterator							it_serv;
	std::vector<int>::iterator								it_port;
	std::vector<std::vector<Server>::iterator>::iterator	it_res;

	it_serv = this->_servs.begin();
	while (it_serv != this->_servs.end())
	{
		ports = (*it_serv).getPorts();
		it_port = ports.begin();
		while (it_port != ports.end())
		{
			if ((*it_port) == port)
				res.push_back((it_serv));
			it_port++;
		}
		it_serv++;
	}
	it_res = res.begin();
	while (it_res != res.end())
	{
		if ((*(*it_res)).getName() == name)
			return (Server(*((*it_res))));
		it_res++;
	}
	return (Server(*(*res.begin())));
}

//////////////////////////////
// Functions: public
void	Manager::defaultconfig(void)
{
	std::vector<int>	vec;
	vec.push_back(8080);
	vec.push_back(8081);
	vec.push_back(8082);
	std::vector<int>	vec2;
	vec2.push_back(8090);
	vec2.push_back(8091);
	vec2.push_back(8092);
	std::vector<int>	vec3;
	vec3.push_back(9000);
	vec3.push_back(9001);
	vec3.push_back(9002);
	std::map<std::string, std::string>	err1;
	std::map<std::string, std::string>	err2;
	std::map<std::string, std::string>	err3;
	err1.insert(std::pair<std::string, std::string>("403", "/error_page/403.html"));
	err1.insert(std::pair<std::string, std::string>("404", "/error_page/404.html"));
	std::vector<Location>	loc1;
	std::vector<Location>	loc2;
	std::vector<Location>	loc3;
		// Methods - Get, Post, Delete (bools)
		// CGI			(bool)
		// Autoindex	(bool)
		// Index		(pair)
		// Post			(pair)
		// Return		(pair)
		// Alias		(pair)
	Location	l1("/", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(true, "/html/index.html"),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l2("/css", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l3("/html", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l4("/images", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l5("/index.html", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(true, "/html/index.html"));

	Location	l6("/error_page", false, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l7("/img", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l8("/js", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l9("/", true, false, false,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(true, "/index.html"),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	Location	l10("/upload", true, true, true,	\
		false,	\
		false,	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(true, "/download"),	\
		std::pair<bool, std::string>(false, ""),	\
		std::pair<bool, std::string>(false, ""));

	loc1.push_back(l1);
	loc1.push_back(l2);
	loc1.push_back(l3);
	loc1.push_back(l4);
	loc1.push_back(l5);
	loc1.push_back(l6);
	loc2.push_back(l7);
	loc2.push_back(l8);
	loc3.push_back(l9);
	loc3.push_back(l10);
	Server	s1("qr.code", "./qr", vec, err1, loc1, 99999, this->_sockets);
	Server	s2("netpractice.net", "./np/", vec2, err2, loc2, 99999, this->_sockets);
	Server	s3("form.org", "./form/", vec3, err3, loc3, 99999, this->_sockets);
	this->_servs.push_back(s1);
	this->_servs.push_back(s2);
	this->_servs.push_back(s3);
	std::cout << std::endl;
}

void	Manager::run(void)
{
	while (1)
	{
		if (g_sigint == true)
		{
			this->shutdown();
			throw (SigintException());
		}
		this->manageFDSets();
		if (select(FD_SETSIZE, &this->_rset, &this->_wset, &this->_errset, &this->_timeout) == -1)
		{
			std::cerr << "\n[!] Critical error on select: " << strerror(errno) << ". Shutting down the program..." << std::endl;
			throw (SigintException());
		}
		this->managePorts();
		this->manageClients();
		this->manageTimeout();
	}
}

void	Manager::shutdown(void)
{
	std::vector<Client>::iterator	it;
	std::map<int, int>::iterator	ite;

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

//////////////////////////////
// Functions: private
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
	std::vector<int>				servport;
	std::map<int, int>::iterator	it_port;
	std::vector<Server>::iterator	it_serv;
	std::vector<int>::iterator		it_servport;
	std::map<int, int>::iterator	tmp;
	int								port;
	int								fdsock;

	it_port = this->_sockets.begin();
	while (it_port != this->_sockets.end())
	{
		port = (*it_port).first;
		fdsock = (*it_port).second;
		if (FD_ISSET(fdsock, &this->_errset))
		{
			std::cerr << "[!] A critical error occured while listening on port " << port;
			std::cerr << ". Closing the connection..." << "\n" << std::endl;
			close(fdsock);
			tmp = it_port;
			it_port--;
			this->_sockets.erase(tmp);
			it_serv = this->_servs.begin();
			while (it_serv != this->_servs.end())
			{
				servport = (*it_serv).getPorts();
				it_servport = servport.begin();
				while (it_servport != servport.end())
				{
					if (port == (*it_servport))
						it_servport = servport.erase(it_servport) - 1;
					it_servport++;
				}
				if (servport.empty())
				{
					std::cout << "[-] " << (*it_serv).getName() << " doesn't have any port to listen to anymore. ";
					std::cout << "Closing the server..." << "\n" << std::endl;
					it_serv = this->_servs.erase(it_serv) - 1;
				}
				if (this->_servs.empty())
				{
					std::cout << "[-] " << "No more server are running. ";
					std::cout << "Shutting down the program..." << "\n" << std::endl;
					this->shutdown();
				}
				it_serv++;
			}
		}
		else if (FD_ISSET(fdsock, &this->_rset))
			this->acceptClient(fdsock, port);
		it_port++;
	}
}

void	Manager::acceptClient(int fdsock, int port)
{
	int					fd;
	struct sockaddr_in	ssock;

	ft_memset(ssock.sin_zero, 0, sizeof(ssock.sin_zero));
	fd = accept(fdsock, 0, 0);
	if (fd == -1)
	{
		std::cerr << "[!] Failed to accept a new client on port " << port << ": " << strerror(errno) << "\n" << std::endl;
		errno = 0;
		return ;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		std::cerr << "[!] Failed setting the fd of a new client on port " << port << " to non-bloquant";
		std::cerr << ": " << strerror(errno) << ". Closing the connection...\n" << std::endl;
		close(fd);
		errno = 0;
		return ;
	}
	try
	{
		Client	cl(this, fd, port);
		this->_clients.push_back(cl);
		std::cout << "[+] Accepted new client on port " << port << ". Gave him fd " << fd << "\n" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[!] Failed creating new client on port " << port << ": " << e.what() << '\n' << std::endl;
		close(fd);
	}
}

void	Manager::manageClients(void)
{
	std::vector<Client>::iterator	it;
	int								fd;
	int								bytes;

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

void	Manager::manageTimeout(void)
{
	std::vector<Client>::iterator	it;

	it = this->_clients.begin();
	this->_timer = std::time(0);
	while (it != this->_clients.end())
	{
		if (this->_timer - (*it).getTimer() > 119)
		{
			std::cout << "[-] A client (fd " << (*it).getFD() << ") timed out. ";
			std::cout << "Closing the connection...\n" << std::endl;
			close((*it).getFD());
			it = this->_clients.erase(it) - 1;
		}
		it++;
	}
}
