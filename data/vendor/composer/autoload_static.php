<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInit3ef6be2ef9a073761b5f7a2a09cddf55
{
    public static $prefixesPsr0 = array (
        'J' => 
        array (
            'JsonRPC' => 
            array (
                0 => __DIR__ . '/..' . '/fguillot/json-rpc/src',
            ),
        ),
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixesPsr0 = ComposerStaticInit3ef6be2ef9a073761b5f7a2a09cddf55::$prefixesPsr0;

        }, null, ClassLoader::class);
    }
}