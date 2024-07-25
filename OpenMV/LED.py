import time, pyb
from machine import LED

#id = 1  RED
#id = 2  GREEN
#id = 3  BLUE

while True:
    for i in range(1,4):
        led = pyb.LED(i)    #创建LED对象
        led.on()            #开灯
        time.sleep_ms(1000)
        led.off()           #关灯
        time.sleep_ms(1000)
