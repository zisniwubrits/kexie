#!/usr/bin/env python3
# sine_table_generator.py

import math

# 配置参数
TABLE_SIZE = 256 # 查找表大小
MAX_VALUE = 1000   # 最大值（对应100%占空比）

def f(x):
    return x ** 2

# 生成正弦波查找表
sine_table = []
for i in range(1, TABLE_SIZE + 1):
    # 生成0到2π之间的角度
    angle = 2 * math.pi * i / TABLE_SIZE
    # 计算正弦值并缩放到0-MAX_VALUE范围
    value = round(f(i / 256) * MAX_VALUE)
    sine_table.append(value)

# 输出C语言数组格式
print("// 正弦波查找表 - 自动生成")
print(f"// 大小: {TABLE_SIZE}")
print(f"// 最大值: {MAX_VALUE}")
print("const uint16_t sine_table[{}] = ".format(TABLE_SIZE) + '{')
for i in range(0, TABLE_SIZE, 10):  # 每行10个值
    line = ", ".join(f"{sine_table[j]:4d}" for j in range(i, min(i+10, TABLE_SIZE)))
    print(f"    {line},")
print("};")
