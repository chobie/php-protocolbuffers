#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

using namespace std;

int add(int x, int y)
{
    return x + y;
}

TEST(AddTest, Test1)
{
    ASSERT_EQ(2, add(1, 1));
}
