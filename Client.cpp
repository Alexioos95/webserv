/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 10:43:22 by apayen            #+#    #+#             */
/*   Updated: 2023/12/12 11:33:43 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// Constructors and Destructor
Client::Client(void) : _fd(-1) { }

Client::~Client(void) {}

// Functions
void	Client::setFD(int fd)
{ this->_fd = fd; }

int	Client::getFD(void) const
{ return (this->_fd); }
