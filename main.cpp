/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:05:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/08 10:29:18 by apayen           ###   ########.fr       */
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
	catch (const std::exception &e)
	{
		std::string res = e.what();
		if (res.empty() || res == "std::exception")
		{
			std::cout << std::endl << "[!] The program has been closed.\n";
			std::cout << "  _ _____ _ _     __  _ _____   _______   _____ _ __ __ ___ _ \n";
			std::cout << " `.\\_   _| | |   |  \\| | __\\ \\_/ /_   _| |_   _| |  V  | __/ \\\n";
			std::cout << "     | | | | |_  | | ' | _| > , <  | |     | | | | \\_/ | _|\\_/\n";
			std::cout << "     |_| |_|___| |_|\\__|___/_/ \\_\\ |_|     |_| |_|_| |_|___(_)\n" << std::endl;
			return (0);
		}
		else
		{
			std::cerr << "[!] Error: " << e.what();
			if (errno != 0)
				std::cerr << strerror(errno);
			std::cerr << std::endl;
			return (1);
		}
	}
	return (0);
}
