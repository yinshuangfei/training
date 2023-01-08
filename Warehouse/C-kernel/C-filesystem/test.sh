#!/bin/bash
module_name="ysffs"

make clean
make


lsmod | grep $module_name >> /dev/null
if [ $? = 0 ] ; then
    rmmod $module_name
fi

insmod ${module_name}.ko
