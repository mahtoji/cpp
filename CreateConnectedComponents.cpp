void CreateConnected(Graph *g) {
	int c = 0;
	for (int i = 0; i < g->MAXVERTICES; i++) {
		if (discovered[i] == false) {
			bfs(g, i, c);
		}
	}
}

bool bipartite = true;

typedef enum COLOR {
	UNCOLORED,
	BLACK,
	WHITE
};

void CreateBipartite(Graph *g) {
	//make all uncolored
	for (int i = 0; i < g->MAXVERTICES; i++) {
		COLOR[i] = UNCOLORED;
	}

	for (int i = 0; i < g->MAXVERICES; i++) {
		if (!discovered[i]) {
			COLOR[i] = WHITE;
			bfs(g, i);
		}
	}
}

void process_edge(int x, int y) {
	if (COLOR[x] == UNCOLORED || COLOR[x] == COLOR[y]) {
		finished = true;
		bipartite = false;
		return;
	}
	COLOR[y] = Complement(x);
}
