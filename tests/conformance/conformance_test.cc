#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>
#include "message_sint32.pb.h"

using namespace std;

int add(int x, int y)
{
    return x + y;
}

TEST(AddTest, Test1)
{
    chobie::github::io::Field message;
    chobie::github::io::Field r;


    message.set_v_double(3.14);
    message.set_v_float(3.14);
    message.set_v_int64(0x7FFFFFFFFFFFFFFF);//0x8000000000000000
    message.set_v_uint64(0xFFFFFFFFFFFFFFFF);
    message.set_v_int32(0x7FFFFFFF);//0x80000000
    message.set_v_fixed64(0xFFFFFFFFFFFFFFFF);
    message.set_v_fixed32(0xFFFFFFFF);
    message.set_v_bool(true);

    message.set_v_uint32(0xFFFFFFFF);
    message.set_v_sfixed32(0x7FFFFFFF);
    message.set_v_sfixed64(0x7FFFFFFFFFFFFFFF);
    message.set_v_sint32(0x7FFFFFFF);
    message.set_v_sint64(0x7FFFFFFFFFFFFFFF);
    message.set_v_string("Hello World");

    string result;

    message.SerializeToString(&result);
    fstream input("result.bin", ios::in | ios::binary);
    //r.ParseFromString(result);
    r.ParseFromIstream(&input);


    cout << "double: " << r.v_double() << endl;
    cout << "float: " << r.v_float() << endl;
    cout << "int64: " << r.v_int64() << endl;
    cout << "uint64: " << r.v_uint64() << endl;
    cout << "int32: " << r.v_int32() << endl;
    cout << "fixed64: " << r.v_fixed64() << endl;
    cout << "fixed32: " << r.v_fixed32() << endl;
    cout << "bool: " << r.v_bool() << endl;
    cout << "uint32: " << r.v_uint32() << endl;
    cout << "sfixed32: " << r.v_sfixed32() << endl;
    cout << "sfixed64: " << r.v_sfixed64() << endl;
    cout << "sint32: " << r.v_sint32() << endl;
    cout << "sint64: " << r.v_sint64() << endl;
    cout << "string: " << r.v_string() << endl;

    fstream output("test.bin", ios::out | ios::trunc | ios::binary);

    if (!message.SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
    }

    ASSERT_EQ(2, add(1, 1));
}
