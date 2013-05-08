<?php

class ProtocolBuffersTest
    extends PHPUnit_Framework_TestCase
{
    public function testProtocolBuffersConstants()
    {
        $this->assertEquals(0,  ProtocolBuffers::WIRETYPE_VARINT);
        $this->assertEquals(1,  ProtocolBuffers::WIRETYPE_FIXED64);
        $this->assertEquals(2,  ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED);
        $this->assertEquals(3,  ProtocolBuffers::WIRETYPE_START_GROUP);
        $this->assertEquals(4,  ProtocolBuffers::WIRETYPE_END_GROUP);
        $this->assertEquals(5,  ProtocolBuffers::WIRETYPE_FIXED32);

        $this->assertEquals(1,  ProtocolBuffers::TYPE_DOUBLE);
        $this->assertEquals(2,  ProtocolBuffers::TYPE_FLOAT);
        $this->assertEquals(3,  ProtocolBuffers::TYPE_INT64);
        $this->assertEquals(4,  ProtocolBuffers::TYPE_UINT64);
        $this->assertEquals(5,  ProtocolBuffers::TYPE_INT32);
        $this->assertEquals(6,  ProtocolBuffers::TYPE_FIXED64);
        $this->assertEquals(7,  ProtocolBuffers::TYPE_FIXED32);
        $this->assertEquals(8,  ProtocolBuffers::TYPE_BOOL);
        $this->assertEquals(9,  ProtocolBuffers::TYPE_STRING);
        $this->assertEquals(10, ProtocolBuffers::TYPE_GROUP);
        $this->assertEquals(11, ProtocolBuffers::TYPE_MESSAGE);
        $this->assertEquals(12, ProtocolBuffers::TYPE_BYTES);
        $this->assertEquals(13, ProtocolBuffers::TYPE_UINT32);
        $this->assertEquals(14, ProtocolBuffers::TYPE_ENUM);
        $this->assertEquals(15, ProtocolBuffers::TYPE_SFIXED32);
        $this->assertEquals(16, ProtocolBuffers::TYPE_SFIXED64);
        $this->assertEquals(17, ProtocolBuffers::TYPE_SINT32);
        $this->assertEquals(18, ProtocolBuffers::TYPE_SINT64);
    }

    public function testEncode()
    {
        $expected = file_get_contents(FIXTURE_PATH . DIRECTORY_SEPARATOR . "fixture001.bin");

        $u = new User();
        $u->setId(20);
        $u->setOffense(105);
        $u->setName("chobie");

        $u2 = new User();
        $u2->setId(21);
        $u2->setOffense(108);

        $u2->setName("charmy");

        $addr = new AddressBook();
        $addr->addUser($u);
        $addr->addUser($u2);

        $data = ProtocolBuffers::encode(AddressBook::getProto(), $addr);
        $this->assertSame($expected, $data);
    }

    public function testUnserialize()
    {
        $data = file_get_contents(FIXTURE_PATH . DIRECTORY_SEPARATOR . "fixture001.bin");
        $addr = ProtocolBuffers::decode(AddressBook::getProto(), "AddressBook", $data);

        $this->assertTrue($addr instanceof AddressBook, "addressbook should be AddressBook instance");
        $this->assertEquals(2, count($addr->getUser()), "user count should be 2");

        $users = $addr->getUser();
        $this->assertEquals(2, count($users), "users should be 2");
        $this->assertEquals(20, $users[0]->getId(), "user id should be 20");
        $this->assertEquals("chobie", $users[0]->getName(), "user name should be chobie");
        $this->assertEquals(21, $users[1]->getId(), "user id should be 21");
        $this->assertEquals("charmy", $users[1]->getName(), "user name should be charmy");
    }
}