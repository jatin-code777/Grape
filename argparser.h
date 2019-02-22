#ifndef ARGPARSER_H
#define ARGPARSER_H

namespace parser{/**
 * @brief      prints help for our program
 */
void print_help();

/**
 * @brief      prints usage instructions
 */
void print_usage();
/**
 * @brief      prints version and copyright information
 */
void print_version();

/**
 * @brief      parse the Command line arguments
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     sucess value
 */
int parse(int argc, char ** argv);
}
#endif