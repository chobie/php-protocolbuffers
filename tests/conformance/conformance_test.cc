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
    chobie::github::io::Message_Sint32 message;
    message.set_sint32(-2147483648);

    cout << "Message Value: " << message.sint32() << endl;

    fstream output("test.bin", ios::out | ios::trunc | ios::binary);

    if (!message.SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
    }

    ASSERT_EQ(2, add(1, 1));
}
