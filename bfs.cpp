#include <vector>

struct Edge {
	int y;
	int w;
	Edge *next;
};

#define MAXV 1000;

Edge *edges[MAXV];

void bfs(int start) {
	vector<bool> discovered;
	vector<bool> processed;
	vector<int> parent;
	queue<int> pq;

	int v = start;
	pq.push(v);
	parent[v] = -1;
	discovered[v] = true; // <===================

	while (pq.empty() == false) {
		v = pq.front();pq.pop();
		process_vertex_early(v);
		processed[v] = true;
		auto q = edges[v];
		while (q != nullptr) {
			int y = q->y;
			if (processed[y] != true || g->directed) {
				process_edge(v, y);
			}
			if (!discovered[y]) {
				discovered[y] = true;
				parent[y] = v;
				pq.push(y);
			}
			if (finished) return;
			q = q->next;
		}
		process_vertex_late(v); // <=================
	}
}
