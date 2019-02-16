#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define ALPHABET_SIZE 256
using namespace std;
int occ[ALPHABET_SIZE];
int n;
//n =  pattern length
//occ[i] = last occurance index of character with ASCII value i in the pattern.
//occ[i] = -1 --> that characeter doesn't exist in the search pattern 
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
	int i=0,j,m = lseek(fd,0,SEEK_END);//i = start
	lseek(fd,0,SEEK_SET);
	while(i+n <= m)
	{
		lseek(fd,i,SEEK_SET);
		char buf[n];
		read(fd,buf,n);
		for(j = n-1; j>=0 && pat[j] == buf[j]; j--);
		if(j==-1)
		{
			cout<<"Match at "<<i<<"\n";
			i+=s[0];
		}
		else
			i += max(s[j+1],j - occ[(int)buf[j]]);
	}
}

int main()
{
	int n; 
	string pat,path;
	cin>>pat>>path;
	n = pat.length();
	s.resize(n+1); f.resize(n+1);
	pre_process(pat);
	int fd = open(path.c_str(),O_RDONLY);
	bm(pat,fd);	
	return 0;		
}