<!--
 * @Author: Alan Yin
 * @Date: 2024-05-02 19:19:50
 * @LastEditTime: 2024-05-02 20:41:22
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/myOS/src/boot/README.md
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
-->
# 计算机启动流程
## 1.加电
    主板加电稳定后，CPU开始模式化工作，执行的第一条指令的地址是0xFFFFFFF0。从这里开始，
CPU 就进入了一个“取指令-翻译指令-执行”的循环了。所以我们需要做的就是在各个阶段提供给CPU
相关的数据，以完成这个“接力赛”。
    0xFFFFFFF0这个地址指向BIOS芯片里，在4G的地址空间里，这个地址是映射到外设BIOS的。计
算机刚加电的时候内存等芯片尚未初始化，所以也只能是指向BIOS芯片里已经被“固化”的指令了。

## 2.BIOS
    紧接着就是BIOS的POST（Power On Self Test，上电自检）过程了，BIOS对计算机各个部件
开始初始化，如果有错误会给出报警音。完成这些工作之后，它的任务就是在外部存储设备中寻找操
作系统。我们最常用的外存是硬盘。
    如果存储设备的第一个扇区中512个字节的最后两个字节是0x55和0xAA，那么该存储设备就是可
启动的。当BIOS找到可启动的设备后，便将该设备的第一个扇区加载到内存的0x7C00地址处，并且
跳转过去执行。

## 3.bootloader
    因为第一个扇区只有512字节，放不下太多的代码，所以常规的做法便是在这里写下载入操作系
统内核的代码，这段代码就是所谓的bootloader程序。

## 4.启动内核
    bootloader程序加载内核后，将执行权限交给内核，实现内核启动。