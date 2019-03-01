#ifndef _ATOMIC_QUEUE_
#define _ATOMIC_QUEUE_

#include <queue>
#include <mutex>
#include <utility>
#include "RAII_utils.h"

/**
 * @brief Namespace for internal detail.
 * 
 * Contains implementation of fast RAII_lock
 * and Atomic_Queue
 */
namespace detail {

	/**
	 * @brief A basic homogenous Atomic Queue using mutex locks
	 * 
	 * @tparam T	Type of the homogenous entities which Queue contains
	 */
	template <class T>
	class Atomic_Queue
	{
	public:
		/**
		 * @brief returns true if Queue is empty
		 */
		bool empty()
		{
			RAII_lock lock(mutex);
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
			RAII_lock lock(mutex);
			Q.push(val);
		}
		/// Additional overload for rvalue to be inserted.
		/// Helpful to avoid making a copy of object to be inserted.
		void push(T &&val)
		{
			RAII_lock lock(mutex);
			Q.push(std::move(val));
		}
		/**
		 * @brief pops the topmost element of queue and returns it
		 * 
		 * @return T Returns the popped value. Hoping for copy elision
		 */
		T pop()
		{
			RAII_lock lock(mutex);
			/// Since we don't want to make an extra copy
			T ret = std::move(Q.front());
			Q.pop();
			return std::move(ret);
		}

		/**
		 * @brief Overloaded utility of pop for better exception handling
		 * 
		 * @param ret returns true if pop was succesfull
		 */
		bool pop(T &ret)
		{
			RAII_lock lock(mutex);
			if(Q.empty()) return 0;
			ret = std::move(Q.front());
			Q.pop();
			return 1;
		}

		T& top()
		{
			RAII_lock lock(mutex);
			return Q.front();
		}

	private:
		std::mutex mutex;
		std::queue<T> Q;
	};

} //end of namespace detail

#endif