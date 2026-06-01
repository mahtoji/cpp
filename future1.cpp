#include <future>
#include <iostream>
#include <thread>
using namespace std;

int main() {
	packaged_task<void(int)> task([](int a){ cout << "Here I am " << a << endl;});
	future<void> f = task.get_future();
	jthread jt(move(task), 5000);
	f.get();
}
