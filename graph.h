struct Edge {
	int y;
	int w;
	Edge *next;
};

#define MAXV 1000


struct Graphs {
	int MAXVERTICES;
	int MAXEDGES;
	bool directed;
	Edge* edges[MAXV];
};

