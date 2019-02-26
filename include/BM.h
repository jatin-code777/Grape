#ifndef BM_H
#define BM_H

namespace BM {

void print_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf);

/**
 * @brief      Builds occ array, used to track last occurance of each character in the input pattern.
 *
 * @param      pat   The pattern
 */  
void build_occ(char* pat);

/**
 * @brief      pre_processes pattern to calculate shift value s[i] for indices(i), based on case 1 of the good suffix rule.
 *
 * All indices for which the following suffix has another instance in the pattern are handled in case 1 of the good suffix rule.
 *			   
 * @param      pat   The pattern
 */
void case1_good_suffix(char* pat);

/**
 * @brief      pre_processes pattern to calculate shift calue s[i] for indices(i), based on case 2 of the good suffix rule. 
 * 
 * s[i] values are computed for all the remaining indices left out in case 1 of the good suffix rule.
 * Essentially for all indices whose following suffix only partially repeats in the pattern, s[i] values are calculated in case 2 of the good suffix rule.
 *			   
 * @param      pat   The pattern
 */
void case2_good_suffix(char* pat);

/**
 * @brief      Does all preprocessing on the pattern using bad character and good suffix rules of Boyer Moore algorithm.
 *
 *
 * @param      patt     The pattern
 * @param[in]  ic       tells whether or not casing is to be ignored
 * @param[in]  ig_name  tells whether or not to print the file names on match detection
 */
void pre_process(char* patt, bool ic = 0, bool ig_name=0);

/**
 * @brief      Function called from thread pool that implements Boyer Moore Algorithm on input file path, and preprocessed pattern.
 *
 * @param[in]  id    The thread identifier
 * @param[in]  path  The file path
 *
 * @return     status signal
 */
int BM(int id, const char* path);

/**
 * @brief      Function called from thread pool that implements Boyer Moore Algorithm on input file path keeping track of line numbers, and preprocessed pattern.
 *
 * @param[in]  id    The thread identifier
 * @param[in]  path  The file path
 *
 * @return     status signal
 */
int BM_N(int id ,const char* path);

}
#endif