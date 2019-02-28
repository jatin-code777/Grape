#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <inttypes.h>
#include "BM.h"
#include <deque>
#include <sstream>
#include <mutex>
#include <unistd.h>


#define ALPHABET_SIZE 256
using namespace std;


auto comp = [](char a, char b) { return a==b ;};//general == character comparision
auto comp_ig = [](char a, char b) { return (a|' ') == (b|' '); };//ignore case comparator
function<bool(char,char)> eq;
int occ[ALPHABET_SIZE],jt[ALPHABET_SIZE];
int n,PAGESIZE = getpagesize()*32;
char* pat;
bool ignore_case = 0;
bool ignore_name = 0;
int state=0;
std::mutex print_mutex;
//n = pattern length
//occ[i] = last occurance index of character with ASCII value i in the pattern.
//occ[i] = -1 --> that characeter doesn't exist in the search pattern 
//jt is jumptable based on bad character heuristic due to mismatch at the last position

vector<int> lps;
vector<int> s,f;
//f[i] = starting point of longest suffix of pat[i....n] that is also its prefix
//f[i] >= pattern length --> such a suffix doesn't exist
//s[i] = shift to be made based on good suffix heuristics, when index i does not match, the rest of the suffix matches
//s[i] is the MINIMUM shift such that the part that is already matched matches, and the mismatched character gets changed


void BM::print_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf, stringstream &ss)
{
	deque<char> out;
	deque<char> lin;
	int blockshift=0;
	int loc = i - 1 - k + PAGESIZE,l=0;
	int64_t left = i-1, right = i + n,k_ = k;
	while(left != -1)
	{ 
		while(loc >= 0)
			if(buf[loc] == '\n')
				break;
			else{
				out.push_front(buf[loc]);
				loc--;
			}

		if(loc == -1)
		{
			blockshift--;
			k -= PAGESIZE;
			left = k - 1;
			loc += PAGESIZE;
			if(k) pread(fd,buf,PAGESIZE,k-PAGESIZE);
		}
		else
		{
			left = k - PAGESIZE + loc;
			break;
		}
	}
	left += 1;

	// if(blockshift<0){
		while(!out.empty()){
			ss<<out.front();//printf("%c",out.front());
			out.pop_front();
		}
	/*} //handle 
	else
		for(loc = left - k + PAGESIZE; loc < i - k + PAGESIZE; loc++)
			printf("%c",buf[loc]);
	//just iterate over block // print from left till pattern point
	*/
	if(isatty(STDOUT_FILENO)==1)ss<<"\033[1;31m"<<pat<<"\033[0m";
	else ss<<pat;//printf("%s",pat);//print the pattern	/*TODO : in red*/
	int red = 0;
	k = k_; loc = right - k + PAGESIZE; right = k - PAGESIZE; l=0; 
	while(right < m)
	{
		while(right + loc < min(m,k))//add here to stop going ahead of end
		{
			if(buf[loc]=='\n') break;
			else{
				while(l!=-1 && buf[loc]!=pat[l]) l = l ? lps[l-1] : -1;
				l++;
				if(l==n){
					red = n;//match(ss);
					l = 0;
				}
				lin.push_back(buf[loc]);
				if(lin.size() == n){
				if(red > 0) {if(isatty(STDOUT_FILENO)==1)ss<<"\033[1;31m"<<lin.front()<<"\033[0m";
	else ss<<lin.front();red--;}//printf("%c",buf[loc]);
				else ss<<lin.front(); lin.pop_front();}
				loc ++;
				
			}
		}

		if(loc == PAGESIZE)
		{
			k += PAGESIZE;
			right = k - PAGESIZE;
			loc = 0;
			if(k<m){
				lseek(fd,k,SEEK_SET);
				read(fd,buf,PAGESIZE);
				// pread(fd,buf,PAGESIZE,right);
			}
		}
		else
		{
			right = k - PAGESIZE + loc;
			break;
		}
	}
	right = min(right,m);
	while(lin.size() > 0){
	if(red > 0) {if(isatty(STDOUT_FILENO)==1)ss<<"\033[1;31m"<<lin.front()<<"\033[0m";
	else ss<<lin.front();red--;}//printf("%c",buf[loc]);
	else ss<<lin.front(); lin.pop_front();}
	ss<<"\n";//printf("\n");
	i = right+1;
}

void BM::build_occ()
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

void BM::case1_good_suffix()
{
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

void BM::case2_good_suffix()
{
	int i=0,j=f[0];
	for(;i<=n;i++)
	{
		if(s[i]==0) s[i] = j;
		if(i==j) j = f[j];
	}
}

void BM::build_lps()
{
	int i = 1 , l = lps[0] = 0;
	while(i<n)
	{
		if(pat[i] == pat[l]) lps[i++] = ++l;
		else l ? l = lps[l-1] : lps[i++] = 0;
	}
}

void BM::pre_process(char* patt, bool ic, bool ig_name, int flags)
{
	state = flags;
	eq = ic?comp_ig:comp;
	ignore_case = ic;
	pat = patt;
	n = strlen(pat);
	ignore_name = ig_name;
	s.resize(n+1); f.resize(n+1); lps.resize(n);
	build_occ();//handle the bad character heuristic
	case1_good_suffix();//Compute f. Compute s when the the already matched part has a copy in the pattern
	case2_good_suffix();//Compute when already matched part only partially exists in the remaining pattern
	build_lps();//compute the longest prefix suffix for prefix substrings of input pattern
}


void BM::skip_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf)
{
	int64_t& right = i;
	int loc = right + n - k + PAGESIZE; right = k - PAGESIZE;
	while(right < m)
	{
		while(right + loc < min(m,k))
		{
			if(buf[loc]=='\n') break;
			loc++;
		}

		if(loc == PAGESIZE)
		{
			k += PAGESIZE;
			right = k - PAGESIZE;
			loc = 0;
			if(k<m){
				lseek(fd,k,SEEK_SET);
				read(fd,buf,PAGESIZE);
			}
		}
		else
		{
			right = k - PAGESIZE + loc;
			break;
		}
	}
	right = min(right,m);
	right++;
}

int BM::BM(int id, const char* path)//standard(0) and count(3) states are here
{
	stringstream ss;
	bool done = 0;
	int fd = open(path,O_RDONLY),jump,j,line_count = 0;
	if(fd == -1) return 1;
	int64_t i=0,k=0,m = lseek(fd,0,SEEK_END);//i = start
	lseek(fd,0,SEEK_SET);
	k += PAGESIZE;
	char buf[PAGESIZE];
	read(fd,buf,PAGESIZE);
	while(!done && i+n <= m)
	{
		while(i + 3*n < min(m,k)){//loop unrolled for speed
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			i += (jump = jt[(int)buf[i - k + PAGESIZE + n - 1]]);
			if(!jump) break;
		}
		
		if(i + n > min(m,k))
		{
			lseek(fd,i,SEEK_SET);
			read(fd,buf,PAGESIZE);
			k = i + PAGESIZE;
			continue;
		}

		for(j = n-1; j>=0 && eq(pat[j],buf[i - k + PAGESIZE + j]); j--);
		
		if(j==-1)
		{
			switch(state)
			{
				case 0:
					if(ignore_name==0) ss<<path;//printf("%s:",path);
						// printf("%" PRId64 ": ",i);
						print_line(fd,i,k,m,buf,ss);
					break;
				
				case 1:
					ss<<path<<"\n";
					done = 1;
					break;

				case 2:
					done = 1;
					break;

				case 3:	
					line_count++;
					skip_line(fd,i,k,m,buf);
					break;

				default:
					ss<<"Invalid flag status\n";
					done = 1;
					break;
			}			
		}
		else
			i += max(s[j+1],j - occ[(int)buf[i - k + PAGESIZE + j]]);
	}

	switch(state)
	{
		case 0:
		case 1:
			break;
		case 2:
			if(!done) ss<<path<<"\n";
			break;
		case 3:
			if(ignore_name) ss<<line_count<<"\n";
			else ss<<path<<":"<<line_count<<"\n";
			break;
		default:
			ss<<"Invalid flag status\n";
	}
	print_mutex.lock();
	printf("%s",ss.str().data());
	print_mutex.unlock();
	close(fd);
	return 0;
}


int BM::BM_N(int id, const char* path)
{
	stringstream ss;
	int fd = open(path,O_RDONLY),jump,j;
	if(fd == -1) return 1;
	int64_t i=0,k=0,m = lseek(fd,0,SEEK_END),line_no = 1,ch;//i = start
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

		for(j = n-1; j>=0 && eq(pat[j],buf[ i - k + PAGESIZE + j ]); j--);

		if(j==-1)
		{
			if(ignore_name==0) ss<<path;//printf("%s:",path);
			ss<<line_no<<":";//printf("%" PRId64 ":",line_no);
			print_line(fd,i,k,m,buf,ss);	
			line_no++;
		}
		else
		{
			jump = max(s[j+1],j - occ[(int)buf[ i - k + PAGESIZE + j ]]);
			for(ch = 0; ch<jump; ch++)	if(buf[ i - k + PAGESIZE + ch ]=='\n') line_no++;
			i += jump;
		}
	}
	print_mutex.lock();
	printf("%s",ss.str().data());
	print_mutex.unlock();
	close(fd);
	return 0;
}