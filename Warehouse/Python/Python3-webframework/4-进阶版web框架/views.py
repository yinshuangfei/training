"""
    该模块存放浏览器请求对应的网页与urls模块中url_list列表中的项存在映射关系
    若要添加新的内容, 只需要定义相应的函数, 并将函数名以字符串的形式加入到__all__列表中
"""

import os  # 导入os模块
import time  # 导入time模块
import auth  # 导入auth.py
from urllib.parse import parse_qs  # 导入parse_qs用于解析数据

# 展示所有可用方法
__all__ = [
    "index",
    "authed"
    # "css"
    ]

# 路径拼接(针对windows"/", linu需要把"/"改为"\")
index_path = os.path.join( os.path.dirname(__file__), "file/index.html")
success_path = os.path.join( os.path.dirname(__file__), "file/success.html")

def index(environ):
    with open(index_path, mode="rb") as f:
        data = f.read().decode("UTF-8")
        # 将特殊符号@替换为当前时间, 实现动态网站
        data = data.replace("@", time.strftime(("%Y-%m-%d %H:%M:%S")))
    return data.encode("UTF-8")

def authed(environ):
    if environ.get("REQUEST_METHOD") == "POST":
        try:
            request_body_size = int(environ.get("CONTENT_LENGTH", 0))
        except (ValueError):
            request_body_size = 0
        request_data = environ["wsgi.input"].read(request_body_size)
        print(">>>", request_data)  # bytes数据类型
        print("????", environ["QUERY_STRING"])  # "空的" - post请求只能按照以上方式获取数据
        # parse_qs负责解析数据
        # 不管是POST还是GET请求都不能直接拿到数据, 拿到的数据仍需要进行分解提取
        # 所以引入urllib模块中的parse_qs方法
        request_data = parse_qs(request_data.decode("UTF-8"))
        print("拆解后的数据", request_data)  # {"username": ["a"], "password": ["1"]}
        username = request_data["username"][0]
        password = request_data["password"][0]
        status = auth.auth(username, password)
        if status:
            with open(success_path, mode="rb") as f:
                data = f.read()
        else:
            # 如果直接返回中文, 没有给浏览器指定编码格式, 默认是gbk, 需要进行gbk编码, 使浏览器能够识别
            # 这里已经指定了编码
            # start_response("200 OK", [("Content-Type", "text/html;charset=UTF8")])
            data = "<h1>用户名或密码错误, 登陆失败</h1>".encode("UTF-8")
        return data
    if environ.get("REQUEST_METHOD") == "GET":
        print("????", environ["QUERY_STRING"])  # "username='a'&password='1'"字符出数据类型        
        request_data = environ["QUERY_STRING"]
        # parse_qs负责解析数据
        # 不管是POST还是GET请求都不能直接拿到数据, 拿到的数据仍需要进行分解提取
        # 所以引入urllib模块中的parse_qs方法
        request_data = parse_qs(request_data)
        print("拆解后的数据", request_data)  # {"username": ["a"], "password": ["1"]}
        username = request_data["username"][0]
        password = request_data["password"][0]
        print(username, password)
        status = auth.auth(username, password)
        if status:
            with open(success_path, mode="rb") as f:
                data = f.read()
        else:
            # 如果直接返回中文, 没有给浏览器指定编码格式, 默认使gbk, 需要进行gbk编码, 是浏览器能够识别
            # 这里已经指定了编码
            # start_response("200 OK", [("Content-Type", "text/html;charset=UTF8")])
            data = "<h1>用户名或密码错误, 登陆失败</h1>".encode("UTF-8")
        return data

# def css(environ):
#     with open("css.css", mode="rb") as f:
#         data = f.read()
#     return data