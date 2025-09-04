#include <vector>

class TreeNode {
 public:
  int value;
  TreeNode *left;
  TreeNode *right;

  TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

class Tree {
 public:
  TreeNode *root;
  Tree(TreeNode *r) : root(r) {}

  std::vector<int> postOrderTraversal();
};
