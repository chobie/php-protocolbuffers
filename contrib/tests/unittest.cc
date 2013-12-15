#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include "../google/protobuf/compiler/php/php_generator.h"
#include "php_options.pb.h"

using namespace std;
using namespace google::protobuf::compiler::php;

TEST(UnderscoresToCamelCaseImplTest, UnderscoresToCamelCaseImpl)
{
    ASSERT_EQ("heloWorld", "heloWorld");
}
