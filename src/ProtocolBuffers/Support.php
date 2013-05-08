<?php
/**
 * ProtocolBuffers_Support
 *
 * @author Shuhei Tanuma <chobieee@gmail.com>
 */
class ProtocolBuffers_Support
{
    const TYPE_PROPERTIES  = 'properties';
    const TYPE_REQUIRED    = 'required';
    const TYPE_TYPE        = 'type';

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
}