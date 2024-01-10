/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:33:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/10 09:07:05 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Manager.hpp"
extern bool g_sigint;

class Server
{
	public:
	// Constructors and destructor
		Server(std::string server_name, std::string root, std::vector<int> ports);
		Server(Server const &rhs);
		~Server(void);
	// Exceptions
		class CreationException : public std::exception
			{ public: const char	*what(void) const throw(); };
	// Overload
		Server						&operator=(Server const &rhs);
	// Getters
		std::string					getName(void) const;
		std::string					getRoot(void) const;
		std::vector<int>			&getSocket(void);
		std::vector<int>			&getPorts(void);

	private:
	// Constructors
		Server(void);
	// Attributes
		std::string					_server_name;
		std::string					_root;
		std::vector<int> 			_socket;
		std::vector<int>			_ports;
		struct sockaddr_in			_structsock;
		int							_addr;
};

void	*ft_memset(void *s, int c, size_t n);
int		setNonblockingFD(int fd);

#endif
