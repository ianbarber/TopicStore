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

// $socket->sendMulti(array("TOPS01_GET", "test"));
// var_dump($socket->recvMulti());
// $socket->sendMulti(array("TOPS01_ADD", "PUB", "test", time()+(24*60*60), "ipc://test", "ipc://test2"));
// var_dump($socket->recvMulti());
// $socket->sendMulti(array("TOPS01_GET", "test"));
// var_dump($socket->recvMulti());

// Test adding

// $keys = array(
//     "test",
//     "foo",
//     "bar",
//     "baz",
//     "iff",
//     "funk",
//     "thomson",
//     "hilly"
//     
// );
// $i = 0;
// $count = 1000000;
// $start = microtime(true);
// $istart = microtime(true);
// while($i++ < $count) {
//     if(rand(1, 10) == 1) {
//         $tcount = rand(1, 5);
//         $topic = "";
//         for($j = 0; $j < $tcount; $j++) {
//             $topic .= $keys[array_rand($keys)] . ".";
//         }
//         $socket->sendMulti(array("TOPS01_ADD", "PUB", $topic, time()+(24*60*60), "ipc://test-" . $i));
//     } else {
//         $socket->sendMulti(array("TOPS01_GET", "test"));
//     }
//     
//     if($i % 10000 == 0) {
//         echo $i, "  - 10000 in ", microtime(true) - $istart, "\n";
//         $istart = microtime(true);
//     } 
//     
//     $socket->recvMulti();
// }
// $time = microtime(true) - $start;
// echo "\nTaken: ", $time, " seconds \n", $count/$time, " per second \n";


