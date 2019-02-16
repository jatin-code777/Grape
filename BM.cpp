#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include<ctime>
#define ALPHABET_SIZE 256
using namespace std;

int occ[ALPHABET_SIZE],jt[ALPHABET_SIZE];
int n,PAGESIZE = getpagesize();
//n =  pattern length
//occ[i] = last occurance index of character with ASCII value i in the pattern.
//occ[i] = -1 --> that characeter doesn't exist in the search pattern 
//jt is jumptable based on bad character heuristic due to mismatch at the last position

vector<int> s,f;
//f[i] = starting point of longest suffix of pat[i....n] that is also its prefix
//f[i] >= pattern length --> such a suffix doesn't exist
//s[i] = shift to be made based on good suffix heuristics, when index
//s[i] is the MINIMUM shift such that the part that is already matched matches, and the mismatched character gets changed

void build_occ(string& pat)
{
	n = pat.length();
	memset(occ,-1,sizeof(occ));
	for(int i=0;i<n;i++)
		occ[(int)pat[i]] = i;
	for(int i=0;i<ALPHABET_SIZE;i++)
		jt[i]=n-1-occ[i];
}

void case1_good_suffix(string& pat)
{
	int i = n + 1, j = n + 2;
	while(i>0)
	{
		while(j<=n && pat[i-1] != pat[j-1])
		{
			if(s[j]==0) s[j] = j-i;//since pat[i-1] != pat[j-1], the mismatched character changes
			// s[j]==0 checks that s[j] stores the MINIMUM required shift
			j = f[j];
		}
		f[--i] = --j;
	}
}

void case2_good_suffix(string& pat)
{
	int i=0,j=f[0];
	for(;i<=n;i++)
	{
		if(s[i]==0) s[i] = j;
		if(i==j) j = f[j];
	}
}

void pre_process(string& pat)
{
	build_occ(pat);//handle the bad character heuristic
	case1_good_suffix(pat);//Compute f. Compute s when the the already matched part has a copy in the pattern
	case2_good_suffix(pat);//Compute when already matched part only partially exists in the remaining pattern
}

void bm(string& pat,int fd)
{
	int i=0,j,jump,k=0,m = lseek(fd,0,SEEK_END);//i = start
	lseek(fd,0,SEEK_SET);
	k += PAGESIZE;
	char buf[PAGESIZE];
	read(fd,buf,PAGESIZE);
	while(i+n <= m)
	{
		while(i + 3*n < k){//loop unrolled for speed
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			if(!jump) break;
		}
		if(i+n >= k)
		{
			lseek(fd,i,SEEK_SET);
			read(fd,buf,PAGESIZE);
			k = i + PAGESIZE;
			continue;
		}
		for(j = n-1; j>=0 && pat[j] == buf[PAGESIZE - k  + i + j]; j--);
		if(j==-1)
		{
			cout<<"Match at "<<i<<"\n";
			i+=s[0];
		}
		else
			i += max(s[j+1],j - occ[(int)buf[PAGESIZE -k + i + j]]);
	}
}

int main(int argc,char* argv[])
{
	int n; 
	string pat = argv[1];
	n = pat.length();
	s.resize(n+1); f.resize(n+1);
	pre_process(pat);
	int fd = open(argv[2],O_RDONLY);
	bm(pat,fd);	
	return 0;		
}