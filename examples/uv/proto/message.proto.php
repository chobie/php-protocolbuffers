<?php
/**
 * ProtocolCached_Request_RequestType
 *
 * @enum ProtocolCached.Request.RequestType
 */
class ProtocolCached_Request_RequestType
{
  const GET = 1;
  const SET = 2;
}

/**
 * ProtocolCached_Request
 *
 * @message ProtocolCached.Request
 *
 */
class ProtocolCached_Request extends ProtocolBuffersMessage
{
  protected $type;

  protected $set;

  protected $get;


  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
      $desc = new ProtocolBuffersDescriptorBuilder();
      $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_ENUM,
        "name"     => "type",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => ProtocolCached_Request_RequestType::GET,
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_MESSAGE,
        "name"     => "set",
        "required" => false,
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => null,
        "message"  => "ProtocolCached_SetRequest",
      )));
      $desc->addField(3, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_MESSAGE,
        "name"     => "get",
        "required" => false,
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => null,
        "message"  => "ProtocolCached_GetRequest",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

/**
 * ProtocolCached_Response_ResponseType
 *
 * @enum ProtocolCached.Response.ResponseType
 */
class ProtocolCached_Response_ResponseType
{
  const GET = 1;
  const SET = 2;
}

/**
 * ProtocolCached_Response
 *
 * @message ProtocolCached.Response
 *
 */
class ProtocolCached_Response extends ProtocolBuffersMessage
{
  protected $type;

  protected $get;


  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
      $desc = new ProtocolBuffersDescriptorBuilder();
      $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_ENUM,
        "name"     => "type",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => ProtocolCached_Response_ResponseType::GET,
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_MESSAGE,
        "name"     => "get",
        "required" => false,
        "optional" => true,
        "repeated" => false,
        "packable" => false,
        "default"  => null,
        "message"  => "ProtocolCached_GetResponse",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

/**
 * ProtocolCached_SetRequest
 *
 * @message ProtocolCached.SetRequest
 *
 */
class ProtocolCached_SetRequest extends ProtocolBuffersMessage
{
  protected $key;

  protected $value;


  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
      $desc = new ProtocolBuffersDescriptorBuilder();
      $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "key",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $desc->addField(2, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "value",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

/**
 * ProtocolCached_GetRequest
 *
 * @message ProtocolCached.GetRequest
 *
 */
class ProtocolCached_GetRequest extends ProtocolBuffersMessage
{
  protected $key;


  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
      $desc = new ProtocolBuffersDescriptorBuilder();
      $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "key",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

/**
 * ProtocolCached_GetResponse
 *
 * @message ProtocolCached.GetResponse
 *
 */
class ProtocolCached_GetResponse extends ProtocolBuffersMessage
{
  protected $value;


  /**
   * get descriptor for protocol buffers
   * 
   * @return array
   */
  public static function getDescriptor()
  {
    static $descriptor;

    if (!isset($descriptor)) {
      $desc = new ProtocolBuffersDescriptorBuilder();
      $desc->addField(1, new ProtocolBuffersFieldDescriptor(array(
        "type"     => ProtocolBuffers::TYPE_BYTES,
        "name"     => "value",
        "required" => true,
        "optional" => false,
        "repeated" => false,
        "packable" => false,
        "default"  => "",
      )));
      $descriptor = $desc->build();
    }

    return $descriptor;
  }

}

