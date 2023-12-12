/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:05:30 by apayen            #+#    #+#             */
/*   Updated: 2023/12/12 12:05:24 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*s2;

	i = 0;
	s2 = static_cast<unsigned char *>(s);
	while (i < n)
	{
		s2[i] = c;
		i++;
	}
	return (s);
}


int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
		{
			std::cout << "Usage: ./webserv <\"config\".conf>" << std::endl;
			return (1);
		}
		static_cast<void>(argv);
		Server	serv;

		serv.run();
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what();
		if (errno != 0)
			std::cout << strerror(errno);
		std::cout << std::endl;
		return (1);
	}
	return (0);
}
