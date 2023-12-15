/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2023/12/14 13:34:49 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

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
	while (i < 2048)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	this->_buffer[i] = '\0';
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
}

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

Client::~Client(void) { }

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

bool	toRead(void) const
{ return (this->_toread); }

// Functions
int	Client::read(void)
{
	int		bytes;
	size_t	pos;
	// Read from the client
	bytes = recv(this->_fd, this->_buffer, 2048);
	// Print
	std::cout << this->_buffer << std::endl;
	// Check if the client is finished
	if (bytes == 0)
	{
		this->_toread = false;
		return (bytes);
	}
	// If not, update values
	this->_totalbytes = this->_totalbytes + bytes;
	this->_request = this->_request + this->_buffer;
	this->_sentbytes = 0;
	// Search end sequence
	pos = this->_request.find("\r\n\r\n");
	if (pos != std::string::npos && this->_header.find("\r\n\r\n") == std::string::npos) // Check if it's the header...
	{
		this->_header = this->_request.substr(0, pos + 4);
		std::cout << this->_header << std::endl;
		this->_request.erase(0, pos + 4);
		std::cout << this->_request << std::endl;
	}
	else if (pos != std::string::npos && this->_header.find("\r\n\r\n") == std::string::npos) // ... or the body
	{
		this->_body = this->_request.substr(0, pos + 4);
		std::cout << this->_body << std::endl;
		this->_request.erase(0, pos + 4);
		std::cout << this->_request << std::endl;
	}
	return (bytes);
}
