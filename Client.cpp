/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/22 14:17:44 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(Manager &main, int fd, int port) : Request(*this), _manager(main), _port(port), _fd(fd), \
	_timer(std::time(0)), _toread(true), _inrequest(false), _keepalive(true) { }

// Client::Client(Client const &rhs) : _main(rhs._main)
// {
// 	this->_request = rhs._request;
// 	this->_port = rhs._port;
// 	this->_fd = rhs._fd;
// 	this->_timer = rhs._timer;
// 	this->_toread = rhs._toread;
// 	this->_inrequest = rhs._inrequest;
// 	this->_keepalive = rhs._keepalive;
// }

Client::~Client(void) { }

//////////////////////////////
// Overloads
Client	&Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_port = rhs._port;
		this->_fd = rhs._fd;
		this->_timer = rhs._timer;
		this->_toread = rhs._toread;
		this->_inrequest = rhs._inrequest;
		this->_keepalive = rhs._keepalive;
		this->_request = rhs._request;
		this->_header = rhs._header;
		this->_body = rhs._body;
		this->_filepath = rhs._filepath;
		this->_bodyresponse = rhs._bodyresponse;
		this->_headerresponse = rhs._headerresponse;
		this->_response = rhs._response;
		this->_contentlength = rhs._contentlength;
		this->_maxcontentlength = rhs._maxcontentlength;
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
Manager	&Client::getManager(void)
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
{ return (this->read()); }

int	Client::writeResponse(void)
{ return (this->write()); }
