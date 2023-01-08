<?php

header("Content-type : text/json");
$file = "/var/lib/collectd/rrd/node1.pve/disk-sda/disk_octets.rrd";

$time_last = shell_exec("rrdtool last $file");
$time_now = shell_exec("date +%s");
$diff = (int)$time_now - (int)$time_last ;

if ($diff < 7 ){
    // skip, do nothing
}
else{
    $date = shell_exec("date +%s");

    $ret=array((int)trim($date),10715870);
    echo json_encode($ret);
}
?>
