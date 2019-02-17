// #pragma once

#ifndef _ATOMIC_QUEUE_
#define _ATOMIC_QUEUE_

#include <queue>
#include <mutex>


template <class T>
class Atomic_Queue {
	public:
		bool empty() const
		{
			mutex.lock();
			bool ret = Q.empty();
			mutex.unlock();
			return ret;
		}

		void push(const T& val)
		{
			Q.push(val);
		}
		void push(T&& val)
		{
			mutex.lock();
			Q.push(std::move(val));
			mutex.unlock();
		}

		T pop()
		{
			mutex.lock();
			T ret = std::move(Q.top());
			mutex.unlock();
			return ret;
		}

	private:
		std::mutex mutex;
		std::queue <T> Q;
};



#endif