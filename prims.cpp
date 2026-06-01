int Prims(Graph* g, int start) {
	vector<bool> intree(nvertices, false);
	vector<int> distance(nvertices, INT_MAX);
	vector<int> parent(nvertices, -1);

	int v = start;
	distance[v] = 0;
	int total = 0;
	while (intree[v] == false) {
		intree[v] = true;
		total += dist;
		p = g->edges[v];
		while (p ! = nullptr) {
			int y = p->y;
			int w = p->w;
			if (intree[y] == false && distance[y] > w) {
				distance[y] = w;
				parent[y] = v;
			}
			p = p->next;
		}
		int dist = INT_MAX;
		for (int i = 0; i < nvertices; i++) {
			if (intree[i] == false && dist > distance[i]) {
				dist = distance[i];
				v = i;
			}
		}
	}
	return total;
}

/*
 * dijkstra's adjustmemnt
 * if (intree[y] == false && dist[v] + p->weight < dist[y]
 * 	dist[y] = dist[v] + p->weight;
 */
