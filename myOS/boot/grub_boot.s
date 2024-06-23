/*
 * @Author: Alan Yin
 * @Date: 2024-05-12 23:01:31
 * @LastEditTime: 2024-05-12 23:29:00
 * @LastEditors: Alan Yin
 * @FilePath: /myOS/boot/grub_boot.s
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

.code32
# #define MULTIBOOT_HEADER_MAGIC          0x1BADB002
# #define MULTIBOOT_HEADER_FLAGS          0x00000003
#define STACK_SIZE                      0x4000


.text
    .globl  start, _start

start:
_start:
    jmp     multiboot_entry

    .align  4

multiboot_header:
    # .long   MULTIBOOT_HEADER_MAGIC
    # .long   MULTIBOOT_HEADER_FLAGS
    .long   0x1BADB002
    .long   0x00000003
#    .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

multiboot_entry:
    /* 初始化堆栈指针。 */
#    movl    $(stack + STACK_SIZE), %esp

    /* 重置 EFLAGS。 */
    pushl   $0
    popf

    pushl   %ebx
    pushl   %eax

    /* 现在进入 C main 函数... */
    call    cmain

loop: hlt
    jmp loop

#    .comm   stack, STACK_SIZE
