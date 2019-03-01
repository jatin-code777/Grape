#include "atomic_queue.h"
#include <thread>
#include "gtest/gtest.h"

TEST(AtomicQueueTest, parallel_push) {
    const int n = 100;
    std::thread threads[n];
    detail::Atomic_Queue <int> Q;
    bool b[n] = {};
	for(int i=0; i<n; ++i)
		threads[i] = std::thread([&Q,i](){Q.push(i);});
	for(auto &th:threads) th.join();
    while(!Q.empty()) b[Q.top()] = 1 , Q.pop();
    int count = 0;
    for(int i=0;i<n;++i) count += b[i];

    ASSERT_EQ (count, n);
}

TEST(AtomicQueueTest, parallel_pop) {
    const int n = 100;
    std::thread threads[n];
    detail::Atomic_Queue <int> Q;
    bool b[n] = {};

	for(int i=0; i<n; ++i)
        Q.push(i);

    for(int i=0;i<n;++i)
		threads[i] = std::thread([&Q,&b](){ b[Q.pop()] = 1;});
	for(auto &th:threads) th.join();

    int count = 0;
    for(int i=0;i<n;++i) count += b[i];

    ASSERT_EQ (count, n);
}