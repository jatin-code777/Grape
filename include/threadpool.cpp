#include "threadpool.h"
#include "atomic_queue.h"

int main()
{
	Atomic_Queue<int> a;
	a.push(1);
}