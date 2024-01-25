/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:06 by apayen            #+#    #+#             */
/*   Updated: 2024/01/25 13:27:03 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Request.hpp"
#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Request::Request(Client &cl) : _file(-1), _contentlength(0), _maxcontentlength(0), \
	_client(cl), _inparse(true), _inprocess(false), _inbuild(false), _inwrite(false) { }

Request::~Request(void) { }

//////////////////////////////
// Functions: public
int	Request::reader(void)
{
	char		buffer[2048 + 1];
	int			bytes;
	size_t		pos;
	std::string	nb;

	ft_memset(buffer, 0, 2048 + 1);
	bytes = recv(this->_client.getFD(), buffer, 2048, 0);
	if (bytes <= 0)
		return (bytes);
	this->_request = this->_request + buffer;
	pos = this->_request.find("\r\n\r\n");
	if (pos == std::string::npos && this->_contentlength + bytes < this->_maxcontentlength)
	{
		std::cout << "[*] Buffer of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
		std::cout << this->_request << "\n" << std::endl;
	}
	else
	{
		if (this->_header.find("\r\n\r\n") == std::string::npos)
		{
			this->_header = this->_header + this->_request.substr(0, pos + 4);
			this->_request.erase(0, pos + 4);
			if (this->_header.find("\r\n\r\n") != std::string::npos)
			{
				std::cout << "[*] Header of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
				std::cout << this->_header.substr(0, this->_header.length() - 4) << "\n" << std::endl;
				pos = this->_header.find("Content-Length: ");
				if (pos == std::string::npos)
					this->_client.setToRead(false);
				else
				{
					nb = this->_header.substr((pos + 16), (this->_header.find("\r\n", pos) - pos - 16));
					this->_maxcontentlength = std::atoi(nb.c_str());
					if (!this->_request.empty())
					{
						this->_body = this->_body + this->_request;
						this->_contentlength = this->_contentlength + this->_request.length();
						this->_request.erase(0, this->_request.length());
						if (this->_contentlength > this->_maxcontentlength)
							this->_body.resize(this->_maxcontentlength);
						if (this->_contentlength >= this->_maxcontentlength || this->_body.find("\r\n\r\n") != std::string::npos)
						{
							std::cout << "[*] Body of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
							if (this->_body.find("\r\n\r\n") != std::string::npos)
								std::cout << this->_body.substr(0, this->_body.length() - 4) << "\n" << std::endl;
							else
								std::cout << this->_body << "\n" << std::endl;
							this->_client.setToRead(false);
						}
					}
				}
			}
		}
		else
		{
			if (this->_request.find("\r\n\r\n") != std::string::npos)
			{
				this->_body = this->_body + this->_request.substr(0, pos + 4);
				this->_request.erase(0, pos + 4);
			}
			else
			{
				this->_body = this->_body + this->_request;
				this->_request.erase(0, this->_request.length());
			}
			this->_contentlength = this->_contentlength + bytes;
			if (this->_contentlength > this->_maxcontentlength)
				this->_body.resize(this->_maxcontentlength);
			if (this->_contentlength >= this->_maxcontentlength || this->_body.find("\r\n\r\n") != std::string::npos)
			{
				std::cout << "[*] Body of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
				std::cout << this->_body << "\n" << std::endl;
				this->_contentlength = 0;
				this->_client.setToRead(false);
			}
		}
	}
	return (bytes);
}

int	Request::writer(void)
{
	if (this->_inparse)
	{
		this->_client.setInRequest(true);
		this->_status = this->parse();
		if (this->_status == "102 Processing")
		{
			if (this->_method == "GET")
				this->_status = this->openf();
			if (this->_method == "DELETE")
				this->_status = this->del();
			if (this->_method == "POST")
				this->_status = this->create();
			errno = 0;
		}
		this->_inparse = false;
		this->_inprocess = true;
	}
	if (this->_inprocess)
	{
		if (this->_status == "102 Processing")
		{
			if (this->_method == "GET")
				this->_status = this->get();
			else if (this->_method == "POST")
				this->_status = this->post();
		}
		if (this->_status == "200 OK" || this->_status == "202 Created")
		{
			this->_inprocess = false;
			this->_inbuild = true;
		}
		else if (this->_status != "102 Processing")
		{
			// Handle error read;
		}
	}
	if (this->_inbuild)
	{
		this->buildResponse(this->_status);
		this->_inbuild = false;
		this->_inwrite = true;
	}
	if (this->_inwrite)
	{
		size_t						len;
		std::vector<char>::iterator	it;

		len = this->_response.size();
		if (len > 4096)
			len = 4096;
		it = this->_response.begin() + len;
		if (send(this->_client.getFD(), this->_response.data(), len, 0) <= 0)
			return (-1);
		this->_response.erase(this->_response.begin(), it);
		if (this->_response.empty())
		{
			this->clear();
			return (0);
		}
	}
	return (1);
}

//////////////////////////////
// Functions: private
std::string	Request::parse(void)
{
	size_t							pos;
	std::string						line;
	std::string						host;
	std::string						port;
	std::vector<Server>::iterator	it;
	std::string						root;
	std::string						file;
	std::string						version;
	std::string						length;

	line = this->_header.substr(0, this->_header.find("\r\n"));
	if (line.empty())
		return ("400 Bad Request");
	pos = this->_header.find("Host: ");
	if (pos == std::string::npos)
		return ("400 Bad Request");
	host = this->_header.substr(pos + 6, this->_header.find("\r\n", pos));
	if (host.empty())
		return ("400 Bad Request");
	this->_name = host.substr(0, host.find(':'));
	port = host.substr(this->_name.length() + 1, host.find("\r\n") - this->_name.length());
	if (this->_name.empty() || port.empty())
		return ("400 Bad Request");
	it = this->_client.getManager()->searchServ(this->_name, std::atoi(port.c_str()));
	root = (*it).getRoot();
	this->_method = line.substr(0, line.find(' '));
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		return ("405 Method Not Allowed");
	pos = this->_method.length() + 1;
	if (pos == std::string::npos)
		return ("400 Bad Request");
	file = line.substr(pos, (line.find(' ', pos) - (pos)));
	if (file.empty())
		return ("400 Bad Request");
	else if (file.length() > 0 && *file.begin() != '/')
		file = '/' + file;
	this->_filepath = root + file;
	pos = file.length() + 2;
	version = line.substr((this->_method.length() + pos), (line.find("\r\n") - (this->_method.length() + pos + 1)));
	if (version != "HTTP/1.1")
		return ("505 HTTP Version not supported");
	pos = this->_header.find("Content-Length: ");
	if (pos != std::string::npos)
	{
		if (this->_method == "DELETE")
			return ("400 Bad Request");
		length = this->_header.substr((pos + 16), (this->_header.find("\r\n", pos) - 16));
		if ((*it).getBodymax() < std::atoi(length.c_str()))
			return ("413 Request Entity Too Large");
	}
	else if (this->_method == "POST")
		return ("400 Bad Request");
	if (this->_header.find("Connection: keep-alive") == std::string::npos)
		this->_client.setKeepAlive(false);
	else
		this->_client.setKeepAlive(true);
	return ("102 Processing");
}

std::string	Request::openf(void)
{
	this->_file = open(this->_filepath.c_str(), O_RDONLY);
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
	if (stat(this->_filepath.c_str(), &this->_stat) == -1)
		return ("500 Internal Server Error");
	return ("102 Processing");
}

std::string	Request::del(void)
{
	std::string	dir;
	const char	*str;

	str = this->_filepath.c_str();
	dir = this->_filepath.substr(0, this->_filepath.find_last_of("/") + 1);
	if (access(str, F_OK) == -1)
		return ("404 Not Found");
	else if (access(dir.c_str(), W_OK | X_OK) == -1)
		return ("403 Forbidden");
	if (std::remove(str) != 0)
		return ("500 Internal Server Error");
	return ("200 OK");
}

std::string	Request::create(void)
{
	if (access(this->_filepath.c_str(), F_OK) != -1)
		return ("409 Conflict");
	errno = 0;
	this->_file = open(this->_filepath.c_str(), O_CREAT | O_WRONLY | O_TRUNC);
	if (errno)
	{
		if (errno == ELOOP)
			return ("310 Too many Redirects");
		else if (errno == EACCES)
			return ("403 Forbidden");
		else if (errno == ENAMETOOLONG)
			return ("414 Request-URI Too Long");
		else if (errno == EMFILE || errno == ENFILE || errno == ENOMEM || errno == ENOSPC)
			return ("503 Service Unavailable");
		else
			return ("500 Internal Server Error");
	}
	return ("102 Processing");
}

std::string	Request::get(void)
{
	char			buffer[4096];
	ssize_t			bytes;

	bytes = read(this->_file, buffer, 4096);
	if (bytes < 0)
	{
		this->_bodyresponse.erase(this->_bodyresponse.begin(), this->_bodyresponse.end());
		close(this->_file);
		return ("500 Internal Server Error");
	}
	this->_bodyresponse.insert(this->_bodyresponse.end(), &buffer[0], &buffer[bytes]);
	this->_contentlength = this->_contentlength + bytes;
	this->_client.setInRequest(true);
	if (bytes < 4096)
	{
		close(this->_file);
		return ("200 OK");
	}
	return ("102 Processing");
}

std::string	Request::post(void)
{
	size_t	i;
	size_t	len;
	ssize_t	bytes;

	len = this->_body.length();
	i = len;
	if (i > 4096)
		i = 4096;
	bytes = write(this->_file, this->_body.c_str(), i);
	if (bytes <= 0)
	{
		close(this->_file);
		return ("500 Internal Server Error");
	}
	if (i == len)
	{
		close(this->_file);
		return ("202 Created");
	}
	return ("102 Processing");
}

void	Request::buildResponse(std::string status)
{
	std::string			str;

	str = "HTTP/1.1 " + status + "\r\n";
	str = str + "Date: " + this->getTime(std::time(0)) + "\r\n";
	str = str + "Server: Webserv-42 (Linux)\r\n";
	if (this->_method == "GET" && status == "200 OK")
	{
		str = str + "Last-Modified: " + getTime(this->_stat.st_mtime) + "\r\n";
		str = str + "Content-Length: " + itoa(this->_contentlength) + "\r\n";
		str = str + "Content-Type: " + this->getMime() + "\r\n";
	}
	str = str + "Cache-Control: no-cache, no-store, must-revalidate\r\n";
	if (this->_client.keepAlive())
		str = str + "Connection: keep-alive\r\n";
	else
		str = str + "Connection: closed\r\n";
	std::cout << "[*] Response's header sent on " << this->_client.getPort();
	std::cout << " (fd " << this->_client.getFD() << ")\n" << str << std::endl;
	str = str + "\r\n";
	this->_response.insert(this->_response.begin(), str.begin(), str.end());
	if (!this->_bodyresponse.empty())
	{
		this->_response.insert(this->_response.end(), this->_bodyresponse.begin(), this->_bodyresponse.end());
		str = "\r\n";
		this->_response.insert(this->_response.end(), str.begin(), str.end());
	}
	str = "\r\n";
	this->_response.insert(this->_response.end(), str.begin(), str.end());
}

std::string	Request::getTime(std::time_t time)
{
	std::string	day;
	std::string	month;
	std::string	nbday;
	std::string	hour;
	std::string	year;

	std::string line(std::asctime(std::localtime(&time)));
	day = line.substr(0, 3);
	month = line.substr(4, 3);
	nbday = line.substr(8, 2);
	hour = line.substr(11, 8);
	year = line.substr(20, 4);
	line = day + ", " + nbday + " " + month + " " + year + " " + hour + " CET";
	return (line);
}

std::string	Request::getMime(void)
{
	std::string	type[] = {"aac", "abw", "arc", "avi", "azw", "bin", "bmp", "bz", "bz2", "csh", "css", "csv", "doc", "docx", "eot", "epub", "gif", \
		"htm", "html", "ico", "ics", "jar", "jpg", "jpeg", "js", "json", "mid", "midi", "mpeg", "mpkg", "odp", "ods", "odt", "oga", "ogv", \
		"ogx", "otf", "png", "pdf", "ppt", "pptx", "rar", "rtf", "sh", "svg", "swf", "tar", "tif", "tiff", "ts", "ttf", "vsd", "wav", "weba", \
		"webm", "webp", "woff", "woff2", "xhtml", "xls", "xlsx", "xml", "xul", "zip", "3gp", "3g2", "7z"};
	std::string	mime[] = {"audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", \
		"application/octet-stream", "image/bmp", "application/x-bzip", "application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/nsword", \
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/vnd.ms-fontobject", "application/epub+zip", \
		"image/gif", "text/html", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "image/jpeg", \
		"application/javascript", "application/json", "audio/midi", "audio/midi", "video/mpeg", "application/vnd.apple.installer+xml", \
		"application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", \
		"audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", \
		"application/vnd.openxmlformats-officedocument.presentationml.presentation", "application/x-rar-compressed", "application/rtf", "application/x-sh", \
		"image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "image/tiff", "application/typescript", "font/ttf", \
		"application/vnd.visio", "audio/x-wav", "audio/webm", "video/webm", "image/webp", "font/woff", "font/woff2", "application/xhtml+xml", \
		"application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "application/xml", "application/vnd.mozilla.xul+xml", \
		"application/zip", "video/3gpp", "video/3gpp2", "application/x-7z-compressed"};
	std::string	line;
	int			i;

	line = this->_filepath;
	line.erase(0, line.find_last_of('.') + 1);
	i = 0;
	while (i < 67 && line.compare(type[i]))
		i++;
	if (i < 67)
		return (mime[i]);
	return ("text/html");
}

void	Request::clear(void)
{
	std::vector<char>	tmp;

	this->_client.actualizeTime();
	this->_request = "";
	this->_header = "";
	this->_body = "";
	this->_status = "";
	this->_name = "";
	this->_method = "";
	this->_filepath = "";
	this->_file = -1;
	ft_memset(&this->_stat, 0, sizeof(this->_stat));
	this->_headerresponse = tmp;
	this->_bodyresponse = tmp;
	this->_response = tmp;
	this->_contentlength = 0;
	this->_maxcontentlength = 0;
	this->_inparse = true;
	this->_inprocess = false;
	this->_inbuild = false;
	this->_inwrite = false;
	this->_client.setInRequest(false);
	this->_client.setToRead(true);
}
