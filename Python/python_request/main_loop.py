#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Author: Alan Yin
Date: 2023-01-08 11:17:29
LastEditTime: 2023-01-08 18:41:16
LastEditors: Alan Yin
FilePath: /tmp_path/mnt/g/局域网共享/douban/main_loop.py
Description:

Copyright (c) 2023 by Alan Yin, All Rights Reserved.
'''

import requests
import pprint


class Request(object):
    def __init__(self, url: str = ""):
        url = ""

    def get(self, kw, headers):
        # kw = {'wd':'长城'}
        # headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36"}

        requests.get(self.url, params=kw, headers=headers)
        # # 查看响应内容，response.text 返回的是Unicode格式的数据
        # print response.text

        # # 查看响应内容，response.content返回的字节流数据
        # print response.content

        # # 查看完整url地址
        # print response.url

        # # 查看响应头部字符编码
        # print response.encoding

        # # 查看响应码
        # print response.status_code

        # # 如果是json文件可以直接显示
        # print response.json()

    def post(self, formdata, headers):
        # formdata = {
        #     "type":"AUTO",
        #     "i":"i love python",
        #     "doctype":"json",
        #     "xmlVersion":"1.8",
        #     "keyfrom":"fanyi.web",
        #     "ue":"UTF-8",
        #     "action":"FY_BY_ENTER",
        #     "typoResult":"true"
        # }

        response = requests.post(url, data=formdata, headers=headers)

    def proxy(self):
        proxies = {
            "http": "http://12.34.56.79:9527",
            "https": "http://12.34.56.79:9527",
        }
        response = requests.get("http://www.baidu.com", proxies=proxies)
        print(response.text)

    def proxy_x(self):
        """私密代理验证"""
        proxy = { "http": "mr_mao_hacker:sffqry9r@61.158.163.130:16816" }
        response = requests.get("http://www.baidu.com", proxies=proxy)
        print(response.text)

    def client(self):
        auth=('test', '123456')
        response = requests.get('http://192.168.199.107', auth=auth)
        print(response.text)

    def cookie(self):
        response = requests.get("http://www.baidu.com/")

        # 7. 返回CookieJar对象:
        cookiejar = response.cookies

        # 8. 将CookieJar转为字典：
        cookiedict = requests.utils.dict_from_cookiejar(cookiejar)

        print(cookiejar)

        print(cookiedict)

    def session(self):
        # 1. 创建session对象，可以保存Cookie值
        ssion = requests.session()

        # 2. 处理 headers
        headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36"}

        # 3. 需要登录的用户名和密码
        data = {"email":"mr_mao_hacker@163.com", "password":"alarmchime"}

        # 4. 发送附带用户名和密码的请求，并获取登录后的Cookie值，保存在ssion里
        ssion.post("http://www.renren.com/PLogin.do", data = data)

        # 5. ssion包含用户登录后的Cookie值，可以直接访问那些登录后才可以访问的页面
        response = ssion.get("http://www.renren.com/410043129/profile")

        # 6. 打印响应内容
        print(response.text)

    def ssl_cert(self):
        response = requests.get("https://www.baidu.com/", verify=True)

        # 也可以省略不写
        # response = requests.get("https://www.baidu.com/")
        print(r.text)

    def test_get(self):
        url = 'https://api.douban.com/v2/user/154075427?apikey=0df993c66c0c636e29ecbb5344252a4a'
        headers = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36"}
        response = requests.get(url, headers=headers)
        print(response.text)
        print(response.url)
        print(response.status_code)
        print(response.cookies)
        print(response.elapsed)
        print(response.encoding)
        print(response.headers)
        print(response.reason)
        print(response.request)
        print(response.history)
        print(response.ok)
        print(response.is_redirect)
        print(response.is_permanent_redirect)
        print(response.next)
        print(response.apparent_encoding)
        print(response.content)
        print(response.json())
        print(response.links)


class DouBan(object):
    def __init__(self):
        self.douban_url = 'https://api.douban.com/v2'
        self.user_id = '154075427'
        self.api_key = '0df993c66c0c636e29ecbb5344252a4a'
        self.api_key2 = '0b2bdeda43b5688921839c8ecb20399b'
        self.headers = {"User-Agent": \
            # "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36"}
            'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36',
            "accept-encoding": "gzip, deflate, br",
            "accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9",
            "upgrade-insecure-requests": "1",
            "sec-fetch-dest": "document",
            "sec-ch-ua-platform": "Windows",
            "sec-fetch-mode": "navigate",
            "sec-ch-ua-mobile": "?0"}

        self.url = "%s/%s/%s?apikey=%s" % (self.douban_url, "user", self.user_id, self.api_key)

    def test_url(self):
        response = requests.get(self.url, headers=self.headers)
        if response.status_code == 200:
            print(response.text)
        else:
            print("%d error, %s" % (response.status_code, response.reason))

    def book(self, url):
        # url = "https://img9.doubanio.com/view/photo/s_ratio_poster/public/p2884182275.jpg"
        # url = "https://img9.doubanio.com/view/photo/s_ratio_poster/public/p2563780504.jpg"
        url = "https://img1.doubanio.com/view/photo/s_ratio_poster/public/p451926968.jpg"
        bgimg_content = requests.get(url, headers=self.headers)
        print(bgimg_content)





        # response = requests.get(url, headers=self.headers)
        # if response.status_code == 200:
        #     print(response.text)
        # else:
        #     json_msg = response.json()
        #     print("%d error, %s.\n%s" % (response.status_code, response.reason, json_msg))
        #     print()


if __name__ == '__main__':
    d = DouBan()
    d.book("https://api.douban.com/v2/movie/subject/4811774?apikey=0df993c66c0c636e29ecbb5344252a4a")



