/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 08:55:05 by apayen            #+#    #+#             */
/*   Updated: 2024/01/30 11:10:05 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

# include <utility>		// pair
# include <iostream>	// cout

class Location
{
	public:
	// Constructors and Destructor
		Location(void);
		Location(bool get, bool post, bool del, bool cgi, std::pair<bool, std::string> autoindex, \
			std::pair<bool, std::string> ret, std::pair<bool, std::string> alias);
		Location(Location const &rhs);
		~Location(void);
	// Overloads
		Location						&operator=(Location const &rhs);
	// Getters
		bool							allowMethod(std::string) const;
		bool							allowCgi(void) const;
		std::pair<bool, std::string>	getAutoindex(void) const;
		std::pair<bool, std::string>	getReturn(void) const;
		std::pair<bool, std::string>	getAlias(void) const;

	private:
	// Attributes
		std::string						_path;
		bool							_get;
		bool							_post;
		bool							_del;
		bool							_cgi;
		std::pair<bool, std::string>	_autoindex;
		std::pair<bool, std::string>	_ret;
		std::pair<bool, std::string>	_alias;
};

#endif
