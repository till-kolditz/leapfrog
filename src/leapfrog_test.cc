#include <vector>

#include <gtest/gtest.h>

#include <leapfrog.hpp>

using std::vector;

class LinearIteratorTest : public testing::Test {
protected:
  vector<int> tab0 = {};
  vector<int> tab1 = {0, 1, 3, 4, 5, 6, 7, 8, 9, 11};
  vector<int> tab2 = {0, 2, 6, 7, 8, 9, 11};
  vector<int> tab3 = {2, 4, 5, 8, 10};
};

TEST_F(LinearIteratorTest, Init0) {
  auto iter = LinearIterator{tab0};
  EXPECT_TRUE(iter.at_end());
}

TEST_F(LinearIteratorTest, Init1) {
  auto iter = LinearIterator{tab1};
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 0);
}

TEST_F(LinearIteratorTest, Init2) {
  auto iter = LinearIterator{tab2};
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 0);
}

TEST_F(LinearIteratorTest, Init3) {
  auto iter = LinearIterator{tab3};
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 2);
}

TEST_F(LinearIteratorTest, Next1) {
  auto iter = LinearIterator{tab1};
  iter.next();
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 1);
}

TEST_F(LinearIteratorTest, Next2) {
  auto iter = LinearIterator{tab2};
  iter.next();
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 2);
}

TEST_F(LinearIteratorTest, Next3) {
  auto iter = LinearIterator{tab3};
  iter.next();
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 4);
}

TEST_F(LinearIteratorTest, Seek1) {
  auto iter = LinearIterator{tab1};
  iter.seek(2);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 3);
  iter.seek(3);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 3);
  iter.seek(8);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 8);
  iter.seek(10);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 11);
  iter.seek(12);
  EXPECT_TRUE(iter.at_end());
}

TEST_F(LinearIteratorTest, Seek2) {
  auto iter = LinearIterator{tab2};
  iter.seek(1);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 2);
  iter.seek(4);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 6);
  iter.seek(8);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 8);
  iter.seek(10);
  ASSERT_FALSE(iter.at_end());
  EXPECT_EQ(iter.key(), 11);
  iter.seek(12);
  EXPECT_TRUE(iter.at_end());
}

#ifndef NDEBUG
using LinearIteratorDeathTest = LinearIteratorTest;

TEST_F(LinearIteratorDeathTest, Next0) {
  auto iter = LinearIterator{tab0};
  EXPECT_DEATH(iter.next(), "iter != source->end\\(\\)");
}

TEST_F(LinearIteratorDeathTest, Seek0) {
  auto iter = LinearIterator{tab0};
  EXPECT_DEATH(iter.seek(3), "iter != source->end\\(\\)");
}
#endif

using LeapFrogJoinTest = LinearIteratorTest;

TEST_F(LeapFrogJoinTest, Init2) {
  auto join = LeapFrogJoin<int>{tab1, tab2};
  ASSERT_FALSE(join.at_end());
  for (int x : {0, 6, 7, 8, 9, 11}) {
    EXPECT_EQ(join.key(), x);
    join.next();
  }
  EXPECT_TRUE(join.at_end());
}

TEST_F(LeapFrogJoinTest, Init3) {
  auto join = LeapFrogJoin<int>{tab1, tab2, tab3};
  ASSERT_FALSE(join.at_end());
  EXPECT_EQ(join.key(), 8);
  join.next();
  EXPECT_TRUE(join.at_end());
}
