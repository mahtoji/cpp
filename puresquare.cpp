bool isSquare(vector<vector<int>> points) {
	set<vector<int>> sp; //set of points
	copy(points.begin(), points.end(), inserter(sp, sp.end()));
	if (sp.size() < 4) return false;
	vector<int> d2;//vector of distances squared
	for (int i = 1; i < points.size(); i++) {
		auto x = (points[0][0] - points[i][0]);
		x = x*x;
		auto y = (points[0][1] - points[i][1]);
		y = y*y;
		auto d = x + y;//square of distance
		d2.push_back(d);
	}
	sort(d2.begin(), d2.end());
	if (d2[0] == d2[1] && d2[2] == (d2[0]+d2[1])) {
		return true;
	}
	return false;
}
