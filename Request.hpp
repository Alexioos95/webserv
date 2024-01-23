/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:12 by apayen            #+#    #+#             */
/*   Updated: 2024/01/23 14:29:38 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <ctime>			// time functions
# include <vector>			// vector
# include <cstdlib>			// atoi
# include <iostream>		// cout
							// stat
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>	// recv

class Request
{
	public:
	// Constructors and Destructor
		Request(Client &cl);
		~Request(void);
	// Functions
		int					reader(void);
		int					writer(void);

	protected:
	// Attributes
		std::string			_request;
		std::string			_header;
		std::string			_body;
		std::string			_status;
		std::string			_name;
		std::string			_method;
		std::string			_filepath;
		int					_file;
		struct stat 		_stat;
		std::vector<char>	_headerresponse;
		std::vector<char>	_bodyresponse;
		std::vector<char>	_response;
		int					_contentlength;
		int					_maxcontentlength;

	private:
	// Attributes
		Client				&_client;
		bool				_inparse;
		bool				_inprocess;
		bool				_inbuild;
		bool				_inwrite;
	// Functions
		std::string			parse(void);
		std::string			openf(void);
		std::string			create(void);
		std::string			get(void);
		std::string			del(void);
		std::string			post(void);
		void				buildResponse(std::string status);
		std::string			getTime(std::time_t time);
		std::string			getMime(void);
		void				clear(void);
};

void		*ft_memset(void *s, int c, size_t n);
std::string	itoa(int nbi);

#endif
