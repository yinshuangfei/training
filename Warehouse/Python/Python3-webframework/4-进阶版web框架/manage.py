from urls import url_list
from wsgiref.simple_server import make_server

def application(environ, start_response):
    """
    :param environ: 包含所有请求信息的字典
    :param start_response: 封装响应信息(相应行与响应头)
    :return: [响应主体]
    """
    # 封装响应信息
    start_response("200 OK", [("Content-Type", "text/html;charset=UTF8")])
    # 打印包含所有请求信息的字典
    print(environ)
    # 打印请求路径信息
    print(environ["PATH_INFO"])
    path = environ["PATH_INFO"]
    for p in url_list:
        if path == p[0]:
            data = p[1](environ)
            break
        else:
            continue
    else:
        data = b"<h1>Sorry 404!, NOT Found The Page</h1>"
    # 返回响应主体
    # 必须遵守此格式[内容]
    return [data]

if __name__ == "__main__":
    # 绑定服务器IP地址与端口号, 调用函数
    frame = make_server("127.0.0.1", 9001, application)
    # 开始监听HTTP请求
    frame.serve_forever()
