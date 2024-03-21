/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:06 by apayen            #+#    #+#             */
/*   Updated: 2024/03/21 08:55:01 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Client.hpp"

//////////////////////////////
// Constructors and Destructor
Request::Request(Client &cl) : _client(cl), _inparse(true), _inprocess(false), \
	_inerror(true), _inbuild(false), _inwrite(false), _fdfile(-1), _contentlength(0), \
	_maxcontentlength(-1), _multi(false), _iscgi(false), _cgi(NULL), _get(true), _post(true), \
	_del(true), _autoindex(false), _redirected(0) { }

Request::Request(Client &cl, Request *rhs) : _client(cl), _serv(rhs->_serv), _inparse(rhs->_inparse), \
	_inprocess(rhs->_inprocess), _inerror(rhs->_inerror), _inbuild(rhs->_inbuild), _inwrite(rhs->_inwrite), \
	_status(rhs->_status), _name(rhs->_name), \
	_method(rhs->_method), _filename(rhs->_filename), _filepath(rhs->_filepath), _fdfile(rhs->_fdfile), \
	_stat(rhs->_stat), _contentlength(rhs->_contentlength), _maxcontentlength(rhs->_maxcontentlength), \
	_multi(rhs->_multi), _boundary(rhs->_boundary), _iscgi(rhs->_iscgi), _cgi(rhs->_cgi), \
	_cookie(rhs->_cookie), _get(rhs->_get), _post(rhs->_post), _del(rhs->_del), _dir(rhs->_dir), \
	_autoindex(rhs->_autoindex), _redirect(rhs->_redirect), _redirected(rhs->_redirected)
{
	this->_request = rhs->_request;
	this->_header = rhs->_header;
	this->_body = rhs->_body;
	this->_headerresponse = rhs->_headerresponse;
	this->_bodyresponse = rhs->_bodyresponse;
	this->_response = rhs->_response;
	this->_files = rhs->_files;
}

Request::~Request(void)
{
	if (this->_fdfile != -1)
		close (this->_fdfile);
	if (this->_cgi != NULL)
		delete (this->_cgi);
}

//////////////////////////////
// Functions: writer - process
std::string	Request::openf(void)
{
	this->_fdfile = open(this->_filepath.c_str(), O_RDONLY);
	if (this->_fdfile == -1)
	{
		std::string	status;

		this->_errno = errno;
		errno = 0;
		if (this->_errno == ELOOP)
			status = "310 Too many Redirects";
		else if (this->_errno == EACCES)
			status = "403 Forbidden";
		else if (this->_errno == ENOENT || this->_errno == EFAULT || this->_errno == ENODEV)
			status = "404 Not Found";
		else if (this->_errno == EFBIG)
			status = "413 Request Entity Too Large";
		else if (this->_errno == ENAMETOOLONG)
			status = "414 Request-URI Too Long";
		else if (this->_errno == EMFILE || this->_errno == ENFILE || this->_errno == ENOMEM || this->_errno == ENOSPC)
			status = "503 Service Unavailable";
		else
			status = "500 Internal Server Error";
		return (status);
	}
	if (stat(this->_filepath.c_str(), &this->_stat) == -1)
	{
		errno = 0;
		close(this->_fdfile);
		this->_fdfile = -1;
		return ("500 Internal Server Error");
	}
	return ("102 Processing");
}

std::string	Request::del(void)
{
	const char	*str;
	std::string	dir;
	std::string	cmd;

	str = this->_filepath.c_str();
	dir = this->_filepath.substr(0, this->_filepath.find_last_of("/") + 1);
	cmd = "rm -rf " + this->_filepath;
	if (access(str, F_OK) == -1)
		return ("404 Not Found");
	else if (access(dir.c_str(), W_OK | X_OK) == -1)
		return ("403 Forbidden");
	if (std::system(cmd.c_str()) != 0)
		return ("500 Internal Server Error");
	return ("200 OK");
}

std::string	Request::create(void)
{
	std::string	dir;
	std::string	cmd;
	std::string	status;

	dir = this->_filepath.substr(0, this->_filepath.find_last_of('/'));
	cmd = "mkdir -p -m 755 " + dir;
	if (access(this->_filepath.c_str(), F_OK) != -1)
	{
		errno = 0;
		return ("409 Conflict");
	}
	if (access(dir.c_str(), F_OK) == -1 && std::system(cmd.c_str()) != 0 && errno != EEXIST)
	{
		errno = 0;
		return ("500 Internal Server Error");
	}
	this->_fdfile = open(this->_filepath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (this->_fdfile == -1)
	{
		this->_errno = errno;
		errno = 0;
		if (this->_errno == ELOOP)
			status = "310 Too many Redirects";
		else if (this->_errno == EACCES)
			status = "403 Forbidden";
		else if (this->_errno == ENAMETOOLONG)
			status = "414 Request-URI Too Long";
		else if (this->_errno == EMFILE || this->_errno == ENFILE || this->_errno == ENOMEM || this->_errno == ENOSPC)
			status = "503 Service Unavailable";
		else
			status = "500 Internal Server Error";
		return (status);
	}
	return ("102 Processing");
}

std::string	Request::get(void)
{
	char			buffer[4096];
	ssize_t			bytes;

	if (this->_fdfile != -1)
	{
		bytes = read(this->_fdfile, buffer, 4096);
		if (bytes < 0)
		{
			errno = 0;
			close(this->_fdfile);
			this->_fdfile = -1;
			this->_bodyresponse.erase(this->_bodyresponse.begin(), this->_bodyresponse.end());
			return ("500 Internal Server Error");
		}
		this->_bodyresponse.insert(this->_bodyresponse.end(), &buffer[0], &buffer[bytes]);
		this->_contentlength = this->_contentlength + bytes;
		if (bytes < 4096)
		{
			close(this->_fdfile);
			this->_fdfile = -1;
			return ("200 OK");
		}
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
	errno = 0;
	this->_body.erase(this->_body.begin(), this->_body.begin() + i);
	if (bytes <= 0)
	{
		close(this->_fdfile);
		this->_fdfile = -1;
		return ("500 Internal Server Error");
	}
	if (i == len)
	{
		close(this->_fdfile);
		this->_fdfile = -1;
		return ("202 Created");
	}
	return ("102 Processing");
}

std::string	Request::multipost(void)
{
	std::vector<std::pair<std::string, std::vector<char> > >::iterator	it;
	size_t																i;
	size_t																len;
	ssize_t																bytes;
	if (this->_fdfile == -1)
		this->createFilesMultipost();
	if (this->_status == "500 Internal Server Error")
		return (this->_status);
	it = this->_files.begin();
	len = (*it).second.size();
	i = len;
	if (i > 4096)
		i = 4096;
	bytes = write(this->_fdfile, (*it).second.data(), i);
	errno = 0;
	(*it).second.erase((*it).second.begin(), (*it).second.begin() + i);
	if (bytes <= 0)
	{
		close(this->_fdfile);
		this->_fdfile = -1;
		this->_files.erase(this->_files.begin());
		return ("500 Internal Server Error");
	}
	if (i == len)
	{
		close(this->_fdfile);
		this->_fdfile = -1;
		this->_files.erase(this->_files.begin());
		if (this->_files.empty())
			return ("200 OK");
		return ("102 Processing");
	}
	return ("102 Processing");
}

void	Request::createFilesMultipost(void)
{
	std::vector<std::pair<std::string, std::vector<char> > >::iterator	it;
	std::string															cmd;
	std::string															status;
	std::string															path;

	cmd = "mkdir -p -m 755 " + this->_filepath;
	if (access(this->_filepath.c_str(), F_OK) == -1 && std::system(cmd.c_str()) != 0 && errno != EEXIST)
	{
		status = "500 Internal Server Error";
		errno = 0;
		return ;
	}
	errno = 0;
	while (1)
	{
		if (this->_files.empty())
			break ;
		it = this->_files.begin();
		path = this->_filepath + (*it).first;
		this->_fdfile = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if (this->_fdfile == -1)
		{
			errno = 0;
			this->_files.erase(this->_files.begin());
			continue ;
		}
		break ;
	}
}

std::string	Request::error(void)
{
	if (this->_inerror)
	{
		std::map<std::string, std::string>				m;
		std::map<std::string, std::string>::iterator	it;
		std::string										tmp;
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
				tmp = this->openf();
				if (tmp != "102 Processing")
				{
					this->_status = tmp;
					it = m.end();
				}
				break ;
			}
			it++;
		}
		if (it == m.end())
		{
			this->generateError();
			return (this->_status);
		}
	}
	return (this->get());
}

void	Request::generateError(void)
{
	std::string	r;

	r = r + "<!DOCTYPE html>\n<html lang=\"en\" style=\"width: 100\%;height: 100%;margin: 0; font-family: 'Press Start 2P', cursive; background: url(\"matrix.gif\")\">\n";
	r = r + "\t<head>\n\t\t<meta charset=\"UTF-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	r = r + "\t\t<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n\t\t<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";
	r = r + "\t\t<link href=\"https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap\" rel=\"stylesheet\">\n";
	r = r + "\t\t<title>" + this->_status + "</title>\n\t\t<style>\n\t\t\t@keyframes blink { 0% { opacity: 0; } 49% { opacity: 0; } 50% { opacity: 1; } 100% { opacity: 1; } }\n";
	r = r + "\t\t\t@keyframes type { from { width: 0px } to { width: 302px } }\n\t\t</style>\n\t</head>\n";
	r = r + "\t<body style=\"width: 100\%;height: 100%;margin: 0;\">\n\t\t<main style=\"background: black; display: flex; height: 100%; justify-content: center; align-items: center;";
	r = r + " color: #54FE55; text-shadow: 0px 0px 10px; font-size: 6rem; box-sizing: border-box\">\n\t\t\t<div style=\"width: 484px; display: flex; gap: 10px;\">\n";
	r = r + "\t\t\t\t<h1 style=\"font-size: 1em; margin: 0; height: 84px; letter-spacing: 10px; max-width: 302px; white-space: nowrap; overflow: hidden; width: 0;";
	r = r + " animation: type 1s steps(3, end) forwards;\">" + this->_status.substr(0, 3) + "</h1>\n\t\t\t\t<div style=\"border-bottom: 0.15em solid #54FE55; height: 86px;";
	r = r + " width: 93px; animation-name: blink; animation-duration: 1s; animation-iteration-count: infinite; animation-delay: 1.5s\"></div>\n";
	r = r + "\t\t\t</div>\n\t\t</main>\n\t</body>\n</html>";
	this->_bodyresponse.insert(this->_bodyresponse.end(), r.begin(), r.end());
	this->_stat.st_mtime = std::time(0);
	this->_contentlength = this->_bodyresponse.size();
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
	if (this->_method == "POST" && this->_status == "200 OK")
		this->generateSuccess();
	if ((this->_method != "DELETE" && this->_status == "200 OK") \
		|| (this->_status != "200 OK" && this->_status != "202 Created" && this->_status != "301 Moved Permanently"))
	{
		str = str + "Last-Modified: " + this->getTime(this->_stat.st_mtime) + "\r\n";
		str = str + "Content-Length: " + itoa(this->_contentlength) + "\r\n";
		if (this->_method != "POST" && this->_status == "200 OK" && this->_dir.empty())
			str = str + "Content-Type: " + this->getMime() + "\r\n";
		else
			str = str + "Content-Type: text/html\r\n";
	}
	str = str + "Cache-Control: no-cache, must-revalidate\r\n";
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

void	Request::generateSuccess(void)
{
	std::string	r;

	r = r + "<!DOCTYPE html>\n<html lang=\"en\" style=\"width: 100\%;height: 100%;margin: 0; font-family: 'Press Start 2P', cursive; background: url(\"matrix.gif\")\">\n";
	r = r + "\t<head>\n\t\t<meta charset=\"UTF-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
	r = r + "\t\t<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n\t\t<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";
	r = r + "\t\t<link href=\"https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap\" rel=\"stylesheet\">\n";
	r = r + "\t\t<title>POST Succesfull</title>\n\t\t<style>\n\t\t\t@keyframes blink { 0% { opacity: 0; } 49% { opacity: 0; } 50% { opacity: 1; } 100% { opacity: 1; } }\n";
	r = r + "\t\t\t@keyframes type { from { width: 0px } to { width: 302px } }\n\t\t</style>\n\t</head>\n";
	r = r + "\t<body style=\"width: 100\%;height: 100%;margin: 0;\">\n\t\t<main style=\"background: black;display: flex; height: 100%; justify-content: center; align-items: center;";
	r = r + " color: #54FE55; text-shadow: 0px 0px 10px; font-size: 6rem; box-sizing: border-box\">\n\t\t\t<div style=\"width: 484px; display: flex; gap: 10px;\">\n";
	r = r + "\t\t\t\t<h1 style=\"font-size: 1em; margin: 0; height: 84px; letter-spacing: 10px; max-width: 302px; white-space: nowrap; overflow: hidden; width: 0;";
	r = r + " animation: type 1s steps(3, end) forwards;\">200</h1>\n\t\t\t\t<div style=\"border-bottom: 0.15em solid #54FE55; height: 86px;";
	r = r + " width: 93px; animation-name: blink; animation-duration: 1s; animation-iteration-count: infinite; animation-delay: 1.5s\"></div>\n";
	r = r + "\t\t\t</div>\n\t\t</main>\n\t</body>\n</html>";
	this->_bodyresponse.insert(this->_bodyresponse.end(), r.begin(), r.end());
	this->_stat.st_mtime = std::time(0);
	this->_contentlength = this->_bodyresponse.size();
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
	return ("text/plain");
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
	this->_request = tmp;
	this->_header = tmp;
	this->_body = tmp;
	this->_headerresponse = tmp;
	this->_bodyresponse = tmp;
	this->_response = tmp;
	this->_status = "";
	this->_name = "";
	this->_method = "";
	this->_filename = "";
	this->_filepath = "";
	if (this->_fdfile != -1)
		close(this->_fdfile);
	this->_fdfile = -1;
	this->_contentlength = 0;
	this->_maxcontentlength = -1;
	this->_multi = false;
	this->_boundary = "";
	this->_files.erase(this->_files.begin(), this->_files.end());
	this->_iscgi = false;
	this->_cookie = "";
	this->_dir = "";
	this->_autoindex = false;
	this->_redirect = "";
	this->_redirected = 0;
	this->_client.setToRead(true);
	this->_client.actualizeTime();
}
