#pragma once

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <functional>

	class regex_search
	{
	public:
		regex_search(const char* pattern, bool ignore_case, bool color)
		{
			using namespace std::regex_constants;
			auto flag_mask = nosubs | optimize ; //Specify grammar
			if(ignore_case) flag_mask |= icase;
			try {
				// std::cout<<pattern;
				expr.assign(pattern, flag_mask);
			}
			catch (std::regex_error& e) {
				std::cerr << "regex_error : " << e.what() << '\n';
				throw e;
				// What to do now ??
			}
		}

		bool match_exists(const std::string& line) {
			try {

			}
		}

		void matched_lines(std::ifstream& input) {

			int lines_matched = 0 , line_num = 0;

			std::string line;
			while(std::getline(input,line)) {
				++line_num;
				if(std::regex_match(line,expr)) {
					++lines_matched;
				}
			}
			printf("Lines Matched : %d\n",lines_matched );
		}

		void search_n(const char* path)
		{
			std::ifstream input(path);
			// std::cerr<<path<<">\n";
			if(!input.is_open()) {
				std::cerr << "grape: Invalid input file. Unable to open " << path << '\n' ;
				return;
			}

			int line_num = 1;
			std::string line;

			while(std::getline(input,line)) {
				output_matches(line,line_num);
				++line_num;
			}

		}

		// void search_n(const char * path) {


		// }

		void output_matches(std::string &line,int line_num)
		{
			try {
				std::sregex_iterator next(line.begin(), line.end(), expr);
				std::sregex_iterator end;
				if(next!=end)
				{
					size_t cur_pos = 0;
					if(n) printf("%d:",line_num);
					while (next != end) {
						std::smatch match = *next;
						printf("%.*s",int(match.position()-cur_pos), line.data()+cur_pos);
						printf("%s%s%s",RED,match.str().data(),END);
						cur_pos = match.position()+match.length();
						next++;
					}
					printf("%s\n",line.data()+cur_pos);
				}
			} catch (std::regex_error& e) {
				std::cerr<<e.what()<<"\n";
			// Syntax error in the regular expression
			}
		}

	private:
		// bool insensitive
		// const char * RED = "\033[31;1m" ;
		// const char * END = "\033[0m" ;
		const char * RED = "" ;
		const char * END = "" ;
		bool n;
		std::regex expr;
		// std::string color_begin;
		// std::string color_end;
	};

/**
 * -i		: Case insensitive
 * -count	: Per file, number of lines matched
 * -n		: Print line number at beginning
 * -l		: Only print file name, if matched
 * --color	: Coloring (Can skip processing if one match found in no coloring)
 * 
 */






// void search_regex(const char *path,const char* pattern, bool n_flag)
// {
// 	std::ifstream input(path);
// 	if(input.is_open()) 
// 	{
		

// 	}
// 	else {
// 		std::cerr<<"grape: Invalid Input file. Unable to open "<<path<<"\n";
// 	}
// }
