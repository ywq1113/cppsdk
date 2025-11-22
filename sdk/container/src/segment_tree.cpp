#include "container/segment_tree.h"

namespace sdk {
namespace container {

SegmentTree::SegmentTree(const std::vector<int> &data) {
  size = data.size();
  tree_sum.assign(size * 4, 0);
  tree_max.assign(size * 4, 0);
  lazy.assign(size * 4, 0);
  build(data, 1, 0, size - 1);
}

void SegmentTree::build(const std::vector<int> &data, int node, int start,
                        int end) {
  if (start == end /*叶子节点*/) {
    tree_sum[node] = data[start];
    tree_max[node] = data[start];
    return;
  }
  auto mid = start + ((end - start) >> 1);
  build(data, node * 2, start, mid);
  build(data, node * 2 + 1, mid + 1, end);
  tree_max[node] = std::max(tree_max[node * 2], tree_max[node * 2 + 1]);
  tree_sum[node] = tree_sum[node * 2] + tree_sum[node * 2 + 1];
}

int SegmentTree::query(int l, int r) {
  return query_range(1, 0, size - 1, l, r);
}

int SegmentTree::query_range(int node, int l, int r, int ql, int qr) {
  // 无交集
  if (l > qr || r < ql) {
    return 0;
  }

  // Lazy update
  push_down(node, l, r);

  // 区间内
  if (l >= ql && r <= qr) {
    return tree_sum[node];
  }

  // 区间合并
  auto mid = l + ((r - l) >> 1);
  return query_range(node * 2, l, mid, ql, qr) +
         query_range(node * 2 + 1, mid + 1, r, ql, qr);
}

void SegmentTree::push_down(int node, int l, int r) {
  // TODO: Think why need to update tree and lazy togather
  if (lazy[node] != 0) {
    auto diff = lazy[node];
    auto mid = l + ((r - l) >> 1);
    // Update left
    tree_sum[2 * node] += diff * (mid - l + 1);
    lazy[2 * node] += diff;
    // Update right
    tree_sum[2 * node + 1] += diff * (r - mid);
    lazy[2 * node + 1] += diff;
    // Clear current node
    lazy[node] = 0;
  }
}

void SegmentTree::update_range(int node, int l, int r, int diff, int ql,
                               int qr) {
  // TODO: Think that why all nodes will not update when update_range
  // 当前节点与更新区间无交集，直接返回
  if (qr < l || ql > r) {
    return;
  }

  // 一个区间内
  if (l >= ql && r <= qr) {
    tree_sum[node] += (r - l + 1) * diff;
    lazy[node] += diff;
    return;
  }

  // 两个区间，更新自己，向下传递标记
  push_down(node, l, r);
  auto mid = l + ((r - l) >> 1);
  update_range(node * 2, l, mid, diff, ql, qr);
  update_range(node * 2 + 1, mid + 1, r, diff, ql, qr);
  // 回溯
  tree_sum[node] = tree_sum[node * 2] + tree_sum[node * 2 + 1];
}

void SegmentTree::update_range(int diff, int l, int r) {
  update_range(1, 0, size - 1, diff, l, r);
}

std::vector<int> SegmentTree::toArray() {
  std::vector<int> res(size);
  for (int i = 0; i < size; ++i) {
    res[i] = query(i, i);
  }
  return res;
}

} // namespace container
} // namespace sdk
