<?php
require 'vendor/autoload.php';

use Adoy\FastCGI\Client;


function fastcgi_get_contents($url)
{
    $url = parse_url($url);
    if (!$url || !isset($url['path'])) {
        die("Malformed URI");
    }

    $req = '/'.basename($url['path']);
    if (isset($url['query'])) {
        $uri = $req .'?'.$url['query'];
    } else {
        $url['query'] = '';
        $uri = $req;
    }
    $client = new Client(
        (isset($url['host']) ? $url['host'] : 'localhost'),
        (isset($url['port']) ? $url['port'] : 9000));

    $params = array(
        'GATEWAY_INTERFACE' => 'FastCGI/1.0',
        'REQUEST_METHOD'    => 'GET',
        'SCRIPT_FILENAME'   => $url['path'],
        'SCRIPT_NAME'       => $req,
        'QUERY_STRING'      => $url['query'],
        'REQUEST_URI'       => $uri,
        'DOCUMENT_URI'      => $req,
        'SERVER_SOFTWARE'   => 'php/fcgiclient',
        'REMOTE_ADDR'       => '127.0.0.1',
        'REMOTE_PORT'       => '9985',
        'SERVER_ADDR'       => '127.0.0.1',
        'SERVER_PORT'       => '80',
        'SERVER_NAME'       => php_uname('n'),
        'SERVER_PROTOCOL'   => 'HTTP/1.1',
        'CONTENT_TYPE'      => '',
        'CONTENT_LENGTH'    => 0
    );

    return $client->request($params, false);
}