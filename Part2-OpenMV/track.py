import sensor, image, time, math
from pyb import Servo

# 舵机初始化
sy = Servo(1)  # Y方向舵机
sx = Servo(2)  # X方向舵机

# 颜色阈值索引和定义
threshold_index = 2  # 0 for red, 1 for green, 2 for blue

# 颜色跟踪阈值 (L Min, L Max, A Min, A Max, B Min, B Max)
thresholds = [
    (30, 100, 15, 127, 15, 127),  # generic_red_thresholds
    (30, 100, -64, -8, -32, 32),  # generic_green_thresholds
    (0, 30, 0, 64, -128, 0)       # generic_blue_thresholds
]

# 摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # 必须关闭此功能以进行颜色跟踪
sensor.set_auto_whitebal(False)  # 必须关闭此功能以进行颜色跟踪
clock = time.clock()

# 中心位置
center_x = 160
center_y = 120

# 初始舵机位置
sx.angle(0)
sy.angle(0)

while True:
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs(
        [thresholds[threshold_index]],
        pixels_threshold=200,
        area_threshold=200,
        merge=True,
    )
    if blobs:
        # 取最大的blob
        largest_blob = max(blobs, key=lambda b: b.pixels())

        # 获取色块中心坐标
        blob_x = largest_blob.cx()
        blob_y = largest_blob.cy()

        # 计算偏移量
        offset_x = blob_x - center_x
        offset_y = blob_y - center_y

        # 绘制检测到的色块
        img.draw_circle(largest_blob.cx(), largest_blob.cy(), int(math.sqrt(largest_blob.area()/math.pi)), color=(0, 0, 255))
        img.draw_cross(largest_blob.cx(), largest_blob.cy())

        # 调整舵机角度以使色块中心对准画面中心
        if abs(offset_x) > 10:  # 偏差大于10像素时进行调整
            new_sx_angle = sx.angle() - offset_x * 0.1  # 调整速度为0.1
            new_sx_angle = max(min(new_sx_angle, 90), -90)  # 限制角度范围在-90到90度之间
            sx.angle(new_sx_angle)

        if abs(offset_y) > 10:  # 偏差大于10像素时进行调整
            new_sy_angle = sy.angle() + offset_y * 0.1  # 调整速度为0.1
            new_sy_angle = max(min(new_sy_angle, 90), -90)  # 限制角度范围在-90到90度之间
            sy.angle(new_sy_angle)

    print(clock.fps())  # 打印帧率
