/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:41:38 by apayen            #+#    #+#             */
/*   Updated: 2023/12/14 13:32:50 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cerrno> // Errno
# include <iostream> // std::cout
# include <fcntl.h> // fnctl

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
		char			*getBuffer(void) const;

	private:
	// Attributes
		int				_fd;
		unsigned int	_totalbytes;
		unsigned int	_sentbytes;
		char			*_buffer;
};

void	*ft_memset(void *s, int c, size_t n);

#endif
