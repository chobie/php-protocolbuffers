<?php

class ProtocolBuffers_Packer_BinaryPackerTest
    extends PHPUnit_Framework_TestCase
{
    public function testPackTag()
    {
        $packer = new ProtocolBuffers_Packer_BinaryPacker();

        $packed = $packer->packTag(1, ProtocolBuffers::WIRETYPE_VARINT);
        $this->assertEquals(0x08, $packed);

        $packed = $packer->packTag(1, ProtocolBuffers::WIRETYPE_FIXED64);
        $this->assertEquals(0x09, $packed);


        $packed = $packer->packTag(1, ProtocolBuffers::WIRETYPE_LENGTH_DELIMITED);
        $this->assertEquals(0x0a, $packed);

        $packed = $packer->packTag(1, ProtocolBuffers::WIRETYPE_FIXED32);
        $this->assertEquals(0x0d, $packed);
    }
}