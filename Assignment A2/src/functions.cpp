#include "functions.h"

void generalSetup(){

  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(SETVAL, OUTPUT);
  pinMode(CURRENTVAL, INPUT);

}

float readTargetVolt(){
    float currentVolt = (float)analogRead(CURRENTVAL)/1024.0f * 5.0f;
    return currentVolt;
}
void rotate(int speed){
  int direction;
  if(speed > 0){
    direction = POSITIVE;
  }
  else if(speed < 0){
    direction = NEGATIVE;
  }
  else{
    direction = 2;
  }

  speed = abs(speed);
  if (speed > PWM_MAX)
  {
    speed = PWM_MAX;
  }
  if (direction == POSITIVE)
  {
    digitalWrite(IN2,HIGH);
    digitalWrite(IN1,LOW);
    analogWrite(PWM,speed);
  }
  else if (direction == NEGATIVE)
  {
    digitalWrite(IN2,LOW);
    digitalWrite(IN1,HIGH);
    analogWrite(PWM,speed);
  }
  else{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    analogWrite(PWM,0);
  }
}

// PID控制器类的构造函数实现
PIDController::PIDController(int fbPin, int ctrlPin, float kp, float ki, float kd)
  : feedbackPin(fbPin), controlPin(ctrlPin), Kp(kp), Ki(ki), Kd(kd),
    setpoint(0), feedback(0), error(0), lastError(0), integral(0), derivative(0), sampleIndex(0) {
  pinMode(feedbackPin, INPUT);
  pinMode(controlPin, OUTPUT);
  // 初始化样本数组
  memset(feedbackSamples, 0, sizeof(feedbackSamples));
}

// 设置目标电压的方法实现
void PIDController::setTargetVolt(float targetVolt) {
  // 将目标电压转换为模拟读数
  setpoint = targetVolt;
}

// PID控制器类的updateSpeed方法实现
float PIDController::updateSpeed() {
  // 采样信号处理和防抖动
  feedbackSamples[sampleIndex] = analogRead(feedbackPin);
  sampleIndex = (sampleIndex + 1) % sampleSize;
  
  // 计算平均值
  int feedbackSum = 0;
  for (int i = 0; i < sampleSize; i++) {
    feedbackSum += feedbackSamples[i];
  }
  feedback = feedbackSum / sampleSize;
  
  // 将模拟读数转换为实际值（假设参考电压为5V）
  feedback = feedback * (5.0 / 1023.0);
  
  // 计算PID控制
  error = setpoint - feedback;
  integral += error;
  derivative = error - lastError;
  float controlSignal = Kp * error + Ki * integral + Kd * derivative;
  lastError = error;
  
  // 将控制信号限制在0到255之间
  controlSignal = constrain(controlSignal, -255, 255);
  
  // 返回速度控制信号
  return controlSignal;
}
