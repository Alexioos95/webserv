/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/11 16:14:47 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(Server &serv) : _server(serv), _fd(-1), _toread(true), \
	_contentlength(0), _maxcontentlength(0), _inrequest(false), _keepalive(true) { }

Client::Client(Client const &rhs) : _server(rhs._server)
{
	this->_fd = rhs._fd;
	this->_toread = rhs._toread;
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	this->_filepath = rhs._filepath;
	this->_filecontent = rhs._filecontent;
	this->_contentlength = rhs._contentlength;
	this->_maxcontentlength = rhs._maxcontentlength;
	this->_inrequest = rhs._inrequest;
	this->_keepalive = rhs._keepalive;
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
	this->_server = rhs._server;
	this->_fd = rhs._fd;
	this->_toread = rhs._toread;
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	if (this->_file.is_open())
		this->_file.close();
	this->_filepath = rhs._filepath;
	this->_filecontent = rhs._filecontent;
	this->_contentlength = rhs._contentlength;
	this->_maxcontentlength = rhs._maxcontentlength;
	this->_inrequest = rhs._inrequest;
	this->_keepalive = rhs._keepalive;
	return (*this);
}

//////////////////////////////
// Setters
void	Client::setFD(int fd)
{ this->_fd = fd; }

void	Client::setInRequest(bool state)
{ this->_inrequest = state; }

void	Client::setKeepAlive(bool state)
{ this->_keepalive = state; }

void	Client::setToRead(bool state)
{ this->_toread = state; }

//////////////////////////////
// Getters
Server	&Client::getServer(void)
{ return (this->_server); }

int	Client::getFD(void) const
{ return (this->_fd); }

std::string	Client::getHeader(void) const
{ return (this->_header); }

std::fstream	&Client::getFile(void)
{ return (this->_file); }

std::string	Client::getFilePath(void) const
{ return (this->_filepath); }

std::vector<char>	&Client::getFileContent(void)
{ return (this->_filecontent); }

int	Client::getContentLength(void) const
{ return (this->_contentlength); }

bool	Client::toRead(void) const
{ return (this->_toread); }

bool	Client::fileIsOpen(void) const
{ return (this->_file.is_open()); }

bool	Client::inRequest(void) const
{ return (this->_inrequest); }

bool	Client::keepAlive(void) const
{ return (this->_keepalive); }

//////////////////////////////
// Functions
int	Client::readRequest(void)
{
	char		buffer[2048 + 1];
	int			bytes;
	size_t		pos;
	std::string	nb;

	ft_memset(buffer, 0, 2048 + 1);
	bytes = recv(this->_fd, buffer, 2048, 0);
	if (bytes <= 0)
		return (bytes);
	this->_request = this->_request + buffer;
	pos = this->_request.find("\r\n\r\n");
	if (pos == std::string::npos)
		std::cout << "[*] Buffer of client: fd " << this->_fd << ":" << std::endl << this->_header << std::endl;
	else
	{
		if (this->_header.find("\r\n\r\n") == std::string::npos)
		{
			this->_header = this->_request.substr(0, pos + 4);
			std::cout << "[*] Header of client: fd " << this->_fd << ":" << std::endl << this->_header.substr(0, this->_header.length() - 6) << std::endl;
			this->_request.erase(0, pos + 4);
			if (this->_request.find("Content-Length: ") == std::string::npos)
				this->_toread = false;
			else
			{
				pos = this->_request.find("Content-Length: ");
				nb = this->_request.substr(pos + 16, this->_request.find("\r\n", pos) - pos);
				this->_maxcontentlength = std::atoi(nb.c_str());
			}
		}
		else
		{
			this->_body = this->_request.substr(0, pos + 4);
			std::cout << "[*] Body of client: fd " << this->_fd << ":" << std::endl << this->_body << std::endl;
			this->_request.erase(0, pos + 4);
			this->_toread = false;
		}
	}
	return (bytes);
}

std::string	Client::openFile(std::string path)
{
	errno = 0;
	this->_file.open(path.c_str(), std::ios::in);
	if (errno)
	{
		if (errno == ELOOP)
			return ("310 Too many Redirects");
		else if (errno == EACCES)
			return ("403 Forbidden");
		else if (errno == ENOENT || errno == EFAULT || errno == ENODEV)
			return ("404 Not Found");
		else if (errno == EFBIG)
			return ("413 Request Entity Too Large");
		else if (errno == ENAMETOOLONG)
			return ("414 Request-URI Too Long");
		else if (errno == EMFILE || errno == ENFILE || errno == ENOMEM || errno == ENOSPC)
			return ("503 Service Unavailable");
		else
			return ("500 Internal Server Error");
	}
	this->_filepath = path;
	return ("200 OK");
}

std::string	Client::readFile(void)
{
	char			buffer[2048];
	std::streamsize	size;

	this->_file.read(buffer, 2048);
	if (this->_file.fail() && !this->_file.eof())
	{
		this->_filecontent.erase(this->_filecontent.begin(), this->_filecontent.end());
		return ("500 Internal Server Error");
	}
	size = this->_file.gcount();
	this->_filecontent.insert(this->_filecontent.end(), &buffer[0], &buffer[size]);
	this->_contentlength = this->_contentlength + size;
	this->_inrequest = true;
	if (size < 2048)
		return ("200 OK");
	return ("102 Processing");
}

void	Client::clear(void)
{
	std::vector<char>	tmp;

	this->_toread = true;
	this->_request = "";
	this->_header = "";
	this->_body = "";
	this->_filepath = "";
	this->_filecontent = tmp;
	this->_contentlength = 0;
	this->_maxcontentlength = 0;
	this->_inrequest = false;
	this->_toread = true;
}
