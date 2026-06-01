class BuddySystem {
	int len;
	struct Node {
		bool flag;
		Node *left, *right, *parent;
	};
	Node *root;
	vector<Node*> leaf;
	BuddySystem(int sz);
	void SetBit(int beg, int len);
	void ClearBit(int beg, int len);
}

BuddySystem::BuddySystem(int sz) {
	len = 1;
	while (len < sz) len = len << 1;
	leaf.resize(len);
	for (int i = 0; i < len; i++) leaf[i] = new Node();
	//build the tree towards the root
	queue<Node*> q;
	copy(leaf.begin(), leaf.end(), back_inserter(q));
	while (q.size() > 1) { //while we have not reached the root
		//pick two nodes at a time
		auto f = q.front();q.pop();
		auto s = q.front();q.pop();
		//build a parent & connect it
		auto p = new Node();
		p->left = f;
		p->right= s;
		f->parent = s->parent = p;
		//stick p on to the q for later processing
		q.push(p);
	}
	//the last node is the root;
	root = q.front();q.pop();
	root->parent = nullptr;
}

void BuddySystem::SetBit(int beg, int len) {
	for (int i = beg; i < beg+len; i++) {
		leaf[i]->flag = true;
		//climb up and set parents accordingly
		auto p = leaf[i]->parent;
		while (p != nullptr) {
			p->flag = p->left->flag & p->right->flag;
			p = p->parent;
		}
	}
}

void BuddySystem::ClearBit(int beg, int len) {
	for (int i = beg; i < beg+len; i++) {
		leaf[i]->flag = false;
		//climb up and set parents accordingly
		auto p = leaf[i]->parent;
		while (p != nullptr) {
			p->flag = p->left->flag | p->right->flag;
			p = p->parent;
		}
	}
}
