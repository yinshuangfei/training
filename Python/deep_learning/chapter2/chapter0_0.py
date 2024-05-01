'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-12 22:56:42
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter0_0.py
Description:

Copyright (c) 2024 by Alan Yin, All Rights Reserved.
'''
from mxnet import np, npx
import os

def matrix():
    A = np.arange(20).reshape(5, 4)
    print(A.T)

if __name__ == "__main__":
    matrix()