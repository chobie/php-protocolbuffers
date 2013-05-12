<?php
class Base
{
    const TYPE_PROPERTIES  = 'properties';
    const TYPE_REQUIRED    = 'required';
    const TYPE_TYPE        = 'type';
    const TYPE_OPTFLAGS    = 'optflags';
    const TYPE_DEFAULT     = 'default';

    public $_properties = array();

    protected static $_scheme_cache = array();
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
        $actual_key = ProtocolBuffers_Support::underscore($key);
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
        if (self::$use_scheme_cache && empty(self::$_scheme_cache)) {
            self::$_scheme_cache = require dirname(__FILE__) . DIRECTORY_SEPARATOR . "__scheme.php";
        }

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
            $opts    = ProtocolBuffers_Support::parseDocCommentForScheme($comment);

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

}