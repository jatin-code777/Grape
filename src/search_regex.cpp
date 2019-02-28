#include <iostream>
#include <fstream>
#include <regex>
#include <string>


	class regex_search
	{
	public:
		regex_search(const char* pattern, bool ignore_case, bool color)
		{
			using namespace std::regex_constants;
			auto flag_mask = nosubs | optimize | basic;
			if(ignore_case) flag_mask |= icase;
			try {
				exp.assign(pattern, flag_mask);
			}
			catch (std::regex_error& e) {
				std::cerr << "regex_error :" << e.what() << '\n';
				// What to do now ??
			}
		}

		void search(const char* path)
		{
			std::ifstream input(path);
			if(!input.is_open()) {
				std::cerr << "grape: Invalid input file. Unable to open " << path << '\n' ;
				return;
			}
			try 
			{



			}
			catch(std::regex_error e) {

			}
		}

	private:
		// bool insensitive
		std::regex exp;
	}






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


/**
 * -i		: Case insensitive
 * -count	: Per file, number of lines matched
 * -n		: Print line number at beginning
 * -l		: Only print file name, if matched
 * --color	: Coloring (Can skip processing if one match found in no coloring)
 * 
 */