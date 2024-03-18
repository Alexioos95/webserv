#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

enum langage {ERROR, PHP, RUBY};
enum file {READ, WRITE};

class Cgi{
	public :
		Cgi();
		Cgi(const std::vector<std::string> &);
		Cgi(const Cgi &cgi);
		~Cgi();
		Cgi	&operator=(const Cgi &);
		void	launchCgi(const std::string &);
		int	getFdWrite();
		int	getFdRead();
		int	getPid();
		std::vector<char> getOutputCgi();

	private :
		int	_pid;
		int	_pipeIn[2];
		int	_pipeOut[2];
		char	**_env;
};

#endif
