#ifndef _SearchStrategy_H_
#define _SearchStrategy_H_

class SearchStrategy
{
	public:
		virtual void pre_process(char* pattern, bool ignore_case, bool single_file, int flags) = 0;
		virtual void search(int id, std::string path) = 0;
};

#endif