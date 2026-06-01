#include <iostream>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>

using namespace std;
class ThreadPool {
	explict ThreadPool(int n);
	~ThreadPool();
	future<void> submit(function<void()> f);
	void shutdown();
	private:
		vector<thread> vt;
		vector<function<void()> funcq;
		mutex m;
		bool shutoff;
		condition_variable cv;
};

void dowork() {
	while (shutoff == false || !funcq.empty()) {
		unique_lock<mutex> lock;
		cv.wait(lock, !funcq.empty());
		function<void()> w(move(funcq.front()));
		funcq.pop_front();
		w();
	}
}

/*
 * Have a pool of n threads waiting to grab functions to execute
 * Have a vector of queued functions to execute
 * Pool consumes functions while there are function otherwise waits
 */
ThreadPool::ThreadPool(int n) {
	shutoff = false;
	for (int i = 0; i < n; i++) {
		thread t(dowork);	
		vt.emplace_back(move(t));
	}
}

future<void> submit(function<void()> f) {
	if (shutoff == true) return;
	lock_guard<mutex> lg{m};
	funcq.emplace_back(move(f));
	packaged_task<void()> pt = funcq.back();
	future<void> r = pt.get_future();
	cv.notify_all();
	return r;
}

void ThreadPool::Shutdown() {
	shutoff = true;
}

ThreadPool::~ThreadPool() {
	unique_lock<mutex> lock;
	cv.wait(lock, funcq.empty());
	for (int i = 0; i < vt.size(); i++) {
		vt[i].join();
	}
}
