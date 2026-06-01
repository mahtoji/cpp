#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;
using namespace chrono_literals;
using namespace this_thread;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define DLOG(x) cout << GREEN << __LINE__ << " " << __FUNCTION__ << " " << RESET << x << endl;

mutex m;

void exclusive_throw(exception_ptr& e) {
	try {
	unique_lock ul{m};
	DLOG("");
	throw 123;
	} catch (...) {
		e = current_exception();
	}
}

void func(int i){ 
	lock_guard<mutex> lg(m);
	DLOG("Get lock");
	DLOG( "Here I am" << " " << i);
	sleep_for(10s);
	DLOG("Drop lock");
}

int main(int arg, char** argv) {
	try {
		exception_ptr e;
		auto t = thread(exclusive_throw, ref(e));
		t.join();
		if (e) {
			DLOG("Handled exception from thread");
		}
	} catch (int e) {
		DLOG("Got exception " << e);
	}
}
