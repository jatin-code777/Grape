#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "BM.h"
#define ALPHABET_SIZE 256
using namespace std;


auto comp = [](char a, char b) ->bool{ return a==b ;};//general == character comparision
auto comp_ig = [](char a, char b) ->bool{ return (a|' ') == (b|' ') ; };//ignore case comparator
int occ[ALPHABET_SIZE],jt[ALPHABET_SIZE];
int n,PAGESIZE = getpagesize();
char* pat;
bool ignore_case = 0;
//n =  pattern length
//occ[i] = last occurance index of character with ASCII value i in the pattern.
//occ[i] = -1 --> that characeter doesn't exist in the search pattern 
//jt is jumptable based on bad character heuristic due to mismatch at the last position

vector<int> s,f;
//f[i] = starting point of longest suffix of pat[i....n] that is also its prefix
//f[i] >= pattern length --> such a suffix doesn't exist
//s[i] = shift to be made based on good suffix heuristics, when index
//s[i] is the MINIMUM shift such that the part that is already matched matches, and the mismatched character gets changed

void BM::build_occ(char* pat)
{
	n = strlen(pat);
	memset(occ,-1,sizeof(occ));
	if(ignore_case)
		for(int i=0;i<n;i++)
			occ[(int)tolower(pat[i])] = occ[(int)toupper(pat[i])] = i;
	else
		for(int i=0;i<n;i++)
			occ[(int)pat[i]] = i;

	for(int i=0;i<ALPHABET_SIZE;i++)
		jt[i] = n - 1 - occ[i];
}

void BM::case1_good_suffix(char* pat)
{
	auto eq = ignore_case?comp_ig:comp;
	int i = n + 1, j = n + 2;
	while(i>0)
	{
		while(j<=n && !eq(pat[i-1],pat[j-1]))
		{
			if(s[j]==0) s[j] = j - i;//since pat[i-1] != pat[j-1], the mismatched character changes
			// s[j]==0 checks that s[j] stores the MINIMUM required shift
			j = f[j];
		}
		f[--i] = --j;
	}
}

void BM::case2_good_suffix(char* pat)
{
	int i=0,j=f[0];
	for(;i<=n;i++)
	{
		if(s[i]==0) s[i] = j;
		if(i==j) j = f[j];
	}
}

void BM::pre_process(char* patt, bool ic)
{
	ignore_case = ic;
	pat = patt;
	n = strlen(pat);
	s.resize(n+1); f.resize(n+1);
	build_occ(pat);//handle the bad character heuristic
	case1_good_suffix(pat);//Compute f. Compute s when the the already matched part has a copy in the pattern
	case2_good_suffix(pat);//Compute when already matched part only partially exists in the remaining pattern
}

int BM::BM(const char* path)
{
	auto eq = ignore_case?comp_ig:comp;
	int fd = open(path,O_RDONLY);
	if(fd == -1) return 1;
	int i=0,j,jump,k=0,m = lseek(fd,0,SEEK_END);//i = start
	lseek(fd,0,SEEK_SET);
	k += PAGESIZE;
	char buf[PAGESIZE];
	read(fd,buf,PAGESIZE);
	while(i+n <= m)
	{
		while(i + 3*n < min(m,k)){//loop unrolled for speed
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			if(!jump) break;
		}
		if(i+n > min(m,k))
		{
			lseek(fd,i,SEEK_SET);
			read(fd,buf,PAGESIZE);
			k = i + PAGESIZE;
			continue;
		}

		for(j = n-1; j>=0 && eq(pat[j],buf[i - k + PAGESIZE + j]); j--);

		if(j==-1)
		{
			printf("%s:%d \n",path,i);
			i += s[0];
		}
		else
			i += max(s[j+1],j - occ[(int)buf[i - k + PAGESIZE + j]]);
	}
	close(fd);
	return 0;
}

int BM::BM_N(const char* path)
{
	auto eq = ignore_case?comp_ig:comp;
	int fd = open(path,O_RDONLY);
	if(fd == -1) return 1;
	int i=0,j,jump,k=0,m = lseek(fd,0,SEEK_END),line_no = 1,ch;//i = start
	lseek(fd,0,SEEK_SET);
	k += PAGESIZE;
	char buf[PAGESIZE];
	read(fd,buf,PAGESIZE);
	while(i+n <= m)
	{
		while(i + 3*n < min(m,k)){//loop unrolled for speed
			jump = jt[(int)buf[i - k + PAGESIZE + n - 1]];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;

			jump = jt[(int)buf[i - k + PAGESIZE + n - 1]];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;

			jump = jt[(int)buf[i - k + PAGESIZE + n - 1]];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;
			if(!jump) break;
		}
		if(i+n > min(m,k))
		{
			lseek(fd,i,SEEK_SET);
			read(fd,buf,PAGESIZE);
			k = i + PAGESIZE;
			continue;
		}
		for(j = n-1; j>=0 && eq(pat[j],buf[PAGESIZE - k  + i + j]); j--);
		if(j==-1)
		{
			printf("%s:%d:\n",path,line_no);
			i += s[0];
		}
		else{
			jump = max(s[j+1],j - occ[(int)buf[PAGESIZE - k + i + j]]);
			for(ch = 0; ch<jump; ch++)	if(buf[i - k + PAGESIZE + ch]=='\n') line_no++;
			i += jump;
		}
	}
	close(fd);
	return 0;
}

// int main()
// {
// 	char pat[20],path[50];
// 	bool inv;
// 	scanf("%s %s",pat,path);
// 	cin>>inv;
// 	BM::pre_process(pat,inv);
// 	BM::BM_N(path);
// }