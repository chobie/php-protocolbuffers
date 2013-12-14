#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include "../php/php_generator.h"
#include "../php/strutil.h" // TODO This header is from the offical protobuf source, but it is not normally installed
#include "php_options.pb.h"

using namespace std;
using namespace google::protobuf::compiler::php;

TEST(UnderscoresToCamelCaseImplTest, UnderscoresToCamelCaseImpl)
{
    ASSERT_EQ("heloWorld", "heloWorld");
}
