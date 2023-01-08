<?php
header("Content-type : text/json");

// shell_exec("cat /proc/meminfo | grep Active: | awk '{print $2}'");
shell_exec("/usr/bin/python3 config/traffic/echarts_update.py 2>& 1");

$filename = "config/traffic/data.json";
$handle = fopen($filename, "r");//读取二进制文件时，需要将第二个参数设置成'rb'
$contents = fread($handle, filesize ($filename));
fclose($handle);
echo $contents;
echo json_encode($contents);
?>