#ifndef _grape_utility_
#define _grape_utility_
#include <fstream>
#include <mutex>

namespace detail {

    class RAII_acquireFile
	{
	public:
		explicit RAII_acquireFile(std::ifstream &file, const std::string& path)
			: file(file)
		{
			file.open(path);
		}
		~RAII_acquireFile()
		{
			file.close();
		}

	private:
		std::ifstream &file;
	};

}

#endif