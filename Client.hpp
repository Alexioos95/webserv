/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2024/01/11 14:59:37 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno>			// errno
# include <iostream>		// std::cout
# include <fcntl.h>			// fnctl
							// stat
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>	// recv

# include "Server.hpp"

class Client
{
	public:
	// Constructors and Destructor
		Client(Server &serv);
		Client(Client const &rhs);
		~Client(void);
	// Overload
		Client				&operator=(Client const &rhs);
	// Setters
		void				setFD(int fd);
		void				setInRequest(bool state);
		void				setKeepAlive(bool state);
		void				setToRead(bool state);
	// Getters
		Server				&getServer(void);
		int					getFD(void) const;
		std::string			getHeader(void) const;
		std::fstream		&getFile(void);
		std::string			getFilePath(void) const;
		int					getContentLength(void) const;
		std::vector<char>	&getFileContent(void);
		bool				toRead(void) const;
		bool				fileIsOpen(void) const;
		bool				inRequest(void) const;
		bool				keepAlive(void) const;
	// Function
		int					readRequest(void);
		std::string			openFile(std::string path);
		std::string			readFile(void);
		void				clear(void);

	private:
	// Constructor
		Client(void);
	// Attributes
		Server				&_server;
		int					_fd;
		bool				_toread;
		std::string			_request;
		std::string			_header;
		std::string			_body;
		std::fstream		_file;
		std::string			_filepath;
		std::vector<char>	_filecontent;
		int					_contentlength;
		int					_maxcontentlength;
		bool				_inrequest;
		bool				_keepalive;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
