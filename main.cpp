/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apayen <apayen@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 09:05:30 by apayen            #+#    #+#             */
/*   Updated: 2024/01/30 15:15:34 by apayen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

int	main(int argc, char **argv)
{
	static_cast<void>(argv);
	// ^ A enlever ^
	Manager	mng;

	try
	{
		if (argc != 2)
		{
			std::cerr << "[*] Usage: ./webserv <\"config\".conf>" << std::endl;
			return (1);
		}
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR || signal(SIGINT, sigint_handler) == SIG_ERR \
			|| signal(SIGQUIT, sigquit_handler) == SIG_ERR)
		{
			std::cerr << "[!] Critical error in the program: signal: " << strerror(errno) << std::endl;
			return (1);
		}
		mng.run();
	}
	catch (const std::exception &e)
	{
		int			ret;
		std::string res;

		ret = 0;
		res = e.what();
		mng.shutdown();
		if (!res.empty() && res != "std::exception")
		{
			std::cerr << "[!] Critical error in the program: " << e.what();
			if (errno != 0)
				std::cerr << strerror(errno);
			std::cerr << "\nAll connection have been closed." << std::endl;
			ret = 1;
		}
		std::cout << std::endl << "[!] The program has been closed.\n";
		std::cout << "  _ _____ _ _     __  _ _____   _______   _____ _ __ __ ___ _ \n";
		std::cout << " `.\\_   _| | |   |  \\| | __\\ \\_/ /_   _| |_   _| |  V  | __/ \\\n";
		std::cout << "     | | | | |_  | | ' | _| > , <  | |     | | | | \\_/ | _|\\_/\n";
		std::cout << "     |_| |_|___| |_|\\__|___/_/ \\_\\ |_|     |_| |_|_| |_|___(_)\n" << std::endl;
		return (ret);
	}
	return (0);
}
