#include <mutex>
#include <shared_mutex>
#include <condition_variable>

mutex m;
void ex1() {
	lock_guard<mutex> lg(m);
}

void ex2() {
	unique_lock<mutex> ul(m, defer_lock);
	ul.lock();
	ul.unlock();
}

shared_mutex sm;
void ex3() { 
	unique_lock<shared_mutex> wl(sm);//the writer
}

void ex4() {
	shared_lock<shared_mutex> rl(sm);//reader lock
}

bool CheckCondition() {
	if (flag == true) return true;
	return false;
}

void ex5() {
	condition_variable cv;
	unique_lock<mutex> ul(m);
	while (true) {
		cv.wait(ul, CheckCondition);
		//do something
		ul.unlock();
		cv.notify_all();
	}
}

//producer-consumer example using condition variable and unique locks
#define MAX 100
vector<int> v(MAX, -1);
int beg = 0, end = 0;//circular queue, end is where next entry lands, beg is where first entry to consume lives, beg == end means empty, beg = (end + 1) % MAX means full
condition_variable cv_producer, cv_consumer;
void producer() {
	unique_lock<mutex> ul(m);
	while (true) {
		cv_producer.wait(ul, [](){ return beg != (end+1) % MAX; });
		//add stuff
		v[end] = random();//put some random data
		end += 1;
		end = end % MAX;
		ul.unlock();
		cv_consumer.notify_all();
		ul.lock();
	}
}
void consumer() {
	unique_lock<mutex> ul(m);
	while (true) {
		cv_consumer.wait(ul, [](){ return beg != end; } );
		//remove stuff
		auto consumed = v[beg];
		beg += 1;
		beg = beg % MAX;
		ul.unlock();
		cv_producer.notify_all();
		ul.lock();
	}
}

//redoing shared circular queue with counting semaphore
counting_semaphore<MAX> slots_to_fill{MAX};
counting_semaphore<MAX> slots_to_empty{0};
void prod() {
	while (true) {
		slots_to_fill.acquire();
		{
			lock_guard<mutex> ul(m);
			v[end] = random();
			end = (end + 1) % MAX;
		}
		slots_to_empty.release();
	}
}
void cons() {
	while (true) {
		slots_to_empty.acquire();
		{
			lock_guard<mutex> ul(m);
			beg = (beg + 1) % MAX;
		}
		slots_to_fill.release();
	}
}
