/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2023/12/12 11:33:45 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno> // Errno
# include <string.h> // strerror
# include <iostream> // std::cout
# include <exception> // Exceptions

class Client
{
	public:
	// Constructors and Destructor
		Client(void);
		~Client(void);
	// Setters and Getters
		void	setFD(int fd);
		int		getFD(void) const;

	private:
	// Constructors and Destructor
		Client(Client const &rhs);
		Client	&operator=(Client const &rhs);
	// Attributes
		int		_fd;
		char	*_buffer[4096];
};

#endif
