#include "threadpool.h"
#include "atomic_queue.h"

int main()
{
	detail::Atomic_Queue<int> a;
	a.push(1);
}