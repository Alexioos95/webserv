/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2024/01/08 11:57:28 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno> // Errno
# include <iostream> // std::cout
# include <fcntl.h> // fnctl
# include <sys/types.h> // recv
# include <sys/socket.h> // recv

class Client
{
	public:
	// Constructors and Destructor
		Client(void);
		Client(Client const &rhs);
		~Client(void);
	// Overload
		Client			&operator=(Client const &rhs);
	// Setters and Getters
		void			setFD(int fd);
		int				getFD(void) const;
		void			setTotalbytes(unsigned int tb);
		unsigned int	getTotalbytes(void) const;
		void			setSentbytes(unsigned int sb);
		unsigned int	getSentbytes(void) const;
		std::string		getHeader(void) const;
		char			*getBuffer(void) const;
		bool			toRead(void) const;
	// Function
		int				readRequest(void);

	private:
	// Attributes
		int				_fd;
		unsigned int	_totalbytes;
		unsigned int	_sentbytes;
		char			_buffer[1024 + 1];
		std::string		_request;
		std::string		_header;
		std::string		_body;
		bool			_toread;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
