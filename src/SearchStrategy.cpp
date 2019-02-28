// class SearchStrategy
// {
// 	public:
// 		virtual void pre_process(char* patt, bool ic = 0, bool ig_name = 0, int flags = 0) = 0;
// 		virtual void search(int id, const char* path) = 0;
// };

// SearchStrategy* searcher;

// void decider(bool regex, SearchStrategy* searcher)
// {
// 	if(regex) searcher = new RegexSearch;
// 	else searcher = new BoyerMooreSearch;
// }

// void search(int id,SearchStrategy* searcher,char* path)
// {
// 	searcher->search(id,path);
// }