#include <iostream>
#include "argparser.h"

int main(int argc, char* argv[])
{
	

	int p_out = parser::parse(argc,argv);
	if (p_out != 0) return p_out;
	std::cout<<"sucessfully parsed the output"<<std::endl;
	
} 