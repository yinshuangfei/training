'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-09 21:39:16
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_2.py
Description:

Copyright (c) 2024 by Alan Yin, All Rights Reserved.
'''
from mxnet import np, npx
import pandas as pd
import os

def train_write_csv():
    os.makedirs(os.path.join('data'), exist_ok=True)
    data_file = os.path.join('data', 'house_tiny.csv')
    with open(data_file, 'w') as f:
        f.write('NumRooms,Alley,Price\n') # 列名
        f.write('NA,Pave,127500\n') # 每行表示一个数据样本
        f.write('2,NA,106000\n')
        f.write('4,NA,178100\n')
        f.write('NA,NA,140000\n')

def train_read_csv():
    data_file = os.path.join('data', 'house_tiny.csv')
    data = pd.read_csv(data_file)

    inputs, outputs = data.iloc[:, 0:2], data.iloc[:, 2]
    print(inputs)

    # 处理缺失值
    inputs = inputs.fillna(inputs.mean())
    print(inputs)

    # 处理缺失类型
    inputs = pd.get_dummies(inputs, dummy_na=True)
    print(inputs)

    # 转换为张量格式
    X, y = np.array(inputs.to_numpy(dtype=float)), np.array(outputs.to_numpy(dtype=float))
    print(X, y)

if __name__ == "__main__":
    # train_write_csv()
    train_read_csv()