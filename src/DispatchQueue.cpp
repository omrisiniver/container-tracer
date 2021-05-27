#include <thread>
#include <functional>
#include <vector>
#include <unistd.h>
#include "DispatchQueue.h"
#include "Tracer.hpp"

dispatch_queue::dispatch_queue(size_t thread_cnt) :
	m_threads(thread_cnt)
{
	printf("Dispatch threads: %zu\n", thread_cnt);

	for(size_t i = 0; i < m_threads.size(); i++)
	{
		m_threads[i] = std::thread(&dispatch_queue::dispatch_thread_handler, this);
	}
}

dispatch_queue::~dispatch_queue()
{
	printf("Destructor: Destroying dispatch threads...\n");

	// Signal to dispatch threads that it's time to wrap up
	std::unique_lock<std::mutex> lock(m_lock);
	m_quit = true;
	lock.unlock();
	m_cv.notify_all();

	// Wait for threads to finish before we exit
	for(size_t i = 0; i < m_threads.size(); i++)
	{
		if(m_threads[i].joinable())
		{
			printf("Destructor: Joining thread %zu until completion\n", i);
			m_threads[i].join();
		}
	}
}

void dispatch_queue::dispatch(const fp& op)
{
	std::unique_lock<std::mutex> lock(m_lock);
	m_queue.push(op);

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	m_cv.notify_all();
}

void dispatch_queue::dispatch(fp&& op)
{
	std::unique_lock<std::mutex> lock(m_lock);
	m_queue.push(std::move(op));

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	m_cv.notify_all();
}

void dispatch_queue::dispatch_thread_handler(void)
{
	std::unique_lock<std::mutex> lock(m_lock);

	do {
		//Wait until we have data or a quit signal
		m_cv.wait(lock, [this]{
			return (m_queue.size() || m_quit);
		});

		//after wait, we own the lock
		if(!m_quit && m_queue.size())
		{
			auto op = std::move(m_queue.front());
			m_queue.pop();

			//unlock now that we're done messing with the queue
			lock.unlock();

			op();

			lock.lock();
		}
	} while (!m_quit);
}

int main(void)
{
	int r = 0;
	int val = 3;
	int val2 = 5;

	dispatch_queue q(3);
	tracer<int> trace(q);

	bool res = trace.Init(std::string("/configProcs"));

	if (!res)
	{
		std::cout << "Failed in main" << std::endl;
	}

	trace.trace();

	sleep(2);
	return 0;
}
