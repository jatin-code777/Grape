#include <ftw.h>
#include <iostream>
#include "argparser.h"
#include "call.h"
#include "BM.h"
#include "threadpool.h"
#include <string>

char * paat;
bool nflag_ftw = 0;
thread_manager::ThreadPool * tp;

int fBM(int id, std::string path)   {return BM::BM(id,path.data());};
int fBM_N(int id, std::string path) {return BM::BM_N(id,path.data());};

int call::push_to_threadpool(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F) //typeflag indicates  normal file
    {   // if(nflag_ftw) BM::BM_N(fpath);  else BM::BM(fpath);
        std::string s = fpath;
        tp->push( (nflag_ftw ? fBM_N : fBM ) , s); // push fpath to threadpool queue 
    }
    else if (typeflag==FTW_D); //typeflag indicates directory
    return 0;                  // To tell nftw() to continue 
}

bool call::is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

// bool call::is_file(const char* path) {
//     struct stat buf;
//     stat(path, &buf);
//     if S_ISREG(buf.st_mode)==0 return 0;
//     else
//     {
      
//     }
    
// }
bool call::is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}



int call::call_file_tree_walk(struct parser::output ret)
{
  paat = ret.PATTERN;
  char* path = ret.PATH;
  bool isfile = is_file(ret.PATH);
  int flags = 0;
  BM::pre_process(paat, ret.i_flag ,isfile);
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
  
  if(is_dir(ret.PATH) && ret.r_flag==0){
      printf("grape: %s: Is a directory\n",ret.PATH);
      return 1;
  }
  else
  {
    paat = ret.PATTERN;
    char* path = ret.PATH;
    bool isfile = is_file(ret.PATH);
    int flags = 0;
    BM::pre_process(paat, ret.i_flag ,isfile);
    int num_threads = 8;
    nflag_ftw = ret.n_flag;
    if(is_file(ret.PATH)){
      if(nflag_ftw) BM::BM_N(0,ret.PATH);
      else BM::BM(0,ret.PATH);
    }
    else if(is_dir(ret.PATH) && ret.r_flag==1){
      tp = thread_manager::ThreadPool::get_instance(num_threads);
      return_value = call_file_tree_walk(ret);
    }
  }  
  
  delete tp;
  return return_value;

}

