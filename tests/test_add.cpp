#include <gtest/gtest.h>
#include "add.h"

//testing successful addition
TEST(AddTest, Successful) {
  EXPECT_EQ(add(2,3), 5);
  EXPECT_EQ(add(-1,1), 0);
}

// testing adding failure
TEST(AddTest, Failure) {
  EXPECT_EQ(add(2,2), 5);
}

// Going to add some sample google test methods for reference later when testing game. (For reuse purposes)
TEST(GeneralTest, MoreTests) {
  EXPECT_NE(add(2,2), 5);
  EXPECT_LT(add(2,2), 5);
  EXPECT_GT(add(3,3), 5);
  EXPECT_LE(add(2,2), 4);
  EXPECT_GE(add(3,3), 5);
  EXPECT_TRUE(add(2,2) == 4);
  EXPECT_FALSE(add(2,2) == 5);
  EXPECT_NO_THROW(add(2,2));
  EXPECT_STREQ("hello", "hello");
  EXPECT_STRNE("hello", "world");
}
