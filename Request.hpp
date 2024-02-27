/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 08:54:12 by apayen            #+#    #+#             */
/*   Updated: 2024/02/19 16:02:20 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <ctime>			// functions time
# include <string>			// string
# include <vector>			// vector
# include <cstdlib>			// atoi
# include <iostream>		// cout
# include <algorithm>		// STL
							// stat
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>	// recv

# include "Manager.hpp"
# include "Server.hpp"

class Request
{
	public:
	// Constructors and Destructor
		Request(Client &cl);
		~Request(void);
	// Functions
		int									reader(void);
		int									writer(void);

	private:
	// Attributes
		Client								&_client;
		Server								_serv;
		// States
		bool								_inparse;
		bool								_inprocess;
		bool								_inerror;
		bool								_inbuild;
		bool								_inwrite;
		// Request
		std::string							_request;
		std::string							_header;
		std::vector<char>					_body;
		std::vector<char>					_headerresponse;
		std::vector<char>					_bodyresponse;
		std::vector<char>					_response;
		// Parse
		std::string							_status;
		std::string							_name;
		std::string							_method;
		std::string							_filename;
		std::string							_filepath;
		int									_fdfile;
		struct stat 						_stat;
		int									_contentlength;
		int									_maxcontentlength;
		// Location
		bool								_get;
		bool								_post;
		bool								_del;
		std::string							_dir;
		bool								_autoindex;
		std::string							_redirect;
		int									_redirected;
	// Functions
		// Parsing
		std::string							parse(void);
		void								fillHeader(size_t pos, int bytes);
		void								fillBody(size_t pos, int bytes);
		bool								searchServ(void);
		std::string							checkLocation(void);
		int									simulateRedirect(std::string path);
		// Methods
		std::string							openf(void);
		std::string							create(void);
		std::string							get(void);
		std::string							del(void);
		std::string							post(void);
		std::string							error(void);
		// Response
		void								buildResponse(void);
		std::string							getMethods(void);
		std::string							getTime(std::time_t time);
		std::string							getMime(void);
		void								clear(void);
};

void		*ft_memset(void *s, int c, size_t n);
std::string	itoa(int nbi);
int			autoindex(const char *directory_name, std::string &res);

#endif
