#include <getopt.h>
#include<iostream>
/// Flags set by ‘--help’,'--usage', '--version' 
static int help_flag=0, usage_flag=0,version_flag=0;

/**
 * @brief      prints help for our program
 */
void print_help()
{
	std::cout<<"Usage: grape [OPTION]... PATTERN [FILE]"<<std::endl;
	std::cout<<"Search for PATTERN in each FILE."<<std::endl;
	std::cout<<"Example: grape -i 'hello world' main.c"<<std::endl;
	std::cout<<""<<std::endl;
	std::cout<<"Pattern selection and interpretation:"<<std::endl;
	std::cout<<" -F,  --fixed-string        PATTERN is a set of newline-separated strings"<<std::endl;
	std::cout<<" -G,  --basic-regexp        PATTERN is a basic regular expression (default)"<<std::endl;
	std::cout<<" -e,  --regexp=PATTERN      use PATTERN for matching"<<std::endl;
	std::cout<<" -i,  --ignore-case         ignore case distinctions"<<std::endl;
	std::cout<<"Miscellaneous:"<<std::endl;
	std::cout<<" -v,  --invert-match         select non-matching lines"<<std::endl;
	std::cout<<"       --version             display version information and exit"<<std::endl;
	std::cout<<"       --help                display this help text and exit"<<std::endl;
	std::cout<<"       --usage               display usage text and exit"<<std::endl;
	std::cout<<""<<std::endl;
	std::cout<<"Output control:"<<std::endl;
	std::cout<<" -n,  --line-number          print line number with output lines"<<std::endl;
	std::cout<<" -r,  --recursive            recursive search inside directory"<<std::endl;
}
/**
 * @brief      prints usage instructions
 */
void print_usage()
{
	std::cout<<"Usage: grape [OPTION]... PATTERN [FILE]"<<std::endl;
	std::cout<<"for more detailed discription use --help"<<std::endl;
}

/**
 * @brief      prints version and copyright information
 */
void print_version()
{
	std::cout<<"grape (multithreaded) 1.0.0"<<std::endl;
	std::cout<<"Copyright (c)"<<std::endl;
	std::cout<<"License "<<std::endl;
	std::cout<<"This is free software: you are free to change and redistribute it."<<std::endl;
	std::cout<<"There is NO WARRANTY, to the extent permitted by law."<<std::endl;
	std::cout<<"Written by Dhananjay Raut, Sahil Shah, Jatin Sharma and Raja Naik"<<std::endl;
}


int main (int argc, char **argv)
{
int c;
bool F_flag=0,r_flag=0,i_flag=0,v_flag=0,G_flag=0,n_flag=0;
char initialpath[] = ".";
char *path = initialpath;
char *PATTERN = NULL;
static struct option long_options[] =
{
  /// These options set a flag.
  {"usage",   no_argument,  &usage_flag,  1},
  {"help",    no_argument,  &help_flag,   1},
  {"version", no_argument,  &version_flag,1},
  /// These options don’t set a flag.We distinguish them by their indices.
  {"regexp",        required_argument, 0, 'F'},
  {"fixed-string",  required_argument, 0, 'F'},
  {"recursive",     no_argument,       0, 'r'},
  {"ignore-case",   no_argument,       0, 'i'},
  {"invert-match",  no_argument,       0, 'v'},
  {"basic-regex",   required_argument, 0, 'G'},
  {"line-number",   no_argument,       0, 'n'},
  {0, 0, 0, 0}
};
int option_index = 0;
if(argc==1)
{
	print_usage();
	return 0;
}
  while (1)
    {
      /// getopt_long stores the option index here.
      option_index = 0;
      c = getopt_long(argc, argv, "F:e:riyvG:n",long_options, &option_index);

      /// Detect the end of the options.
      if (c == -1) break;

      switch (c)
        {
        case 0:
          /// If this option set a flag, do nothing else now.
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'F':
          if (G_flag)
          {
          	printf("only one PATTERN is allowed\n");
          	return 0;
          }
          else
          {
          	F_flag = 1;
          	PATTERN = optarg;
          }
          break;
        case 'e':
          if (G_flag)
          {
          	printf("only one PATTERN is allowed\n");
          	return 0;
          }
          else
          {
          	F_flag = 1;
          	PATTERN = optarg;
          }
          break;

        case 'r':
          r_flag=1;
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
          if (F_flag)
          {
          	printf("only one PATTERN is allowed\n");
          	return 0;
          }
          else
          {
          	G_flag = 1;
          	PATTERN = optarg;
          }
		  break;

        case 'n':
          n_flag=1;
          break;
        
        case '?':
          printf("Use --help to see options avilable\n");
          return 0;
          /// getopt_long already printed an error message.
          break;

        default:
          abort();
        }
    }

 	 /* Instead of reporting '--help’ as they are encountered,
     we report the final status resulting from them. */
  	if (help_flag)
    {
    	print_help();
    	return 0;
    }

    if(usage_flag)
    {
    	print_usage();
    	return 0;
    }
    if(version_flag)
    {
    	print_version();
    	return 0;
    }

    for(int i=0; optind < argc; optind++ ,i++)
    {
    	if(i==1) path = argv[optind];
 		else if (i==0)
    	{
    		if (F_flag || G_flag)
    		{
    			printf("only one PATTERN is allowed\n");
    			return 0;
    		}
    		else PATTERN = argv[optind];
		}
		else
		{
			printf("non-option argument found %s\n",argv[optind]);
			return 0;
		}
	}
	printf("%s %s\n",path,PATTERN);
	printf("F_flag:%d\n",F_flag);
	printf("r_flag:%d\n",r_flag);
	printf("i_flag:%d\n",i_flag);
	printf("v_flag:%d\n",v_flag);
	printf("G_flag:%d\n",G_flag);
	printf("n_flag:%d\n",n_flag);
  exit (0);
}
