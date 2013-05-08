Yet Another Protocol Buffers for PHP
====================================

Motivation
----------

there are several protocol buffers implementation on php. but PHP really poor about decoding pb messeage.
especially reading `varint` is the bottleneck as it have to be call `ord` function each bytes.
(PHP's calling function is one of big cost. but we can't convert byte to int without `ord` or some function. as far as i known)

this library will provide really fast decoding pb message.
pure php part is just understanding `protocol buffers` implementation for me.

Status
------

Under Development (this means you can't play my pb ext)

License
-------

MIT License

Suggestions
-----------

- if you wanna use pb on php. you should use drslump's Protobuf-PHP (https://github.com/drslump/Protobuf-PHP)