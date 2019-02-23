#ifndef CALL_H
#define CALL_H
#include "argparser.h"
#include <ftw.h>
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
	int display_info(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

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
	int call(struct parser::output ret);

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