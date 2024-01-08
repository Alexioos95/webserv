/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:33:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/08 09:37:20 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include "WebServer.hpp"
extern bool g_sigint;

class ServerBlock
{
	public:
	// Constructors and destructor
		ServerBlock(std::string server_name, std::string root, std::vector<int> ports);
		ServerBlock(ServerBlock const &rhs);
		~ServerBlock(void);
	// Exceptions
		class CreationException : public std::exception
		{ public: const char	*what(void) const throw(); };
	// Overload
		ServerBlock				&operator=(ServerBlock const &rhs);
	// Getters
		std::string				getName(void) const;
		std::vector<int>		&getSocket(void);
		std::vector<int>		&getPorts(void);

	private:
	// Constructors
		ServerBlock(void);
	// Overload
	// Attributes
		std::string				_server_name;
		std::string				_root;
		std::vector<int> 		_socket;
		std::vector<int>		_ports;
		struct sockaddr_in		_structsock;
		int						_addr;
};

void	*ft_memset(void *s, int c, size_t n);
int		setNonblockingFD(int fd);

#endif
