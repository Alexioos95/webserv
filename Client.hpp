/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2024/01/12 15:39:43 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno>			// errno
# include <fstream>			// fstream
# include <cstdlib>			// atoi
# include <fcntl.h>			// fnctl
# include <iostream>		// std::cout
							// stat
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>	// recv

# include "VirtualServer.hpp"

class Client
{
	public:
	// Constructors and Destructor
		Client(int fd, int port);
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
		// info
		int					getPort(void) const;
		int					getFD(void) const;
		// request
		std::string			getHeader(void) const;
		std::fstream		&getFile(void);
		std::string			getFilePath(void) const;
		int					getContentLength(void) const;
		std::vector<char>	&getFileContent(void);
		// state
		bool				toRead(void) const;
		bool				inRequest(void) const;
		bool				fileIsOpen(void) const;
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
		// Info
		int					_port;
		int					_fd;
		// Request
		std::string			_request;
		std::string			_header;
		std::string			_body;
		std::fstream		_file;
		std::string			_filepath;
		std::vector<char>	_filecontent;
		int					_contentlength;
		int					_maxcontentlength;
		// State
		bool				_toread;
		bool				_inrequest;
		bool				_keepalive;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
