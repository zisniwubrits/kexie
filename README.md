# 项目说明

本项目基于 STM32F103 微控制器，包含以下主要功能：

- 使用 OLED 显示屏进行数据显示
- 通过 ADC 采集模拟电压
- 利用定时器 TIM3 产生 PWM 波形
- 支持多种工作模式（通过按键切换）
- 包含正弦波查找表和自定义 PWM 查找表

## 目录结构

```
Hardware/         # OLED 驱动及字模数据
Library/          # STM32 标准外设库
User/             # 用户主程序(main.c等)
Startup/          # 启动文件
DebugConfig/      # 调试配置
Objects/          # 编译生成的目标文件
Listings/         # 编译生成的列表文件
sin_table.py      # 正弦表生成脚本
sin_table2.py     # 另一种正弦表生成脚本
table.py          # 查找表生成脚本
```

## 主要文件说明

- main.c  
  主程序入口，包含模式切换、ADC采样、PWM输出、OLED数据显示等核心逻辑。

- OLED.c、OLED.h  
  OLED 显示屏驱动及相关显示函数。

- OLED_Data.c、OLED_Data.h  
  OLED 字模和图片数据。

- Library  
  STM32F10x 标准外设库源码。

- sin_table.py、sin_table2.py、table.py  
  用于生成正弦波或自定义查找表的 Python 脚本。

## 编译与烧录

1. 使用 Keil MDK 打开 kdemo.uvprojx 工程文件。
2. 编译工程，生成固件。
3. 通过 ST-Link 或其他下载器将固件烧录到 STM32F103C8T6 开发板。

## 工作模式说明

- **模式0**：根据 ADC 电压查表输出 PWM，占空比随电压变化。
- **模式1**：根据电压区间控制 GPIO 输出和 PWM。
- **模式2**：输出正弦波 PWM，频率随电压变化。

## 依赖

- STM32F10x 标准外设库
- Keil MDK 或其他 ARM 编译环境

---

如需更多帮助，请参考代码注释或相关硬件资料。
