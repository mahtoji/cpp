#include <sstream>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

bool IsPure(int n) {
	stringstream ss;
	ss << n;
	string s = ss.str();
	int len = s.size();
	//calculate the purity
	int tot = 0;
	for (auto c : s) {
		int i = stoi(string(1, c));
		int p = pow(i, len);
		tot += p;
	}
	return (tot == n);
}

int main(int argc, char* argv[]) {
	cout << IsPure(143) << endl;
}
