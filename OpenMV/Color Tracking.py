# Single Color RGB565 Blob Tracking Example
#
# This example shows off single color RGB565 tracking using the OpenMV Cam.

import sensor, math, time, image
from pyb import Servo

threshold_index = 0  # 0 for red, 1 for green, 2 for blue

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green/blue things. You may wish to tune them...
thresholds = (0, 100, -61, 0, -128, -13)     #通过阈值编译器找到蓝色的LAB阈值


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
clock = time.clock()

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" merges all overlapping blobs in the image.

s1 = Servo(1)    #在P7引脚创建servo对象
s2 = Servo(2)    #在P8引脚创建servo对象
center_x = 160   #QVGA屏幕的中心点坐标
center_y = 120

while True:
    clock.tick()
    img = sensor.snapshot()
    for blob in img.find_blobs([thresholds], pixels_threshold=200, area_threshold=200, merge=True,):
        # These values depend on the blob not being circular - otherwise they will be shaky.
        pass
        # These values are stable all the time.
        img.draw_circle(blob.enclosing_circle())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        angle_x = s2.angle()      #找到当前舵机的P7和P8引脚的旋转角度
        angle_y = s1.angle()
        if abs(blob.cx() - center_x) > 20:     #若x坐标不在中心点范围，则适当转动s2
            print(center_x, center_y, blob.cx(), blob.cy(), angle_x, angle_y)
            if blob.cx() < center_x:
                s2.angle(angle_x - 2)
            if blob.cx() > center_x:
                s2.angle(angle_x + 2)
        if abs(blob.cy() - center_y) > 10:     #若y坐标不在中心点范围，则适当转动s1

            if blob.cy() < center_y:
                s1.angle(angle_y + 2)
            if blob.cy() > center_y:
                s1.angle(angle_y - 2)

    print(clock.fps())

