#include <unistd.h>

#ifndef _SearchStrategy_H_
#define _SearchStrategy_H_

class SearchStrategy
{
	public:
		virtual void pre_process(char* pattern, bool ignore_case, bool single_file, int flags) = 0;
		virtual void search(int id, std::string path) = 0;
		const std::string COLOR_PURPLE = 	(isatty(STDOUT_FILENO) ?"\033[35m" 	: "");
		const std::string COLOR_RESET = 	(isatty(STDOUT_FILENO) ?"\033[m" 	: "");
		const std::string COLOR_RED_BOLD = 	(isatty(STDOUT_FILENO) ?"\033[1;31m": ""); 
		const std::string COLOR_CYAN = 		(isatty(STDOUT_FILENO) ?"\033[36m"	: "");
		const std::string COLOR_RED = 		(isatty(STDOUT_FILENO) ?"\033[32m" 	: "");
};

#endif