/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:06 by apayen            #+#    #+#             */
/*   Updated: 2024/02/08 11:10:16 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Request::Request(Client &cl) : _client(cl), _inparse(true), _inprocess(false), \
	_inerror(true), _inbuild(false), _inwrite(false), _fdfile(-1), _contentlength(0), \
	_maxcontentlength(-1), _get(true), _post(true), _del(true), _autoindex(false), _redirected(0) \
{ }

Request::~Request(void) { }

//////////////////////////////
// Functions: public
int	Request::reader(void)
{
	char		buffer[2048 + 1];
	int			bytes;
	size_t		pos;

	ft_memset(buffer, 0, 2048 + 1);
	bytes = recv(this->_client.getFD(), buffer, 2048, 0);
	if (bytes <= 0)
		return (bytes);
	this->_request = this->_request + buffer;
	pos = this->_request.find("\r\n\r\n");
	if (pos == std::string::npos || this->_contentlength + bytes < this->_maxcontentlength)
	{
		std::cout << "[*] Buffer of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
		std::cout << this->_request << "\n" << std::endl;
		return (bytes);
	}
	if (this->_header.find("\r\n\r\n") == std::string::npos)
		this->fillHeader(pos, bytes);
	else
		this->fillBody(pos, bytes);
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
			std::string tmp;

			if (this->_autoindex)
			{
				// Do AutoIndex;
				this->_stat.st_mtime = std::time(0);
			}
			else if (this->_status != "301 Moved Permanently")
				tmp = this->error();
			if (tmp != "102 Processing")
			{
				this->_inprocess = false;
				this->_inbuild = true;
			}
		}
	}
	if (this->_inbuild)
	{
		this->buildResponse();
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
// Functions: private - reader
void	Request::fillHeader(size_t pos, int bytes)
{
	std::string	nb;

	this->_header = this->_header + this->_request.substr(0, pos + 4);
	this->_request.erase(0, pos + 4);
	std::cout << "[*] Header of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n";
	std::cout << this->_header.substr(0, this->_header.length() - 4) << "\n" << std::endl;
	pos = this->_header.find("Content-Length: ");
	if (pos == std::string::npos)
	{
		this->_client.setToRead(false);
		return ;
	}
	nb = this->_header.substr((pos + 16), (this->_header.find("\r\n", pos) - pos - 16));
	this->_maxcontentlength = std::atoi(nb.c_str());
	if (!this->_request.empty())
	{
		pos = this->_request.find("\r\n\r\n");
		this->fillBody(pos, bytes);
	}
}

void	Request::fillBody(size_t pos, int bytes)
{
	std::string	crlf;

	crlf = "\r\n\r\n";
	if (this->_request.find("\r\n\r\n") != std::string::npos)
	{
		this->_body.insert(this->_body.end(), this->_request.begin(), this->_request.begin() + pos + 4);
		this->_request.erase(0, pos + 4);
	}
	else
	{
		this->_body.insert(this->_body.end(), this->_request.begin(), this->_request.end());
		this->_request.erase(0, this->_request.length());
	}
	this->_contentlength = this->_contentlength + bytes;
	if (this->_contentlength > this->_maxcontentlength)
		this->_body.resize(this->_maxcontentlength);
	if (this->_contentlength >= this->_maxcontentlength \
		|| std::search(this->_body.begin(), this->_body.end(), crlf.begin(), crlf.end()) != this->_body.end())
	{
		this->_contentlength = 0;
		this->_client.setToRead(false);
	}
}

//////////////////////////////
// Functions: private - writer
std::string	Request::parse(void)
{
	size_t							pos;
	std::string						line;
	std::string						root;
	std::string						version;
	std::string						length;

	line = this->_header.substr(0, this->_header.find("\r\n"));
	if (line.empty())
		return ("400 Bad Request");
	if (!this->searchServ())
		return ("400 Bad Request");
	root = this->_serv.getRoot();
	this->_method = line.substr(0, line.find(' '));
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		return ("405 Method Not Allowed");
	pos = this->_method.length() + 1;
	this->_filename = line.substr(pos, (line.find(' ', pos) - (pos)));
	if (this->_filename.empty())
		return ("400 Bad Request");
	else if (this->_filename.find("..") != std::string::npos)
		return ("403 Forbidden");
	this->_filepath = "Servers/" + root + '/' + this->_filename;
	pos = this->_filename.length() + 2;
	version = line.substr((this->_method.length() + pos), (line.find("\r\n") - (this->_method.length() + pos + 1)));
	if (version != "HTTP/1.1")
		return ("505 HTTP Version not supported");
	pos = this->_header.find("Content-Length: ");
	if (pos != std::string::npos)
	{
		if (this->_method == "DELETE")
			return ("400 Bad Request");
		length = this->_header.substr((pos + 16), (this->_header.find("\r\n", pos) - 16));
		if (this->_serv.getBodymax() < std::atoi(length.c_str()))
			return ("413 Request Entity Too Large");
	}
	else if (this->_method == "POST")
		return ("400 Bad Request");
	if (this->_header.find("Connection: keep-alive") == std::string::npos)
		this->_client.setKeepAlive(false);
	else
		this->_client.setKeepAlive(true);
	return (this->checkLocation());
}

bool	Request::searchServ()
{
	size_t							pos;
	std::string						host;
	std::string						port;

	pos = this->_header.find("Host: ");
	if (pos == std::string::npos)
		return (false);
	host = this->_header.substr(pos + 6, this->_header.find("\r\n", pos));
	if (host.empty())
		return (false);
	this->_name = host.substr(0, host.find(':'));
	if (this->_name.empty())
		return (false);
	port = host.substr(this->_name.length() + 1, host.find("\r\n") - this->_name.length());
	if (port.empty())
		return (false);
	this->_serv = this->_client.getManager()->getServ(this->_name, std::atoi(port.c_str()));
	return (true);
}

std::string	Request::checkLocation(void)
{
	Location 	l;
	struct stat st;

	ft_memset(&st, 0, sizeof(st));
	l = this->_serv.getLocation(this->_filename);
	if (!l.allowMethod(this->_method, this->_get, this->_post, this->_del))
		return ("405 Method Not Allowed");
	if (this->_method == "POST" && l.getDirPost().first)
	{
		this->_filename = l.getDirPost().second + '/' + this->_filename;
		this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
		return ("102 Processing");
	}
	if (l.getAlias().first)
	{
		this->_filename.replace(this->_filename.find(l.getPath()), l.getPath().length(), l.getAlias().second);
		this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
	}
	if (stat(this->_filepath.c_str(), &st) == -1 && errno != ENOENT)
		return ("500 Internal Server Error");
	errno = 0;
	if (S_ISDIR(st.st_mode))
	{
		if (l.getIndex().first)
		{
			this->_filename = l.getIndex().second;
			this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
		}
		else if (l.allowAutoindex())
			this->_autoindex = true;
		else
			return ("403 Forbidden");
	}
	if (l.getReturn().first)
	{
		this->_filename = l.getReturn().second;
		if (this->simulateRedirect(this->_filename) == -1)
			return ("310 Too many Redirects");
		return ("301 Moved Permanently");
	}
	return ("102 Processing");
}

int	Request::simulateRedirect(std::string path)
{
	Location	l;

	this->_redirected++;
	if (this->_redirected >= 9)
		return (-1);
	l = this->_serv.getLocation(path);
	if (l.getReturn().first)
		return (this->simulateRedirect(l.getReturn().second));
	return (0);
}

std::string	Request::openf(void)
{
	this->_fdfile = open(this->_filepath.c_str(), O_RDONLY);
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
	std::string	cmd;
	cmd = "mkdir -p -m 755 " + this->_filepath.substr(0, this->_filepath.find_last_of('/'));
	if (std::system(cmd.c_str()) != 0)
	{
		std::cout << this->_filepath << std::endl;
		std::cout << strerror(errno) << std::endl;
		return ("500 Internal Server Error...");
	}
	errno = 0;
	this->_fdfile = open(this->_filepath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
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
		{
			std::cout << this->_filepath << std::endl;
			std::cout << strerror(errno) << std::endl;
			return ("500 Internal Server Error");
		}
	}
	return ("102 Processing");
}

std::string	Request::get(void)
{
	char			buffer[4096];
	ssize_t			bytes;

	bytes = read(this->_fdfile, buffer, 4096);
	if (bytes < 0)
	{
		this->_bodyresponse.erase(this->_bodyresponse.begin(), this->_bodyresponse.end());
		close(this->_fdfile);
		return ("500 Internal Server Error");
	}
	this->_bodyresponse.insert(this->_bodyresponse.end(), &buffer[0], &buffer[bytes]);
	this->_contentlength = this->_contentlength + bytes;
	this->_client.setInRequest(true);
	if (bytes < 4096)
	{
		close(this->_fdfile);
		return ("200 OK");
	}
	return ("102 Processing");
}

std::string	Request::post(void)
{
	size_t	i;
	size_t	len;
	ssize_t	bytes;

	len = this->_body.size();
	i = len;
	if (i > 4096)
		i = 4096;
	bytes = write(this->_fdfile, this->_body.data(), i);
	this->_body.erase(this->_body.begin(), this->_body.begin() + i);
	if (bytes <= 0)
	{
		close(this->_fdfile);
		return ("500 Internal Server Error");
	}
	if (i == len)
	{
		close(this->_fdfile);
		return ("202 Created");
	}
	return ("102 Processing");
}

std::string	Request::error(void)
{
	if (this->_inerror)
	{
		std::map<std::string, std::string>				m;
		std::map<std::string, std::string>::iterator	it;
		std::string										root;
		std::string										error;

		this->_inerror = false;
		m = this->_serv.getErrors();
		it = m.begin();
		error = this->_status.substr(0, 3);
		while (it != m.end())
		{
			if ((*it).first == error)
			{
				root = this->_serv.getRoot();
				this->_filepath = "Servers/" + root + '/' + (*it).second;
				if (this->openf() != "102 Processing")
					it = m.end();
				break ;
			}
			it++;
		}
		if (it == m.end())
		{
			std::string	r;

			r = r + "<!DOCTYPE html>\n<html style=\"height:100%;\"lang=\"en\">\n\t<head>\n\t\t<meta charset=\"UTF-8\">\n\t\t";
			r = r + "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">\n\t\t";
			r = r + "<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n\t\t";
			r = r + "<title>" + this->_status.substr(0, 3) + "</title>\n\t</head>\n";
			r = r + "\t<body style=\"height:100\%;display:flex;align-items:center;justify-content:center;\">\n\t\t";
			r = r + "<main>\n\t\t\t\t<h1 style=\"font-size:5em;\">" + this->_status.substr(0, 3) + "</h1>\n\t\t</main>\n\t</body>\n</html>";
			this->_bodyresponse.insert(this->_bodyresponse.end(), r.begin(), r.end());
			this->_stat.st_mtime = std::time(0);
			this->_contentlength = this->_bodyresponse.size();
			return (this->_status);
		}
	}
	return (this->get());
}

void	Request::buildResponse(void)
{
	std::string			str;

	str = "HTTP/1.1 " + this->_status + "\r\n";
	str = str + "Date: " + this->getTime(std::time(0)) + "\r\n";
	str = str + "Server: Webserv-42 (Linux)\r\n";
	if (this->_status == "301 Moved Permanently" || this->_status == "202 Created")
		str = str + "Location: " + this->_filename + "\r\n";
	else if (this->_status == "405 Method Not Allowed" && !this->getMethods().empty())
		str = str + "Allow: " + this->getMethods() + "\r\n";
	if ((this->_method == "GET" && this->_status == "200 OK") \
		|| (this->_status != "200 OK" && this->_status != "202 Created" && this->_status != "301 Moved Permanently"))
	{
		str = str + "Last-Modified: " + this->getTime(this->_stat.st_mtime) + "\r\n";
		str = str + "Content-Length: " + itoa(this->_contentlength) + "\r\n";
		if (this->_status == "200 OK")
			str = str + "Content-Type: " + this->getMime() + "\r\n";
		else
			str = str + "Content-Type: text/html\r\n";
	}
	str = str + "Cache-Control: no-cache, no-store, must-revalidate\r\n";
	if (this->_client.keepAlive())
		str = str + "Connection: keep-alive\r\n";
	else
		str = str + "Connection: closed\r\n";
	std::cout << "[*] Response's header sent on port " << this->_client.getPort();
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

std::string	Request::getMethods(void)
{
	std::string line;
	size_t		pos;
	size_t		end;

	if (this->_get)
		line = line + "GET ";
	if (this->_post)
		line = line + "POST ";
	if (this->_del)
		line = line + "DELETE";
	if (*(line.end() - 1) == ' ')
		line.resize(line.length() - 1);
	end = 0;
	while (1)
	{
		pos = line.find(' ', end);
		if (pos == std::string::npos)
			break ;
		line.insert(pos, ",");
		end = pos + 2;
	}
	return (line);
}

std::string	Request::getTime(std::time_t time)
{
	std::string	day;
	int			space;
	std::string	month;
	std::string	nbday;
	std::string	hour;
	std::string	year;

	std::string line(std::asctime(std::localtime(&time)));
	space = 0;
	if (line[8] == ' ')
		space++;
	day = line.substr(0, 3);
	month = line.substr(4, 3);
	nbday = line.substr(8 + space, 2 - space);
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
	while (i < 67 && line.compare(type[i]) != 0)
		i++;
	if (i < 67)
		return (mime[i]);
	return ("text/html");
}

void	Request::clear(void)
{
	std::vector<char>	tmp;

	ft_memset(&this->_stat, 0, sizeof(this->_stat));
	this->_inparse = true;
	this->_inprocess = false;
	this->_inerror = false;
	this->_inbuild = false;
	this->_inwrite = false;
	this->_request = "";
	this->_header = "";
	this->_body = tmp;
	this->_headerresponse = tmp;
	this->_bodyresponse = tmp;
	this->_response = tmp;
	this->_status = "";
	this->_name = "";
	this->_method = "";
	this->_filename = "";
	this->_filepath = "";
	this->_fdfile = -1;
	this->_contentlength = 0;
	this->_maxcontentlength = 0;
	this->_get = true;
	this->_post = true;
	this->_del = true;
	this->_autoindex = false;
	this->_redirect = "";
	this->_redirected = 0;
	this->_client.setInRequest(false);
	this->_client.setToRead(true);
	this->_client.actualizeTime();
}
