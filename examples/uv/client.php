<?php
require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . "proto" . DIRECTORY_SEPARATOR . "message.proto.php";

$type = $_SERVER['argv'][1];
$key = $_SERVER['argv'][2];

if ($type == "SET") {
    $value = $_SERVER['argv'][3];
}


function getRequestType($type)
{
    $key = strtolower($type);
    switch ($key) {
    case "set":
        return ProtocolCached_Request_RequestType::SET;
        break;
    case "get":
        return ProtocolCached_Request_RequestType::GET;
        break;
    default:
        throw new Exception("GET or SET only");
    }
}

$c = uv_tcp_init();
uv_tcp_connect6($c, uv_ip6_addr('::1', 9999), function($client, $stat){
    if ($stat == 0) {
        $request = new ProtocolCached_Request();
        $type = getRequestType($_SERVER['argv'][1]);
        $request->setType($type);

        if ($type == ProtocolCached_Request_RequestType::SET) {
            $set = new ProtocolCached_SetRequest();
            $set->setKey($_SERVER['argv'][2]);
            $set->setValue($_SERVER['argv'][3]);

            $request->setSet($set);
        } else if ($type == ProtocolCached_Request_RequestType::GET) {
            $set = new ProtocolCached_GetRequest();
            $set->setKey($_SERVER['argv'][2]);

            $request->setGet($set);
        }

        uv_write($client, $request->serializeToString(),function($socket, $stat) use($type){
            if ($type == ProtocolCached_Request_RequestType::GET) {
                uv_read_start($socket, function($socket, $nread, $buffer) use ($type){
                    var_dump(ProtocolCached_Response::parseFromString($buffer));
                    uv_close($socket);
                });
            } else {
                uv_close($socket);
            }
        });
    }
});

uv_run();