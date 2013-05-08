<?php
class ProtocolBuffers_Packer_BinaryPacker
{
    /**
     * @param $filed_number int Expected to be an integer in the range [1, 1 << 29)
     * @param $wiretype int One of the WIRETYPE_* constants.
     * @return int
     */
    public function packTag($filed_number, $wiretype)
    {
        //if (!(0 <= $filed_number))
        return ($filed_number << TAG_TYPE_BITS) | ($wiretype);
    }

    /*
     * The inverse of PackTag().  Given an unsigned 32-bit number,
     * @returns a (field_number, wire_type) array.
     */
    public function unpackTag($tag)
    {
        return array(($tag >> TAG_TYPE_BITS), ($tag & TAG_TYPE_MASK));
    }

//0	Varint	int32, int64, uint32, uint64, sint32, sint64, bool, enum
//1	64-bit	fixed64, sfixed64, double
//2	Length-delimited	string, bytes, embedded messages, packed repeated fields
//3	Start group	groups (deprecated)
//4	End group	groups (deprecated)
//5	32-bit	fixed32, sfixed32, float

    public function readVarint32($value)
    {
// Memo: another approach.
//    $i = 0;
//    $result = $shift = 0;
//    do {
//        $byte = ord($value[$i]);
//        $result |= ($byte & 0x7f) << $shift;
//        $shift += 7;
//        $i++;
//    } while ($byte > 0x7f);

        // protocol buffers extension
        //return pb_read_varint32($value);

        $b = ord($value[0]);$result  = ($b & 0x7F)      ;if (!($b & 0x80)) return $result;
        $b = ord($value[1]);$result |= ($b & 0x7F) << 7 ;if (!($b & 0x80)) return $result;
        $b = ord($value[2]);$result |= ($b & 0x7F) << 14;if (!($b & 0x80)) return $result;
        $b = ord($value[3]);$result |= ($b & 0x7F) << 21;if (!($b & 0x80)) return $result;
        $b = ord($value[4]);$result |= $b          << 28;if (!($b & 0x80)) return $result;

        // If the input is larger than 32 bits, we still need to read it all
        // and discard the high-order bits.
//    for ($i = 0; $i < kMaxVarintBytes - kMaxVarint32Bytes; $i++) {
//        $b = ord($value[$i]); if (!($b & 0x80)) goto done;
//    }

        return $result;
    }

    public function writeVarint32($value)
    {
        $bytes = "";
        if (is_string($value)) {
            error_log("force cast string to integer.");
            $value = (int)$value;
        }

        if ($value > 0xFFFFFF) {
            throw new OutOfBoundsException(sprintf("%d limit exceeds", $value));
        }

        if ($value <= 0x7F) {
            $bytes = chr($value);
        } else {
            while ($value > 0x7F) {
                $bytes .= chr($value & 0x7F | 0x80);
                $value >>= 7;
            }
            $bytes .= chr($value & 0x7F);
        }

        // NOTE: using call_user_func_array('pack', array_merge(array("C*"), $bytes)) requires 2x times..
        return $bytes;
    }

    public function writeVarint64($value)
    {
        $bytes = "";
        if (is_string($value)) {
            error_log("force cast string to integer.");
            $value = (int)$value;
        }

        if ($value > 0xFFFFFF) {
            throw new OutOfBoundsException(sprintf("%d limit exceeds", $value));
        }

        if ($value <= 0x7F) {
            $bytes = chr($value);
        } else {
            while ($value > 0x7F) {
                $bytes .= chr($value & 0x7F | 0x80);
                $value >>= 7;
            }
            $bytes .= chr($value & 0x7F);
        }

        // NOTE: using call_user_func_array('pack', array_merge(array("C*"), $bytes)) requires 2x times..
        return $bytes;
    }

    public function zigzag_encode32($n)
    {
        return ($n << 1) ^ ($n >> 31);
    }

    public function zigzag_decode32($n)
    {
        return ($n >> 1) ^ - ($n & 1);
    }
}