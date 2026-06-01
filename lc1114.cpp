#include <semaphore>
#include <thread>
#include <iostream>
using namespace std;

binary_semaphore S{0}, T{0};

void f() {
	cout << "F";
	S.release();
}
void s() {
	S.acquire();
	cout << "S";
	T.release();
}
void t() {
	T.acquire();
	cout << "T";
}

int main() {
	jthread a(f);
	jthread b(s);
	jthread c(t);
}
