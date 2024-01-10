/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:35:24 by apayen            #+#    #+#             */
/*   Updated: 2024/01/10 13:42:50 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

bool g_sigint = false;

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

int	setNonblockingFD(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return (1);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return (1);
	return (0);
}

void	sigint_handler(int sig)
{
	static_cast<void>(sig);
	g_sigint = true;
}

std::string	getLineTime(std::time_t time)
{
	std::string	day;
	std::string	month;
	std::string	nbday;
	std::string	hour;
	std::string	year;

	std::string line(std::asctime(std::localtime(&time)));
	day = line.substr(0, 3);
	month = line.substr(4, 3);
	nbday = line.substr(8, 2);
	hour = line.substr(11, 8);
	year = line.substr(20, 4);
	line = day + ", " + nbday + " " + month + " " + year + " " + hour + " CET";
	return (line);
}
