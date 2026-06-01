void solve(vector<int> v) {
	int n = v.size();
	vector<int> dp(n, 0);
	dp[0] = 1;//init base condition

	for (int i = 1; i < n; i++) {
		dp[i] = max(dp[i-1], v[i]);
	}

	return dp[n-1];
}
