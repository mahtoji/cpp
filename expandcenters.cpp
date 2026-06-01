void palins(string s) {
	int count = 0;
	for (int i = 0; i < s.size(); i++) {
		count += expand(s, i, i);   //odd size palins
		count += expand(s, i, i+1); //even size palins
	}
	return count;
}

int expand(string s, int l, int r) {
	int count = 0;
	while (l >= 0 && r < s.size() && s[l] == s[r]) {
		count += 1;
		l -= 1;
		r += 1;
	}
	return count;
}
