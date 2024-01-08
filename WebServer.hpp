/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:33:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/08 11:04:14 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <cerrno>			// Errno
# include <vector>			// Vector
# include <cstdlib>			// exit
# include <fcntl.h>			// fcntl
# include <string.h>		// strerror
# include <signal.h>		// signal
# include <iostream>		// std::cout/cerr
# include <exception>		// Exceptions
# include <arpa/inet.h>		// Functions HostTONetwork
# include <sys/socket.h>	// Sockets
# include <netinet/in.h>	// Struct sockaddr_in
							// Select
# include <unistd.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/select.h>

extern bool g_sigint;
class ServerBlock;
class Client;
class WebServer
{
	public:
	// Constructors and destructor
		WebServer(void);
		~WebServer(void);
	// Exceptions
		class SigintException : public std::exception {};
		class SelectException : public std::exception
			{ public: const char	*what(void) const throw(); };
	// Functions
		void	run(void);

	private:
	// Constructors
		WebServer(WebServer const &rhs);
	// Overload
		WebServer			&operator=(WebServer const &rhs);
	// Functions
		void	manageFDSets(void);
		void	checkServerBlocks(void);
		void	newClient(std::string name, int port, int socket);
		void	checkClients(void);
	// Attributes
		std::vector<ServerBlock>	_servs;
		std::vector<Client>			_clients;
		fd_set						_rset;
		fd_set						_wset;
		fd_set						_errset;
};

void	*ft_memset(void *s, int c, size_t n);
void	sigint_handler(int sig);

#endif
