<?php
use JsonRPC\Exception\AccessDeniedException;
use JsonRPC\Exception\AuthenticationFailureException;
use JsonRPC\Exception\ResponseException;
use JsonRPC\MiddlewareInterface;
use JsonRPC\Response\HeaderMockTest;
use JsonRPC\Server;
require_once ("vendor/autoload.php");

class Api
{
    public function doSomething($arg1, $arg2 = 3)
    {
        return $arg1 + $arg2;
    }
}

$server = new Server();
$procedureHandler = $server->getProcedureHandler();

// Bind the method Api::doSomething() to the procedure myProcedure
$procedureHandler->withClassAndMethod('myProcedure', 'Api', 'doSomething');

// Use a class instance instead of the class name
$procedureHandler->withClassAndMethod('mySecondProcedure', new Api, 'doSomething');

// The procedure and the method are the same
$procedureHandler->withClassAndMethod('doSomething', 'Api');

// Attach the class, the client will be able to call directly Api::doSomething()
$procedureHandler->withObject(new Api());

echo $server->execute();
?>