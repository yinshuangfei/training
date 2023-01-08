from views import index, authed
"""
    可在此处按照类似格式添加任意内容
    例如再向url_list列表中添加一项, 按照如下格式
        ("/css.css", css), 只需要再在views.py文件中创建一个对应的函数即可
"""
url_list = [
    ("/", index),
    ("/auth/", authed)
    # ("/css.css", css)
]