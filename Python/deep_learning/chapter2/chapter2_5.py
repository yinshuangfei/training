'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-12 23:29:18
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_5.py
Description:

Copyright (c) 2024 by HT706, All Rights Reserved.
'''
from mxnet import autograd, np, npx
import os

'''不加这句话，执行标量和矩阵的乘法会报错
    y = 2 * np.dot(x, x)
    MXNetError: Operator _npi_multiply_scalar inferring shapes failed.
'''
npx.set_np()

def matrix():
    '''
    description: 自动微分
    return {*}
    '''
    x = np.arange(4.0)

    # 通过调用attach_grad来为一个张量的梯度分配内存
    x.attach_grad()
     # 在计算关于x的梯度后，将能够通过'grad'属性访问它，它的值被初始化为0
    print(x.grad)

    # 把代码放到autograd.record内，以建立计算图
    with autograd.record():
        y = 2 * np.dot(x, x)

    # 调用反向传播函数来自动计算y关于x每个分量的梯度
    y.backward()
    # 打印这些梯度
    print(x.grad)

    # 快速验证这个梯度是否计算正确
    print(x.grad == 4 * x)

    # 计算x的另一个函数
    with autograd.record():
        y = x.sum()
    y.backward()
    # 被新计算的梯度覆盖
    print(x.grad)

    # 非标量变量的反向传播
    # 当对向量值变量y（关于x的函数）调用backward时，将通过对y中的元素求和来创建
    # 一个新的标量变量。然后计算这个标量变量相对于x的梯度
    with autograd.record():
        y = x * x # y是一个向量
    y.backward()
    print(x.grad) # 等价于y=sum(x*x)

    # 分离计算
    # 分离y来返回一个新变量u，该变量与y具有相同的值，但丢弃计算图中如何计算y的任何信息。
    # 换句话说，梯度不会向后流经u到x。
    with autograd.record():
        y = x * x
        u = y.detach()
        z = u * x
    z.backward()
    print(x.grad)
    print(x.grad == u)
    # 由于记录了y的计算结果，我们可以随后在y上调用反向传播，得到y=x*x关于的x的导数，
    # 即2*x
    y.backward()
    print(x.grad, x.grad == 2 * x)


def f(a):
    b = a * 2
    while np.linalg.norm(b) < 1000:
        b = b * 2
    if b.sum() > 0:
        c = b
    else:
        c = 100 * b
    return c


def test_f():
    '''
    description: Python控制流的梯度计算
    return {*}
    '''
    a = np.random.normal()
    a.attach_grad()
    with autograd.record():
        d = f(a)
    d.backward()
    print(a.grad, a.grad == d / a)

if __name__ == "__main__":
    matrix()
    test_f()