//#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdint.h>
char * pat;
static int display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    { 
        //typeflag indicates  normal file
        //printf("normal file %c\n",pat[0]);
        printf("BM called at %s with pattern %s\n",fpath, pat);

    }
    else if (typeflag==FTW_D); //typeflag indicates directory
            
    return 0;                  // To tell nftw() to continue 
}

int main(int argc, char *argv[])
{
    //input should be ./a.out pattern filename/foldername/None(current dir)
   int flags = 0;
   if(argc==1) return 0;
   if(argc>=2) pat = argv[1];
    if(nftw((argc <= 2) ? "." : argv[2], display_info, 20, flags)== -1) {
    	perror("nftw");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

