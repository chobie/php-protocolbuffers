--TEST--
Check for protocol buffers unknown fields implementations
--FILE--
<?php
class UnknownFields extends ProtocolBuffersMessage
{
    protected static $descriptor;

    protected $value;

    protected $_unknown;

    /**
     * get descriptor for protocol buffers
     *
     * @return array
     */
    public static function getDescriptor()
    {
        if (!isset(self::$descriptor)) {
            $desc = new ProtocolBuffersDescriptorBuilder();
            $phpoptions = $desc->getOptions()->getExtension(ProtocolBuffers::PHP_MESSAGE_OPTION);
            $phpoptions->setProcessUnknownFields(true);
            self::$descriptor = $desc->build();
        }

        return self::$descriptor;
    }
}

$bytes = file_get_contents(dirname(__FILE__) . DIRECTORY_SEPARATOR . "fixtures" . DIRECTORY_SEPARATOR . "003_has_message.bin");

$data = ProtocolBuffers::decode("UnknownFields", $bytes);
$unk = $data->getUnknownFieldSet();
if ($unk instanceof ProtocolBuffersUnknownFieldSet) {
	$u = $unk->getField(1);
	foreach($u->getAsLengthDelimitedList() as $d) {echo $d . PHP_EOL;};
	$u = $unk->getField(2);
	foreach($u->getAsVarintList() as $d) {echo $d . PHP_EOL;};
} else {
	var_dump($u);
}
$data->discardUnknownFields();
$unk = $data->getUnknownFieldSet();
echo $unk->count();
--EXPECT--
chobie
20
0