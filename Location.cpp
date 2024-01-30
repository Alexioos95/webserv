/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 08:54:55 by apayen            #+#    #+#             */
/*   Updated: 2024/01/30 15:16:21 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

//////////////////////////////
// Constructors and Destructor
Location::Location(void) : _get(true), _post(true), _del(true), _cgi(false)
{
	this->_autoindex = std::pair<bool, std::string>(false, "");
	this->_ret = std::pair<bool, std::string>(false, "");
	this->_alias = std::pair<bool, std::string>(false, "");
}

Location::Location(bool get, bool post, bool del, bool cgi, std::pair<bool, std::string> autoindex, \
	std::pair<bool, std::string> ret, std::pair<bool, std::string> alias) : _get(get), _post(post), \
	_del(del), _cgi(cgi), _autoindex(autoindex), _ret(ret), _alias(alias) { }

Location::Location(Location const &rhs)
{
	this->_get = rhs._get;
	this->_post = rhs._post;
	this->_del = rhs._del;
	this->_cgi = rhs._cgi;
	this->_autoindex = rhs._autoindex;
	this->_ret = rhs._ret;
	this->_alias = rhs._alias;
}

Location::~Location(void) { }

//////////////////////////////
// Overloads
Location	&Location::operator=(Location const &rhs)
{
	if (this != &rhs)
	{
		this->_get = rhs._get;
		this->_post = rhs._post;
		this->_del = rhs._del;
		this->_cgi = rhs._cgi;
		this->_autoindex = rhs._autoindex;
		this->_ret = rhs._ret;
		this->_alias = rhs._alias;
	}
	return (*this);
}

//////////////////////////////
// Functions
bool	Location::allowMethod(std::string method) const
{
	if (method == "GET")
		return (this->_get);
	else if (method == "POST")
		return (this->_post);
	else if (method == "DELETE")
		return (this->_del);
	return (true);
}

bool	Location::allowCgi(void) const
{ return (this->_cgi); }

std::pair<bool, std::string>	Location::getAutoindex(void) const
{ return (this->_autoindex); }

std::pair<bool, std::string>	Location::getReturn(void) const
{ return (this->_ret); }

std::pair<bool, std::string>	Location::getAlias(void) const
{ return (this->_alias); }
