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
