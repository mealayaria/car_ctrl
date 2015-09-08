<?php
define("UNIX_DOMAIN","/tmp/UNIX.domain");
echo UNIX_DOMAIN;

$val = $_POST['val'];

if ($val == '#') {
    system("../main_ctrl_v1");
}
else {
    $socket = socket_create(AF_UNIX, SOCK_STREAM, 0);  
    if ($socket < 0) { 
    	echo "socket_create() failed: reason: " . socket_strerror($socket) . "\n"; 
    } 
    
    $result = socket_connect($socket, UNIX_DOMAIN);  

    if ($result < 0) { 
    	echo "socket_connect() failed.\nReason: ($result) " . socket_strerror($result) . "\n"; 
    } 
	if(!socket_write($socket, $val, strlen($val))) { 
		echo "socket_write() failed: reason: " . socket_strerror($socket) . "\n"; 
	} 
    socket_close($socket); 
}
?>
