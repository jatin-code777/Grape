#ifndef ARGPARSER_H
#define ARGPARSER_H

namespace parser {
  
  /**
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
  
  struct output
  {
  char * PATTERN;
  char * PATH;
  bool F_flag=0,r_flag=0,i_flag=0,v_flag=0,G_flag=0,n_flag=0;
  int return_value = 0;
  };

  /**
   * @brief      parse the Command line arguments
   *
   * @param[in]  argc  The argc
   * @param      argv  The argv
   *
   * @return     sucess value
   */
  struct output parse(int argc, char ** argv);
}
#endif
