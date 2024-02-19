/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:35:24 by apayen            #+#    #+#             */
/*   Updated: 2024/02/19 12:21:37 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

bool g_sigint = false;

void	sig_handler(int sig)
{
	if (sig == SIGINT)
		g_sigint = true;
}

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

std::string	itoa(int nbi)
{
	std::string nb;

	if (nbi == 0)
		return ("0");
	while (nbi > 0)
	{
		nb.insert(nb.begin(), (nbi % 10) + '0');
		nbi = nbi / 10;
	}
	return (nb);
}

std::string	asciiart(void)
{
	std::string	ret;

	ret = ret + "\n[!] The program has been closed.\n";
	ret = ret + "  _ _____ _ _     __  _ _____   _______   _____ _ __ __ ___ _ \n";
	ret = ret + " `.\\_   _| | |   |  \\| | __\\ \\_/ /_   _| |_   _| |  V  | __/ \\\n";
	ret = ret + "     | | | | |_  | | ' | _| > , <  | |     | | | | \\_/ | _|\\_/\n";
	ret = ret + "     |_| |_|___| |_|\\__|___/_/ \\_\\ |_|     |_| |_|_| |_|___(_)\n";
	return (ret);
}
