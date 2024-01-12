/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualVirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/12 12:00:43 by apayen            #+#    #+#             */
/*   Updated: 2024/01/12 12:00:43 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

# include <map>				// map
# include <vector>			// vector
# include <errno.h>			// errno
# include <cstring>			// strerror, close
# include <iostream>		// cout, string
# include <unistd.h>		// close
# include <netinet/in.h>	// socket functions

class VirtualServer
{
	public:
	// Constructors and destructor
		VirtualServer(std::string name, std::string root, std::vector<int> ports, std::map<int, int> &m, int bodymax);
		VirtualServer(VirtualServer const &rhs);
		~VirtualServer(void);
	// Exceptions
		class CreationException : public std::exception
			{  };
	// Overload
		VirtualServer				&operator=(VirtualServer const &rhs);
	// Getters
		std::string					getName(void) const;
		std::string					getRoot(void) const;
		std::vector<int>			&getPorts(void);
		int							getBodyMax(void) const;

	private:
	// Constructors
		VirtualServer(void);
	// Functions
		bool						bindPort(std::map<int, int> &m, int port);

	// Attributes
		std::string					_name;
		std::string					_root;
		std::vector<int>			_ports;
		int							_bodymax;
};

void	*ft_memset(void *s, int c, size_t n);
int		unblockFD(int fd);

#endif
