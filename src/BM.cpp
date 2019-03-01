#include "BM.h"
using namespace std;

void BoyerMooreSearch::error_handler(int status,int fd,std::string message)
{
	if(status == -1)
	{
		fprintf(stderr,"%s\n",message.data());
		if(fd != -1) close(fd);
		throw 1;
	}
}

int BoyerMooreSearch::char_to_int(char c)
{
	return ((int)c)<0 ? 256 + ((int)c) : ((int)c);
}

void BoyerMooreSearch::print_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf, stringstream &ss)
{
	deque<char> out;
	deque<char> lin;
	int blockshift = 0;
	int loc = i - 1 - k + PAGESIZE,l = 0;
	int64_t left = i - 1, right = i + n, k_ = k;
	while(left != -1)
	{ 
		while(loc >= 0){
			if(buf[loc] == 	'\n')
				break;
			else
				out.push_front(buf[loc--]);
		}

		if(loc == -1)
		{
			blockshift--;
			k -= PAGESIZE;
			left = k - 1;
			loc += PAGESIZE;
			if(k)
				error_handler(pread(fd,buf,PAGESIZE,k-PAGESIZE),fd,"File Read error");
		}
		else
		{
			left = k - PAGESIZE + loc;
			break;
		}
	}
	left += 1;

	while(!out.empty()){
		ss<<out.front();
		out.pop_front();
	}

	//blockshift<0 reload original buffer
	if(blockshift < 0)
		error_handler(pread(fd,buf,PAGESIZE,k_-PAGESIZE),fd,"File Read error");
	
	
	ss<<COLOR_RED_BOLD<<pat<<COLOR_RESET;
	int red = 0;

	//go right and apply KMP
	k = k_; loc = right - k + PAGESIZE; right = k - PAGESIZE; l = 0;
	while(right < m)
	{
		while(right + loc < min(m,k))
		{
			if(buf[loc] == '\n') break;
			else{
				while(l!=-1 && buf[loc] != pat[l]) l = l ? lps[l-1] : -1;
				l++;
				if(l==n){
					red = n;
					l = 0;
				}
				lin.push_back(buf[loc]);
				if(lin.size() == n)
				{
					if(red > 0)
					{
						ss<<COLOR_RED_BOLD<<lin.front()<<COLOR_RESET;
						red--;
					}
					else ss<<lin.front();
					lin.pop_front();
				}
				loc ++;				
			}
		}

		if(loc == PAGESIZE)
		{
			if(k<m){
				error_handler(lseek(fd,k,SEEK_SET),fd,"File Read error");
				error_handler(read(fd,buf,PAGESIZE),fd,"File Read error");
			}
			k += PAGESIZE;
			right = k - PAGESIZE;
			loc = 0;
		}
		else
		{
			right = k - PAGESIZE + loc;
			break;
		}
	}

	right = min(right,m);
	while(lin.size() > 0){
		if(red > 0)
		{
			ss<<COLOR_RED_BOLD<<lin.front()<<COLOR_RESET;
			red--;
		}
		else ss<<lin.front();
		lin.pop_front();
	}
	ss<<"\n";

	i = right + 1;
}

void BoyerMooreSearch::build_occ()
{
	n = strlen(pat);
	memset(occ,-1,sizeof(occ));
	if(ignore_case)
		for(int i=0;i<n;i++)
			occ[char_to_int(tolower(pat[i]))] = occ[char_to_int(toupper(pat[i]))] = i;
	else
		for(int i=0;i<n;i++)
			occ[char_to_int(pat[i])] = i;

	for(int i=0;i<ALPHABET_SIZE;i++)
		jt[i] = n - 1 - occ[i];
}

void BoyerMooreSearch::case1_good_suffix()
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

void BoyerMooreSearch::case2_good_suffix()
{
	int i=0,j=f[0];
	for(;i<=n;i++)
	{
		if(s[i]==0) s[i] = j;
		if(i==j) j = f[j];
	}
}

void BoyerMooreSearch::build_lps()
{
	int i = 1 , l = lps[0] = 0;
	while(i<n)
	{
		if(pat[i] == pat[l]) lps[i++] = ++l;
		else l ? l = lps[l-1] : lps[i++] = 0;
	}
}


void BoyerMooreSearch::pre_process(char* patt, bool ic, bool ig_name, int flags)
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


void BoyerMooreSearch::skip_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf)
{
	int64_t& right = i;
	int loc = right + n - k + PAGESIZE; right = k - PAGESIZE;
	while(right < m)
	{
		while( right + loc < min(m,k) )
		{
			if(buf[loc] == '\n') break;
			loc++;
		}

		if(loc == PAGESIZE)
		{
			if(k<m){
				error_handler(lseek(fd,k,SEEK_SET),fd,"File Read error");
				error_handler(read(fd,buf,PAGESIZE),fd,"File Read error");
			}
			k += PAGESIZE;
			right = k - PAGESIZE;
			loc = 0;
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


int BoyerMooreSearch::BM(int id, const char* path)//states (0-3) are here
{
	stringstream ss;
	bool done = 0;
	
	int fd = open(path,O_RDONLY), jump, j, line_count = 0;
	error_handler(fd,fd,"File open error");
	
	int64_t i = 0, k = 0,m = lseek(fd,0,SEEK_END);//i = start
	error_handler(m,fd,"File read error");

	error_handler(lseek(fd,0,SEEK_SET),fd,"File read error");

	k = PAGESIZE;
	char buf[PAGESIZE];

	error_handler(read(fd,buf,PAGESIZE),fd,"File read error");

	while(!done && i+n <= m)
	{
		while(i + 3*n < min(m,k)){//loop unrolled for speed
			i += (jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])]);
			i += (jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])]);
			i += (jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])]);
			if(!jump) break;
		}
		
		if(i + n > min(m,k))
		{
			error_handler(lseek(fd,i,SEEK_SET),fd,"File read error");
			error_handler(read(fd,buf,PAGESIZE),fd,"File read error");
			k = i + PAGESIZE;
			continue;
		}

		for(j = n-1; j>=0 && eq(pat[j],buf[i - k + PAGESIZE + j]); j--);
		
		if(j==-1)
		{
			switch(state)
			{
				case 0:
					if(ignore_name == 0)
						ss<<COLOR_PURPLE<<path<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;
					print_line(fd,i,k,m,buf,ss);
					break;
				
				case 1:
					ss<<COLOR_PURPLE<<path<<COLOR_RESET<<"\n";
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
			i += max(s[j+1],j - occ[char_to_int(buf[i - k + PAGESIZE + j])]);
	}

	switch(state)
	{
		case 0:
		case 1:
			break;
		case 2:
			if(!done) ss<<COLOR_PURPLE<<path<<COLOR_RESET<<"\n";
			break;
		case 3:
			if(ignore_name) ss<<line_count<<"\n";
			else ss<<COLOR_PURPLE<<path<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET<<line_count<<"\n";
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


int BoyerMooreSearch::BM_N(int id, const char* path)
{
	stringstream ss;
	
	int fd = open(path,O_RDONLY), jump, j;
	error_handler(fd,fd,"File open error");
	
	int64_t i = 0,k = 0,m = lseek(fd,0,SEEK_END),line_no = 1,ch;//i = start
	error_handler(lseek(fd,0,SEEK_SET),fd,"File read error");
	
	k = PAGESIZE;
	char buf[PAGESIZE];
	
	error_handler(read(fd,buf,PAGESIZE),fd,"File read error");

	while(i + n <= m)
	{
		while(i + 3*n < min(m,k)){//loop unrolled for speed
			jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;

			jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;

			jump = jt[char_to_int(buf[i - k + PAGESIZE + n - 1])];
			for(ch = 0; ch<jump; ch++) if( buf[ i - k + PAGESIZE + ch ]=='\n' ) line_no++;
			i += jump;
			if(!jump) break;
		}

		if(i + n > min(m,k))
		{
			error_handler(lseek(fd,i,SEEK_SET),fd,"File read error");
			error_handler(read(fd,buf,PAGESIZE),fd,"File read error");
			k = i + PAGESIZE;
			continue;
		}

		for(j = n-1; j>=0 && eq(pat[j],buf[ i - k + PAGESIZE + j ]); j--);

		if(j==-1)
		{
			if(ignore_name==0) ss<<COLOR_PURPLE<<path<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;
			ss<<COLOR_RED<<line_no<<COLOR_RESET<<COLOR_CYAN<<":"<<COLOR_RESET;
			print_line(fd,i,k,m,buf,ss);
			line_no++;
		}
		else
		{
			jump = max(s[j+1],j - occ[char_to_int(buf[ i - k + PAGESIZE + j ])]);
			for(ch = 0; ch<jump; ch++) if(buf[ i - k + PAGESIZE + ch ]=='\n') line_no++;
			i += jump;
		}
	}
	print_mutex.lock();
	printf("%s",ss.str().data());
	print_mutex.unlock();
	close(fd);
	return 0;
}

void BoyerMooreSearch::search(int id,std::string path)
{
	try{
		if(state == 4) BM_N(id,path.data());
		else BM(id,path.data());
	}
	catch(int){
		fprintf(stderr,"Stopped searching file : %s because of error in system calls\n",path.data());
	}
}
