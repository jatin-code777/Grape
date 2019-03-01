#include <ftw.h>
#include <iostream>
#include "argparser.h"
#include "call.h"
#include "BM.h"
#include "threadpool.h"
#include <string>
#include <string.h>
#include <unordered_set> 
#include "search_regex.cpp"
#include "SearchStrategy.h"

char * Pattern;
thread_manager::ThreadPool * tpool;
SearchStrategy * searcher;

std::unordered_set <std::string> extset{".txt",".cpp", ".c", ".txt",
										".h", ".csv",".py",".md",".hpp", ".yml" }; 



void decider(bool regex, SearchStrategy* & searcher)
{
	if(regex) searcher = new regex_search;
	else searcher = new BoyerMooreSearch;
}

void search(int id, SearchStrategy * searcher , std::string path) {
	searcher->search(id,path);
}

int call::push_to_threadpool(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
		if (typeflag == FTW_F && is_file(fpath)) //typeflag indicates  normal file
		{     
				std::string s = fpath;
				tpool->push(search ,searcher, s);
		}
		else if (typeflag==FTW_D); //typeflag indicates directory
		return 0;                  // To tell nftw() to continue 
}

bool call::is_file(const char* path) 
{
	struct stat buf;
	stat(path, &buf);
	int len = strlen(path);
	if( S_ISREG(buf.st_mode) == 0 ) return 0;
	else{
		int i;
		for( i=len-1 ; i>=0 ; i--) if( path[i] == '.' ||  i == len - 5) break;
		if( i < 0 || path[i] !='.') return 0;
		else {
			std::string ext(path + i);
			return (extset.find(ext) != extset.end() ) ; 
		}
	}
}

bool isvalid(const char * path)
{
	struct stat buf;
	int t = stat(path,&buf);
	if (t==0) return 1;
	else return 0;
}

bool call::is_dir(const char* path) 
{
		struct stat buf;
		stat(path, &buf);
		return S_ISDIR(buf.st_mode);
}



int call::call_file_tree_walk(struct parser::output ret)
{
	Pattern = ret.PATTERN;
	char* path = ret.PATH;
	int flags = 0;
	if(nftw(path, push_to_threadpool, 20, flags)== -1) 
	{
		perror("grape");
		return 1;
	}
	return 0;
}


int call::go(struct parser::output ret)
{
	int return_value = 0;
	if (ret.return_value != 0) return ret.return_value;
	if(!isvalid(ret.PATH)) {
		printf("grape: %s: No such file or directory\n",ret.PATH);
		return 1;
	}
	if(is_dir(ret.PATH) && ret.r_flag==0){
			printf("grape: %s: Is a directory\n",ret.PATH);
			return 1;
	}
	else
	{
		std::string p = ret.PATTERN;
		decider(ret.G_flag | (p == "") ,searcher);
		Pattern = ret.PATTERN;
		char* path = ret.PATH;
		bool isfile = is_file(ret.PATH);
		int flags = 0;
		if(ret.n_flag == 1) flags = 4;
		if(ret.c_flag == 1) flags = 3;
		if (ret.l !=-1) flags = ret.l +1;
		searcher->pre_process(Pattern, ret.i_flag ,isfile, flags);
		int num_threads = 8;
		if(isfile){
			search(0,searcher,ret.PATH);
		}
		else if(is_dir(ret.PATH) && ret.r_flag==1){
			tpool = thread_manager::ThreadPool::get_instance(num_threads);
			return_value = call_file_tree_walk(ret);
		}
	}  
	


	return return_value;

}

