#pragma once

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <functional>

	class regex_search
	{
	public:
/**
 * 0 : normal
 * 1 : print file name if it contains a match
 * 2 : print file names that don't contain a match
 * 3 : print only line count for files that match.
 * 4 : print line numbers for each match
 * 
 */
// bool ic, bool ig_name, int flags
		regex_search(const char* pattern, bool ignore_case, bool single_file , int flags)
		{
			using namespace std::regex_constants;
			auto flag_mask = nosubs | optimize ; //Specify grammar
			if(ignore_case) flag_mask |= icase;
			try {
				expr.assign(pattern, flag_mask);
			} catch (std::regex_error& e) {
				std::cerr << "regex_error : " << e.what() << '\n';
				throw e;	// What to do now ??
			}

			print_file_name = !single_file;
			if(flags == 4) n = true;
			else if(flags == 1 or flags == 2) {
				print_matched_file = (flags == 1);
			}

			// auto strategy = flags == 0 or flags == 4 ? 	this->normal	 	 :
			// 				flags == 1 or flags == 2 ? 	this->file_name_only :
			// 											this->line_count_only;

			// auto f = [this](std::ifstream& input) {
			// 	this->file_name_only(input);
			// }

			// strategy = (flags == 0 or flags == 4 ? 	 [this](std::ifstream& input) {this->normal(input);} :
			// 			flags == 1 or flags == 2 ? 	this->file_name_only :
			// 								this->line_count_only) (input);
			// }

			if(flags == 0 or flags == 4) strategy = [this](std::ifstream& input) {this->normal(input);};
			else if(flags == 3) strategy = [this](std::ifstream& input) {this->line_count_only(input);};
			else strategy = [this](std::ifstream& input) {this->file_name_only(input);};
			// func = [this](int id, const char * fpath)->int {

			// 	std::ifstream input(path); //RAII acquire file
			// 	if(!input.is_open()) {
			// 		perror("grape: Unable to open: %s\n");
			// 	}
			// 	// switch(flags)

			// 	input.clear();
			// 	// return 1;
			// };

		}

		void initiate(std::string fpath) {
			path = std::move(fpath);
			std::ifstream input(path); //RAII acquire file
			if(!input.is_open()) {
				perror("grape: Unable to open: %s\n");
			}
			strategy(input);
			input.close();
		}

		// bool match_exists(const std::string& line) {
		// 	try {

		// 	}
		// }
		void file_name_only(std::ifstream& input) {
			std::string line;
			while(std::getline(input,line)) {
				if(std::regex_match(line,expr)) {
					if(print_matched_file) printf("%s\n",path.data());
					return;
				}
			}
			if(!print_matched_file) printf("%s\n",path.data());
		}

		void line_count_only(std::ifstream& input) {

			int lines_matched = 0;
			std::string line;
			while(std::getline(input,line)) {
				if(std::regex_match(line,expr))
					++lines_matched;
			}
			printf("Lines Matched : %d\n",lines_matched );
		}

		// void matched_lines(std::ifstream& input) {

		// 	int lines_matched = 0 , line_num = 0;

		// 	std::string line;
		// 	while(std::getline(input,line)) {
		// 		++line_num;
		// 		if(std::regex_match(line,expr)) {
		// 			++lines_matched;
		// 		}
		// 	}
		// 	printf("Lines Matched : %d\n",lines_matched );
		// }

		void normal(std::ifstream& input)
		{
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
		bool n , print_file_name, print_matched_file;
		std::regex expr;
		std::function <void(std::ifstream&)> strategy;
		std::string path;
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
