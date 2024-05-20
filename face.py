# -*- coding: utf-8 -*-
# 引入依赖包
# pip install alibabacloud_facebody20191230
# face.py
import os
import io
from urllib.request import urlopen
from alibabacloud_facebody20191230.client import Client
from alibabacloud_facebody20191230.models import SearchFaceAdvanceRequest
from alibabacloud_tea_openapi.models import Config
from alibabacloud_tea_util.models import RuntimeOptions
config = Config(
        access_key_id=os.environ.get('ALIBABA_CLOUD_ACCESS_KEY_ID'),
        access_key_secret=os.environ.get('ALIBABA_CLOUD_ACCESS_KEY_SECRET'),
        # 访问的域名
        endpoint='facebody.cn-shanghai.aliyuncs.com',
        # 访问的域名对应的region
        region_id='cn-shanghai'
        )
def face_search():
    search_face_request = SearchFaceAdvanceRequest()
    #场景一：文件在本地
    stream0 = open(r'/home/zhihui/yuanxueshe/pic/myface.jpg', 'rb')
    search_face_request.image_url_object = stream0
    #场景二：使用任意可访问的url
    #url = 'https://viapi-test-bj.oss-cn-beijing.aliyuncs.com/viapi3.0domepic/facebody/SearchFace1.png'
    #img = urlopen(url).read()
    #search_face_request.image_url_object = io.BytesIO(img)
    search_face_request.db_name = 'default'
    search_face_request.limit = 5
    runtime_option = RuntimeOptions()
    try:
      # 初始化Client
      client = Client(config)
      response = client.search_face_advance(search_face_request, runtime_option)
      # 获取整体结果
      max_score = max(item['Score'] for item in response.body.to_map()['Data']['MatchList'][0]['FaceItems'])
      return (round(max_score,2))
    except Exception as error:
        # 获取整体报错信息
      print(error)
      # 获取单个字段
      print(error.code)
      # tips: 可通过error.__dict__查看属性名称
    #关闭流
    #stream0.close()
if __name__ == '__main__':
    face_search()
