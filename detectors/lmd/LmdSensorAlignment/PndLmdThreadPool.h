/*
 * @author Sehe (https://stackoverflow.com/users/85371/sehe)
 *
 * Thread Pool implementation using boost::threads.
 *
 * taken from this StackOverflow answer:
 * https://stackoverflow.com/questions/22685176/boost-group-threads-maximal-number-of-parallel-thread
 *
 */

#include <boost/thread.hpp>
#include <boost/phoenix.hpp>
#include <boost/optional.hpp>

//using namespace boost;
using namespace boost::phoenix::arg_names;

class PndLmdThreadPool {
private:
	boost::mutex mx;
	boost::condition_variable cv;

	typedef boost::function<void()> job_t;
	std::deque<job_t> _queue;

	boost::thread_group pool;

	boost::atomic_bool shutdown;
	static void worker_thread(PndLmdThreadPool& q) {
		while (boost::optional<job_t> job = q.dequeue())
			(*job)();
	}

public:
	//create thread pool with maximum possible threads
	PndLmdThreadPool() :
			shutdown(false) {
		for (unsigned i = 0; i < boost::thread::hardware_concurrency(); ++i)
			pool.create_thread(boost::bind(worker_thread, boost::ref(*this)));
	}

	//create thread pool with maxThreads threads
	PndLmdThreadPool(unsigned int maxThreads) :
			shutdown(false) {
		if (maxThreads > boost::thread::hardware_concurrency() || maxThreads == 0) maxThreads =
		    boost::thread::hardware_concurrency();
		for (unsigned i = 0; i < maxThreads; ++i)
			pool.create_thread(boost::bind(worker_thread, boost::ref(*this)));
	}

	void enqueue(job_t job) {
		boost::lock_guard<boost::mutex> lk(mx);
		_queue.push_back(job);
		cv.notify_one();
	}

	boost::optional<job_t> dequeue() {
		boost::unique_lock<boost::mutex> lk(mx);
		namespace phx = boost::phoenix;
		cv.wait(lk, phx::ref(shutdown) || !phx::empty(phx::ref(_queue)));
		if (_queue.empty()) return boost::none;
		job_t job = _queue.front();
		_queue.pop_front();
		return job;
	}

	// wait for all threads to complete
	void wait() {
		shutdown = true;
		{
			boost::lock_guard<boost::mutex> lk(mx);
			cv.notify_all();
		}
		pool.join_all();
	}

	~PndLmdThreadPool() {
		shutdown = true;
		{
			boost::lock_guard<boost::mutex> lk(mx);
			cv.notify_all();
		}
		pool.join_all();
	}
};
