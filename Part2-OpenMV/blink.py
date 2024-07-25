import time
from pyb import LED

# 初始化LED灯
red_led = LED(1)    # 红色LED
green_led = LED(2)  # 绿色LED
blue_led = LED(3)   # 蓝色LED

# 定义闪烁函数
def blink_led(led, duration):
    led.on()
    time.sleep(duration)
    led.off()
    time.sleep(duration)

while True:
    # 红灯亮1秒，灭1秒
    blink_led(red_led, 1)
    # 蓝灯亮1秒，灭1秒
    blink_led(blue_led, 1)
    # 绿灯亮1秒，灭1秒
    blink_led(green_led, 1)
