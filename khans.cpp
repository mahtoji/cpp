bool FindCycle(Graph *g) {
	queue<int> q;
	int indeg[MAXVERTICES];
	for (int i = 0; i < g->MAXVERTICES; i++) {
		p = g->edges[i];
		whle (p != nullptr) {
			auto y = p->y;
			indeg[y] += 1;
		}
	}
	for (int i = 0; i < g->MAXVERTICES; i++) {
		if (indeg[i] == 0) {
			q.push(i);
		}
	}

	while (q.empty() == false) {
		auto f = q.front();q.pop();
		l2r.push(f);
		auto p = g->edges[f];
		while (p != nullptr) {
			int y = p->y;
			indeg[y] -= 1;
			if (indeg[y] == 0) {
				q.push(y);
			}
		}
	}

	if (l2r.size() == g->MAXVERTICES) return false;
	return true;
}


//It is topological sort
//Applies only to directed graph
//When removing vertex, dec where ever it's outgoing edges go to
