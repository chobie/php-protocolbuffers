<?php
spl_autoload_register(function($name){
  static $classmap;
  if (!$classmap) {
    $classmap = array(
      'pecl\protocolbuffers\User' => '/pecl/protocolbuffers/User.php',
      // @@protoc_insertion_point(autoloader_scope:classmap)
    );
  }
  if (isset($classmap[$name])) {
    require __DIR__ . DIRECTORY_SEPARATOR . $classmap[$name];
  }
});

call_user_func(function(){
  $registry = \ProtocolBuffers\ExtensionRegistry::getInstance();
  // @@protoc_insertion_point(extension_scope:registry)
});
