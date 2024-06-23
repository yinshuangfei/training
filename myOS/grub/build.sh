#!/bin/bash
###
 # @Author: Alan Yin
 # @Date: 2024-05-12 23:06:57
 # @LastEditTime: 2024-05-12 23:28:40
 # @LastEditors: Alan Yin
 # @FilePath: /myOS/grub/build.sh
 # @Description:
 # // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 # // vim: ts=8 sw=2 smarttab
 # Copyright (c) 2024 by Alan Yin, All Rights Reserved.
###

BOOT=../boot
OBJDIR=../objs
KERNEL="../kernel"

NASM="nasm -f elf"
GCC=gcc
LD=ld
OBJCPY="objcopy"

rm -fr kernel.o grub_boot.o

gcc ${KERNEL}/kernel.c -c -fno-builtin
gcc ${BOOT}/grub_boot.s -c
ld kernel.o grub_boot.o -o kernel -s -Ttext 0x100000 --entry=start