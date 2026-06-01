class UnionFind {
	vector<int> arr;
	vector<int> rank;

	UnionFind(int n) {
		arr.resize(n);
		rank.resize(n);
		for (int i = 0; i < arr.size(); i++) {
			arr[i] = i;
		}
		for (int i = 0; i < rank.size(); i++) {
			rank[i] = 0;
		}
	}

	int Find(int i) {
		if (arr[i] == i) return i;
		auto root = Find(arr[i]);
		arr[i] = root;
		return arr[i];
	}

	void Union(int x, int y) {
		int rootx = Find(x);
		int rooty = Find(y);

		if (rootx == rooty) return;

		if (rank[rootx] > rank[rooty]) {
			arr[rooty] = rootx;
		} else if (rank[rootx] < rank[rooty]) {
			arr[rootx] = rooty;
		} else {
			arr[rootx] = rooty;
			rank[y] += 1;
		}
	}

	bool isconnected(int x, int y) {
		return FindParent(x) == FindParent(y);
	}
};
