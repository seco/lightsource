<?php
use JsonRPC\Exception\AccessDeniedException;
use JsonRPC\Exception\AuthenticationFailureException;
use JsonRPC\Exception\ResponseException;
use JsonRPC\MiddlewareInterface;
use JsonRPC\Response\HeaderMockTest;
use JsonRPC\Server;
require_once ("vendor/autoload.php");

$server = new Server();
$procedureHandler = $server->getProcedureHandler();
$procedureHandler->withCallback('lightsource.init', function()
{
	return json_decode(file_get_contents("config.json"));
});
$procedureHandler->withCallback('lightsource.testConfig', function($params)
{
	return true;
});
echo $server->execute();
?>