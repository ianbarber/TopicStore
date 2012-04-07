<?php

$context = new ZMQContext();
$socket = new ZMQSocket($context, ZMQ::SOCKET_REQ);
$socket->connect("tcp://localhost:9292");

$sub = new ZMQSocket($context, ZMQ::SOCKET_SUB);
$sub->connect("tcp://localhost:9293");
$sub->setSockOpt(ZMQ::SOCKOPT_SUBSCRIBE, "");

sleep(1);
$socket->sendMulti(array("TOPS01_ADD", "PUB", "test", time()+(24*60*60), "ipc://test", "ipc://test2"));
var_dump($socket->recvMulti());
var_dump($sub->recvMulti());