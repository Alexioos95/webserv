/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/04 11:09:29 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(void) : _fd(-1), _totalbytes(0), _sentbytes(0), _toread(true)
{ ft_memset(this->_buffer, 0, sizeof(this->_buffer)); }

Client::Client(Client const &rhs)
{
	int	i;

	i = 0;
	this->_fd = rhs._fd;
	this->_totalbytes = rhs._totalbytes;
	this->_sentbytes = rhs._sentbytes;
	this->_toread = rhs._toread;
	while (i < 1024)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	this->_buffer[i] = '\0';
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
}

Client::~Client(void) { }

//////////////////////////////
// Overload
Client	&Client::operator=(Client const &rhs)
{
	int	i;

	i = 0;
	this->_fd = rhs._fd;
	this->_totalbytes = rhs._totalbytes;
	this->_sentbytes = rhs._sentbytes;
	this->_toread = rhs._toread;
	while (i < 2048)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	this->_buffer[i] = '\0';
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	return (*this);
}

//////////////////////////////
// Setters and Getters
void	Client::setFD(int fd)
{ this->_fd = fd; }

int	Client::getFD(void) const
{ return (this->_fd); }

void	Client::setTotalbytes(unsigned int tb)
{ this->_totalbytes = tb; }

unsigned int	Client::getTotalbytes(void) const
{ return (this->_totalbytes); }

void	Client::setSentbytes(unsigned int sb)
{ this->_sentbytes = sb; }

unsigned int	Client::getSentbytes(void) const
{ return (this->_sentbytes); }

std::string	Client::getHeader(void) const
{ return (this->_header); }

bool	Client::toRead(void) const
{ return (this->_toread); }

//////////////////////////////
// Functions
int	Client::read(void)
{
	int		bytes;
	size_t	pos;
	bytes = recv(this->_fd, this->_buffer, 1024, 0);
	std::cout << "[*] Buffer of client's fd " << this->_fd << ":" << std::endl << this->_buffer << std::endl;
	this->_totalbytes = this->_totalbytes + bytes;
	this->_request = this->_request + this->_buffer;
	this->_sentbytes = 0;
	pos = this->_request.find("\r\n\r\n");
	if (pos != std::string::npos && this->_header.find("\r\n\r\n") == std::string::npos)
	{
		this->_header = this->_request.substr(0, pos + 4);
		this->_request.erase(0, pos + 4);
		if (this->_request.find("Content-Length: ") == std::string::npos)
			this->_toread = false;
	}
	else if (pos != std::string::npos && this->_header.find("\r\n\r\n") == std::string::npos)
	{
		this->_body = this->_request.substr(0, pos + 4);
		this->_request.erase(0, pos + 4);
		if (_request == "")
			this->_toread = false;
	}
	return (bytes);
}
