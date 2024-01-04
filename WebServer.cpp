/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:38:00 by apayen            #+#    #+#             */
/*   Updated: 2024/01/04 16:15:09 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "ServerBlock.hpp"
#include "Client.hpp"

//////////////////////////////
// Exception
const char	*WebServer::SelectException::what(void) const throw()
{ return (""); }

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
	ServerBlock	tmp("no_name", "/np/", vec);
	this->_servs.push_back(tmp);

	while (1)
	{
		if (g_sigint == true)
			throw (SigintException());
		this->ManageFDSets();
		if (select(FD_SETSIZE, &this->_rset, &this->_wset, &this->_errset, NULL) == -1)
			throw (SelectException());
		// else
		// {
		// 	if (FD_ISSET(this->_socket, &this->_errset))
		// 		throw (CriticalErrorException());
		// 	else if (FD_ISSET(this->_socket, &this->_rset))
		// 		this->NewClient();
		// 	this->Communicate();
		// }
	}
}

void	WebServer::ManageFDSets(void)
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

// void	ServerBlock::Communicate(void)
// {
// 	std::vector<Client>::iterator	it;
// 	int								bytes;

// 	it = this->_clients.begin();
// 	while (it != this->_clients.end())
// 	{
// 		if (FD_ISSET((*it).getFD(), &this->_errset))
// 		{
// 			std::cerr << "[-] An error occured with the client's fd " << (*it).getFD() << ". Closed the connection" << std::endl;
// 			close((*it).getFD());
// 			(*it).setFD(-1);
// 			it = this->_clients.erase(it) - 1;
// 		}
// 		else if (FD_ISSET((*it).getFD(), &this->_rset))
// 		{
// 			bytes = (*it).read();
// 			if (bytes <= 0)
// 			{
// 				std::cerr << "[-] An error occured when reading from client's fd " << (*it).getFD() << ". Closed the connection" << std::endl;
// 				close((*it).getFD());
// 				(*it).setFD(-1);
// 				it = this->_clients.erase(it) - 1;
// 				if (bytes == -1)
// 					throw (FailReadException());
// 			}
// 		}
// 		else if (FD_ISSET((*it).getFD(), &this->_wset))
// 		{
// 			std::string hello("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 13\n\nHello world!\n");
// 			write((*it).getFD(), hello.c_str(), strlen(hello.c_str()));
// 			(*it).setTotalbytes(0);
// 		}
// 		it++;
// 	}
// }

// void	ServerBlock::NewClient(void)
// {
// 	Client	cl;

// 	cl.setFD(accept(this->_socket, 0, 0));
// 	// cl.setFD(accept(this->_socket, reinterpret_cast<struct sockaddr *>(&this->_structsock), reinterpret_cast<socklen_t *>(&this->_addr)));
// 	if (cl.getFD() == -1)
// 		throw (FailAcceptingClientException());
// 	this->setNonblockingFD(cl.getFD());
// 	this->_clients.push_back(cl);
// 	std::cout << "[+] Accepted new client. Gave him fd " << cl.getFD() << std::endl;
// }
