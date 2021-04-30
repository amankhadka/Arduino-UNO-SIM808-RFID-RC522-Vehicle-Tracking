<?php
date_default_timezone_set("Asia/Kathmandu");

// set some variables
$host = "0.0.0.0";
$port = 8001;
$con = 1;
// don't timeout!
set_time_limit(0);
// create socket
$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
// bind socket to port
$result = socket_bind($socket, $host, $port) or die("Could not bind to socket\n");
// start listening for connections
$result = socket_listen($socket, 3) or die("Could not set up socket listener\n");
$i = 0;
while ($con == 1)
{
        // accept incoming connections
        // spawn another socket to handle communication
        $spawn = socket_accept($socket) or die("Could not accept incoming connection\n");
        // read client input
        $input = socket_read($spawn, 1024) or die("Could not read input\n");
        // clean up input string
        $input = trim($input);
        if ($input == 'exit' || $input=='' || $input==='false')
    {
        }
        if($con == 1)
        {
                $msg=Save($input);
    }
        // reverse client input an send back
}

function Save($input)
{
        $servername = "localhost";
        $username = "*******";
        $password = "*******";
        $dbname = "*******";

        $now=date('Y-m-d H:i:s');
        // Create connection
        $conn = mysqli_connect($servername, $username, $password, $dbname);
        if (!$conn)
        {
                $msg="error";
                die("Connection failed: " . mysqli_connect_error());
        }
        else
        {
        $sql = "INSERT INTO gps (data,schooldate) VALUES ('".$input."','".$now."')";

        mysqli_query($conn, $sql);
        }
}
?>
     
