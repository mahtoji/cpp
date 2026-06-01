#include <iostream>
#include <thread>
#include <future>

using namespace std;
using namespace std::chrono_literals;

int TestDrive(int& x) {
	throw runtime_error{"random runtime error"};
	return x + 1;
}

int main(int argc, char* argv[]) {
	int y = 10;
	future<int> r = async(std::launch::async, TestDrive, ref(y));
	try {
		r.get();
	} catch (runtime_error ex) {
		cout << ex.what();
	}
}
