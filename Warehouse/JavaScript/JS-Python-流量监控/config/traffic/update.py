#!/usr/bin/python3
import rrdtool
import sys
import json

data = []

def printMetric():

    args = ["/var/lib/collectd/rrd/node1.pve/disk-sda/disk_octets.rrd", "AVERAGE","-r","120"]
    rrdMetric = rrdtool.fetch(args)

    time_start = rrdMetric[0][0]
    time_end = rrdMetric[0][1]
    step = rrdMetric[0][2]

    #key1 = rrdMetric[1][0]
    #key2 = rrdMetric[1][1]

    for tuple in rrdMetric[2]:
        time_start = time_start + step
        #data.append([time_start,tuple[0],tuple[1]])
        if tuple[1] == None:
            data.append([time_start,0])
        else:
            data.append([time_start,tuple[1]])

    if time_start != time_end :
        print("error")
        data.clear()
    else:
        # delete the last item
        data.pop()
        data_json=json.dumps(data, indent=1)
        f = open("config/traffic/data.json", "w") 
        print(data_json, file = f)
        print(data)
        d=data.pop()
        print(d)

printMetric()
