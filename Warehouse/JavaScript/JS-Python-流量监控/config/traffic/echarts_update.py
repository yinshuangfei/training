#!/usr/bin/python3
import rrdtool
import sys
import json
import time

data = {}
# for time step
data_time = []
# for read
data_value_1= []
# for write
data_value_2 = []

oneDay = 60 * 60 * 24
oneHour = 60 * 60 
oneMinute = 60 

def printMetric():
    now = (int)(time.time())

    args = [
        "/var/lib/collectd/rrd/node1.pve/disk-sda/disk_octets.rrd", 
        "AVERAGE", 
        "-r" , str( 10 ),
        "-s" , str( now - oneMinute )
        ]
    rrdMetric = rrdtool.fetch(args)

    time_start = rrdMetric[0][0]
    time_end = rrdMetric[0][1]
    step = rrdMetric[0][2]
    print(rrdMetric[2])

    # key1 = rrdMetric[1][0]
    # key2 = rrdMetric[1][1]
    # ...

    for tuple in rrdMetric[2]:
        # time
        time_start = time_start + step
        data_time.append(time_start)

        # read 
        if tuple[0] == None:
            data_value_1.append(0)
        else:
            data_value_1.append(tuple[0])

        # write
        if tuple[1] == None:
            data_value_2.append(0)
        else:
            data_value_2.append(tuple[1])

    if time_start != time_end :
        print("error")
        pass
    else:
        # delete the last item
        data_time.pop()
        data_value_1.pop()
        data_value_2.pop()

        data = { "time" : data_time , 
                 "read" : data_value_1 ,
                 "write" : data_value_2 }

    data_json=json.dumps(data, indent=1)
    # data_json=json.dumps(data)
    f = open("config/traffic/data.json", "w") 
    # f = open("data.json", "w") 
    print(data_json, file = f)

printMetric()
