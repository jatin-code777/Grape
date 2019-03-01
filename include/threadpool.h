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
		/**
		 * @brief Construct a new Thread Pool object
		 * 
		 * @param n Initial number of threads to be created.
		 */
		ThreadPool(int n) { resize(n); }

	public:
		/**
		 * @brief Get the single instance object pointer
		 * 
		 * @param n Initial number os threads to be created
		 * @return ThreadPool* The instance pointer
		 * 
		 * This is a threadsafe implementation from C++11 onwards
		 * This also destroys the object in case of exceptions or end of program
		 * /// https://stackoverflow.com/questions/1661529/is-meyers-implementation-of-the-singleton-pattern-thread-safe
		 */
		static ThreadPool* get_instance(int n = 0)
		{
			static ThreadPool instance(n);
			return &instance;
		}

		/**
		 * @brief Finishes pending tasks then destroys the Thread Pool object
		 * 
		 */
		~ThreadPool() { stop(true); }

		/// No copies or moves allowed for ThreadPool class
		ThreadPool(const ThreadPool &) 				= delete;
		ThreadPool(ThreadPool &&) 					= delete;
		ThreadPool & operator=(const ThreadPool &) 	= delete;
		ThreadPool & operator=(ThreadPool &&) 		= delete;

		/**
		 * @brief Returns number of threads assigned to Thread Pool
		 * 
		 * @return size_t 
		 */
		size_t size() {
			return threads.size();
		}

		/**
		 * @brief Returns number of free threads waiting without work
		 * 
		 * @return size_t 
		 */
		size_t n_idle() {
			return nIdle;
		}

		/**
		 * @brief Get the i'th thread object
		 * i must liew between 0 and size(). Behavior undefined otherwise
		 * 
		 * @param i index of thread
		 * @return std::thread& 
		 */
		std::thread& get_thread(int i) {
			return *threads[i];
		}
		
		/**
		 * @brief Empties the queue without running functions
		 * 
		 * Deletes the function pointers to avoid memory leaks
		 */
		void clear_queue() {
			while(!Q.empty()) delete Q.pop(); // empty the queue
		}

		/**
		 * @brief Change the number of threads assigned to pool
		 * This function is not threadsafe. Must be called from a single thread at a time.
		 * 
		 * @param nThreads Number of threads to be assigned. Must be >=0
		 */
		void resize(int nThreads) {
			if(nThreads == size())	return;
			if (!isStop && !isDone) {
				size_t oldNThreads = threads.size();
				if (oldNThreads <= nThreads) {  // if the number of threads is increased
					threads.resize(nThreads);
					flags.resize(nThreads);

					for (int i = oldNThreads; i < nThreads; ++i) {
						flags[i] = std::make_shared<std::atomic<bool> >(false);
						configure_thread(i);
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

		/**
		 * @brief Stops all threads and destroys their objects.
		 * 
		 * @param isWait true Wait till the current Queue finishes execution
		 * @param isWait false Clear all functions in queue without executing
		 */
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

			/// Delete all remaining functors here (in case of no threads)
			clear_queue();
			flags.clear();
			threads.clear();
		}

		// pops a functional wrapper to the original function
		/**
		 * @brief An advanced utility function for the user to pop next function
		 * 
		 * @return std::function<void(int)> 
		 */
		std::function<void(int)> pop() {
			std::function<void(int id)> * f_ptr = Q.pop();
			std::unique_ptr<std::function<void(int id)>> func(f_ptr); // at return, delete the function even if an exception occurred
			std::function<void(int)> f;
			if (f_ptr) f = *f_ptr;	//For exception safety
			return f;
		}
		
		/**
		 * @brief Inserts a function into the pool which will run when a thread is available
		 * 
		 * @tparam Func_t Type of func (can be any Callate type)
		 * @tparam Param_t Parameter pack type for Arguments
		 * @param func Callable to be executed
		 * @param params Parameter pack for arguments
		 * @return a std::future object pertaining to the passed function
		 */
		template <class Func_t, class... Param_t>
		auto push(Func_t&& func, Param_t&&... params)
		{
			auto func_pack = std::make_shared< std::packaged_task<decltype(func(7, params...))(int)> > (
				std::bind(std::forward<Func_t>(func) , std::placeholders::_1 , std::forward<Param_t>(params)...  )
			);
			auto f = new std::function<void(int)>(
						[func_pack](int id) { (*func_pack)(id); }
					);

			Q.push(f);
			detail::autoRAII_lock lock(mutex);
			cv.notify_one();
			return func_pack->get_future();
		}

		/**
		 * @brief Overload for a Callable with 0 arguments
		 * 
		 * Has simpler method to increase speed
		 */
		template <class Func_t>
		auto push(Func_t&& func)
		{
			auto func_pack = std::make_shared< std::packaged_task<decltype(func(7))(int)> > (
				std::forward<Func_t>(func)
			);
			auto f = new std::function<void(int)>(
						[func_pack](int id) { (*func_pack)(id); }
					);
			Q.push(f);
			detail::autoRAII_lock lock(mutex);
			cv.notify_one();
			return func_pack->get_future();
		}

	private:

		/**
		 * @brief Configures the thread on creation. Assigns it the function
		 * 
		 * @param i The index of thread to be configured
		 */
		void configure_thread(int i) 
		{
			auto func = [this, i, &flag = *flags[i]]() {
				std::function<void(int id)> * f;
				bool popped = this->Q.pop(f);
				while (true)		/// Infinite loop which takes next function from ATomic Queue and executes it
				{
					while (popped) {	// if there is anything in the queue
						std::unique_ptr<std::function<void(int id)>> func(f); // Exception safety for memory leak
						(*f)(i);		// then execute it
						if (flag) return;  // the thread is to be stopped, return even if the queue is not empty yet
						else
							popped = this->Q.pop(f);	//Try to pop next element
					}

					std::unique_lock<std::mutex> lock(this->mutex);
					++this->nIdle;
					this->cv.wait(lock, [this, &f, &popped, &flag](){
											popped = this->Q.pop(f);
											return popped || this->isDone || flag ;
										});	// Waits until either a new function was popped
											// or it is commanded to stop.
					--this->nIdle;
					if (!popped)
						return;  // if the queue is empty and this->isDone == true or *flag then return
				}
			};
			threads[i].reset(new std::thread(func));
		}

	private:
		std::mutex mutex;
		std::condition_variable cv;

		std::atomic<bool>  isDone {false} ;
		std::atomic<bool>  isStop {false} ;
		std::atomic<int> nIdle {0} ;

		detail::Atomic_Queue<std::function<void(int id)> *> Q;

		std::vector <std::unique_ptr<std::thread>> threads;
		std::vector <std::shared_ptr<std::atomic<bool>>> flags;
	};

}

#endif