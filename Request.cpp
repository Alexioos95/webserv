/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:06 by apayen            #+#    #+#             */
/*   Updated: 2024/03/19 10:20:30 by apayen           ###   ########.fr       */
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

Request::~Request(void)
{
	if (this->_fdfile != -1)
		close (this->_fdfile);
	if (this->_cgi != NULL)
		delete (this->_cgi);
}

//////////////////////////////
// Functions: public
int	Request::reader(void)
{
	char						buffer[2048 + 1];
	int							bytes;
	std::string					crlf;
	std::vector<char>::iterator	pos;

	ft_memset(buffer, 0, 2048 + 1);
	bytes = recv(this->_client.getFD(), buffer, 2048, 0);
	if (bytes <= 0)
		return (bytes);
	crlf = "\r\n\r\n";
	this->_request.insert(this->_request.end(), &buffer[0], &buffer[bytes]);
	pos = std::search(this->_request.begin(), this->_request.end(), crlf.begin(), crlf.end());
	if (!this->_multi && (pos == this->_request.end() || this->_contentlength + bytes < this->_maxcontentlength))
		return (bytes);
	if (std::search(this->_header.begin(), this->_header.end(), crlf.begin(), crlf.end()) == this->_header.end())
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
			if (this->_iscgi)
			{
				try
				{
					this->_cgi = new Cgi;
					std::string					env;
					std::vector<Cgi>::iterator	it;

					// Do env.
					this->_cgi->launchCgi(this->_filepath);
				}
				catch (const std::exception &e)
				{
					std::cerr << "[-] Couldn't start CGI for client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n" << std::endl;
					this->_status = "500 Internal Server Error";
				}
			}
			else if (this->_method == "GET")
			{
				if (this->_autoindex)
				{
					std::string tmp;

					if (autoindex(this->_dir.c_str(), tmp))
					{
						this->_bodyresponse.insert(this->_bodyresponse.end(), tmp.begin(), tmp.end());
						this->_contentlength = this->_bodyresponse.size();
						this->_stat.st_mtime = std::time(0);
						this->_status = "200 OK";
					}
					else
						this->_status = "500 Internal Server Error";
				}
				else
					this->_status = this->openf();
			}
			else if (this->_method == "DELETE")
				this->_status = this->del();
			else if (this->_method == "POST" && !this->_multi)
				this->_status = this->create();
		}
		this->_inparse = false;
		this->_inprocess = true;
	}
	if (this->_inprocess)
	{
		if (this->_status == "102 Processing")
		{
			if (this->_iscgi)
			{

				char			buffer[4096];
				ssize_t			bytes;

				waitpid(this->_cgi->getPid(), NULL, WNOHANG);
				bytes = read(this->_cgi->getFdRead(), buffer, 4096);
				if (bytes < 0)
				{
					this->_response.erase(this->_response.begin(), this->_response.end());
					this->_status = "500 Internal Server Error";
				}
				else
				{
					this->_response.insert(this->_response.end(), &buffer[0], &buffer[bytes]);
					printvector(this->_response, 0);
					if (bytes < 4096)
					{
						this->_inprocess = false;
						this->_inwrite = true;
					}
					return (1);
				}
			}
			else if (this->_multi)
			{
				this->_status = this->multipost();
				if (this->_status == "102 Processing")
					return (1);
			}
			else if (this->_method == "GET")
				this->_status = this->get();
			else if (this->_method == "POST" && !this->_multi)
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

			if (this->_status != "301 Moved Permanently")
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
		return (1);
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
void	Request::fillHeader(std::vector<char>::iterator pos, int bytes)
{
	std::vector<char>::iterator	it;
	std::string					contentlen;
	std::string					crlf;
	std::string					crlf2;
	std::string					multi;
	std::string					bound;
	std::string					nb;

	contentlen = "Content-Length: ";
	crlf = "\r\n";
	crlf2 = "\r\n\r\n";
	multi = "Content-Type: multipart/form-data";
	bound = "boundary=";
	this->_header.insert(this->_header.end(), this->_request.begin(), pos + 4);
	this->_request.erase(this->_request.begin(), pos + 4);
	std::cout << "[*] Header of client (fd " << this->_client.getFD() << ") on port " << this->_client.getPort() << "\n"; printvector(this->_header, 2);
	pos = std::search(this->_header.begin(), this->_header.end(), contentlen.begin(), contentlen.end());
	if (pos == this->_header.end())
	{
		this->_client.setToRead(false);
		return ;
	}
	it = std::search(pos + 16, this->_header.end(), crlf.begin(), crlf.end());
	nb = std::string(pos + 16, it);
	this->_maxcontentlength = std::atoi(nb.c_str());
	pos = std::search(this->_header.begin(), this->_header.end(), multi.begin(), multi.end());
	if (pos != this->_header.end())
	{
		std::vector<char>::iterator	it;

		this->_multi = true;
		pos = std::search(pos, this->_header.end(), bound.begin(), bound.end());
		it = std::search(pos + 10, this->_header.end(), crlf.begin(), crlf.end());
		std::string	tmp(pos + 10, it);
		this->_boundary = tmp;
	}
	if (!this->_request.empty())
	{
		pos = std::search(this->_request.begin(), this->_request.end(), crlf2.begin(), crlf2.end());
		this->fillBody(pos, bytes);
	}
}

void	Request::fillBody(std::vector<char>::iterator pos, int bytes)
{
	std::string	crlf;

	crlf = "\r\n\r\n";
	if (!this->_multi)
	{
		if (std::search(this->_request.begin(), this->_request.end(), crlf.begin(), crlf.end()) != this->_request.end())
		{
			this->_body.insert(this->_body.end(), this->_request.begin(), pos + 4);
			this->_request.erase(this->_request.begin(), pos + 4);
		}
		else
		{
			this->_body.insert(this->_body.end(), this->_request.begin(), this->_request.end());
			this->_request.erase(this->_request.begin(), this->_request.end());
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
	else if (this->_request.size() < static_cast<unsigned long>(this->_maxcontentlength) \
		&& (std::find_end(this->_request.begin(), this->_request.end(), this->_boundary.begin(), this->_boundary.end()) + this->_boundary.length() + 5) != this->_request.end())
		return ;
	else
	{
		std::string							crlf;
		std::string							crlf2;
		std::string							filename;
		std::string							bound;
		std::vector<char>::iterator			it;
		std::vector<char>::iterator			ite;
		std::string							name;
		std::vector<char>					content;

		crlf = "\r\n";
		crlf2 = "\r\n\r\n";
		filename = "filename=\"";
		bound = "--" + this->_boundary;
		while (1)
		{
			it = std::search(this->_request.begin(), this->_request.end(), bound.begin(), bound.end());
			if (it == this->_request.end())
				break ;
			this->_request.erase(this->_request.begin(), (it + bound.length() + 2));
			it = std::search(this->_request.begin(), this->_request.end(), filename.begin(), filename.end());
			if (it == this->_request.end())
				break ;
			ite = std::search(it, this->_request.end(), crlf.begin(), crlf.end());
			if (ite == this->_request.end())
				break ;
			name = std::string(it + 10, ite - 1);
			it = std::search(this->_request.begin(), this->_request.end(), crlf2.begin(), crlf2.end());
			if (it == this->_request.end())
				break ;
			this->_request.erase(this->_request.begin(), it + 4);
			it = std::search(this->_request.begin(), this->_request.end(), bound.begin(), bound.end());
			if (it == this->_request.end())
				break ;
			content.insert(content.end(), this->_request.begin(), it - 3);
			this->_files.push_back(std::pair<std::string, std::vector<char> >(name, content));
			content.erase(content.begin(), content.end());
			this->_request.erase(this->_request.begin(), it);
			if (this->_request.size() == 2)
				break ;
		}
		this->_client.setToRead(false);
	}
}

//////////////////////////////
// Functions: private - writer
std::string	Request::parse(void)
{
	size_t							pos;
	std::vector<char>::iterator		it;
	std::vector<char>::iterator		ite;
	std::string						crlf;
	std::string						contentlen;
	std::string						connection;
	std::string						line;
	std::string						root;
	std::string						version;
	std::string						length;

	crlf = "\r\n";
	contentlen = "Content-Length: ";
	connection = "Connection: keep-alive";
	it = std::search(this->_header.begin(), this->_header.end(), crlf.begin(), crlf.end());
	line = std::string(this->_header.begin(), it);
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
	it = std::search(this->_header.begin(), this->_header.end(), contentlen.begin(), contentlen.end());
	if (it != this->_header.end())
	{
		if (this->_method == "DELETE")
			return ("400 Bad Request");
		it = std::search(this->_header.begin(), this->_header.end(), contentlen.begin(), contentlen.end());
		ite = std::search(it, this->_header.end(), crlf.begin(), crlf.end());
		length = std::string(it + 16, ite);
		if (this->_serv.getBodymax() < std::atoi(length.c_str()))
			return ("413 Request Entity Too Large");
	}
	else if (this->_method == "POST")
		return ("400 Bad Request");;
	if (std::search(this->_header.begin(), this->_header.end(), connection.begin(), connection.end()) == this->_header.end())
		this->_client.setKeepAlive(false);
	else
		this->_client.setKeepAlive(true);
	return (this->checkLocation());
}

bool	Request::searchServ()
{
	std::vector<char>::iterator	it;
	std::vector<char>::iterator	ite;
	std::string					crlf;
	std::string					host;
	std::string					port;

	crlf = "\r\n";
	host = "Host: ";
	it = std::search(this->_header.begin(), this->_header.end(), host.begin(), host.end());
	if (it == this->_header.end())
		return (false);
	ite = std::search(it, this->_header.end(), crlf.begin(), crlf.end());
	host = std::string(it + 6, ite);
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
	std::string	status;

	status = "102 Processing";
	ft_memset(&st, 0, sizeof(st));
	l = this->_serv.getLocation(this->_filename);
	if (!l.allowMethod(this->_method, this->_get, this->_post, this->_del))
		return ("405 Method Not Allowed");
	if (this->_filename.find_last_of(".php") == this->_filename.length() - 1)
	{
		if (!l.allowCgi() || this->_method == "DELETE")
			return ("403 Forbiddden");
		else
			this->_iscgi = true;
	}
	if (this->_method == "POST")
	{
		if (l.getDirPost().first)
		{
			if (this->_multi)
				this->_filename = l.getDirPost().second + '/';
			else
				this->_filename = l.getDirPost().second + '/' + this->_filename;
		}
		else
			this->_filename.resize(this->_filename.find_last_of('/'));
		this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
	}
	if (l.getAlias().first)
	{
		this->_filename.replace(this->_filename.find(l.getPath()), l.getPath().length(), l.getAlias().second);
		this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
	}
	if (this->_method == "GET")
	{
		if (stat(this->_filepath.c_str(), &st) == -1 && errno != ENOENT)
			return ("500 Internal Server Error");
		if (S_ISDIR(st.st_mode))
		{
			this->_dir = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
			if (l.getIndex().first)
			{
				this->_filename = l.getIndex().second;
				this->_filepath = "Servers/" + this->_serv.getRoot() + '/' + this->_filename;
			}
			else if (l.allowAutoindex())
				this->_autoindex = true;
			else
				status = "403 Forbidden";
		}
	}
	if (l.getReturn().first)
	{
		this->_filename = l.getReturn().second;
		if (this->simulateRedirect(this->_filename) == -1)
			status = "310 Too many Redirects";
		status = "301 Moved Permanently";
	}
	return (status);
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
		std::string	status;

		if (errno == ELOOP)
			status = "310 Too many Redirects";
		else if (errno == EACCES)
			status = "403 Forbidden";
		else if (errno == ENOENT || errno == EFAULT || errno == ENODEV)
			status = "404 Not Found";
		else if (errno == EFBIG)
			status = "413 Request Entity Too Large";
		else if (errno == ENAMETOOLONG)
			status = "414 Request-URI Too Long";
		else if (errno == EMFILE || errno == ENFILE || errno == ENOMEM || errno == ENOSPC)
			status = "503 Service Unavailable";
		else
			status = "500 Internal Server Error";
		errno = 0;
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
	if (errno)
	{
		if (errno == ELOOP)
			status = "310 Too many Redirects";
		else if (errno == EACCES)
			status = "403 Forbidden";
		else if (errno == ENAMETOOLONG)
			status = "414 Request-URI Too Long";
		else if (errno == EMFILE || errno == ENFILE || errno == ENOMEM || errno == ENOSPC)
			status = "503 Service Unavailable";
		else
			status = "500 Internal Server Error";
		errno = 0;
		return (status);
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
		close(this->_fdfile);
		this->_fdfile = -1;
		this->_bodyresponse.erase(this->_bodyresponse.begin(), this->_bodyresponse.end());
		return ("500 Internal Server Error");
	}
	this->_bodyresponse.insert(this->_bodyresponse.end(), &buffer[0], &buffer[bytes]);
	this->_contentlength = this->_contentlength + bytes;
	this->_client.setInRequest(true);
	if (bytes < 4096)
	{
		close(this->_fdfile);
		this->_fdfile = -1;
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
	{
		std::vector<std::pair<std::string, std::vector<char> > >::iterator	it;
		std::string															cmd;
		std::string															status;
		std::string															path;

		cmd = "mkdir -p -m 755 " + this->_filepath;
		if (access(this->_filepath.c_str(), F_OK) == -1 && std::system(cmd.c_str()) != 0 && errno != EEXIST)
			status = "500 Internal Server Error";
		errno = 0;
		while (1)
		{
			if (this->_files.empty())
				break ;
			it = this->_files.begin();
			path = this->_filepath + (*it).first;
			this->_fdfile = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
			if (errno)
			{
				this->_files.erase(this->_files.begin());
				errno = 0;
				continue ;
			}
			break ;
		}
	}
	it = this->_files.begin();
	len = (*it).second.size();
	i = len;
	if (i > 4096)
		i = 4096;
	bytes = write(this->_fdfile, (*it).second.data(), i);
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
	if (this->_method == "POST" && this->_status == "200 OK")
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
	this->_fdfile = -1;
	this->_contentlength = 0;
	this->_maxcontentlength = 0;
	this->_multi = false;
	this->_boundary = "";
	this->_files.erase(this->_files.begin(), this->_files.end());
	this->_iscgi = false;
	this->_dir = "";
	this->_autoindex = false;
	this->_redirect = "";
	this->_redirected = 0;
	this->_client.setInRequest(false);
	this->_client.setToRead(true);
	this->_client.actualizeTime();
}
