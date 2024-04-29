'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-12 23:14:19
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_3.py
Description:

Copyright (c) 2024 by HT706, All Rights Reserved.
'''
from mxnet import np, npx
import os

'''不加这句话，执行标量和矩阵的乘法会报错
    y = 2 * np.dot(x, x)
    MXNetError: Operator _npi_multiply_scalar inferring shapes failed.
'''
npx.set_np()

def matrix():
    A = np.arange(20).reshape(5, 4)
    # 矩阵的转置
    print(A.T)

def tensor():
    '''
    description: 张量计算
    return {*}
    '''
    A = np.arange(20).reshape(5, 4)
    B = A.copy()

    # 张量加法
    print(A, A + B)

    # 张量Hadamard积 ⊙ (Hadamard product)
    # 两个矩阵的按元素乘法
    # 两个矩阵的按元素乘法称为Hadamard积（Hadamard product）（数学符号⊙）
    print(A * B)

    # 张量乘以或加上一个标量不会改变张量的形状，其中张量的每个元素都将与标量相加或相乘
    a = 2
    print(a + A)
    print(a * A)

    # 求和 ∑
    x = np.arange(4)
    print(x, x.sum())
    print(A.shape, A.sum())
    # 沿着指定轴求和, 0 表示行，1 表示列
    print(A, A.sum(axis=0))
    A_asis_y = A.sum(axis=1)
    print(A_asis_y, A_asis_y.shape)
    # 沿着行列求和
    print(A, A.sum(axis=[0, 1]))

    # 求平均值
    print(A.mean(), A.sum() / A.size)
    # 沿着指定轴求维度
    print(A.mean(axis=0), A.sum(axis=0) / A.shape[0])

    # 非降维求和
    sum_A = A.sum(axis=1, keepdims=True)
    print(sum_A)
    print(A / sum_A)    # 通过广播将 A 除以 sum_A

    # 沿某个轴计算累计总和
    print(A.cumsum(axis=0))

    # 点积 (Dot product)
    # 相同位置的按元素乘积的和
    y = np.ones(4)
    print(x, y, np.dot(x, y))
    # 我们可以通过执行按元素乘法，然后进行求和来表示两个向量的点积
    print(np.sum(x * y))

    # 矩阵-向量积(matrix‐vector product)
    print(A.shape, x.shape, np.dot(A, x))

    # 矩阵-矩阵乘法
    # 矩阵‐矩阵乘法可以简单地称为矩阵乘法，不应与“Hadamard积”混淆
    B = np.ones(shape=(4,3))
    print(B, np.dot(A, B))

    # 范数
    u = np.array([3, -4])
    # L2 范数
    print(np.linalg.norm(u))
    # L1 范数: 是矩阵元素平方和的平方根
    print(np.abs(u).sum())
    # Frobenius范数
    print(np.linalg.norm(np.ones((4, 9))))


if __name__ == "__main__":
    # matrix()
    tensor()