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

namespace thread_manager {

	class ThreadPool
	{
		static ThreadPool * instance;
		static std::mutex instance_mutex;
		ThreadPool() = default;
		ThreadPool(int n) { resize(n); }
	public: 

		static ThreadPool * get_instance()
		{
			detail::autoRAII_lock lock(instance_mutex);
			if(!instance)
				instance = new ThreadPool;
			return instance;
		}
		static ThreadPool * get_instance(int n)
		{
			detail::autoRAII_lock lock(instance_mutex);
			if(!instance)
				instance = new ThreadPool(n);
			return instance;
		}

		~ThreadPool() { stop(true); }

		/// No copies or moves allowed for ThreadPool class
		ThreadPool(const ThreadPool &) 				= delete;
		ThreadPool(ThreadPool &&) 					= delete;
		ThreadPool & operator=(const ThreadPool &) 	= delete;
		ThreadPool & operator=(ThreadPool &&) 		= delete;

		size_t size() {
			return threads.size();
		}

		size_t n_waiting() {
			return nWaiting;
		}

		std::thread& get_thread(int i) {
			return *threads[i];
		}
		
		// empty the queue
		void clear_queue() {
			while(!Q.empty()) delete Q.pop(); // empty the queue
		}

		// change the number of threads in the pool
		// should be called from one thread, otherwise be careful to not interleave, also with this->stop()
		// nThreads must be >= 0
		void resize(int nThreads) {
			if (!isStop && !isDone) {
				size_t oldNThreads = threads.size();
				if (oldNThreads <= nThreads) {  // if the number of threads is increased
					threads.resize(nThreads);
					flags.resize(nThreads);

					for (int i = oldNThreads; i < nThreads; ++i) {
						flags[i] = std::make_shared<std::atomic<bool> >(false);
						set_thread(i);
					}
				}
				else {  // the number of threads is decreased
					for (int i = oldNThreads - 1; i >= nThreads; --i) {
						*flags[i] = true;  // this thread will finish
						threads[i]->detach();
					}

					{// Begin scope for lock
						// stop the detached threads that were waiting
						detail::autoRAII_lock lock(mutex);
						cv.notify_all();  // stop all waiting threads
					}// End of scope for lock

					threads.resize(nThreads);  // safe to delete because the threads are detached
					flags.resize(nThreads);  // safe to delete because the threads have copies of shared_ptr of the flags, not originals
				}
			}
		}

		// wait for all computing threads to finish and stop all threads
		// may be called asynchronously to not pause the calling thread while waiting
		// if isWait == true, all the functions in the queue are run, otherwise the queue is cleared without running the functions
		void stop(bool isWait = false) {
			if(isStop) return;

			if (!isWait) {
				isStop = true;
				for(auto &flag:flags) *flag = true;	// Command all threads to stop
				clear_queue();  // empty the queue
			}
			else {
				if (isDone) return;
				isDone = true;  // give the waiting threads a command to finish
			}

			{// Begin scope for lock
				detail::autoRAII_lock lock(mutex);
				cv.notify_all();  // stop all waiting threads
			}// End of scope for lock

			for (auto& thread:threads) {  // wait for the computing threads to finish
				if (thread->joinable()) thread->join();
			}

			// if there were no threads in the pool but some functors in the queue, the functors are not deleted by the threads
			// therefore delete them here
			clear_queue();
			flags.clear();
			threads.clear();
		}

		// pops a functional wrapper to the original function
		std::function<void(int)> pop() {
			std::function<void(int id)> * f_ptr = Q.pop();
			std::unique_ptr<std::function<void(int id)>> func(f_ptr); // at return, delete the function even if an exception occurred
			std::function<void(int)> f;
			if (f_ptr) f = *f_ptr;
			return f;
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

		void set_thread(int i) 
		{
			std::shared_ptr<std::atomic<bool>> flag(this->flags[i]); // a copy of the shared ptr to the flag
			auto func = [this, i, &flag = *flags[i]/* a copy of the shared ptr to the flag */]() {
				std::function<void(int id)> * f;
				bool isPop = this->Q.pop(f);
				while (true) {
					while (isPop) {  // if there is anything in the queue
						std::unique_ptr<std::function<void(int id)>> func(f); // at return, delete the function even if an exception occurred
						(*f)(i);
						if (flag)
							return;  // the thread is wanted to stop, return even if the queue is not empty yet
						else
							isPop = this->Q.pop(f);
					}
					// while(!Q.empty()) {
					// 	(*Q.pop())(i);
					// 	if(flag) return;
					// }
					// the queue is empty here, wait for the next command
					// detail::autoRAII_lock lock(this->mutex);
					std::unique_lock<std::mutex> lock(this->mutex);
					++this->nWaiting;
					this->cv.wait(lock, [this, &f, &isPop, &flag](){
											isPop = this->Q.pop(f);
											return isPop || this->isDone || flag ;
										});
					--this->nWaiting;
					if (!isPop)
						return;  // if the queue is empty and this->isDone == true or *flag then return
				}
			};
			threads[i].reset(new std::thread(func)); // compiler may not support std::make_unique()
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


	//Initialize pointer to zero so that it can be initialized in first call to getInstance
	ThreadPool* ThreadPool::instance = nullptr;

}

#endif