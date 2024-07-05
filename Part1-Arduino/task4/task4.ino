#define LED 12
#define STBY 7
#define N 8
#define P 9
#define PWM 10
#define SetPoint A1
#define Sensor A0

float kp = 1.0; // P系数
float ki = 0.1; // I系数
float kd = 0.01; // D系数
float e_sum = 0; // 误差积分
float e_last = 0; // 上一个误差值
unsigned long last_time; // 上一次更新时间

// PID控制函数
float pid(float error) {
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0;
  last_time = now;
  
  e_sum += error * dt;
  float e_diff = (error - e_last) / dt;
  float output = kp * error + ki * e_sum + kd * e_diff;
  e_last = error;
  return output;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(LED, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(N, OUTPUT);
  pinMode(P, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(SetPoint, INPUT);
  pinMode(Sensor, INPUT);
  
  last_time = millis(); // 初始化时间
}

void loop() {
  int set_point = analogRead(SetPoint); // 读取设定值
  int curr_pos = analogRead(Sensor); // 读取当前传感器值
  
  Serial.print("Set Point: ");
  Serial.println(set_point);
  
  Serial.print("Current Position: ");
  Serial.println(curr_pos);
  
  float error = set_point - curr_pos; // 计算误差
  float control_signal = pid(error); // 计算PID控制信号
  
  // 将控制信号限制在-255到255之间
  control_signal = constrain(control_signal, -255, 255);
  
  Serial.print("Control Signal: ");
  Serial.println(control_signal);

  digitalWrite(STBY, 1); // 启用电机驱动
  
  if (control_signal > 0) {
    digitalWrite(N, 0);
    digitalWrite(P, 1);
    analogWrite(PWM, control_signal);
  } else {
    digitalWrite(N, 1);
    digitalWrite(P, 0);
    analogWrite(PWM, -control_signal);
  }
  
  // 判断是否达到设定值
  if (abs(set_point - curr_pos) < 10) {
    digitalWrite(LED, 1); // 如果达到设定值，点亮LED
  } else {
    digitalWrite(LED, 0);
  }

  delay(50); // 短暂延时以避免过度频繁的控制信号更新
}
