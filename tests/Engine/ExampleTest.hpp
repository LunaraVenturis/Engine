#include <gtest/gtest.h>

TEST(ExampleTest, ExampleTest) { EXPECT_EQ(1, 1); }

TEST(ExampleTest, ExampleTestFail) { EXPECT_EQ(1, 0); }