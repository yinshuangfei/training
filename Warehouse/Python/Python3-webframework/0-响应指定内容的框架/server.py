#!/bin/env python
import socket

def main():
    # 实例化socket对象
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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
            conn.send(b"<h1>Hello World</h1>")
            conn.close()
        else:
            continue

if __name__ == "__main__":
    main()