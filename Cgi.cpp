#include "Cgi.hpp"

Cgi::Cgi(Manager &man, const std::vector<std::string> &env) : _man(man)
{
	int i = 0;
	_pid = -1;
	_env = new char*[(env.size() + 1) * sizeof(char *)];
	for (std::vector<std::string>::const_iterator ite = env.begin(); ite != env.end(); i++, ite++)
		_env[i] = (char *)ite->c_str();
	_env[i] = NULL;
	_pipeIn[0] = -2;
	_pipeIn[1] = -2;
	_pipeOut[0] = -2;
	_pipeOut[1] = -2;
	if (pipe(_pipeIn) == -1)
		throw std::runtime_error("cgi : pipe error");
	if (pipe(_pipeOut) == -1)
		throw std::runtime_error("cgi : pipe error");
}

// Cgi::Cgi(const Cgi &cgi) : _env(NULL), _man(cgi._man)
// {
// 	*this = cgi;
// }

// Cgi &Cgi::operator=(const Cgi &rhs)
// {
// 	if (this != &rhs)
// 	{
// 		int i;
// 		_pid = rhs._pid;
// 		_pipeIn[0] = rhs._pipeIn[0];
// 		_pipeIn[1] = rhs._pipeIn[0];
// 		_pipeOut[0] =rhs._pipeOut[0];
// 		_pipeOut[1] =rhs._pipeOut[0];
// 		int size;
// 		size = 0;
// 		if (rhs._env)
// 			for (size = 0; rhs._env[size];size++);
// 		if (this->_env)
// 			delete (this->_env);
// 		_env = new char*[(size + 1) * sizeof(char *)];
// 		for (i = 0; i < size; i++)
// 			_env[i] = rhs._env[i];
// 		_env[i] = NULL;
// 	}
// 	return (*this);
// }

Cgi::~Cgi()
{
	if (_pid > 0)
	{
		kill(_pid, SIGKILL);
		_pid = -1;
	}
	if (_pipeIn[0] > 0)
		close(_pipeIn[0]);
	if (_pipeIn[1] > 0)
		close(_pipeIn[1]);
	if (_pipeOut[0] > 0)
		close(_pipeOut[0]);
	if (_pipeOut[1] > 0)
		close(_pipeOut[1]);
	if (_env)
	{
		delete[] _env;
		_env = NULL;
	}
}

bool checkEnd(const char *a, const char *b)
{
	size_t aEnd = strlen(a);
	size_t bEnd = strlen(b);

	while (aEnd > 0 && bEnd > 0 && a[aEnd - 1] == b[bEnd - 1])
	{
		bEnd--;
		aEnd--;
	}
	return (bEnd == 0);
}

void Cgi::launchCgi(const std::string &f)
{
	int cgiType;

	cgiType = checkEnd(f.c_str(), ".php") ? PHP : checkEnd(f.c_str(), ".ruby") ? RUBY : ERROR;
	if (cgiType == ERROR)
		throw std::invalid_argument("only php and ruby are handdled");
	if (_pipeIn[1] > 0)
	{
		close(_pipeIn[1]);
		_pipeIn[1] = -2;
	}
	_pid = fork();
	if (_pid == 0)
	{
		_man.shutdown(false);
		if (_pipeOut[0] > -1)
			close(_pipeOut[0]);
		if (_pipeIn[1] > -1)
			close(_pipeIn[1]);
		dup2(_pipeIn[0], STDIN_FILENO);
		dup2(_pipeOut[1], STDOUT_FILENO);
		if (_pipeIn[0] > -1)
			close(_pipeIn[0]);
		if (_pipeOut[1] > -1)
			close(_pipeOut[1]);
		if (cgiType == PHP)
		{
			const char *args[] = {"php" ,f.c_str(), NULL};
			if (execve("/usr/bin/php", const_cast<char* const*>(args), _env) == -1)
				std::cerr<<"fail to execute php cgi"<<std::endl;
		}
		else if (cgiType == RUBY)
		{
			const char *args[] = {"ruby" ,f.c_str(), NULL};
			if (execve("/usr/bin/ruby", const_cast<char* const*>(args), _env) == -1)
				std::cerr<<"fail to execute ruby cgi"<<std::endl;
		}
		exit(5);
	}
	if (_pid == -1)
		throw std::runtime_error("cgi : fail to fork");
	if (_pipeIn[0] > -1)
	{
		close(_pipeIn[0]);
		_pipeIn[0] = -1;
	}
	if (_pipeOut[1] > -1)
	{
		close(_pipeOut[1]);
		_pipeOut[1] = -1;
	}
}

int Cgi::getFdWrite()
{
	return (_pipeIn[1]);
}

int Cgi::getFdRead()
{
	return (_pipeOut[0]);
}

// std::vector<char> Cgi::getOutputCgi()
// {
// 	char	buf[2048];
// 	int	 readRes;
// 	std::vector<char>   res;
// 	std::vector<char>::iterator   it;

// 	while (_pipeOut[0] != -1 && (readRes = read(_pipeOut[0], buf, 2048)) > 0)
// 	{
// 		for (int i = 0; i < readRes; i++)
// 		{
// 			res.push_back(buf[i]);
// 		}
// 	}
// 	if (readRes == -1)
// 		throw std::runtime_error("cgi : fail to read");
// 	return (res);
// }

int Cgi::getPid()
{
	return (_pid);
}
