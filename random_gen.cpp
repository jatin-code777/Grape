#include <bits/stdc++.h>
#define PAT 3
#define TXT 100000
using namespace std;
int main()
{
	int i;
	srand(time(NULL));
	for(i=0;i<PAT;i++)
		cout<<(char)((rand()%26)+((rand()%3)?'a':'A'));
	cout<<endl;
	for(i=0;i<TXT;i++){
		cout<<(char)((rand()%26)+((rand()%3)?'a':'A'));
		if((i+1)%50==0) cout<<"\n";
	}
	cout<<endl;
}
