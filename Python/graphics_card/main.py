#!/bin/env python3
'''
Author: Alan Yin
Date: 2023-02-04 15:13:36
LastEditTime: 2023-02-04 23:41:07
LastEditors: Alan Yin
FilePath: /tmp_path/mnt/g/局域网共享/training/Python/graphics_card/main.py
Description:

Copyright (c) 2023 by Alan Yin, All Rights Reserved.
'''

import pynvml

pynvml.nvmlInit()


nums = pynvml.nvmlDeviceGetCount()
for gpu_id in range(nums):
    handle = pynvml.nvmlDeviceGetHandleByIndex(gpu_id)
    print(pynvml.nvmlDeviceGetName(handle))
    meminfo = pynvml.nvmlDeviceGetMemoryInfo(handle)
    print(meminfo.total)  # 显卡总的显存大小,6442450944Bit
    print(meminfo.used)  # 显存使用大小,4401950720Bit
    print(meminfo.free)  # 显卡剩余显存大小,2040500224Bit
    print("Temperature is %d C" % pynvml.nvmlDeviceGetTemperature(handle,0))
    print("Fan speed is ", pynvml.nvmlDeviceGetFanSpeed(handle))
    print("Power ststus", pynvml.nvmlDeviceGetPowerState(handle))

pynvml.nvmlShutdown()