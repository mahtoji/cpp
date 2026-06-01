int EditDist(string s1, string s2, int l1, int l2) {
	if (l1 == 0) return l2;
	if (l2 == 0) return l1;

	if (s1[l1-1] == s2[l2-1]) {
		return EditDist(s1, s2, l1-1, l2-1);
	}
	auto r1 = EditDist(s1, s2, l1-1, l2-1);//replace
	auto r2 = EditDist(s1, s2, l1-1, l2); //delete a char in s1
	auto r3 = EditDist(s1, s2, l1, l2-1); //add a char in s1
	return 1 + min<int>({r1, r2, r3});
}
