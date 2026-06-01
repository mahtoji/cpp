vector<char> infixToPostfix(string s) {
	vector<char> output;
	stack<char> operators;
	for (auto c : s) {
		if (isalnum(c)) { //is isalnum
			output.push_back(c)
		} else if (c == '(') {
			operators.push(c); //string(1, c)
		} else if (c == ')') {
			while (opertors.top() != '(') {
				output.push_back(operators.top());
				opeartors.pop();
			}
			opearators.pop();
		} else {
			while (operators.empty() == false && (priority(c) <= priority(opeators.top()))) {
				output.push_back(operators.top());
				opeartors.pop();
			}
			operators.push(c);
		}
	}
	while (operators.empty() == false) {
		output.push_back(operators.top());
		operators.pop();
	}
}

int processPostFix(vector<char> v) {
	stack<int> r;
	for (auto e : v) {
		if (notoperator(e)) {
			r.push(string(c, 1));
			continue;
		}
		switch (c) {
			case '+':
				op1 = r.top();r.pop();
				op2 = r.top();r.pop();
				res = op1 + op2;
				r.push(res);
				break;
			case '-':
			case '*':
			case '/':
				op1 = r.top();r.pop();
				op2 = r.top();r.pop();
				res = op1/op2;
				r.push(res);
				break;
		}

	}
	return r.top();
}
