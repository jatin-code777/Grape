#include <functional>
#include <inttypes.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include <vector>
#include <mutex>
#include <deque>
#include "SearchStrategy.h"

#ifndef BM_H
#define BM_H

#define ALPHABET_SIZE 256

/**
 * @brief      Class for Boyer Moore search
 */
class BoyerMooreSearch : public SearchStrategy
{
	private:
		std::function <bool(char,char)> comp = [](char a, char b) { return a==b ;};//general == character comparision
		std::function <bool(char,char)> comp_ig = [](char a, char b) { return (a|' ') == (b|' '); };//ignore case comparator
		
		/**
		 * eq is the equality function to be employed for checking equality between two characters
		 */
		std::function <bool(char,char)> eq;
		
		/**
		 * occ[c] stores last index of occurance of character c in pattern 
		 */
		int occ[ALPHABET_SIZE];

		/**
		 * jump table for mismatch detected at last character position
		 */
		int jt[ALPHABET_SIZE];

		/**
		 * All reads are to a buffer of size PAGESIZE
		 */
		int PAGESIZE = getpagesize()*32;

		/**
		 * pattern length
		 */
		int n;
		
		/**
		 * the pattern
		 */
		char* pat;
		
		/**
		 * boolean flag for whether case distinctions are to be ignored
		 */
		bool ignore_case = 0;

		/**
		 * boolean flag for whether name of the file/s is/are to be printed
		 */
		bool ignore_name = 0;

		/**
		 * state represents the state of flags
		 * 0 
		 */
		int state = 0;
		
		/**
		 * for printing output for each file seperately
		 */
		std::mutex print_mutex;
		//n = pattern length
		//occ[i] = last occurance index of character with ASCII value i in the pattern.
		//occ[i] = -1 --> that characeter doesn't exist in the search pattern 
		//jt is jumptable based on bad character heuristic due to mismatch at the last position

		/**
		 * precomputation vector for KMP
		 */
		std::vector<int> lps;

		/**
		 * precomputation vector for Boyer Moore
		 */
		std::vector<int> s,f;
		//f[i] = starting point of longest suffix of pat[i....n] that is also its prefix
		//f[i] >= pattern length --> such a suffix doesn't exist
		//s[i] = shift to be made based on good suffix heuristics, when index i does not match, the rest of the suffix matches
		//s[i] is the MINIMUM shift such that the part that is already matched matches, and the mismatched character gets changed
		




		//pre-processing
		/**
		 * @brief      Builds occ array, used to track last occurance of each character in the input pattern.
		 *
		 * @param      pat   The pattern
		 */
		void build_occ();
		
		/**
		 * @brief      pre_processes pattern to calculate shift value s[i] for indices(i), based on case 1 of the good suffix rule.
		 *
		 * All indices for which the following suffix has another instance in the pattern are handled in case 1 of the good suffix rule.
		 *			   
		 */
		void case1_good_suffix();
		
		/**
		 * @brief      pre_processes pattern to calculate shift calue s[i] for indices(i), based on case 2 of the good suffix rule. 
		 * 
		 * s[i] values are computed for all the remaining indices left out in case 1 of the good suffix rule.
		 * Essentially for all indices whose following suffix only partially repeats in the pattern, s[i] values are calculated in case 2 of the good suffix rule.
		 *
		 */
		void case2_good_suffix();

		/**
		 * @brief      Builds Longest Prefix Suffix array for the input pattern sequence
		 */
		void build_lps();
		
	public:
		/**
		 * @brief      Does all preprocessing on the pattern using bad character and good suffix rules of Boyer Moore algorithm.
		 *
		 *
		 * @param      patt     The pattern
		 * @param[in]  ic       tells whether or not casing is to be ignored
		 * @param[in]  ig_name  tells whether or not to print the file names on match detection
		 */
		void pre_process(char* patt, bool ic = 0, bool ig_name=0,int flags=0);




	private:
		//searching
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

	public:
		/**
		 * @brief      Searches for pattern in file denoted by path
		 *
		 * @param[in]  id    The thread identifier
		 * @param[in]  path  The input file path
		 */
		void search(int id, std::string path);



		
	private:
		//Output functions to manage '\n's.		

		/**
		 * @brief      skips to next line
		 * 
		 * @param[in]  fd    file descriptor
		 * @param      i     position where match is detected
		 * @param      k     buffer end position
		 * @param[in]  m     file end position
		 * @param      buf   The buffer
		 */
		void skip_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf);

		/**
		 * @brief      prints line on match detection
		 *
		 * @param[in]  fd    file descriptor
		 * @param      i     position where match is detected
		 * @param      k     buffer end position
		 * @param[in]  m     file end position
		 * @param      buf   The buffer
		 * @param      ss    output string stream for file
		 */
		void print_line(int fd,int64_t& i,int64_t& k,int64_t m,char* buf, std::stringstream &ss);

		/**
		 * @brief      error_handler
		 *
		 * @param[in]  status   status of system call
		 * @param[in]  message  error message to be displayed in erraneous state
		 */
		void error_handler(int status,int fd,std::string message="");
		
		/**
		 * @brief      convert 8 bit character to int
		 *
		 * @param[in]  c     character to be converted
		 */
		int char_to_int(char c);
};
#endif