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
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(PWM,speed);
  }
  else if (direction == NEGATIVE)
  {
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    analogWrite(PWM,speed);
  }
  else{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    analogWrite(PWM,0);
  }
}

class PIDController {
private:
  int setpointPin;
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
  const int sampleSize = 10;
  int setpointSamples[10];
  int feedbackSamples[10];
  int sampleIndex;
  
public:
  // 构造函数
  PIDController(int spPin, int fbPin, int ctrlPin, float kp, float ki, float kd)
    : setpointPin(spPin), feedbackPin(fbPin), controlPin(ctrlPin), Kp(kp), Ki(ki), Kd(kd),
      setpoint(0), feedback(0), error(0), lastError(0), integral(0), derivative(0), sampleIndex(0) {
    pinMode(setpointPin, INPUT);
    pinMode(feedbackPin, INPUT);
    pinMode(controlPin, OUTPUT);
    // 初始化样本数组
    memset(setpointSamples, 0, sizeof(setpointSamples));
    memset(feedbackSamples, 0, sizeof(feedbackSamples));
  }
  
  // 更新并计算速度
  float updateSpeed() {
    // 采样信号处理和防抖动
    setpointSamples[sampleIndex] = analogRead(setpointPin);
    feedbackSamples[sampleIndex] = analogRead(feedbackPin);
    sampleIndex = (sampleIndex + 1) % sampleSize;
    
    // 计算平均值
    int setpointSum = 0;
    int feedbackSum = 0;
    for (int i = 0; i < sampleSize; i++) {
      setpointSum += setpointSamples[i];
      feedbackSum += feedbackSamples[i];
    }
    setpoint = setpointSum / sampleSize;
    feedback = feedbackSum / sampleSize;
    
    // 将模拟读数转换为实际值（假设参考电压为5V）
    setpoint = setpoint * (5.0 / 1023.0);
    feedback = feedback * (5.0 / 1023.0);
    
    // 计算PID控制
    error = setpoint - feedback;
    integral += error;
    derivative = error - lastError;
    float controlSignal = Kp * error + Ki * integral + Kd * derivative;
    lastError = error;
    
    // 将控制信号限制在0到255之间
    controlSignal = constrain(controlSignal, 0, 255);
    
    // 输出控制信号到PWM引脚
    analogWrite(controlPin, controlSignal);
    
    // 返回速度控制信号
    return controlSignal;
  }
};

// const int setpointPin = A0; // 给定值输入引脚
// const int feedbackPin = A1; // 反馈值输入引脚
// const int controlPin = 9;   // PWM输出引脚

// // PID控制器参数
// float Kp = 2.0;
// float Ki = 5.0;
// float Kd = 1.0;

// // 创建PID控制器实例
// PIDController pid(setpointPin, feedbackPin, controlPin, Kp, Ki, Kd);

// void setup() {
//   Serial.begin(9600);
// }

// void loop() {
//   // 更新并获取速度控制信号
//   float speed = pid.updateSpeed();
  
//   // 打印调试信息到串口监视器
//   Serial.print("Speed: ");
//   Serial.println(speed);
  
//   // 延迟一段时间
//   delay(100);
// }
