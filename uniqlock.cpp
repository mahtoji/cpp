#include <thread>
#include <mutex>
#include <shared_mutex>
#include <iostream>

using namespace std;

mutex m;
int x = 10;
#define DLOG(x) cout << __LINE__ << ":" << __FUNCTION__ << " " << x << endl;

auto f1 = []() {
	for (int i = 0; i < 10; i++) {
		unique_lock<mutex> lg(m);
		x = x + 1;
		DLOG(this_thread::get_id() << " " << x);
		//this_thread::sleep_for(1ms);
	}
};
auto f2 = []() {
	for (int i = 0; i < 10; i++) {
		unique_lock<mutex> lg(m);
		x = x - 1;
		DLOG(this_thread::get_id() << " " << x);
		//this_thread::sleep_for(1ms);
	}
};

int main(int argc, char* argv[]){
	jthread jt1(f1);
	jthread jt2(f2);
}
