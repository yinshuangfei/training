<!--
 * @Author: Alan Yin
 * @Date: 2023-07-22 19:42:39
 * @LastEditTime: 2024-05-03 09:37:34
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/myOS/README.md
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
-->
# This is a os project for myself.
# It was small.

# How to use bochs
1.安装 Bochs-x.x.x.exe；
## Install-Windows
双击运行 Bochs-x.x.x.exe
## Install-Linux
```shell
# wget https://dl.fedoraproject.org/pub/fedora/linux/releases/40/Everything/source/tree/Packages/b/bochs-2.8-1.fc40.src.rpm
# rpm -i bochs-2.8-1.fc40.src.rpm
# cd /root/rpmbuild/SPECS
# rpmbuild -ba bochs.spec

# yum install seavgabios-bin
# cd /root/rpmbuild/RPMS/
# rpm -i noarch/bochs-bios-2.8-1.el8.noarch.rpm x86_64/bochs-2.8-1.el8.x86_64.rpm x86_64/bochs-devel-2.8-1.el8.x86_64.rpm

# wget https://dl.fedoraproject.org/pub/epel/7/x86_64/Packages/d/dev86-0.16.21-2.el7.x86_64.rpm
# rpm -i dev86-0.16.21-2.el7.x86_64.rpm
```

2.制作 bochdsrc.txt 配置文件；



https://github.com/chenyukang/Panda
https://github.com/chenyukang/Panda/tree/master

https://github.com/hurley25/Hurlex-II
https://github.com/hurley25/hurlex-doc/blob/master