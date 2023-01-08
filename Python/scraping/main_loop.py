#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Author: Alan Yin
Date: 2022-12-14 15:41:57
LastEditTime: 2023-01-08 11:19:50
LastEditors: Alan Yin
FilePath: /pmxcfs/mnt/g/软件共享/scraping/main_loop.py
Description:

Copyright (c) 2022 by HT706, All Rights Reserved.
'''

import builtwith
import json
import os

import time
import urllib3
import whois

from selenium import webdriver
from pprint import pprint

BW_FILE = "cache_builtwith.txt"


class TimeSpend(object):
    def get_spendtime(self, lasttime: float):
        return time.time() - lasttime


class Cache(TimeSpend):
    def __init__(self):
        pass

    def __getitem__(self, url):
        pass

    def __setitem__(self, url):
        pass


class Downloader(TimeSpend):
    """'__call__'为对象名的调用方法, 使得类实例对象可以像调用普通函数那样，以“对象名()”的形式使用"""
    def __call__(self):
        print("Downloader")


""" 以'__'开头的属性或方法是隐藏的，无法通过对象访问
    以'_'开头的属性或方法是私有的，一般不通过外部访问"""
class ParseUrl(TimeSpend):
    def __init__(self):
        self.__bw_file = BW_FILE
        self.__start_time = time.time()
        self.load_info = self.__init_load()

    def __init_load(self):
        if os.path.exists(self.__bw_file):
            return self.__load_file(self.__bw_file)
        else:
            return {}

    def __print_runtime(self):
        print("cost %10s seconds" % self.get_spendtime(self.__start_time))
        self.__start_time = time.time()

    def __save_file(self, file, info):
        with open(file, "w+") as f:
            json.dump(info, f)

    def __load_file(self, file):
        with open(file, "r") as f:
            return json.load(f)

    def parse_built(self, urls: list = []):
        '''
        description: 返回指定网站使用的技术
        return {*}
        '''
        # 将单字符串转为列表
        if str(type(urls)) == "<class 'str'>":
            urls = [urls]

        for url in urls:
            print("正在解析网址 %s" % url)
            if url in self.load_info.keys():
                ret = self.load_info[url]
            else:
                bw = builtwith.parse(url)
                ret = bw

                if bw:
                    self.load_info.update({url: bw})
                    self.__save_file(self.__bw_file, self.load_info)
                else:
                    msg = "'builtwith' 返回空, 解析 %(url)s 失败" % {"url": url}
                    print(msg)
                    # raise BaseException(msg)

            self.__print_runtime()
        return {url: ret}

    def get_built(self, url):
        if url in self.load_info.keys():
            return self.load_info[url]
        return {}

    def get_whois(self, url):
        return whois.whois(url)


if __name__ == "__main__":
    urls = ["https://ddys2.me/",
            "https://www.bilibili.com/",
            "https://www.jd.com/",
            "https://www.runoob.com/",
            "https://www.zhihu.com/",
            "https://app.yinxiang.com/",
            "https://www.youtube.com/",
            "https://www.taobao.com/",
            "https://www.douban.com/",
            "https://music.163.com/"]

    p = ParseUrl()
    bw = p.parse_built(urls)
    # pprint(bw)

    print(p.get_built("https://music.163.com/"))
    # print(p.get_whois("https://music.163.com/"))
    pprint(p.load_info)

    d = Downloader()
    d()

    # driver = webdriver.Chrome()

    # print(urllib3.HTTPConnectionPool.urlopen("GET", "https://music.163.com/"))
