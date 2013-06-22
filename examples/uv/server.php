<?php
require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . "proto" . DIRECTORY_SEPARATOR . "message.proto.php";

class KeyValueStore
{
    protected $store = array();

    public function __construct()
    {
    }

    public function count()
    {
        return count($this->store);
    }

    public function set($key, $value)
    {
        $this->store[$key] = $value;
    }

    public function get($key)
    {
        $result = NULL;

        if (isset($this->store[$key])) {
            $result = $this->store[$key];
        }
        return $result;
    }
}

$store = new KeyValueStore();

$tcp = uv_tcp_init();
uv_tcp_bind6($tcp, uv_ip6_addr('::1',9999));
echo "# Server listening to ::1 9999\n";

uv_listen($tcp, 100, function($server) use ($store){
    $client = uv_tcp_init();
    uv_accept($server, $client);
    echo "# Accepted\n";

    uv_read_start($client, function($socket, $nread, $buffer) use ($server, $store){
        try {
            $result = null;
            $request = ProtocolCached_Request::parseFromString($buffer);

            switch ($request->getType()) {
            case ProtocolCached_Request_RequestType::SET:
                echo sprintf("# set operation: %d, %d\n", $request->getSet()->getKey(), strlen($request->getSet()->getValue()));
                $store->set($request->getSet()->getKey(), $request->getSet()->getValue());
                var_dump($store);
                var_dump($request);
                break;
            case ProtocolCached_Request_RequestType::GET:
                $result = $store->get($request->getGet()->getKey());
                break;
            }

        } catch (ProtocolBuffers\InvalidProtocoBufferException $e) {
            echo $e->getMessage();
        }

        if ($result) {
            $response = new ProtocolCached_Response();
            $response->setType(ProtocolCached_Response_ResponseType::GET);
            $payload = new ProtocolCached_GetResponse();
            $payload->setValue($result);
            $response->setGet($payload);

            uv_write($socket, $response->serializeToString(), function($socket, $stat) {
                echo "# Connection closed\n";
                uv_close($socket);
            });
        } else {
            echo "# Connection closed\n";
            uv_close($socket);
        }
    });
});

$timer = uv_timer_init();

$stat = array();
$stat['begin'] = memory_get_usage();

uv_timer_start($timer, 10, 1000, function($stat, $timer) use (&$stat, $store){
    $stat["current"] = memory_get_usage();
    printf("# memory: %d\n", $stat["current"] - $stat['begin']);
    printf("# collection size: %d\n", $store->count());
});

uv_run();