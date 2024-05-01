'''
Author: Alan Yin
Date: 2024-03-09 10:17:09
LastEditTime: 2024-03-12 23:51:45
LastEditors: Alan Yin
FilePath: \training\Python\deep_learning\chapter2\chapter2_6.py
Description:

Copyright (c) 2024 by Alan Yin, All Rights Reserved.
'''
# %%matplotlib inline
import random
from mxnet import np, npx
from d2l import mxnet as d2l
npx.set_np()

def train():
    fair_probs = [1.0 / 6] * 6
    print(fair_probs)
    print(np.random.multinomial(10, fair_probs))

    counts = np.random.multinomial(1000, fair_probs).astype(np.float32)
    print(counts / 1000)

    counts = np.random.multinomial(10, fair_probs, size=500)
    cum_counts = counts.astype(np.float32).cumsum(axis=0)
    estimates = cum_counts / cum_counts.sum(axis=1, keepdims=True)

    d2l.set_figsize((6, 4.5))
    for i in range(6):
        d2l.plt.plot(estimates[:, i].asnumpy(),
                     label=("P(die=" + str(i + 1) + ")"))
    d2l.plt.axhline(y=0.167, color='black', linestyle='dashed')
    d2l.plt.gca().set_xlabel('Groups of experiments')
    d2l.plt.gca().set_ylabel('Estimated probability')
    d2l.plt.legend();

if __name__ == "__main__":
    train()
# %%
