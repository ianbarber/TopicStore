<?php

$context = new ZMQContext();
$socket = new ZMQSocket($context, ZMQ::SOCKET_REQ);
$socket->connect("tcp://localhost:9292");
$socket->sendMulti(array("TOPS01_GET", "test"));
var_dump($socket->recvMulti());
$socket->sendMulti(array("TOPS01_ADD", "PUB", "test", time()+(24*60*60), "ipc://test", "ipc://test2"));
var_dump($socket->recvMulti());
$socket->sendMulti(array("TOPS01_GET", "test"));
var_dump($socket->recvMulti());
