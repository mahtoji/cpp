vector<string> InfixToPostix(string s) {
	vector<string> outbuf;
	stack<char> operators;
	for (auto c : s) {
		if (isalnum(c)) {
			outbuf.push_back(string(1, c));
		} else if (c == '(') {
			operators.push(string(1, c)); // <========== push char & not string
		} else if (c == ')') {
			while (operators.empty() == false && operators.top() != '(') {
				outbuf.push_back(string(1, operators.top()));
				operators.pop();
			}
			operators.pop();
		} else {
			while (operators.empty() == false && priority(c) <= operators.top()) {
				outbuf.push_back(string(1, operators.top()));
				operators.pop();
			}
			operators.push(string(1, c)); //sasa
		}
	}
	while (operators.empty() == false) { // <========== should be !=
		outbuf.push_back(string(1, operators.top()));
	}
	return outbuf; // <============ missing return
}

int priority(char c) {
	if (c == '*' || c == '/') return 2;
	if (c == '+' || c == '-') return 1;
	assert(false);
}

bool isoperator(string s) {
	return (s == "*" || s == "/" || s == "+" || s == "-");
}

int computePostFix(vector<string> vs) {
	stack<int> stk;
	for (s : vs) {
		if (isoperator(s) == false) {
			stk.push(stoi(s));
			continue;
		}
		int r = 0;
		auto op1 = stk.top();stk.pop();
		auto op2 = stk.top();stk.pop();
		if (s == "*") r = op1*op2;
		if (s == "/") r = op2/op1;
		if (s == "+") r = op1+op2;
		if (s == "-") r = op2-op1;
		stk.push(r);
	}
	assert(stk.size() == 1);
	return stk.top();
}
