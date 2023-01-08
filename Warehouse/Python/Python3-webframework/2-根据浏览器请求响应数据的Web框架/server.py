import os  # 导入os模块
import socket  # 导入socket模块
# 导入线程模块
from threading import Thread
# 实例化socket对象
server = socket.socket()
# 绑定IP及端口
server.bind(("0.0.0.0", 9001))
server.listen()

# 路径拼接
html_path = os.path.join(os.path.dirname(__file__), "index.html")
css_path = os.path.join(os.path.dirname(__file__), "css.css")
js_path = os.path.join(os.path.dirname(__file__), "js.js")

def html(conn):
	"""
	响应"/"请求
	"""
	conn.send(b'HTTP/1.1 200 ok \r\n\r\n')
	with open(html_path, mode="rb") as f:
		content = f.read()
	conn.send(content)
	conn.close()

def css(conn):
	"""
	响应"/css.css"请求
	"""
	conn.send(b"HTTP/1.1 200 ok \r\n\r\n")
	with open(css_path, mode="rb") as f:
		content = f.read()
	conn.send(content)
	conn.close()

def js(conn):
	"""
	响应"/js.js"请求
	"""
	conn.send(b"HTTP/1.1 200 ok \r\n\r\n")
	with open(js_path, mode="rb") as f:
		content = f.read()
	conn.send(content)
	conn.close()

def NotFound(conn):
	conn.send(b"HTTP/1.1 200 ok \r\n\r\n")
	conn.send(b"<h1>404NotFound!</h1>")

# 请求列表
request_list = [
	("/", html),
	("/css.css", css),
	("/js.js", js)
]

def get(conn):
	"""
	处理响应函数
	"""
	try:  # 异常处理
		req = conn.recv(1024).decode("UTF-8")
		req = req.split("\r\n")[0].split()[1]
		# 打印浏览器请求
		print(req)
	except IndexError:
		pass

	# 遍历请求列表进行响应
	for request in request_list:
	# 若浏览器请求信息等于请求列表中的项,则进行响应
	# 判断服务端是否能够进行响应
		if req == request[0]:
			# 获取线程对象, 实现并发
			t = Thread(target=request[1], args=(conn, ))
			# 启动线程
			t.start()
			# 响应后结束遍历
			break
		else:  # 若本次循环未匹配则跳过本次循环开始下一次
			continue
	else:  # 若所有请求皆不匹配则调用NotFound函数, 表示无法响应
		NotFound(conn)

def main():
	while True:
		# 利用线程实现并发
		# 获取TCP连接
		conn, _ = server.accept()
		t = Thread(target=get, args=(conn,))
		t.start()

if __name__ == "__main__":
	main()
