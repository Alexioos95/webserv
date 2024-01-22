/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:12 by apayen            #+#    #+#             */
/*   Updated: 2024/01/22 15:16:59 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <vector>			// vector
# include <fstream>			// fstream
# include <iostream>		// cout
							// stat
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>	// recv

# include "Client.hpp"

class Request
{
	public:
	// Constructors and Destructor
		Request(Client &cl);
		// Request(Request const &rhs);
		~Request(void);
	// Overloads
		Request	&operator=(Request const &rhs);
	// Functions
		int					read(void);
		int					write(void);

	protected:
	// Attributes
		std::string			_request;
		std::string			_header;
		std::string			_body;
		std::string			_name;
		std::string			_method;
		std::string			_filepath;
		std::fstream		_file;
		struct stat 		_stat;
		std::vector<char>	_headerresponse;
		std::vector<char>	_bodyresponse;
		std::vector<char>	_response;
		int					_contentlength;
		int					_maxcontentlength;

	private:
	// Attributes
		Client				&_client;
	// Functions
		std::string			parse(void);
		std::string			open(void);
		std::string			create(void);
		std::string			get(void);
		std::string			del(void);
		std::string			post(void);
		std::vector<char>	buildResponse(std::string status);
		std::string			getTime(std::time_t time);
		std::string			getMime(void);
};

void		*ft_memset(void *s, int c, size_t n);
std::string	itoa(int nbi);

#endif
