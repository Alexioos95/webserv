/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:35:24 by apayen            #+#    #+#             */
/*   Updated: 2024/01/25 10:07:39 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Client.hpp"

bool g_sigint = false;

void	sigint_handler(int sig)
{
	static_cast<void>(sig);
	g_sigint = true;
}

void		sigquit_handler(int sig)
{ static_cast<void>(sig); }

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
