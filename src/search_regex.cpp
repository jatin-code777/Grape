#include "search_regex.h"
		
void regex_search::pre_process(char* pattern, bool ignore_case, bool single_file , int flags)
{
	using namespace std::regex_constants;
	auto flag_mask = nosubs | optimize | basic ; //Specify grammar
	if(ignore_case) flag_mask |= icase;
	try {
		expr.assign(pattern, flag_mask);
	} catch (std::regex_error& e) {
		std::cerr << "regex_error : " << e.what() << '\n';
		throw e;	// What to do now ??
	}

	print_file_name		= !single_file;
	print_line_num		= (flags == 4);
	print_matched_file	= (flags == 1);

	if(flags == 0 or flags == 4) strategy = [this](std::ifstream& input,std::string s) {this->normal(input,s);};
	else if(flags == 3) strategy = [this](std::ifstream& input,std::string s) {this->line_count_only(input,s);};
	else strategy = [this](std::ifstream& input,std::string s) {this->file_name_only(input,s);};

}

void regex_search::search(int id, std::string fpath) {
	std::ifstream input(fpath); //RAII acquire file
	if(not input.is_open()) {
		std::lock_guard<std::mutex> lock(print_mutex);
		fprintf(stderr,"grape: Unable to open: %s\n",fpath.data());
		return;
	}
	strategy(input,fpath);
	input.close();
}

void regex_search::file_name_only(std::ifstream& input, const std::string &path) {
	std::string line;
	while(std::getline(input,line)) {
		if(std::regex_search(line,expr)) {
			if(print_matched_file) {
				std::lock_guard<std::mutex> lock(print_mutex);
				std::cout<<COLOR_PURPLE<<path<<COLOR_RESET;
			}
			return;
		}
	}
	if(!print_matched_file) {
		std::lock_guard<std::mutex> lock(print_mutex);
		std::cout<<COLOR_PURPLE<<path<<COLOR_RESET;
	}
}

void regex_search::line_count_only(std::ifstream& input, const std::string &path) {

	int lines_matched = 0;
	std::string line;
	while(std::getline(input,line)) {
		if(std::regex_search(line,expr))
			++lines_matched;
	}
	std::lock_guard<std::mutex> lock(print_mutex);
	if(print_file_name) std::cout<<COLOR_PURPLE<<path<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;
	printf("%d\n",lines_matched);
}

void regex_search::normal(std::ifstream& input, const std::string &path)
{
	int line_num = 1;
	std::string line;
	std::stringstream ss;
	while(std::getline(input,line)) {
		output_matches(line,line_num,path,ss);
		++line_num;
	}
	std::lock_guard<std::mutex> lock(print_mutex);
	printf("%s",ss.str().data());
}

void regex_search::output_matches(std::string &line,int line_num, const std::string &path, std::stringstream& ss)
{
	try {
		std::sregex_iterator next(line.begin(), line.end(), expr);
		std::sregex_iterator end;
		if(next!=end)
		{
			size_t cur_pos = 0;

			if(print_file_name) ss<<COLOR_PURPLE<<path.data()<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;//printf(COLOR_PURPLE "%s:" COLOR_RESET, path.data());
			if(print_line_num)  ss<<COLOR_RED<<line_num<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;//printf(COLOR_RED	"%d:" COLOR_RESET, line_num);

			while (next != end) {
				std::smatch match = *next;
				ss.rdbuf()->sputn(line.data()+cur_pos,int(match.position()-cur_pos));
				ss<<COLOR_RED_BOLD<<match.str()<<COLOR_RESET;
				cur_pos = match.position()+match.length();
				next++;
			}
			ss<<line.data()+cur_pos<<"\n";
		}
	} catch (std::regex_error& e) {
		std::cerr<<e.what()<<"\n";
	}
}
