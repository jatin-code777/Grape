#include <getopt.h>
#include <iostream>
#include "argparser.h"

/// Flags set by '--help','--usage', '--version' 

/**
 * @brief      prints help for our program
 */
void parser::print_help()
{
  printf("Usage: grape [OPTION]... PATTERN [FILE]\n");
  printf("Search for PATTERN in each FILE.\n");
  printf("Example: grape -i 'hello world' main.c\n");
  printf("\n");
  printf("Pattern selection and interpretation:\n");
  printf(" -F,   --fixed-string         {default}PATTERN is a normal string\n");
  printf(" -G,   --basic-regexp         PATTERN is a basic regular expression (default)\n");
  printf(" -e,   --regexp=PATTERN       use PATTERN for matching\n");
  printf(" -i,   --ignore-case          ignore case distinctions\n");
  printf("\n");
  printf("Miscellaneous:\n");
  printf("       --version             display version information and exit\n");
  printf("       --help                display this help text and exit\n");
  printf("       --usage               display usage text and exit\n");
  printf("\n");
  printf("Output control:\n");
  printf(" -n,   --line-number         print line number with output lines\n");
  printf(" -r,   --recursive           recursive search inside directory\n");
}
/**
 * @brief      prints usage instructions
 */
void parser::print_usage()
{
  printf("Usage: grape [OPTION]... PATTERN [FILE]\n");
  printf("for more detailed discription use --help\n");
}

/**
 * @brief      prints version and copyright information
 */
void parser::print_version()
{
  printf("grape (multithreaded) 1.0.0\n");
  printf("Copyright (c)\n");
  printf("License \n");
  printf("This is free software: you are free to change and redistribute it.\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
  printf("Written by Dhananjay Raut, Sahil Shah, Jatin Sharma and Raja Naik\n");
}

char initialpath[] = ".";
/**
 * @brief      parse Command line input
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 *
 * @return     return 0 on sucess returns 1 else 
 */
struct parser::output parser::parse(int argc, char **argv)
{
  static int help_flag=0, usage_flag=0,version_flag=0;
  int c;
  bool F_flag=0,r_flag=0,i_flag=0,v_flag=0,G_flag=0,n_flag=0,c_flag=0;
  char *path = initialpath;
  char *PATTERN = NULL;
  int l = -1;
  int p = 0;
  struct output ret;
  //ret.PATH = path;
  static struct option long_options[] =
  {
    /// These options set a flag.
    {"usage",   no_argument,  &usage_flag,  1},
    {"help",    no_argument,  &help_flag,   1},
    {"version", no_argument,  &version_flag,1},
    /// These options don't set a flag.We distinguish them by their indices.
    {"regexp",        optional_argument, 0, 'e'},
    {"fixed-string",  optional_argument, 0, 'F'},
    {"recursive",     no_argument,       0, 'r'},
    {"ignore-case",   no_argument,       0, 'i'},
    {"basic-regex",   optional_argument, 0, 'G'},
    {"line-number",   no_argument,       0, 'n'},
    {"count",   no_argument,             0, 'c'},
    {"files-without-match", no_argument, 0, 'l'},
    {"files-with-matches", no_argument,  0, 'L'},
    {0, 0, 0, 0}
  };
  int option_index = 0;
  if(argc==1){
    print_usage();
    ret.return_value = 1;
    return ret;
  }
  while(1)
    {
      /// getopt_long stores the option index here.
      option_index = 0;
      c = getopt_long(argc, argv, "F::e::riyG::nclL",long_options, &option_index);

      /// Detect the end of the options.
      if (c == -1) break;

      switch (c)
        {
        case 0:
          /// If this option set a flag, do nothing else now.
          if (long_options[option_index].flag != 0) break;
          printf("something is wrong\n");
          break;
        case 'F':
          if (G_flag){
            printf("only one type of PATTERN is allowed\n");
            ret.return_value = 1;
            return ret;
          }
          else{
            F_flag = 1;
            if(optarg!=NULL){
                PATTERN = optarg;
                p = 1;
              }
          }
          break;
        case 'e':
          if (F_flag){
            printf("only one type of PATTERN is allowed\n");
            ret.return_value = 1; 
            return ret;
          }
          else{
            G_flag = 1;
            if(optarg!=NULL){
                PATTERN = optarg;
                p = 1;
              }
          }
          break;
        case 'r':
          r_flag=1;
          break;
        case 'l':
          l = 0;
          break;
        case 'L':
          l = 1;
          break;
        case 'c':
          c_flag=1;
          break;
        case 'i':
          i_flag=1;
          break;
        case 'y':
          i_flag=1;
          break;
        case 'v':
          v_flag=1;
          break;
        case 'G':
          if (F_flag){
            printf("only one type of PATTERN is allowed\n");
            ret.return_value = 1; 
            return ret;
          }
          else{
            G_flag = 1;
            if(optarg!=NULL){
                PATTERN = optarg;
                p = 1;
              }
          }
          break;
        case 'n':
          n_flag=1;
          break;
        case '?':
          /// getopt_long already printed an error message.
          printf("Use --help to see options avilable\n");
          ret.return_value = 1; 
          return ret;
          break;
        default:
          abort();
        }
    }

  /* Instead of reporting '--help' as they are encountered,
   we report the final status resulting from them. */
  if (help_flag){
    print_help();
    ret.return_value = 1; 
    return ret;
  }
  if(usage_flag){
    print_usage();
    ret.return_value = 1; 
    return ret;
  }
  if(version_flag){
    print_version();
    ret.return_value = 1; 
    return ret;
  }
  for(int i=0; optind < argc; optind++,i++)
  {
  	if(i==1){
      if (p==1) path = argv[optind];
			else{
          printf("non-option argument found %s\n",argv[optind]);
          ret.return_value = 1; 
          return ret;
        }
    }
    else if (i==0){
      if (F_flag || G_flag){
  			if(p == 0) PATTERN = argv[optind];
        else path = argv[optind];
      }
      else{
        PATTERN = argv[optind];
        F_flag = 1;
      }
      p = 1;
	  }
	  else{
		printf("non-option argument found %s\n",argv[optind]);
		ret.return_value = 1; 
    return ret;
	  }
  }
  if(p==0)
  {
    print_usage();
    ret.return_value = 1; 
    return ret;
  }
  ret.PATTERN = PATTERN;
  ret.PATH = path;
  ret.F_flag = F_flag;
  ret.r_flag =r_flag;
  ret.i_flag =i_flag;
  ret.v_flag =v_flag;
  ret.G_flag =G_flag;
  ret.n_flag =n_flag;
  ret.c_flag =c_flag;
  ret.return_value = 0;
  ret.l = l;
  /*printf("pattern : %s\npath : %s\n",ret.PATTERN,ret.PATH);
  printf("F_flag:%d\n",F_flag);
  printf("r_flag:%d\n",r_flag);
  printf("i_flag:%d\n",i_flag);
  printf("v_flag:%d\n",v_flag);
  printf("G_flag:%d\n",G_flag);
  printf("n_flag:%d\n",n_flag);*/
  return ret;
  
}
