/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/25 13:36:30 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(Manager *main, int fd, int port) : _manager(main), _fd(fd), \
	_port(port), _timer(std::time(0)), _toread(true), _inrequest(false), \
	_keepalive(true), _request(new Request(*this)) { }

Client::Client(Client const &rhs) : _manager(rhs._manager), _fd(rhs._fd), \
	_port(rhs._port), _timer(rhs._timer), _toread(rhs._toread), \
	_inrequest(rhs._inrequest), _keepalive(rhs._keepalive), _request(new Request(*this)) { }

Client::~Client(void)
{ delete this->_request; }

//////////////////////////////
// Overloads
Client	&Client::operator=(Client &rhs)
{
	if (this != &rhs)
	{
		this->_manager = rhs._manager;
		this->_fd = rhs._fd;
		this->_port = rhs._port;
		this->_timer = rhs._timer;
		this->_toread = rhs._toread;
		this->_inrequest = rhs._inrequest;
		this->_keepalive = rhs._keepalive;
		delete this->_request;
		this->_request = new Request(*this);
	}
	return (*this);
}

//////////////////////////////
// Setters
void	Client::setToRead(bool state)
{ this->_toread = state; }

void	Client::setInRequest(bool state)
{ this->_inrequest = state; }

void	Client::setKeepAlive(bool state)
{ this->_keepalive = state; }

//////////////////////////////
// Getters
Manager	*Client::getManager(void)
{ return (this->_manager); }

int	Client::getFD(void) const
{ return (this->_fd); }

int	Client::getPort(void) const
{ return (this->_port); }

time_t	Client::getTimer(void) const
{ return (this->_timer); }

bool	Client::toRead(void) const
{ return (this->_toread); }

bool	Client::inRequest(void) const
{ return (this->_inrequest); }

bool	Client::keepAlive(void) const
{ return (this->_keepalive); }

//////////////////////////////
// Functions
void	Client::actualizeTime(void)
{ this->_timer = std::time(0); }

int	Client::readRequest(void)
{ return (this->_request->reader()); }

int	Client::writeResponse(void)
{ return (this->_request->writer()); }
