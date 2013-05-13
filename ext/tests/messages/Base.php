<?php
class Base
{
    const TYPE_PROPERTIES  = 'properties';
    const TYPE_REQUIRED    = 'required';
    const TYPE_TYPE        = 'type';
    const TYPE_OPTFLAGS    = 'optflags';
    const TYPE_DEFAULT     = 'default';
    const TYPE_MESSAGE     = 'message';
    const TYPE_ENUM        = 'enum';

    public $_properties = array();

    protected static $_scheme_cache    = array();
    protected static $use_scheme_cache = false;

    /**
     * @param array $attributes
     */
    public function __construct($attributes = array())
    {
        if (!is_array($attributes)) {
            throw new InvalidArgumentException("passed parameters is not an array.");
        }

        // first, create an empty box here.
        $rule = $this->setup();
        foreach ($rule[self::TYPE_PROPERTIES] as $key => $opts) {
            if (isset($opts[self::TYPE_OPTFLAGS][self::TYPE_DEFAULT])) {
                $this->_properties[$key] = $opts[self::TYPE_OPTFLAGS][self::TYPE_DEFAULT];
            } else {

                if (isset($rule[self::TYPE_PROPERTIES][$key][self::TYPE_TYPE])) {
                    switch ($rule[self::TYPE_PROPERTIES][$key][self::TYPE_TYPE])  {
                    case "int":
                    case "integer":
                        $this->_properties[$key] = 0;
                        break;
                    case "float":
                        $this->_properties[$key] = (double)0;
                        break;
                    case "double":
                        $this->_properties[$key] = (float)0;
                        break;
                    case "string":
                        $this->_properties[$key] = "";
                        break;
                    case "array":
                        $this->_properties[$key] = array();
                        break;
                    default:
                        $this->_properties[$key] = null;
                        break;
                    }
                } else {
                    $this->_properties[$key] = null;
                }
            }
        }

        if (is_array($attributes)) {
            foreach($attributes as $key => $value) {
                if (!isset($rule[self::TYPE_PROPERTIES][$key])) {
                    throw new RuntimeException("Unknown property {$key}");
                }

                $this->_properties[$key] = $value;
            }
        }
    }

    /**
     * get specified value
     *
     * NB: You should not use this function in your project.
     *     we highly recommend to use getXXXX method instead of this as it's easy to grep.
     *
     * @param $target
     * @return mixed
     * @throws Exception
     */
    public function get($target)
    {
        $rule = $this->setup();
        if (!isset($rule[self::TYPE_PROPERTIES][$target])) {
            throw new Exception("the property $target does not exist on " . get_class($this) . " class");
        }

        if (!array_key_exists($target, $this->_properties)) {
            throw new Exception("the property $target does not exist on " . get_class($this) . " class");
        }

        return $this->_properties[$target];
    }

    /**
     * magic method for get, count, add, is, set, clear operation.
     *
     * basically these operation are patchable. so you can reproduce by patcher.
     *
     * @param $key
     * @param $args
     *
     * @return mixed
     *
     * @throws RuntimeException
     * @throws InvalidArgumentException
     */
    public function __call($key, $args = array())
    {
        $actual_key = self::underscore($key);
        if (strpos($actual_key, "_") === false) {
            throw new RuntimeException("malformed magic method called. magic method have to be camerized method name. called {$key}");
        }

        list($method, $property) = explode("_", $actual_key, 2);


        switch ($method) {
        case "get":
            $rule = $this->setup();

            if (!isset($rule[self::TYPE_PROPERTIES][$property])) {
                throw new InvalidArgumentException("the property $property does not exist");
            }

            if (array_key_exists($property, $this->_properties)) {
                switch ($rule[self::TYPE_PROPERTIES][$property][self::TYPE_TYPE]) {
                case "float":
                    return (float)$this->_properties[$property];
                    break;
                case "double":
                    return (double)$this->_properties[$property];
                    break;
                case "int":
                case "integer":
                    return (int)$this->_properties[$property];
                default:
                    return $this->_properties[$property];
                }
            }

            break;
        case "set":
            $rule = $this->setup();

            if(count($args) != 1) {
                throw new InvalidArgumentException("the {$property} args must be an {$rule[self::TYPE_PROPERTIES][$property][self::TYPE_TYPE]} instance. too many arguments");
            }

            $arg = array_shift($args);
            if (!isset($rule[self::TYPE_PROPERTIES][$property])) {
                throw new InvalidArgumentException("the {$property} declaration does not exist");
            }

            $this->set($property, $arg);

            return $this->get($property);
            break;
        default:
            throw new RuntimeException("method {$key} does not exist.");
        }
    }

    /**
     * set specified value to the property
     *
     * NB: You should not use this function in your project.
     *
     * @param $target
     * @param $value
     * @throws Exception
     */
    public function set($target, $value) {
        $rule = $this->setup();

        if (!isset($rule[self::TYPE_PROPERTIES][$target])) {
            throw new Exception(sprintf("%s does not have {$target} property", get_class($this)));
        }

        switch ($rule[self::TYPE_PROPERTIES][$target][self::TYPE_TYPE]) {
        case "float":
            if (!is_float($value)) {
                $value = floatval($value);
            }
            break;
        case "double":
            if (!is_double($value)) {
                $value = doubleval($value);
            }
            break;
        case "int":
        case "integer":
            if (!is_int($value)) {
                $value = intval($value);
            }
            break;
        case "string":
            if (!is_string($value)) {
                $value = strval($value);
            }
            break;
        case "array":
            break;
        default:
            if (empty($value)) {
                return;
            }

            if (is_object($value)) {
                $value = clone $value;

                if (get_class($value) != $rule[self::TYPE_PROPERTIES][$target][self::TYPE_TYPE]) {
                    throw new InvalidArgumentException("the {$target} args must be an {$rule[self::TYPE_PROPERTIES][$target][self::TYPE_TYPE]} instance");
                }
            } else {
                throw new InvalidArgumentException("the {$target} args must be an {$rule[self::TYPE_PROPERTIES][$target][self::TYPE_TYPE]} instance");
            }

            // TODO: we need to check array here.
        }

        $this->_properties[$target] = $value;
    }

    /**
     * setup scheme
     *
     * @return mixed
     */
    public function setup()
    {
        $classname = get_class($this);
        if (!isset(self::$_scheme_cache[$classname])) {
            self::$_scheme_cache[$classname] = $this->_setup($classname);
        }

        return self::$_scheme_cache[$classname];
    }

    protected function _setup($classname, &$inherit = 0)
    {
        $parent_opts = array();

        if ($inherit > 5) {
            throw new RuntimeException("too many inheritance.");
        }

        do {
            $parent = get_parent_class($classname);
            if (!$parent) {
                break;
            }

            $parent_opts = $this->_setup($parent, $inherit);
        } while (!$parent);

        if (!isset(self::$_scheme_cache[$classname])) {
            $ref     = new ReflectionClass($classname);
            $comment = $ref->getDocComment();
            $opts    = self::parseDocCommentForScheme($comment);

            $real_opts = array();

            if (empty($parent_opts)) {
                $real_opts = $opts;
            } else {

                foreach ($parent_opts as $key => $value) {
                    $real_opts[$key] = $value;
                }

                foreach ($opts as $key => $property) {
                    if (!isset($real_opts[$key])) {
                        $real_opts[$key] = $property;
                    } else {
                        switch ($key) {
                        case self::TYPE_PROPERTIES:

                            foreach ($property as $k => $rule) {
                                if (isset($rule[self::TYPE_TYPE])) {
                                    if (isset($real_opts[$key][$k][self::TYPE_TYPE])
                                        && $real_opts[$key][$k][self::TYPE_TYPE] != $rule[self::TYPE_TYPE]) {
                                        throw new RuntimeException("mismatched parent property.");
                                    } else {
                                        $real_opts[$key][$k][self::TYPE_TYPE] = $rule[self::TYPE_TYPE];
                                    }
                                }

                                if (isset($rule['options'])) {
                                    $real_opts[$key][$k]['options'] = $rule['options'];
                                }

                                if (isset($rule[self::TYPE_OPTFLAGS])) {
                                    foreach ($rule[self::TYPE_OPTFLAGS] as $kk => $vv) {
                                        $real_opts[$key][$k][self::TYPE_OPTFLAGS][$kk] = $vv;
                                    }
                                }
                            }
                            break;
                        case self::TYPE_REQUIRED:

                            foreach ($property as $rule) {
                                if (!in_array($rule, $real_opts[$key])) {
                                    $real_opts[$key][] = $rule;
                                }
                            }

                            break;
                        }
                    }
                }
            }
            self::$_scheme_cache[$classname] = $real_opts;
        }

        return self::$_scheme_cache[$classname];
    }

    public static function parseDocCommentForScheme($comment)
    {
        $length  = strlen($comment);
        $capture = false;
        $first_char = false;
        $offset = 0;

        $opts = array(
            self::TYPE_PROPERTIES  => array(),
            self::TYPE_REQUIRED    => array(),
        );

        for ($offset = $i = 0; $i < $length; $i++) {
            $char = $comment[$i];
            if (!$first_char) {
                for (; $i < $length; $i++) {
                    if (!($comment[$i] == "/"
                        || $comment[$i] == "*"
                        || $comment[$i] == " "
                        || $comment[$i] == "\n"
                        || $comment[$i] == "\r"
                        || $comment[$i] == "\t"
                    )) {
                        $first_char = true;
                        $i--;
                        break;
                    }
                }
            } else {
                if ($char == "\r" || $char == "\n") {
                    $first_char = false;
                    continue;
                }

                if ($char == '@') {
                    $offset = $i;
                    for (; $i< $length; $i++) {
                        if ($comment[$i] == "\r" || $comment[$i] == "\n") {
                            break;
                        }
                    }

                    $line = substr($comment, $offset, $i - $offset);
                    $length2 = strlen($line);
                    $key = null;


                    for ($x = 0; $x < $length2; $x++) {
                        if (is_null($key)) {
                            if ($line[$x] == " " || $line[$x] == "\t") {
                                $key = substr($line, 0, $x);
                            }
                        } else {
                            switch ($key) {
                            case "@method":
                                break;
                            case "@property":
                                $offset3 = $offset2 = $x;
                                $parts = array();
                                $count = 0;

                                for (; $x < $length2 && $count < 2; $x++) {
                                    if ($line[$x] == " " || $line[$x] == "\t") {
                                        $offset3 = $x;
                                        for (; $x < $length2; $x++) {
                                            if (!($line[$x] == " " || $line[$x] == "\t")) {
                                                break;
                                            }
                                        }
                                        $parts[] = substr($line, $offset2, $offset3 - $offset2);
                                        $offset2 = $x;
                                        $count++;
                                    }
                                }

                                if ($offset2 > $offset3) {
                                    $parts[] = substr($line, $offset2);
                                }

                                $opts[self::TYPE_PROPERTIES][substr($parts[1], 1)] = array(
                                    self::TYPE_TYPE => $parts[0],
                                    "options" => (isset($parts[2])) ? $parts[2] : null,
                                );

                                if (isset($parts[2])) {
                                    $optlen = strlen($parts[2]);
                                    $optmsg = $parts[2];
                                    $optflags = array();
                                    $optkey = null;
                                    for ($y = 0; $y < $optlen; $y++) {
                                        if ($optmsg[$y] == ':') {
                                            for ($yy = $y; $yy < $optlen; $yy++) {
                                                if ($optmsg[$yy] == " " || $optmsg[$yy] == "=" || $optmsg[$yy] == "," || $yy+1 == $optlen) {
                                                    if ($yy+1 == $optlen) {
                                                        $optkey = substr($optmsg, $y+1, $yy - $y);
                                                    } else {
                                                        $optkey = substr($optmsg, $y+1, $yy - ($y+1));
                                                    }
                                                    $y = $yy -1;
                                                    break;
                                                }
                                            }
                                        } else if ($optkey && $optmsg[$y] == '=' && $optmsg[$y+1] == '>') {
                                            for ($yy = $y; $yy <= $optlen; $yy++) {
                                                if (isset($optmsg[$yy]) && $optmsg[$yy] == " ") {
                                                    $y = $yy+1;
                                                    continue;
                                                }

                                                if (isset($optmsg[$yy]) && $optmsg[$yy] == "," || $yy == $optlen) {
                                                    $optval = substr($optmsg, $y, $yy - $y);
                                                    switch ($optkey) {
                                                    case "min":
                                                    case "max":
                                                        $optval = (int)$optval;
                                                        break;
                                                    case "default":
                                                        if ($optval == "null") {
                                                            $optval = null;
                                                        }
                                                        if ($optval == "true") {
                                                            $optval = true;
                                                        }
                                                        if ($optval == "false") {
                                                            $optval = false;
                                                        }
                                                        if ($optval == "[]") {
                                                            $optval = array();
                                                        }

                                                        switch ($parts[0]) {
                                                        case "int":
                                                        case "integer":
                                                            if (!is_null($optval)) {
                                                                $optval = (int)$optval;
                                                            }
                                                        }
                                                    }
                                                    $optflags[$optkey] = $optval;
                                                    $optkey = null;
                                                    $y = $yy - 1;
                                                    break;
                                                }
                                            }

                                        } else if ($optmsg[$y] == ' ') {
                                            // nothing to do.
                                        }
                                    }

                                    if ($optkey) {
                                        $optflags[$optkey] = true;
                                    }

                                    if($optflags) {
                                        $opts['properties'][substr($parts[1], 1)]['optflags'] = $optflags;
                                        if (isset($optflags['required'])) {
                                            $opts["required"][] = substr($parts[1], 1);
                                        }

                                        $optflags = array();
                                    }
                                }
                                break;
                            }

                            $key = null;
                        }
                    }
                }
            }
        }

        return $opts;
    }

    /**
     * convert cameraized string to underscore string
     *
     * @param $str
     * @return string
     */
    public static function underscore($str)
    {
        return strtolower(preg_replace('/(?!^)[A-Z]/', '_$0', $str));
    }

    /**
     * convert underscore string to camerized string
     *
     * @param $str
     * @return mixed
     */
    public static function camelize($str)
    {
        return str_replace(' ', '', ucwords(str_replace('_', ' ', $str)));
    }


    public static function generateDescriptor($class)
    {
        $proto = $class::getDescriptor();
        $map = array(
            self::TYPE_DOUBLE   => "double",
            self::TYPE_FLOAT    => "float",
            self::TYPE_INT64    => "int64",
            self::TYPE_UINT64   => "uint64",
            self::TYPE_INT32    => "int32",
            self::TYPE_FIXED64  => "fixed64",
            self::TYPE_FIXED32  => "fixed32",
            self::TYPE_BOOL     => "bool",
            self::TYPE_STRING   => "string",
            self::TYPE_GROUP    => "group",
            self::TYPE_MESSAGE  => "",
            self::TYPE_BYTES    => "bytes",
            self::TYPE_UINT32   => "uint32",
            self::TYPE_ENUM     => "",
            self::TYPE_SFIXED32 => "sfixed32",
            self::TYPE_SFIXED64 => "sfixed64",
            self::TYPE_SINT32   => "sint32",
            self::TYPE_SINT64   => "sint64",
        );

        $injection = array();
        foreach ($proto as $tag => $opts) {
            $line = array();

            if ($opts['repeated']) {
                $line[] = "repeated";
            }

            if ($opts['required']) {
                $line[] = "required";
            } else {
                $line[] = "optional";
            }

            if ($opts['type'] == self::TYPE_MESSAGE) {
                $line[] = $opts['message'];
            } else if ($opts['type'] == self::TYPE_ENUM) {
                $line[] = $opts['enum'];
            } else {
                $line[] = $map[$opts['type']];
            }

            $line[] = $opts['name'];
            $line[] = "= " . $tag;

            $injection[] = "    " . join(" ", $line) . ";";
        }
        $injection = join("\r\n", $injection);

        $template = <<<EOF
message $class
{
$injection
}

EOF;
        return $template;
    }
}