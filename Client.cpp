/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/09 14:38:46 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(ServerBlock &server) : _server(server), _fd(-1), _totalbytes(0), _toread(true), _inrequest(false)
{
	ft_memset(this->_buffer, 0, sizeof(this->_buffer));
	ft_memset(this->_buffile, 0, sizeof(this->_buffile));
}

Client::Client(Client const &rhs) : _server(rhs._server)
{
	int	i;

	i = 0;
	this->_fd = rhs._fd;
	this->_totalbytes = rhs._totalbytes;
	this->_toread = rhs._toread;
	while (i < 1024)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	i = 0;
	while (i < 1024)
	{
		this->_buffile[i] = rhs._buffile[i];
		i++;
	}
	this->_buffer[i] = '\0';
	this->_buffile[i] = '\0';
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	this->_inrequest = rhs._inrequest;
}

Client::~Client(void)
{
	if (this->_file.is_open())
		this->_file.close();
}

//////////////////////////////
// Overload
Client	&Client::operator=(Client const &rhs)
{
	int	i;

	i = 0;
	this->_fd = rhs._fd;
	this->_totalbytes = rhs._totalbytes;
	this->_toread = rhs._toread;
	while (i < 1024)
	{
		this->_buffer[i] = rhs._buffer[i];
		i++;
	}
	i = 0;
	while (i < 1024)
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
// Setters

void	Client::setFD(int fd)
{ this->_fd = fd; }

void	Client::setTotalbytes(unsigned int tb)
{ this->_totalbytes = tb; }

void	Client::setInRequest(bool state)
{ this->_inrequest = state; }

//////////////////////////////
// Getters
ServerBlock		&Client::getServer(void)
{ return (this->_server); }

int	Client::getFD(void) const
{ return (this->_fd); }

unsigned int	Client::getTotalbytes(void) const
{ return (this->_totalbytes); }

std::string	Client::getHeader(void) const
{ return (this->_header); }

std::fstream	&Client::getFile(void)
{ return (this->_file); }

bool	Client::toRead(void) const
{ return (this->_toread); }

bool	Client::fileIsOpen(void) const
{ return (this->_file.is_open()); }

bool	Client::inRequest(void) const
{ return (this->_inrequest); }

//////////////////////////////
// Functions
int	Client::readRequest(void)
{
	int		bytes;
	size_t	pos;

	bytes = recv(this->_fd, this->_buffer, 1024, 0);
	// std::cout << "[*] Buffer of client's fd " << this->_fd << ":" << std::endl << this->_buffer << std::endl;
	this->_totalbytes = this->_totalbytes + bytes;
	this->_request = this->_request + this->_buffer;
	pos = this->_request.find("\r\n\r\n");
	if (pos != std::string::npos && this->_header.find("\r\n\r\n") == std::string::npos)
	{
		this->_header = this->_request.substr(0, pos + 4);
		std::cout << "[*] Header of client: fd " << this->_fd << ":" << std::endl << this->_header << std::endl;
		this->_request.erase(0, pos + 4);
		if (this->_request.find("Content-Length: ") == std::string::npos)
			this->_toread = false;
	}
	else if (pos != std::string::npos && this->_body.find("\r\n\r\n") == std::string::npos)
	{
		this->_body = this->_request.substr(0, pos + 4);
		this->_request.erase(0, pos + 4);
		if (_request == "")
			this->_toread = false;
	}
	return (bytes);
}

int	Client::openFile(std::string path)
{
	this->_file.open(path, std::ios::in);
	if (errno)
	{
		if (errno == ELOOP)
			return (310);
		else if (errno == EACCES)
			return (403);
		else if (errno == ENOENT || errno == EFAULT || errno == ENODEV)
			return (404);
		else if (errno == EFBIG)
			return (413);
		else if (errno == ENAMETOOLONG)
			return (414);
		else if (errno == EMFILE || errno == ENFILE || errno == ENOMEM || errno == ENOSPC)
			return (503);
		else
			return (500);
	}
	return (0);
}

int	Client::readFile(void)
{
	this->_file.read(this->_buffile, 1024);
	this->_text = this->_text + this->_buffile;
	if (this->_file.eof())
	{
		this->_inrequest = false;
		return (0);
	}
	this->_inrequest = true;
	return (1);
}
