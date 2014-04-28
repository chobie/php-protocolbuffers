<?php

class MultipleRequestTest extends PHPUnit_Framework_TestCase
{
    public function testRequest()
    {
        $data = fastcgi_get_contents("localhost:9000/home/travis/build/chobie/php-protocolbuffers/tests/integration/src/MultipleRequest.php");
        $data = fastcgi_get_contents("localhost:9000/home/travis/build/chobie/php-protocolbuffers/tests/integration/src/MultipleRequest.php");

        $this->assertRegExp("/Hello/", $data);
    }
}