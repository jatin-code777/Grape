// #pragma once

#ifndef _ATOMIC_QUEUE_
#define _ATOMIC_QUEUE_

#include <queue>
#include <mutex>

/**
 * @brief A mutex handler class which unlocks it on going out of scope.
 * 
 * This class cannot be copied or moved so as to improve readability
 * and consistency. Only std::mutex is allowed.
 */
class autoRAII_lock
{
  public:
	explicit autoRAII_lock(std::mutex &m)
		: mutex(m)
	{
		mutex.lock();
	}
	~autoRAII_lock()
	{
		mutex.unlock();
	}

  private:
	std::mutex &mutex;
};


/**
 * @brief A basic homogenous Atomic Queue using mutex locks
 * 
 * @tparam T	Type of the homogenous entities which Queue contains
 */
template <class T>
class Atomic_Queue
{
  public:
	/// returns True if Queue is currently empty.
	bool empty()
	{
		autoRAII_lock lock(mutex);
		bool ret = Q.empty();
		return ret;
	}
	/**
	 * @brief Push a value into the queue, for lvalues
	 * 
	 * @param val	lvalue of the object to be inserted into queue
	 */
	void push(const T &val)
	{
		autoRAII_lock lock(mutex);
		Q.push(val);
	}
	/// Additional overload for rvalue to be inserted.
	/// Helpful to avoid making a copy of object to be inserted.
	void push(T &&val)
	{
		autoRAII_lock lock(mutex);
		Q.push(std::move(val));
	}
	/**
	 * @brief pops the topmost element of queue and returns it
	 * 
	 * @return T Returns the popped value. Hoping for copy elision
	 */
	T pop()
	{
		autoRAII_lock lock(mutex);
		/// Since we don't want to make an extra copy
		T ret = std::move(Q.top());
		Q.pop();
		return ret;
	}

  private:
	std::mutex mutex;
	std::queue<T> Q;
};

#endif