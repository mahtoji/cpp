void diffArray(vector<int> v) {
	vector<int> diff(v.size(), 0);
	diff[0] = v[0];
	for (int i = 1; i < v.size(); i++) {
		diff[i] = v[i] - v[i-1];
	}
}

void update(int l, int r, int c) {
	diff[l] += c;
	if (r + 1 < diff.size()) {
		diff[r+1] -= c;
	}
}

vector<int> getResult() {
	vector<int> r(v.size(), 0);
	r[0] = diff[0];
	for (int i = 1; i < diff.size(); i++) {
		r[i] = r[i-1] + diff[i];
	}
}
