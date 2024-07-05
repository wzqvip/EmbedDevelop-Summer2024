#define LED 12
#define STBY 7
#define N 8
#define P 9
#define PWM 10
#define SetPoint A1
#define Sensor A0

float kp = 3; // P系数
float ki = 0.5; // I系数
float kd = 0; // D系数
float e_sum = 0; // 误差积分
float e_last = 0; // 上一个误差值
unsigned long last_time; // 上一次更新时间

const int MIN_PWM = 20; // 定义电机转动的最小PWM值

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
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    parseInput(input);
  }
  
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

  // 确保控制信号的绝对值不低于最小PWM值
  if (control_signal > 0) {
    control_signal = max(control_signal, MIN_PWM);
  } else if (control_signal < 0) {
    control_signal = min(control_signal, -MIN_PWM);
  }
  
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

  delay(20); // 缩短延时时间，提高更新频率
}

void parseInput(String input) {
  input.trim(); // 去除字符串首尾的空白字符
  
  if (input.startsWith("p=")) {
    kp = input.substring(2).toFloat();
    Serial.print("Updated kp to ");
    Serial.println(kp);
  } else if (input.startsWith("i=")) {
    ki = input.substring(2).toFloat();
    Serial.print("Updated ki to ");
    Serial.println(ki);
  } else if (input.startsWith("d=")) {
    kd = input.substring(2).toFloat();
    Serial.print("Updated kd to ");
    Serial.println(kd);
  } else {
    Serial.println("Invalid input");
  }
}
