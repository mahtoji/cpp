int binsearch(vector<int> v, int x) {
	int low = 0, hi = v.size() - 1;
	while (low <= hi) {
		auto mid = (low + hi)/2;
		if (v[mid] == x) return mid;
		if (v[mid] > x) {
			hi = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	return -1;
}

/*
 *  3 4 6, 5
 *  low = 0, hi = 2, mid = 1, v[1] < 5
 *  low = 2, hi = 2, mid = 2, v[1] < 5
 *  low = 3, hi = 2, exit loop
 *  3 4 6, 3
 *  low = 0, hi = 2, mid = 1, v[1] > 3
 *  low = 0, hi = 0, mid = 0, v[0] == 3
 */

//rotated bin search

int binrotated(vector<int> arry, int v) {
	int lo = 0, hi = arry.size() - 1;
	while (lo <= hi) {
		auto mid = (lo + hi)/2;
		if (arry[mid] == v) return mid;
		if (arry[low] < arry[mid]) {
			if (arry[low] <= v && v < arry[mid]) {
				hi = mid - 1;
			} else {
				lo = mid + 1;
			}
		} else {
			if (arry[hi] >= v && v > arry[mid]) {
				lo = mid + 1;
			} else {
				hi = mid - 1;
			}
		}
	}
}
