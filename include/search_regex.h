#ifndef _search_regex_strat_
#define _search_regex_strat_

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <functional>
#include "SearchStrategy.h"
#include <mutex>
#include <sstream>


	class regex_search : public SearchStrategy
	{
		
	public:
		
		void pre_process(char* pattern, bool ignore_case, bool single_file , int flags);

		void search(int id, std::string fpath);
    
    private:

		void file_name_only(std::ifstream& input, const std::string &path);

		void line_count_only(std::ifstream& input, const std::string &path);

		void normal(std::ifstream& input, const std::string &path);

		void output_matches(std::string &line,int line_num, const std::string &path, std::stringstream& ss);

	private:
		std::regex expr;
		std::mutex print_mutex;
		bool print_line_num , print_file_name, print_matched_file;
		std::function <void(std::ifstream&,std::string)> strategy;		
	};


#endif