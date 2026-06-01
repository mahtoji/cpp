#include <shared_mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
using namespace std;
vector<jthread> jv;
shared_mutex sm;
int main() {
	auto w = [&]() {
	    unique_lock<shared_mutex> wl(sm);
	    cout << "w";
	};
	auto r = [&]() {
	    shared_lock<shared_mutex> rl(sm);
	    cout << "r";
	};
    for (int i = 0; i < 10; i++) {
        jv.emplace_back(w);
        jv.emplace_back(r);
    }
}

