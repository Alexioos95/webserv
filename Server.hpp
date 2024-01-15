/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 12:00:43 by apayen            #+#    #+#             */
/*   Updated: 2024/01/15 12:13:06 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>				// map
# include <vector>			// vector
# include <errno.h>			// errno
# include <fcntl.h>			// fcntl
# include <cstring>			// strerror, close
# include <iostream>		// cout, string
# include <unistd.h>		// close
# include <netinet/in.h>	// socket functions

class Server
{
	public:
	// Constructors and destructor
		Server(std::string name, std::string root, std::vector<int> ports, std::map<int, int> &m, int bodymax);
		Server(Server const &rhs);
		~Server(void);
	// Exceptions
		class CreationException : public std::exception
			{  };
	// Overload
		Server				&operator=(Server const &rhs);
	// Getters
		std::string			getName(void) const;
		std::string			getRoot(void) const;
		std::vector<int>	&getPorts(void);
		int					getBodyMax(void) const;

	private:
	// Constructors
		Server(void);
	// Functions
		bool				bindPort(std::map<int, int> &m, int port);

	// Attributes
		std::string			_name;
		std::string			_root;
		std::vector<int>	_ports;
		int					_bodymax;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
