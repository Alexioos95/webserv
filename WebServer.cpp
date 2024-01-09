/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2024/01/09 15:08:48 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "ServerBlock.hpp"
#include "Client.hpp"

//////////////////////////////
// Exception
const char	*WebServer::SelectException::what(void) const throw()
{ return ("Select: "); }

//////////////////////////////
// Constructors and Destructor
WebServer::WebServer(void) { }

WebServer::~WebServer(void)
{
	std::vector<int>					sock;
	std::vector<ServerBlock>::iterator	it;
	std::vector<int>::iterator			ite;

	it = this->_servs.begin();
	while (it != this->_servs.end())
	{
		sock = (*it).getSocket();
		ite = sock.begin();
		while (ite != sock.end())
		{
			close(*ite);
			ite++;
		}
		it++;
	}
}

//////////////////////////////
// Functions
void	WebServer::run(void)
{
	std::vector<int>	vec;
	vec.push_back(8080);
	vec.push_back(8081);
	ServerBlock	tmp("no_name", "./np/", vec);
	this->_servs.push_back(tmp);
	// ^ A enlever, c'est juste pour test. ^

	std::cout << std::endl;
	while (1)
	{
		if (g_sigint == true)
		{
			this->shutdown();
			throw (SigintException());
		}
		this->manageFDSets();
		if (select(FD_SETSIZE, &this->_rset, &this->_wset, &this->_errset, NULL) == -1)
		{
			std::cerr << "\n[!] Critical error on select: " << strerror(errno) << ". Shutting down the program..." << std::endl;
			this->shutdown();
			throw(SigintException());
		}
		else
		{
			this->checkServerBlocks();
			this->checkClients();
		}
	}
}

void	WebServer::shutdown(void)
{
	std::vector<Client>::iterator		it;
	std::vector<ServerBlock>::iterator	ite;
	std::vector<int>					sock;
	std::vector<int>::iterator			iter;

	it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if ((*it).fileIsOpen())
			(*it).getFile().close();
		close((*it).getFD());
		it++;
	}
	ite = this->_servs.begin();
	while (ite != this->_servs.end())
	{
		sock = (*ite).getSocket();
		iter = sock.begin();
		while (iter != sock.end())
		{
			close((*iter));
			iter++;
		}
		ite++;
	}
}

void	WebServer::manageFDSets(void)
{
	std::vector<int>					sock;
	std::vector<ServerBlock>::iterator	it;
	std::vector<int>::iterator			ite;
	std::vector<Client>::iterator		iter;

	FD_ZERO(&this->_rset);
	FD_ZERO(&this->_wset);
	FD_ZERO(&this->_errset);
	it = this->_servs.begin();
	while (it != this->_servs.end())
	{
		sock = (*it).getSocket();
		ite = sock.begin();
		while (ite != sock.end())
		{
			FD_SET((*ite), &this->_rset);
			FD_SET((*ite), &this->_errset);
			ite++;
		}
		it++;
	}
	iter = this->_clients.begin();
	while (iter != this->_clients.end())
	{
		if ((*iter).toRead())
			FD_SET((*iter).getFD(), &this->_rset);
		else
			FD_SET((*iter).getFD(), &this->_wset);
		FD_SET((*iter).getFD(), &this->_errset);
		iter++;
	}
}

void	WebServer::checkServerBlocks(void)
{
	std::vector<int>					sock;
	std::vector<int>					port;
	std::vector<ServerBlock>::iterator	sb;
	std::vector<int>::iterator			sck;
	std::vector<int>::iterator			prt;

	sb = this->_servs.begin();
	while (sb != this->_servs.end())
	{
		sock = (*sb).getSocket();
		port = (*sb).getPorts();
		sck = sock.begin();
		prt = port.begin();
		while (sck != sock.end())
		{
			if (FD_ISSET((*sck), &this->_errset))
			{
				std::cerr << "[!] Error on " << (*sb).getName() << ":" << (*prt) << ". Closed the connection" << std::endl;
				close(*sck);
				sck = sock.erase(sck) - 1;
				prt = port.erase(prt) - 1;
			}
			else if (FD_ISSET((*sck), &this->_rset))
				this->newClient((*sb), (*prt), (*sck));
			sck++;
			prt++;
		}
		if (sock.empty())
		{
			std::cerr << "[-] " << (*sb).getName() << " isn't binded to any port anymore. Shutting down the server..." << std::endl;
			sb = this->_servs.erase(sb) - 1;
		}
		if (this->_servs.empty())
		{
			std::cerr << std::endl << "[-] There is no server running anymore. Shutting down the program...";
			g_sigint = true;
		}
		sb++;
	}
}

void	WebServer::newClient(ServerBlock sb, int port, int socket)
{
	Client		cl(sb);
	std::string	name;

	name = sb.getName();
	cl.setFD(accept(socket, 0, 0));
	// cl.setFD(accept(socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), reinterpret_cast<socklen_t *>(&this->_addr)));
	// ^ A enlever ? ^
	if (cl.getFD() == -1)
	{
		std::cerr << "[!] Failed to accept new client on " << name << ":" << port << ":" << strerror(errno) << std::endl;
		return ;
	}
	if (setNonblockingFD(cl.getFD()) == 1)
	{
		std::cerr << "[!] Failed setting the fd of a new client of " << name << ":" << port << " to non-bloquant";
		std::cerr << ": " << strerror(errno) << ". Closed the connection" << std::endl;
		return ;
	}
	this->_clients.push_back(cl);
	std::cout << "[+] Accepted new client on " << name << ":" << port << ". Gave him fd " << cl.getFD() << std::endl;
}

void	WebServer::checkClients(void)
{
	std::vector<Client>::iterator	it;
	int								fd;
	int								bytes;
	int								status;

	it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		fd = (*it).getFD();
		if (FD_ISSET(fd, &this->_errset))
		{
			std::cerr << "[-] An error occured with a client: fd " << fd << ". Closed the connection" << std::endl;
			close(fd);
			it = this->_clients.erase(it) - 1;
		}
		else if (FD_ISSET(fd, &this->_rset))
		{
			bytes = (*it).readRequest();
			if (bytes <= 0)
			{
				std::cerr << "[-] An error occured when reading from client: fd " << fd << ". Closed the connection" << std::endl;
				close(fd);
				it = this->_clients.erase(it) - 1;
			}
		}
		else if (FD_ISSET(fd, &this->_wset))
		{
			status = this->parseRequest((*it));
			if (status != 102)
			{
				// Request KO.
			}
			else if (!(*it).readFile());
			{
				// Finished reading, send request.
			}
		}
		it++;
	}
}

int	WebServer::parseRequest(Client &cl)
{
	if (cl.inRequest())
		return (102);
	std::string		header;
	std::string		root;
	std::string		path;
	std::string		line;
	std::string		method;
	std::string		file;
	std::string		version;

	header = cl.getHeader();
	root = cl.getServer().getRoot();
	if (root.find('/', root.length() - 1) == std::string::npos)
		root = root + '/';
	line = header.substr(0, header.find("\r\n"));
	method = line.substr(0, line.find(' '));
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (405);
	file = line.substr((method.length() + 1), (line.find(' ', method.length() + 1) - method.length()));
	path = root + file;
	cl.openFile(path);
	version = line.substr((method.length() + file.length() + 1), (line.find(' ', method.length() + file.length()) + 2));
	if (version != "HTTP/1.1")
	{
		cl.getFile().close();
		return (505);
	}
	cl.setInRequest(true);
	return (102);
}
