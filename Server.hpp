/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:33:30 by apayen            #+#    #+#             */
/*   Updated: 2023/12/14 13:14:57 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp" // Client's Class
# include <cerrno> // Errno
# include <vector> // Vector
# include <string.h> // strerror
# include <iostream> // std::cout
# include <exception> // Exceptions
# include <arpa/inet.h> // Functions HostTONetwork
# include <sys/socket.h> // Sockets
# include <netinet/in.h> // Struct sockaddr_in
	// Select
# include <unistd.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/select.h>

class Server
{
	public:
	// Constructors and destructor
		Server(void);
		Server(int const port);
		~Server(void);
	// Exceptions
		class FailSocketException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class FailBindException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class FailFcntlException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class FailListenException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class FailSelectException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class FailAcceptingClientException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class CriticalErrorException : public std::exception
			{ public: const char	*what(void) const throw(); };
		class ClientFailReadException : public std::exception
			{ public: const char	*what(void) const throw(); };
	// Getters
		int		getSocket(void) const;
	// Functions
		void	run(void);

	private:
	// Constructors
		Server(Server const &rhs);
	// Overload
		Server				&operator=(Server const &rhs);
	// Attributes
		const int			_port;
		const int			_backlog;
		int 				_socket;
		struct sockaddr_in	_structsock;
		int					_addr;
		fd_set				_rset;
		fd_set				_wset;
		fd_set				_errset;
		std::vector<Client>	_clients;
	// Functions
		void				setFDSet(void);
		void				setNonblockingFD(int fd);
};

void	*ft_memset(void *s, int c, size_t n);

#endif
