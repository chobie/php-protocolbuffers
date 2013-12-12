#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include "../protoc-gen-php.h"
#include "php_options.pb.h"

using namespace std;
using namespace google::protobuf::compiler::php;

TEST(UnderscoresToCamelCaseImplTest, UnderscoresToCamelCaseImpl)
{
    string result = UnderscoresToCamelCaseImpl("helo_world", true);
    ASSERT_EQ("HeloWorld", result);

    result = UnderscoresToCamelCaseImpl("helo_world", false);
    ASSERT_EQ("heloWorld", result);
}
