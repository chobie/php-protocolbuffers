--TEST--
Problems with numbers larger than 2^63-1
--FILE--
<?php
if (8 !== PHP_INT_SIZE) { die('Only for 64-bit platform'); }
class TestCase
{
  protected static $descriptor;
  protected $value;

  public function getValue()
  {
    return $this->value;
  }
  public function setValue($value)
  {
    $this->value = $value;
  }
  public static function getDescriptor()
  {
      if (!isset(self::$descriptor)) {
          $desc = new ProtocolBuffersDescriptorBuilder();
          $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
              "type"     => ProtocolBuffers::TYPE_FIXED64,
              "name"     => "value",
              "packable" => false,
              "repeated" => false,
              "default"  => null,
          )));

          self::$descriptor = $desc->build();
      }
      return self::$descriptor;
  }
}

// Pretty formatted debug_zval_dump
function debug($v) {
    ob_start();
    debug_zval_dump($v);
    $d = ob_get_contents();
    ob_end_clean();
    return str_replace(PHP_EOL,'',$d);
    return preg_replace('/ refcount\(\d+\)/','',str_replace(PHP_EOL,'',$d));
}

$testCases = array(
    "0",
    "1",
    "9223372036854775807",    // 2^63-1
    "9223372036854775808",    // 2^63
    "18446744073709550590",   // (2^64-1)-1024-1
    "18446744073709550591",   // (2^64-1)-1024
    "18446744073709551615",    // (2^64-1)
);
$names = array(
    'zero',
    'one',
    '9223372036854775807 aka 2^63-1',
    '9223372036854775808 aka 2^63',
    '18446744073709550590 aka (2^64-1)-1024-1',
    '18446744073709550591 aka (2^64-1)-1024',
    '18446744073709551615 aka (2^64-1)'
);

foreach ($testCases as $i => $t) {
    echo "\n== Testing ".($names[$i] ? $names[$i] : debug($t))." ==\n";
    $a = $t;
    $e = new TestCase();
    $e->setValue($t);

    // Encoding changes variable underneath, make a copy of the variable
    $copy = unserialize(serialize($t));

    $data = ProtocolBuffers::encode($e);
    if ($copy != $t) echo "Encoding changed variable value!\n";

    $d = ProtocolBuffers::decode("TestCase", $data);

    // Result?
    $r = '';
    if ($e->getValue() == $d->getValue() && $d->getValue() == $copy) {
        $r = 'OK';
    } else if ($d->getValue() == $copy) {
        $r = 'FAIL (Decoded match input)';
    } else {
        $r = 'FAIL';
    }
    printf("Result  : %s\n",$r);
}
--EXPECT--
== Testing zero ==
Result  : OK

== Testing one ==
Result  : OK

== Testing 9223372036854775807 aka 2^63-1 ==
Result  : OK

== Testing 9223372036854775808 aka 2^63 ==
Result  : OK

== Testing 18446744073709550590 aka (2^64-1)-1024-1 ==
Result  : OK

== Testing 18446744073709550591 aka (2^64-1)-1024 ==
Result  : OK

== Testing 18446744073709551615 aka (2^64-1) ==
Result  : OK