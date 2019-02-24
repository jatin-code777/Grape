// #pragma once

#ifndef _THREAD_POOL_
#define _THREAD_POOL_


#include <vector>
#include <functional>
#include "atomic_queue.h"

#include <memory>

#include <thread>
#include <future>
#include <atomic>
#include <mutex>

// namespace internal {
// 	using std::thread;
// 	using std::shared_ptr;
// 	using std::atomic;
// 	using namespace detail;
// 	using std::condition_variable;
// 	using std::function>
// }

	class threadpool
	{
		/* data */
	public: 
		threadpool() = default;
		~threadpool();

		size_t size() {
			return threads.size();
		}

		size_t n_waiting() {
			return nWaiting;
		}

		std::thread& get_thread(int i) {
			return *threads[i];
		}

		

	private:
        std::mutex mutex;
        std::condition_variable cv;

        std::atomic<bool>  isDone = false;
        std::atomic<bool>  isStop = false;
        std::atomic<int> nWaiting = 0;

        detail::Atomic_Queue<std::function<void(int id)> *> q;

		std::vector <std::unique_ptr<std::thread>> threads;
        std::vector <std::shared_ptr<std::atomic<bool>>> flags;
	};





#endif