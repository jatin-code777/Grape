// #pragma once

#ifndef _THREAD_POOL_
#define _THREAD_POOL_


#include <vector>
#include <functional>
#include "atomic_queue.h"

#include <memory>
#include <type_traits>

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

		template <class Func_t, class... Param_t>
		auto push(Func_t&& func, Param_t&&... params)
		{
			using std::placeholders;
			auto func_pack = std::make_shared< std::packaged_task<decltype(f(7,rest...))(int)> > (
				std::bind(std::forward<F>(f) , _1 , std::forward<Rest>(rest)...);
			)
			auto f = new std::function<void(int)>(
						[func_pack](int id) { (*func_pack)(id); }
					);
			Q.push(f);
			detail::autoRAII_lock lock(mutex);
			cv.notify_one();
			return func_pack->get_future();
		}


		template <class F>
		auto push(F&& f)
		{
			using std::placeholders;
			auto func_pack = std::make_shared< std::packaged_task<decltype(f(7))(int)> > (
				std::forward<F>(f);
			)
			auto f = new std::function<void(int)>(
						[func_pack](int id) { (*func_pack)(id); }
					);
			Q.push(f);
			detail::autoRAII_lock lock(mutex);
			cv.notify_one();
			return func_pack->get_future();
		}

	private:
        std::mutex mutex;
        std::condition_variable cv;

        std::atomic<bool>  isDone = false;
        std::atomic<bool>  isStop = false;
        std::atomic<int> nWaiting = 0;

        detail::Atomic_Queue<std::function<void(int id)> *> Q;

		std::vector <std::unique_ptr<std::thread>> threads;
        std::vector <std::shared_ptr<std::atomic<bool>>> flags;
	};





#endif