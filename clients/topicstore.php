<?php

$context = new ZMQContext();
$socket = new ZMQSocket($context, ZMQ::SOCKET_REQ);
$socket->connect("tcp://localhost:9292");

// $socket->sendMulti(array("TOPS01_GET", "test"));
// var_dump($socket->recvMulti());
// $socket->sendMulti(array("TOPS01_ADD", "PUB", "test", time()+(24*60*60), "ipc://test", "ipc://test2"));
// var_dump($socket->recvMulti());
// $socket->sendMulti(array("TOPS01_GET", "test"));
// var_dump($socket->recvMulti());

// Test adding

$keys = array(
    "test",
    "test.test",
    "foo",
    "foo.bar",
    "foo.bar.baz",
    "test.foo",
    "foo.test",
    "iff",
    "funk",
    "iff.funk",
    "funk.iff"
);
$i = 0;
$count = 1000000;
$start = microtime(true);
while($i++ < $count) {
    if(rand(1, 10) == 1) {
        $socket->sendMulti(array("TOPS01_ADD", "PUB", $keys[array_rand($keys)], time()+(24*60*60), "ipc://test-" . $i));
    } else {
        $socket->sendMulti(array("TOPS01_GET", "test"));
    }
    
    if(rand(1, 1000) == 0) {
        var_dump($socket->recvMulti());
    } else {
        $socket->recvMulti();
    }
}
$time = microtime(true) - $start;
echo "\nTaken: ", $time, " seconds \n", $count/$time, " per second \n";
