#include "threadpool.h"
#include <atomic>
#include <thread>
#include "gtest/gtest.h"


TEST(ThreadPoolTest, push_and_stop) {
    const int n = 100000;
    std::atomic<int> count{0};
    const int nthreads = 2*std::thread::hardware_concurrency();
    auto *tpool = thread_manager::ThreadPool::get_instance(nthreads);
    for(int i=0;i<n;++i) tpool->push([&count](int id,int x){++count;},1);
    tpool->stop(true);
    
    ASSERT_EQ (count, n);
}