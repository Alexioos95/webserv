/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:05:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/04 15:45:44 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: ./webserv <\"config\".conf>" << std::endl;
			return (1);
		}
		static_cast<void>(argv);
		WebServer	ws;
		signal(SIGPIPE, SIG_IGN);
		signal(SIGINT, sigint_handler);
		// <- parsing config here.
		ws.run();
	}
	catch (const WebServer::SigintException &e)
	{
		std::cout << std::endl << "[!] The Webserver has been closed through SIGINT." << " 'til next time!" << std::endl;
		return (0);
	}
	catch (const std::exception &e)
	{
		std::cerr << "[!] Error: " << e.what();
		if (errno != 0)
			std::cerr << strerror(errno);
		std::cerr << std::endl;
		return (1);
	}
	return (0);
}
