systemctl控制的进程，如果是一个无限循环的程序，则被systemctl启动之后，进程的父进程变为init，无须在进程xxx.service文件的ExecStart处添加&使之后台运行。
systemctl控制的xxx.service脚本需要复制到/lib/systemd/system目录下，并使用systemctl daemon-reload重新加载；

若xxx.service脚本中为添加[Install]分片，则使用systemctl list-unit-files查看到的服务状态为static，如添加了[Install]分片，则为disable，设置为enable之后则变为enable。