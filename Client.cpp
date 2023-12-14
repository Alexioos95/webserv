/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2023/12/14 11:13:59 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Constructors and Destructor
Client::Client(void) : _fd(-1), _totalbytes(0), _sentbytes(0), _buffer(new char[4096 + 1])
{ ft_memset(this->_buffer, 0, sizeof(this->_buffer)); }

Client::Client(Client const &rhs)
{
	int	i;

	i = 0;
	this->_fd = rhs._fd;
	this->_totalbytes = rhs._totalbytes;
	this->_sentbytes = rhs._sentbytes;
	this->_buffer = new char[4096 + 1];
	while (i < 4096)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	this->_buffer[i] = '\0';
}

Client::~Client(void){ delete [] _buffer; }

// Setters and Getters
void	Client::setFD(int fd)
{ this->_fd = fd; }

int	Client::getFD(void) const
{ return (this->_fd); }

char	*Client::getBuffer(void) const
{ return (this->_buffer); }

void	Client::setTotalbytes(unsigned int tb)
{ this->_totalbytes = tb; }

unsigned int	Client::getTotalbytes(void) const
{ return (this->_totalbytes); }

void	Client::setSentbytes(unsigned int sb)
{ this->_sentbytes = sb; }

unsigned int	Client::getSentbytes(void) const
{ return (this->_sentbytes); }
