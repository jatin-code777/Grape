#ifndef CALL_H
#define CALL_H
#include "argparser.h"
#include <ftw.h>
#include "SearchStrategy.h"
#include <string>
/**
 * @brief      decider funtion
 *
 * @param[in]  regex     The regular expression
 * @param      searcher  The searcher
 */
void decider(bool regex, SearchStrategy* & searcher);



/**
 * @brief      search funtion for searchStrategy
 *
 * @param[in]  id        The thread_id
 * @param      searcher  The searcher
 * @param[in]  path      The path
 */
void search(int id, SearchStrategy * searcher , std::string path);


namespace call
{
	/**
	 * @brief      adds filepath to threadpool queue
	 *
	 * @param[in]  fpath     The filepath
	 * @param[in]  sb        struct stat provided by FTW
	 * @param[in]  typeflag  The typeflag
	 * @param      ftwbuf    The struct FTW provided by FTW
	 *
	 * @return     on sucess returns 0 to continue file tree walk
	 */
	int push_to_threadpool(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

	/**
	 * @brief      Determines if file.
	 *
	 * @param[in]  path  The path
	 *
	 * @return     True if file, False otherwise.
	 */
	bool is_file(const char* path);

	/**
	 * @brief      Determines if dir.
	 *
	 * @param[in]  path  The path
	 *
	 * @return     True if dir, False otherwise.
	 */
	bool is_dir(const char* path);

	/**
	 * @brief      calls the file tree walk
	 *
	 * @param[in]  ret   The struct ret
	 *
	 * @return     sucess value
	 */
	int call_file_tree_walk(struct parser::output ret);

	/**
	 * @brief      processes the parser output
	 *
	 * @param[in]  ret   The ret(parser output)
	 *
	 * @return     sucess value
	 */
	int go(struct parser::output ret);

}
#endif