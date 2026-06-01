#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>

using namespace std;
#define DLOG(x) cout << __LINE__ << x << endl;
vector<int> tokenize(string s, string delims) {
	size_t p = 0, np = 0;
	p = s.find_first_not_of(delims, 0);
	while (p != string::npos) {
		np = s.find_first_of(delims, p);
		tok = s.substr(p, np-p);
		while (np != string::npos) {
			rv.push_back(string(1, s[np]);
		}
		p = s.find_first_not_of(delims, np);
	}
}

int main() {
	vector<string> tokens;
	//assume first pass string has been cleaned of all non digit & arithmetic chars
	string s = "45+3+66+9-89+(33+32)+73";
	//tokenize arithmetic string expression
	size_t p = 0;
	p = s.find_first_of("0123456789", p);
	while (p != string::npos) { //I have a start of a number
		auto np = s.find_first_not_of("0123456789", p);
		auto tok = s.substr(p, np-p);
		if (tok.size()) {
			tokens.push_back(tok);
		}
		//grab the arithmetic char
		p = s.find_first_of("+-*/()", np);
		if (p != string::npos) {
			tok = string(1, s[p]);
			tokens.push_back(tok);
			p = p+1;
		}
	}
	copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, ","));
}

/*
 *"45+3+66+9-89+(33+32)+73"
   ^
     ^ (at this point it is non-digit)
     So reset p
      ^
       ^ (and find the next number and so on)
 */
