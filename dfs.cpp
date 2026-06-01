void dfs(int v) {
	if (finished == true) return;
	discovered[v] = true;
	t = t + 1;
	entry_time[v] = t;
	process_vertex_early(v);

	q = g->edges[v];
	wile (q != nullptr) {
		auto y = q->y;
		if (!discovered[y]) {
			parent[y] = v;
			process_edge(v, y);
			dfs(y);
		} else if (!processed[y] && parent[v] != y || g->directed) {
			process_edge(v, y);
		}
		q = q->next;
	}
	process_vertex_late(v);
	t = t + 1;
	exit_time[v] = t;
	processed[v] = true;
}
