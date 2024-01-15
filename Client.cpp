/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2024/01/15 12:59:49 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Client::Client(int fd, int port) :_port(port), _fd(fd),  _timer(std::time(0)), \
	_contentlength(0), _maxcontentlength(0), _toread(true), _inrequest(false), _keepalive(true) { }

Client::Client(Client const &rhs)
{
	this->_port = rhs._port;
	this->_fd = rhs._fd;
	this->_timer = rhs._timer;
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	this->_filepath = rhs._filepath;
	this->_filecontent = rhs._filecontent;
	this->_contentlength = rhs._contentlength;
	this->_maxcontentlength = rhs._maxcontentlength;
	this->_toread = rhs._toread;
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
	if (this->_file.is_open())
		this->_file.close();
	this->_port = rhs._port;
	this->_fd = rhs._fd;
	this->_timer = rhs._timer;
	this->_request = rhs._request;
	this->_header = rhs._header;
	this->_body = rhs._body;
	this->_filepath = rhs._filepath;
	this->_filecontent = rhs._filecontent;
	this->_contentlength = rhs._contentlength;
	this->_maxcontentlength = rhs._maxcontentlength;
	this->_toread = rhs._toread;
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
int	Client::getPort(void) const
{ return (this->_port); }

int	Client::getFD(void) const
{ return (this->_fd); }

time_t	Client::getTimer(void) const
{ return (this->_timer); }

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

int	Client::getMaxContentLength(void) const
{ return (this->_maxcontentlength); }

bool	Client::toRead(void) const
{ return (this->_toread); }

bool	Client::inRequest(void) const
{ return (this->_inrequest); }

bool	Client::fileIsOpen(void) const
{ return (this->_file.is_open()); }

bool	Client::keepAlive(void) const
{ return (this->_keepalive); }

//////////////////////////////
// Functions
void	Client::actualizeTime(void)
{ this->_timer = std::time(0); }


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
	{
		std::cout << "[*] Buffer of client (fd " << this->_fd << ") on port " << this->_port << "\n";
		std::cout << this->_header << "\n" << std::endl;
	}
	else
	{
		if (this->_header.find("\r\n\r\n") == std::string::npos)
		{
			this->_header = this->_header + this->_request.substr(0, pos + 4);
			std::cout << "[*] Header of client (fd " << this->_fd << ") on port " << this->_port << "\n";
			std::cout << this->_header.substr(0, this->_header.length() - 4) << "\n" << std::endl;
			this->_request.erase(0, pos + 4);
			if (this->_header.find("\r\n\r\n") != std::string::npos \
				&& this->_header.find("Content-Length: ") == std::string::npos)
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
			this->_body = this->_body + this->_request.substr(0, pos + 4);
			std::cout << "[*] Body of client (fd " << this->_fd << ") on port " << this->_port << "\n";
			std::cout << this->_body << "\n" << std::endl;
			this->_request.erase(0, pos + 4);
			this->_contentlength = this->_contentlength + bytes;
			if (this->_contentlength >= this->_maxcontentlength || this->_body.find("\r\n\r\n") != std::string::npos)
			{
				this->_contentlength = 0;
				this->_toread = false;
			}
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

std::string	Client::searchFile(std::string path)
{
	std::string	dir;
	dir = path.substr(0, path.find_last_of("/") + 1);
	if (access(path.c_str(), F_OK) == -1)
		return ("404 Not Found");
	else if (access(dir.c_str(), W_OK | X_OK) == -1)
		return ("403 Forbidden");
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
	this->_inrequest = false;
}
