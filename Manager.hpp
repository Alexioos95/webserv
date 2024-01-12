/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:33:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/10 09:07:27 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGER_HPP
# define MANAGER_HPP

# include <map>				// map
# include <ctime>			// functions time
# include <cerrno>			// errno
# include <vector>			// vector
# include <fcntl.h>			// fcntl
# include <fstream>			// fstream
# include <string.h>		// strerror
# include <signal.h>		// signal
# include <iostream>		// std::cout/cerr
# include <exception>		// exceptions
# include <sys/stat.h>		// stat
# include <arpa/inet.h>		// functions HostTONetwork
# include <sys/socket.h>	// sockets
# include <netinet/in.h>	// struct sockaddr_in
							// select
# include <unistd.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/select.h>

extern bool g_sigint;

class Client;
class PortSocket;
class VirtualServer;

class Manager
{
	public:
	// Constructors and destructor
		Manager(void);
		~Manager(void);
	// Exceptions
		class SigintException : public std::exception
			{ };
		class SelectException : public std::exception
			{ public: const char	*what(void) const throw(); };
	// Getters
		std::map<int, int>			&getSockets(void);
	// Functions
		void						run(void);

	private:
	// Constructors
		Manager(Manager const &rhs);
	// Overload
		Manager						&operator=(Manager const &rhs);
	// Functions
		void						shutdown(void);
		void						manageFDSets(void);
		void						checkPorts(void);
		void						checkClients(void);
		std::string					parseRequest(Client &cl);
		std::vector<char>			buildResponse(Client &cl, std::string status);
	// Attributes
		std::map<int, int>			_sockets;
		std::vector<VirtualServer>	_servs;
		std::vector<Client>			_clients;
		fd_set						_rset;
		fd_set						_wset;
		fd_set						_errset;
};

void		*ft_memset(void *s, int c, size_t n);
void		sigint_handler(int sig);
void		sigquit_handler(int sig);
std::string	getTime(std::time_t time);
std::string	itoa(int nbi);
std::string	getMime(Client &cl);

#endif
