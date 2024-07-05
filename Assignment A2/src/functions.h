#include <Arduino.h>
#include <stdio.h>
#include "config.h"

void generalSetup();
float readTargetVolt();
void rotate(int speed);
int updatePWM(int currentPWM, float targetVolt, float currentVolt);

#ifndef PIDController_h
#define PIDController_h
// PID控制器类
class PIDController {
private:
  int feedbackPin;
  int controlPin;
  
  // PID参数
  float Kp;
  float Ki;
  float Kd;
  
  // PID变量
  float setpoint;
  float feedback;
  float error;
  float lastError;
  float integral;
  float derivative;
  
  // 防抖动变量
  static const int sampleSize = 10;
  int feedbackSamples[sampleSize];
  int sampleIndex;
  
public:
  // 构造函数
  PIDController(int fbPin, int ctrlPin, float kp, float ki, float kd);
  
  // 设置目标电压
  void setTargetVolt(float targetVolt);

  // 更新并计算速度
  float updateSpeed();
};
#endif