rpcgen time.x

#客户端编译
gcc rtime.c time_clnt.c -o rtime

#服务器端编译
gcc time_proc.c time_svc.c -o time_server