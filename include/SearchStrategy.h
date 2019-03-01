#ifndef _SearchStrategy_H_
#define _SearchStrategy_H_

#include <unistd.h>
#include <string>

/**
 * @brief A pure virtual Interface class to specify search algorithm.
 * 
 * Makes use of Strategy Design Pattern
 */
class SearchStrategy
{
	public:
		/**
		 * @brief Pre process function. The class gets all information it needs for further search
		 * 
		 * @param pattern 
		 * @param ignore_case 
		 * @param single_file 
		 * @param flags 
		 */
		virtual void pre_process(char* pattern, bool ignore_case, bool single_file, int flags) = 0;

		/**
		 * @brief Actual search. It gets the file path on which the search is to be applied
		 * 
		 * @param id 
		 * @param path 
		 */
		virtual void search(int id, std::string path) = 0;

		///	Output Color variables. To be consistent throughout searchStrategy instances
		const std::string COLOR_PURPLE = 	(isatty(STDOUT_FILENO) ?"\033[35m" 	: "");
		const std::string COLOR_RESET = 	(isatty(STDOUT_FILENO) ?"\033[m" 	: "");
		const std::string COLOR_RED_BOLD = 	(isatty(STDOUT_FILENO) ?"\033[1;31m": ""); 
		const std::string COLOR_CYAN = 		(isatty(STDOUT_FILENO) ?"\033[36m"	: "");
		const std::string COLOR_RED = 		(isatty(STDOUT_FILENO) ?"\033[32m" 	: "");
};

#endif