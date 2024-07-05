#include "A4.h"
// 设置PID参数和初始化PID控制器
PIDController pid(CURRENTVAL, PWM, 2.0, 5.0, 1.0);

void A4setup() {
  // 调用通用设置函数
  generalSetup();
  
  // 设定目标电压为2.5V
  pid.setTargetVolt(2.5);
}

void A4loop() {
  // 更新PID控制器并获取控制信号
  float controlSignal = pid.updateSpeed();
  
  // 控制舵机旋转
  rotate(controlSignal);
  Serial.print(controlSignal);
  Serial.print(" ");
    Serial.println(analogRead(CURRENTVAL));
  // 延迟一段时间
  delay(100);
}
