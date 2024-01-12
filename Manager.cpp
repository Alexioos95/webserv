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
#include "VirtualServer.hpp"
#include "Client.hpp"

//////////////////////////////
// Exception
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
// Overload
Manager	&Manager::operator=(Manager const &rhs)
{
	this->shutdown();
	this->_servs = rhs._servs;
	this->_clients = rhs._clients;
	this->_rset = rhs._rset;
	this->_wset = rhs._wset;
	this->_errset = rhs._errset;
	return (*this);
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
	std::vector<int>	vec2;
	vec2.push_back(8090);
	vec2.push_back(8091);
	vec2.push_back(8092);
	VirtualServer	tmp("test.com", "./qr", vec, this->_sockets, 99999);
	VirtualServer	tmp2("netpractice.net", "./np", vec2, this->_sockets, 99999);
	this->_servs.push_back(tmp);
	this->_servs.push_back(tmp2);
	std::cout << std::endl;
	// ^ A enlever, c'est juste pour test. ^

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
			throw (SigintException());
		}
		else
		{
			this->checkPorts();
			this->checkClients();
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
		if ((*it).fileIsOpen())
			(*it).getFile().close();
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

void	Manager::checkPorts(void)
{
	std::map<int, int>::iterator			itport;
	std::map<int, int>::iterator			tmp;
	std::vector<VirtualServer>::iterator	itserv;
	std::vector<int>						servport;
	std::vector<int>::iterator				itservport;
	struct sockaddr_in						ssock;
	int										port;
	int										fdsock;
	int										clfd;

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
					std::cout << "[-] " << (*itserv).getName() << " doesn't have any port to listen to anymore";
					std::cout << ". Closing the server..." << "\n" << std::endl;
					itserv = this->_servs.erase(itserv) - 1;
				}
				if (this->_servs.empty())
				{
					std::cout << "[-] " << "No more server are running";
					std::cout << ". Shutting down the program..." << "\n" << std::endl;
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
				std::cerr << "[!] Failed to accept a new client on port " << port << ":" << strerror(errno) << "\n" << std::endl;
			if (unblockFD(clfd) == 1)
			{
				std::cerr << "[!] Failed setting the fd of a new client on port " << port << " to non-bloquant";
				std::cerr << ": " << strerror(errno) << ". Closing the connection...\n" << std::endl;
				close(clfd);
			}
			Client	cl(clfd, port);
			this->_clients.push_back(cl);
			std::cout << "[+] Accepted new client on port " << port << ". Gave him fd " << clfd << "\n" << std::endl;
		}
		itport++;
	}
}

void	Manager::checkClients(void)
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
				std::cout << ". Closing the connection...\n" << std::endl;
				close(fd);
				it = this->_clients.erase(it) - 1;
			}
		}
		else if (FD_ISSET(fd, &this->_wset))
		{
			status = this->parseRequest((*it));
			rdstatus = (*it).readFile();
			if (status != "102 Processing" || rdstatus != "102 Processing")
			{
				response = this->buildResponse((*it), status);
				if (send(fd, response.data(), response.size(), 0) <= 0)
				{
					std::cerr << "[-] An error occured when sending the response to a client (fd " << fd << ") on port " << (*it).getPort();
					std::cerr << ". Closing the connection...\n" << std::endl;
					close(fd);
					it = this->_clients.erase(it) - 1;
				}
				else
					(*it).clear();
			}
			else if (!(*it).keepAlive())
			{
				close(fd);
				it = this->_clients.erase(it) - 1;
			}
		}
		it++;
	}
}

std::string	Manager::parseRequest(Client &cl)
{
	if (cl.inRequest())
		return ("102 Processing");
	std::string		status;
	std::string		header;
	std::string		line;
	std::string		host;
	std::string		port;
	std::string		name;
	std::string		length;

	std::string		method;
	std::string		root;
	std::string		path;
	std::string		file;
	std::string		version;
	std::string		keepalive;

	header = cl.getHeader();
	line = header.substr(0, header.find("\r\n"));

	host = header.substr(header.find("Host: ") + 6, header.find("\r\n", header.find("Host: ")));
	name = host.substr(0, host.find(':'));
	port = host.substr(name.length() + 1, host.find("\r\n") - name.length());
	// VirtualServer	tmp = searchServ(name, std::atoi(port));
	method = line.substr(0, line.find(' '));
	if (method != "GET" && method != "POST" && method != "DELETE")
		return ("405 Method Not Allowed");
	if (file.length() > 0 && *file.begin() != '/')
		file = '/' + file;
	file = line.substr((method.length() + 1), (line.find(' ', method.length() + 1) - (method.length() + 1)));
	if (file.empty())
		return ("400 Bad Request");
	path = root + file;
	status = cl.openFile(path);
	if (status != "200 OK")
		return (status);
	version = line.substr((method.length() + file.length() + 2), (line.find("\r\n") - (method.length() + file.length() + 3)));
	if (version != "HTTP/1.1")
	{
		cl.getFile().close();
		return ("505 HTTP Version not supported");
	}
	if (header.find("Connection: keep-alive") != std::string::npos)
		cl.setKeepAlive(true);
	else
		cl.setKeepAlive(false);
	return ("102 Processing");
}

std::vector<char>	Manager::buildResponse(Client &cl, std::string status)
{
	struct stat 		st;
	std::vector<char>	ret;
	std::string			str;

	if (status == "102 Processing")
		status = "200 OK";
	if (status == "200 OK" && stat(cl.getFilePath().c_str(), &st) == -1)
		status = "500 Internal Server Error";
	str = "HTTP/1.1 " + status + "\r\n";
	str = str + "Date: " + getTime(std::time(0)) + "\r\n";
	str = str + "Server: Webserv-42 (Linux)\r\n";
	if (status == "200 OK")
	{
		str = str + "Last-Modified: " + getTime(st.st_mtime) + "\r\n";
		str = str + "Content-Length: " + itoa(cl.getContentLength()) + "\r\n";

		str = str + "Content-Type: " + getMime(cl) + "\r\n";
	}
	else
	{
		// Pages d'erreur perso SI pas presente dans le dir;
	}
	str = str + "Cache-control: no-cache\r\n";
	if (cl.keepAlive())
		str = str + "Connection: keep-alive\r\n";
	else
		str = str + "Connection: closed\r\n";
	std::cout << "[*] Response's header sent on " << cl.getPort();
	std::cout << " (fd " << cl.getFD() << ")\n" << str << std::endl;
	str = str + "\r\n";
	ret.insert(ret.begin(), str.begin(), str.end());
	ret.insert(ret.end(), cl.getFileContent().begin(), cl.getFileContent().end());
	str = "\r\n\r\n";
	ret.insert(ret.end(), str.begin(), str.end());
	return (ret);
}
