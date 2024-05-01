'''
Author: Alan Yin
Date: 2023-12-17 17:16:56
LastEditTime: 2024-03-12 23:15:51
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_1.py
Description:

Copyright (c) 2024 by Alan Yin, All Rights Reserved.
'''
from mxnet import np, npx

'''
当使用张量时，几乎总是会调用set_np函数，这是为了兼容MXNet的其他张量处理组件。
'''
npx.set_np()

# 初始化向量，打印 shape, size
x = np.arange(12)
print(x, x.shape, x.size)

# 改变张量的形状
X = x.reshape(3, 4)
print(X)

# 改变张量的形状，自动计算维度
X = np.arange(24).reshape(2, -1)
print(X)

# 元素值为0的张量
print(np.zeros((2,3,4)))

# 元素值为1的张量
print(np.ones((2,3,4)))

# 每个元素都从均值为0、标准差为1的标准高斯分布（正态分布）中随机采样
print(np.random.normal(0, 1, size=(3, 4)))

# 列表赋值张量
print(np.array([[2, 1, 4, 3], [1, 2, 3, 4], [4, 3, 2, 1]]))

# 张量运算
x = np.array([1, 2, 4, 8])
y = np.array([2, 2, 2, 2])
print(x + y, x - y, x * y, x / y, x ** y)
print(np.exp(x))

# 连结张量
X = np.arange(12).reshape(3, 4)
Y = np.array([[2, 1, 4, 3], [1, 2, 3, 4], [4, 3, 2, 1]])
print(np.concatenate([X, Y], axis=0))
print(np.concatenate([X, Y], axis=1))

# 张量逻辑运算
print(X == Y)

# 张量元素求和
print(X.sum())

# 索引和切片
print(X[-1], "\n", X[1:3])

# 单元素赋值
X[1, 2] = 9
print(X)

# 多元素赋值
X[0:2, :] = 12
print(X)

# 广播机制
a = np.arange(3).reshape(3, 1)
b = np.arange(2).reshape(1, 2)
print(a, b)
print(a + b)

# 节省内存
before = id(Y)
Y = Y + X       # Y 的空间重新分配了，浪费内存
print(id(Y) == before)

# 内存节省了，原地更新
Z = np.zeros_like(Y)
print('id(Z):', id(Z))
Z[:] = X + Y
print('id(Z):', id(Z))

# += 原地更新
before = id(X)
X += Y
print(id(X) == before)

# 张量转换为Python标量
a = np.array([3.5])
print(a, a.item(), float(a), int(a))

# Python 对象和 mxnet 对象
print(type(X.asnumpy()), type(np.array(X)))
