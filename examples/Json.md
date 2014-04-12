# JsonSerialize specification

## Types

Basically, json serialize implementation respects protocol buffers serialization rule.

| PB Type     | Json Type |
|-------------|-----------|
|TYPE_DOUBLE  | NUMBER    |
|TYPE_FLOAT   | NUMBER    |
|TYPE_INT64   | NUMBER    |
|TYPE_UINT64  | NUMBER    |
|TYPE_INT32   | NUMBER    |
|TYPE_FIXED64 | NUMBER    |
|TYPE_FIXED32 | NUMBER    |
|TYPE_BOOL    | BOOLEAN   |
|TYPE_STRING  | STRING    |
|TYPE_BYTES   | STRING    |
|TYPE_UINT32  | NUMBER    |
|TYPE_ENUM    | NUMBER    |
|TYPE_SFIXED32| NUMBER    |
|TYPE_SFIXED64| NUMBER    |
|TYPE_SINT32  | NUMBER    |
|TYPE_SINT64  | NUMBER    |
|TYPE_MESSAGE | OBJECT    |

* JsonSerialize does not serialize NULL value.
* JsonSerialize uses original field name (same as proto file) as property key.
* JsonSerialize might loose precision when using big number. this is PHP limitation.

### Error Handling

* jsonSerialize throws an exception when serializing invalid protocol buffers object (e.g lacks some required field).

## Unknown Field

* does not support.

## Extensions

* supported

## Overriding

`jsonSerialize` method can override. you can filter needless values with overriding jsonSerialize method.

```
public function jsonSerialize()
{
    $array = parent::jsonSerialize();
    return $this->filter_needless_values($array);
}

```

## Others

* `__sleep` and `__wakeup` method doesn't invoke when serializing json.
