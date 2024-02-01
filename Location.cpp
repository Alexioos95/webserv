/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 08:54:55 by apayen            #+#    #+#             */
/*   Updated: 2024/02/01 13:45:07 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

//////////////////////////////
// Constructors and Destructor
Location::Location(void) : _get(true), _post(true), _del(true), _cgi(false), _autoindex(false)
{
	this->_ret = std::pair<bool, std::string>(false, "");
	this->_alias = std::pair<bool, std::string>(false, "");
}

Location::Location(bool get, bool post, bool del, bool cgi, bool autoindex, \
	std::pair<bool, std::string> index, std::pair<bool, std::string> ret, std::pair<bool, std::string> alias) \
	: _get(get), _post(post), _del(del), _cgi(cgi), _autoindex(autoindex), _index(index), _ret(ret), _alias(alias) { }

Location::Location(Location const &rhs)
{
	this->_get = rhs._get;
	this->_post = rhs._post;
	this->_del = rhs._del;
	this->_cgi = rhs._cgi;
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
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
		this->_index = rhs._index;
		this->_ret = rhs._ret;
		this->_alias = rhs._alias;
	}
	return (*this);
}

//////////////////////////////
// Functions
bool	Location::allowMethod(std::string method, bool &get, bool &post, bool &del) const
{
	get = this->_get;
	post = this->_post;
	del = this->_del;
	if (method == "GET")
		return (get);
	else if (method == "POST")
		return (post);
	else if (method == "DELETE")
		return (del);
	return (false);
}

bool	Location::allowCgi(void) const
{ return (this->_cgi); }

bool	Location::allowAutoindex(void) const
{ return (this->_autoindex); }

std::pair<bool, std::string>	Location::getIndex(void) const
{ return (this->_index); }

std::pair<bool, std::string>	Location::getReturn(void) const
{ return (this->_ret); }

std::pair<bool, std::string>	Location::getAlias(void) const
{ return (this->_alias); }
