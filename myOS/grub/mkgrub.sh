#!/bin/bash
###
 # @Author: Alan Yin
 # @Date: 2024-05-09 23:47:19
 # @LastEditTime: 2024-05-12 22:54:52
 # @LastEditors: Alan Yin
 # @FilePath: /myOS/grub/mkgrub.sh
 # @Description:
 # // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 # // vim: ts=8 sw=2 smarttab
 # Copyright (c) 2024 by Alan Yin, All Rights Reserved.
###
FILE=$1

echo "
menuentry 'Alan Yin -> OS' {
    insmod part_msdos
    insmod ext2
    set root='hd0'
    multiboot2 /boot/kernel.grub.bin
    boot
}

set timeout_style=menu

if [ \"\${timeout}\" = 0 ]; then
    set timeout=0
fi
" > ${FILE}

exit 0
