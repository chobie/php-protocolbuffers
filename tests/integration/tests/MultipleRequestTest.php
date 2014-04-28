<?php

class MultipleRequestTest extends PHPUnit_Framework_TestCase
{
    public function testRequest()
    {
        $data = fastcgi_get_contents("localhost:9000/home/travis/build/chobie/php-protocolbuffers/tests/integration/src/MultipleRequest.php");
        // NOTE(chobie): check segmentation fault
        $data = fastcgi_get_contents("localhost:9000/home/travis/build/chobie/php-protocolbuffers/tests/integration/src/MultipleRequest.php");
        list($_, $body) = preg_split("/\r?\n/", $data, 2);

        $user = \pecl\protocolbuffers\User::parseFromString($body);
        $this->assertInstanceOf('\pecl\protocolbuffers\User', \pecl\protocolbuffers\User);
        $this->assertEquals("12345", $user->getUserId());
        $this->assertEquals("chobie", $user->getName());
    }
}