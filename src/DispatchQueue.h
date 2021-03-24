#ifndef _DISPATCH_QUEUE_
#define _DISPATCH_QUEUE_

#include <cstdint>
#include <cstdio>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>

class dispatch_queue {
	using fp = std::function<void(void)>;

public:
	dispatch_queue(size_t thread_cnt = 1);
	~dispatch_queue();

	// dispatch and copy
	void dispatch(const fp& op);
	// dispatch and move
	void dispatch(fp&& op);

	// Deleted operations
	dispatch_queue(const dispatch_queue& rhs) = delete;
	dispatch_queue& operator=(const dispatch_queue& rhs) = delete;
	dispatch_queue(dispatch_queue&& rhs) = delete;
	dispatch_queue& operator=(dispatch_queue&& rhs) = delete;

private:
	std::mutex m_lock;
	std::vector<std::thread> m_threads;
	std::queue<fp> m_queue;
	std::condition_variable m_cv;
	bool m_quit = false;

	void dispatch_thread_handler(void);
};

#endif // _DISPATCH_QUEUE_