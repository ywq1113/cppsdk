#include <cstdint>
#include <vector>

namespace algorithem_v1 {

class SegmentTree {
 private:
  std::vector<int64_t> tree;
  std::vector<int64_t> lazy;
  int64_t size;

 public:
  SegmentTree(const std::vector<int> &data);

  int query(int l, int r);

  void update(int diff, int l, int r);

  /*
   * For debug
   */
  std::vector<int> toArray();

 private:
  /*
   * @brief Build segment tree recursively
   * @param const std::vector<int> &data
   * @param int node
   * @param int start, end
   */
  void build(const std::vector<int> &data,
             int node,
             int start,
             int end);

  /*
   * @brief Get sum of [l, r]
   * @param int node, node is index of tree
   * @param int start, end
   * @param int l, r
   * @return int
   */
  int query_range(int node, int l, int r, int ql, int qr);

  /*
   * @brief Update diff of [l, r]
   * @param int node, node is index of tree
   * @param int start, end, current node range
   * @param int diff
   * @param int l, r
   * @return void
   */
  void update_range(int node, int l, int r, int diff, int ql, int qr);

  void push_down(int node, int l, int r);
};

} // namespace algorithem_v1
