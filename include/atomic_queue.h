// #pragma once

#ifndef _ATOMIC_QUEUE_
#define _ATOMIC_QUEUE_

#include <queue>
#include <mutex>

class autoRAII_lock {
	public:
		autoRAII_lock(std::mutex& m)
			:mutex(m)
		{
			mutex.lock();
		}
		~autoRAII_lock()
		{
			mutex.unlock();
		}
	private:
		std::mutex& mutex;
};

template <class T>
class Atomic_Queue {
	public:

		bool empty()
		{
			autoRAII_lock lock(mutex);
			bool ret = Q.empty();
			return ret;
		}

		void push(const T& val)
		{
			autoRAII_lock lock(mutex);
			Q.push(val);
		}
		void push(T&& val)
		{
			autoRAII_lock lock(mutex);
			Q.push(std::move(val));
		}

		T pop()
		{
			autoRAII_lock lock(mutex);
			T ret = std::move(Q.top());
			return ret;
		}

	private:
		std::mutex mutex;
		std::queue <T> Q;
};

#endif