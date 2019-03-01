#include "argparser.h"
#include "call.h"

int main(int argc, char* argv[])
{
	struct parser::output ret = parser::parse(argc,argv);
	if (ret.return_value != 0) return ret.return_value;

	return call::go(ret);
}
