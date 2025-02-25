import json
import pinyin

# 读取json文件
with open('city.json', 'r', encoding='utf-8') as f:
    data = json.load(f)

# 遍历json数组
for city in data:
    # 获取每个城市的拼音
    city_pinyin = pinyin.get(city['city_name'], format='strip')
    # 首字母大写
    city_pinyin = city_pinyin.capitalize()
    #在json中添加新字段ENcity_name
    city['ENcity_name'] = city_pinyin

# 将更新后的json写入新文件
with open('city_pinyin.json', 'w', encoding='utf-8') as f:
    json.dump(data, f, ensure_ascii=False)
