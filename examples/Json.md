# JsonSerialize specification

## Ini

| key                                           | TYPE | Default | Description                                            |
|-----------------------------------------------|------|---------|--------------------------------------------------------|
| protocolbuffers.json_serializer.respect_name? | BOOL | TRUE    | use field name as object key. otherwise use tag number |

## Types

Basically, json serialize implementation respects protocol buffers serialization rule.

| PB Type     | Json Type |
|-------------|-----------|
|TYPE_DOUBLE  | STRING    |
|TYPE_FLOAT   | STRING    |
|TYPE_INT64   | STRING    |
|TYPE_UINT64  | STRING    |
|TYPE_INT32   | STRING    |
|TYPE_FIXED64 | STRING    |
|TYPE_FIXED32 | STRING    |
|TYPE_BOOL    | BOOLEAN   |
|TYPE_STRING  | STRING    |
|TYPE_BYTES   | STRING    |
|TYPE_UINT32  | STRING    |
|TYPE_ENUM    | STRING    |
|TYPE_SFIXED32| STRING    |
|TYPE_SFIXED64| STRING    |
|TYPE_SINT32  | STRING    |
|TYPE_SINT64  | STRING    |
|TYPE_MESSAGE | OBJECT    |

* client have to parse numerical value by theirselves.

### Error Handling

* jsonSerialize throws an exception when serializing invalid protocol buffers object (e.g lacks some required field).

## Unknown Field

* (considering) respects descriptor builder setting. or don't support unknown field.

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


* should support custom serializer class?