#include <ftw.h>
#include <iostream>
#include "argparser.h"
#include "call.h"
#include "BM.h"
char * paat;
bool nflag_ftw = 0;
int call::display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    { 
        //typeflag indicates  normal file
        //printf("normal file %c\n",pat[0]);
        printf("BM called at %s with pattern %s\n",fpath, paat);
        if(nflag_ftw) BM::BM_N(fpath);
        else BM::BM(fpath);
        //push fpath to threadpool queue 
    }
    else if (typeflag==FTW_D); //typeflag indicates directory
    return 0;                  // To tell nftw() to continue 
}

bool call::is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool call::is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
int call::call(struct parser::output ret)
{
  paat = ret.PATTERN;
  char* path = ret.PATH;
  nflag_ftw = ret.n_flag;
  int flags = 0;
  BM::pre_process(paat,ret.i_flag);
  if(nftw(path, display_info, 20, flags)== -1) 
  {
    perror("grape");
		return 1;
	}
  return 0;
}

int call::go(struct parser::output ret)
{
  if (ret.return_value != 0) return ret.return_value;
  
  if(is_dir(ret.PATH) && ret.r_flag==0)
  {
      printf("grape: %s: Is a directory\n",ret.PATH);
      return 1;
  }
  else if(is_file(ret.PATH))
  {
    // call appropriate function
  }
  // create threadpool here with appropriate 
  return call(ret);

}

