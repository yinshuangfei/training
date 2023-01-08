#!/bin/env python
import socket
import os

def main():
    html_path = os.path.join(os.path.dirname(__file__), "index.html")
    # 实例化socket对象
    sock = socket.socket()
    sock.bind(('0.0.0.0', 9001))
    sock.listen()
    while True:
        conn, addr = sock.accept()
        data = conn.recv(1024)
        str = data.decode("UTF-8").strip(" ")
        print("浏览器请求信息 >>>", str)
        # 如果浏览器请求信息非空则进行回复
        if str:
            conn.send(b"HTTP/1.1 200 OK\r\n\r\n")
            # 以bytes数据类型打开html文件
            with open(html_path,'rb') as f:
                # 读取数据
                data = f.read()
                # 发送html文件数据
                conn.send(data)
            conn.close()
        else:
            conn.close()
            continue

if __name__ == "__main__":
    main()