#include "algorithm_v1/tree.h"

#include <stack>

std::vector<int> Tree::postOrderTraversal() {
  if (root == nullptr) {
    return {};
  }
  std::vector<int> res;
  std::stack<TreeNode *> st;
  TreeNode *last = nullptr;
  TreeNode *cur = root;
  while (cur != nullptr || !st.empty()) {
    while (cur != nullptr) {
      st.push(cur);
      cur = cur->left;
    }
    cur = st.top();
    if (cur->right != nullptr && last != cur->right) {
      cur = cur->right;
    } else {
      st.pop();
      res.push_back(cur->value);
      last = cur;
      cur = nullptr;
    }
  }
  return res;
}
