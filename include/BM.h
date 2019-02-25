#ifndef BM_H
#define BM_H

namespace BM {
  
void build_occ(char* pat);
void case1_good_suffix(char* pat);
void case2_good_suffix(char* pat);
void pre_process(char* patt, bool ic = 0, bool ig_name=0);
int BM(int id, const char* path);
int BM_N(int id ,const char* path);

}
#endif