--TEST--
Check for protocol buffers decode implementations
--FILE--
<?php
/**
 * Tutorial_Bytes
 *
 * @message Tutorial.Bytes
 *
 * -*- magic properties -*-
 *
 * @property string $value
 */
class Tutorial_Bytes
{
  protected static $descriptor;

  public function getValue()
  {
    $result = null;

    if (array_key_exists('value', $this->_properties)) {
      $result = $this->_properties['value'];
    }

    return $result;
  }

  public function setValue($value)
  {
    $this->_properties['value'] = $value;
  }

  /**
   * get descriptor for protocol buffers
   *
   * @return array
   */
  public static function getDescriptor()
  {
    // return nothing.
  }
}


$b = new Tutorial_Bytes();
$droid  = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "android_logo.gif");
$b->setValue($droid);

try {
	ProtocolBuffers::encode($b);
} catch (ProtocolBuffers_InvalidProtocolBufferException $e) {
	echo "OK";
}

--EXPECT--
OK