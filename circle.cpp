vector<vector<int>> reflect(vector<int> p) {
	int x = p[0];
	int y = p[1];
	return {{x, y}, {y, x}, {-y, x}, {x, -y}, {-x, -y}, {-y, -x}, {y, -x}, {-x, y} }; //<======reflection
}

vector<vector<int>> drawCircle(int r, int cx, int cy) {
	vector<vector<int>> v;
	//to begin with assume circle is at 0,0 & later adjust to cx, cy
	//Consider one octant and reflect it all other octants
	int sx = 1, sy = r;//starting x & y for the 0,0 center & circumference //<========= start from 1, r. The 0, r is taken care of later.
	for (; sx <= sy; sx++) {
		//corresponding sy, will at most drop by 1, for each sx increment of 1
		auto sy1 = sy;
		auto sy2 = sy1-1;
		auto d1 = sx*sx + sy1*sy1;
		auto d2 = sx*sx + sy2*sy2;
		vector<int> soln;
		if (abs(d1 - r*r) < abs(d2 - r*r)) {
			soln = {sx, sy1};
		} else {
			soln = {sx, sy2};
		}
		sy = soln[1];//update sy to last known good coordinate <===== use y from previous chosen coordinate
		//reflect it to other octants
		auto octet = reflect(soln);
		copy(octet.begin(), octet.end(), back_inserter(v));
	}
	v.push_back({0,r});
	v.push_back({r,0});
	v.push_back({0,-r});
	v.push_back({-r,0});
	//adjust coordinates to cx, cy center
	for (auto& e : v) {
		e[0] += cx;
		e[1] += cy;
	}
	return v;
}
