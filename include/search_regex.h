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
#include "RAII_utils.h"


	class regex_search : public SearchStrategy
	{
		
	public:
		
		/**
		 * @brief Implements pre_process() of Interface SearchStrategy
		 * 
		 * @param pattern 
		 * @param ignore_case 
		 * @param single_file 
		 * @param flags 
		 */
		void pre_process(char* pattern, bool ignore_case, bool single_file , int flags);

		/**
		 * @brief Implements search() of Interfacr SearchStrategy
		 * 
		 * @param id 
		 * @param fpath 
		 */
		void search(int id, std::string fpath);
    
    private:

		/**
		 * @brief Utility | Case: Print only File name
		 * 
		 * @param input 
		 * @param path 
		 */
		void file_name_only(std::ifstream& input, const std::string &path);

		/**
		 * @brief Utility | Case: Print only Line count
		 * 
		 * @param input 
		 * @param path 
		 */
		void line_count_only(std::ifstream& input, const std::string &path);

		/**
		 * @brief Utility | Case: Default case
		 * 
		 * @param input 
		 * @param path 
		 */
		void normal(std::ifstream& input, const std::string &path);
		
		/**
		 * @brief Utility for normal case
		 * 
		 * @param line 
		 * @param line_num 
		 * @param path 
		 * @param ss 
		 */
		void output_matches(std::string &line,int line_num, const std::string &path, std::stringstream& ss);

	private:
		std::regex expr;
		std::mutex print_mutex;
		bool print_line_num , print_file_name, print_matched_file;
		std::function <void(std::ifstream&,std::string)> strategy;		
	};


#endif