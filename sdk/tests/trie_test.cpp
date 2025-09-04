#include "trie.hpp" // 你的 Trie 定义头文件

#include <gtest/gtest.h>

class TrieTest : public ::testing::Test {
 protected:
  Trie trie;
};

TEST_F(TrieTest, InsertAndFind) {
  trie.insert("apple");
  EXPECT_TRUE(trie.find("apple"));
  EXPECT_FALSE(trie.find("app"));
  EXPECT_TRUE(trie.starts_with("app"));
}

TEST_F(TrieTest, InsertPrefixAsWord) {
  trie.insert("apple");
  trie.insert("app");
  EXPECT_TRUE(trie.find("app"));
}

TEST_F(TrieTest, NonExistentWord) {
  trie.insert("apple");
  EXPECT_FALSE(trie.find("banana"));
  EXPECT_FALSE(trie.starts_with("ban"));
}

TEST_F(TrieTest, CommonPrefix) {
  trie.insert("apple");
  trie.insert("application");
  EXPECT_TRUE(trie.find("application"));
  EXPECT_TRUE(trie.starts_with("appl"));
}

TEST_F(TrieTest, EmptyString) {
  trie.insert("apple");
  EXPECT_FALSE(trie.find(""));
  EXPECT_TRUE(trie.starts_with(""));
}
