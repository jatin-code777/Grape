#ifndef _grape_utility_
#define _grape_utility_


#include <fstream>
#include <mutex>

namespace detail {

	/**
	 * @brief A mutex handler class which unlocks it on going out of scope.
	 * 
	 * This class cannot be copied or moved so as to improve readability
	 * and consistency. Only std::mutex is allowed.
	 */
	class RAII_lock
	{
	public:
		explicit RAII_lock(std::mutex &m)
			: mutex(m)
		{
			mutex.lock();
		}
		~RAII_lock()
		{
			mutex.unlock();
		}

	private:
		std::mutex &mutex;
	};


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