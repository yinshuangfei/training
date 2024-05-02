#!/bin/bash
###
 # @Author: Alan Yin
 # @Date: 2024-05-02 20:59:06
 # @LastEditTime: 2024-05-03 01:58:30
 # @LastEditors: Alan Yin
 # @FilePath: /windows_cifs/training/myOS/build.sh
 # @Description:
 # // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 # // vim: ts=8 sw=2 smarttab
 # Copyright (c) 2024 by Alan Yin, All Rights Reserved.
###

BOOT=./boot
OBJDIR=./objs
KERNEL="./core"
TOOL="./tools"
INCLUDE="./inc"

DISK="tmp_disk.img"

NASM="nasm -f elf"
GCC=gcc
LD=ld

CFLAGS="-Wall -m32 -O -nostdinc -fno-builtin -fno-stack-protector \
        -Wno-implicit-function-declaration -finline-functions \
        -finline-functions-called-once -I./inc/ -ggdb -gstabs+"
# -m32: 生成32位代码
# -O: 编译器尝试减小代码尺寸减少执行时间，不执行任何需要大量编译时间的优化
# -nostdinc: 不包含 C 语言的标准库里的头文件
# -fno-builtin: 要求 gcc 不主动使用自己的内建函数，除非显式声明
# -fno-stack-protector: 不使用栈保护等检测
# -Wno-implicit-function-declaration: 没有隐式函数声明的警告
# -finline-functions: 编译器对函数进行内联优化
# -finline-functions-called-once: 考虑所有调用一次的静态函数以内联到它们的调用者，即
#       使它们没有标记为内联
# -ggdb -gstabs+: 添加相关的调试信息，调试对后期的排错很重要


# C_FLAGS		= -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin \
# 			  -fno-stack-protector -I include
# LD_FLAGS	= -T scripts/kernel.ld -m elf_i386 -nostdlib
# ASM_FLAGS	= -f elf -g -F stabs
# QEMU		= /usr/libexec/qemu-kvm
# DISK		= disk.img

[ ! -e $OBJDIR ] && mkdir -p $OBJDIR

do_compile_bootloader() {
    echo "building boot ..."
    flist=`cd $BOOT/; find ./ -name "*.s"`
    for f in $flist;
    do
        # 字符串替换语法
        `$NASM $BOOT/$f -o $OBJDIR/${f/.s/.O}`
    done
}

do_compile_kernel() {
    echo "building kernel ..."
    flist=`cd $KERNEL/; find ./ -name "*.s"`
    `cd ../`
    for f in $flist;
    do
        `$NASM $KERNEL/$f -o $OBJDIR/${f/.s/.O}`
        if [ $? -ne 0 ]
            then exit;
        fi
    done

    flist=`cd $KERNEL/; find ./ -name "*.c"`
    `cd ../`
    for f in $flist;
    do
        `$GCC $CFLAGS -c $KERNEL/$f -o $OBJDIR/${f/.c/.o}`
        if [ $? -ne 0 ]
            then exit;
        fi
    done
}

do_link() {
    echo "linking ..."
    pushd $OBJDIR

    $LD -m elf_i386 boot.O -o boot.bin -T ../$TOOL/boot.ld
    $LD -m elf_i386 setup.O -o setup.bin -T ../$TOOL/setup.ld

    if [ $? -ne 0 ]
    then
        echo "link error!"
        exit
    else
        echo "making disk.img head ..."
        # cat boot.bin setup.bin kernel.bin > ../a.img
        cat boot.bin setup.bin > ../$DISK
        # ls -lh kernel.bin;
        # cd .a./;
    fi

    popd
}

do_auto() {
    do_compile_bootloader
    do_compile_kernel
    do_link
}

do_help() {
    echo "-auto       | -a      : auto compile and link"
    echo "-bootloader | -bl     : compile bootloader"
    echo "-kernel     | -k      : compile kernel"
    echo "-link       | -l      : link"
    # echo "-clean  | x   : do clean"
    # echo "-compile| c   : do compile"
    # echo "-all    | a   : do above two, and run simulation"
    # echo "-commit | u   : do git commit"
    # echo "-qemu   | q   : do all and run qemu"
    # echo "-bochs  | b   : do all and run bochs"
    # echo "-line   | l   : do count code line :)"
}

while [ $# -gt 0 ]
do
    case $1 in
        -auto|-a)
            do_auto
            ;;
        -bootloader|-bl)
            do_compile_bootloader
            ;;
        -kernel|-k)
            do_compile_kernel
            ;;
        -help|-h)
            do_help;
            exit 0;;
        # -clean|-x)   do_clean;       exit 0;;
        # -line |-l)   do_wc_line;     exit 0;;
        # -qemu|-q)    do_all "qemu";  exit 0;;
        # -bochs|-b)   do_all "bochs"; exit 0;;
        # -all|-a)     do_all "qemu";  exit 0;;
        # -commit |-u) shift; log=$1;  do_commit;  exit 0;
        *)
            do_help;
            exit 0;;
    esac
    shift
done