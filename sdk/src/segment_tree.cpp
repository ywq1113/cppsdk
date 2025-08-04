#include "algorithm_v1/segment_tree.h"

namespace algorithem_v1 {

SegmentTree::SegmentTree(const std::vector<int> &data) {
  size = data.size();
  tree.assign(size * 4, 0);
  lazy.assign(size * 4, 0);
  build(data, 1, 0, size - 1);
}

void SegmentTree::build(const std::vector<int> &data,
                        int node,
                        int start,
                        int end) {
  if (start == end /*叶子节点*/) {
    tree[node] = data[start];
    return;
  }
  auto mid = start + ((end - start) >> 1);
  build(data, node * 2, start, mid);
  build(data, node * 2 + 1, mid + 1, end);
  tree[node] = tree[node * 2] + tree[node * 2 + 1];
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
    return tree[node];
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
    tree[2 * node] += diff * (mid - l + 1);
    lazy[2 * node] += diff;
    // Update right
    tree[2 * node + 1] += diff * (r - mid);
    lazy[2 * node + 1] += diff;
    // Clear current node
    lazy[node] = 0;
  }
}

void SegmentTree::update_range(
    int node, int start, int end, int diff, int l, int r) {
  // TODO: Think that why all nodes will not update when update_range
  // recursively called 不相交，剪枝
  if (r < start || l > end) {
    return;
  }

  // 一个区间内
  if (start >= l && end <= r) {
    tree[node] += (end - start + 1) * diff;
    lazy[node] += diff;
    return;
  }

  // 两个区间，更新自己，向下传递标记
  push_down(node, start, end);
  auto mid = start + ((end - start) >> 1);
  update_range(node * 2, start, mid, diff, l, r);
  update_range(node * 2 + 1, mid + 1, end, diff, l, r);
  // 回溯
  tree[node] = tree[node * 2] + tree[node * 2 + 1];
}

void SegmentTree::update(int diff, int l, int r) {
  update_range(1, 0, size - 1, diff, l, r);
}

} // namespace algorithem_v1
