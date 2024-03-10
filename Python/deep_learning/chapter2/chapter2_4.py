'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-10 22:00:41
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_4.py
Description:

Copyright (c) 2024 by HT706, All Rights Reserved.
'''
# %matplotlib inline
# from matplotlib_inline import backend_inline
from mxnet import np, npx
# from d2l import mxnet as d2l

npx.set_np()

def f(x):
    '''
    description: 3x**2 - 4x
    return {*}
    '''
    return 3 * x ** 2 - 4 * x

def numerical_lim(f, x, h):
    '''
    description: 求导公式
    return {*}
    '''
    return (f(x + h) - f(x)) / h

def train():
    h = 0.1
    for i in range(5):
        print(f'h={h:.5f}, numerical limit={numerical_lim(f, 1, h):.5f}')
        h *= 0.1


if __name__ == "__main__":
    train()
