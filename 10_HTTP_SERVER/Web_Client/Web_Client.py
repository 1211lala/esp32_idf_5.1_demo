#!/usr/bin/env python3

import urllib.request
import json
import urllib.parse



# POST请求，带参数，对方也会返回接收是否成功
url = "http://192.168.8.62:1000/postdata"
data ={
    "name":"liuao",
    "age":18,
    "time": "2024-3-5:16:47:20"
}
json_data = json.dumps(data)
byte_data = json_data.encode("utf-8")
res  = urllib.request.urlopen(url, data=byte_data)
json_str = res.read().decode()
print(json_str)


# GET请求，不带参数，对方会返回数据
url = "http://192.168.8.62:1000/"
res  = urllib.request.urlopen(url)
json_str = res.read().decode()
print(json_str)