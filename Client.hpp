/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2024/01/09 14:25:17 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno>			// errno
# include <iostream>		// std::cout
# include <fcntl.h>			// fnctl
							// recv
# include <sys/socket.h>

# include "ServerBlock.hpp"

class Client
{
	public:
	// Constructors and Destructor
		Client(ServerBlock &server);
		Client(Client const &rhs);
		~Client(void);
	// Overload
		Client			&operator=(Client const &rhs);
	// Setters
		void			setFD(int fd);
		void			setTotalbytes(unsigned int tb);
		void			setInRequest(bool state);
	// Getters
		ServerBlock		&getServer(void);
		int				getFD(void) const;
		unsigned int	getTotalbytes(void) const;
		std::string		getHeader(void) const;
		char			*getBuffer(void) const;
		std::fstream	&getFile(void);
		bool			toRead(void) const;
		bool			fileIsOpen(void) const;
		bool			inRequest(void) const;
	// Function
		int				readRequest(void);
		int				openFile(std::string path);
		int				readFile(void);

	private:
	// Constructor
		Client(void);
	// Attributes
		ServerBlock		&_server;
		int				_fd;
		bool			_toread;
		unsigned int	_totalbytes;
		char			_buffer[1024 + 1];
		std::string		_request;
		std::string		_header;
		std::string		_body;
		std::fstream	_file;
		char			_buffile[1024 + 1];
		std::string		_text;
		bool			_inrequest;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
