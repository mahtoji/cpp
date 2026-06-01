#include <iostream>
#include <thread>
#include <future>

using namespace std;

auto dowork = [](int i){cout << "I am here " << i << endl;};

int main(int argc, char* argv[]) {
	thread th(dowork, 1);
	th.join();

	future<void> f = async(dowork, 2);
	f.get();

	packaged_task<void(int)> task(dowork);
	future<void> r = task.get_future();
	thread t(move(task), 3);
	r.get();
	t.join();
}
