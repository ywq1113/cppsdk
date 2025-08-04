#include "algorithm_v1/segment_tree.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace algorithem_v1;

TEST(SegmentTreeTest, BasicQuery) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    SegmentTree st(data);

    EXPECT_EQ(st.query(0, 4), 15); // 1+2+3+4+5
    EXPECT_EQ(st.query(1, 3), 9);  // 2+3+4
    EXPECT_EQ(st.query(2, 2), 3);  // 单元素
}

TEST(SegmentTreeTest, SingleUpdate) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    SegmentTree st(data);

    st.update(2, 1, 3); // [1,3] 每个元素+2 => [1,4,5,6,5]
    EXPECT_EQ(st.query(0, 4), 21);
    EXPECT_EQ(st.query(1, 3), 15);
}

TEST(SegmentTreeTest, MultipleUpdates) {
    std::vector<int> data = {0, 0, 0, 0, 0};
    SegmentTree st(data);

    st.update(1, 0, 2); // => [1,1,1,0,0]
    st.update(3, 1, 4); // => [1,4,4,3,3]
    EXPECT_EQ(st.query(0, 4), 15);
    EXPECT_EQ(st.query(1, 2), 8);
    EXPECT_EQ(st.query(3, 4), 6);
}

TEST(SegmentTreeTest, EdgeCases) {
    std::vector<int> data = {5};
    SegmentTree st(data);

    EXPECT_EQ(st.query(0, 0), 5);

    st.update(10, 0, 0);
    EXPECT_EQ(st.query(0, 0), 15);

    // 测试超出范围的查询
    EXPECT_EQ(st.query(1, 5), 0);
}

TEST(SegmentTreeBugTest, LazyPushDownWrongParam) {
    std::vector<int> data = {1, 1, 1, 1};
    SegmentTree seg(data);

    // 区间[1,3]每个元素+5
    seg.update(5, 1, 3);

    // 期望 19
    int result = seg.query(0, 3);
    EXPECT_EQ(result, 19) << "Lazy propagation parameter issue triggered!";
}

TEST(SegmentTreeBugTest, LazyPushDownTrigger) {
    std::vector<int> data = {1, 1, 1, 1};
    SegmentTree seg(data);

    // 先对整个区间更新，触发lazy标记
    seg.update(5, 0, 3);  // root lazy=5

    // 再查询子区间，迫使push_down执行
    int result = seg.query(1, 3);  
    EXPECT_EQ(result, 18) << "Lazy propagation parameter issue triggered!";
}
