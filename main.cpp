#include <iostream>
#include "argparser.h"
#include "call.h"
#include "BM.h"
// #include "threadpool.h"

int main(int argc, char* argv[])
{


	struct parser::output ret = parser::parse(argc,argv);
	if (ret.return_value != 0) return ret.return_value;
	//std::cout<<"sucessfully parsed the output"<<std::endl;
	return call::go(ret);

}
