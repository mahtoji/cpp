bool lca(Node *root, int l, int r) {
	if (root == nullptr) return nullptr;
	if (root->val == l) || (root->var == r) return root;

	auto lh = lca(root->left, l, r);
	auto rh = lca(root->right, l, r);

	if (lh != nullptr && rh != nullptr) return root;

	return (lh != nullptr) ? lh : rh;
}
