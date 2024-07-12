/*

====串口未初始化前程序不会运行！====

PLOTTER设置为1的话用绘图器可视化调节，设置为0用串口监视器人眼看

串口调参方法：

1. 打开串口监视器, 波特率默认115200
2. 输入以下命令进行参数调整：
   - p=1.5: 设置P系数为1.5
   - i=0.5: 设置I系数为0.5
   - d=0.1: 设置D系数为0.1
   - s=100: 设置设定值为100
3. 输入以上命令后，可以通过串口监视器查看当前参数值
*/

#define PLOTTER 1

#define LED 12
#define STBY 7
#define N 8
#define P 9
#define PWM 10
#define SetPoint A1
#define Sensor A0

#define delay_time 20

float kp = 3;            // P系数
float ki = 0.5;          // I系数
float kd = 0.1;          // D系数
float e_sum = 0;         // 误差积分
float e_last = 0;        // 上一个误差值
unsigned long last_time; // 上一次更新时间

const int MIN_PWM = 68;           // 定义电机转动的最小PWM值
const int POSITION_THRESHOLD = 2; // 位置变化阈值
const int CONTROL_THRESHOLD = 2;  // 控制信号阈值

int set_point = 0;
int prev_set_point = 0;
int prev_pos = 0;

// PID控制函数
float pid(float pos_error)
{
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0;
  last_time = now;
  e_sum += pos_error * dt;
  float e_diff = (pos_error - e_last) / dt;
  float output = kp * pos_error + ki * e_sum + kd * e_diff;
  e_last = pos_error;
  return output;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  pinMode(LED, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(N, OUTPUT);
  pinMode(P, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(SetPoint, INPUT);
  pinMode(Sensor, INPUT);

  last_time = millis(); // 初始化时间
}

void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n');
    parseInput(input);
  }

  int curr_pos = analogRead(Sensor); // 读取当前传感器值
  set_point = analogRead(SetPoint);

  int power = analogRead(A2);
  int ready = 0;

  // 判断是否达到设定值
  if (abs(set_point - curr_pos) < 10)
  {
    ready = 1;
    digitalWrite(LED, 1); // 如果达到设定值，点亮LED
  }
  else
  {
    ready = 0;
    digitalWrite(LED, 0);
  }

  if (PLOTTER)
  {
    Serial.print(float(set_point) / 10.24);
    Serial.print(",");
    Serial.print(float(curr_pos) / 10.24);
    Serial.print(",");
    Serial.print(kp);
    Serial.print(",");
    Serial.print(ki);
    Serial.print(",");
    Serial.print(kd);
    Serial.print(",");
    Serial.print(ready);
    Serial.print("\n");
  }
  else
  {
    Serial.print("Set Point: ");
    Serial.print(set_point / 1023);

    Serial.print(" Current Position: ");
    Serial.print(curr_pos / 1023);

    Serial.print(" P: ");
    Serial.print(kp);
    Serial.print(" I: ");
    Serial.print(ki);
    Serial.print(" D: ");
    Serial.print(kd);

    Serial.print("\n");
  }

  float pos_error = set_point - curr_pos;           // 计算误差
  float control_error = set_point - prev_set_point; // 计算控制信号误差
  float control_signal = pid(pos_error);            // 计算PID控制信号

  // 将控制信号限制在-255到255之间
  control_signal = constrain(control_signal, -255, 255);

  // 确保控制信号的绝对值不低于最小PWM值
  if (control_signal > 40)
  {
    control_signal = max(control_signal, MIN_PWM);
  }
  else if (control_signal < -40)
  {
    control_signal = min(control_signal, -MIN_PWM);
  }
  else
  {
    // control_signal = 0;
  }

  if (abs(pos_error) < POSITION_THRESHOLD && abs(control_error) < CONTROL_THRESHOLD)
  {
    digitalWrite(STBY, 0); // 关闭电机驱动
  }
  else
  {
    digitalWrite(STBY, 1); // 启用电机驱动

    if (control_signal > 0)
    {
      digitalWrite(N, 0);
      digitalWrite(P, 1);
      analogWrite(PWM, control_signal);
    }
    else
    {
      digitalWrite(N, 1);
      digitalWrite(P, 0);
      analogWrite(PWM, -control_signal);
    }

    prev_set_point = set_point;
    prev_pos = curr_pos;

    delay(delay_time); // 短暂延时以避免过度频繁的控制信号更新
  }
}

void parseInput(String input)
{
  input.trim(); // 去除字符串首尾的空白字符

  if (input.startsWith("p="))
  {
    kp = input.substring(2).toFloat();
    Serial.print("Updated kp to ");
    Serial.println(kp);
  }
  else if (input.startsWith("i="))
  {
    ki = input.substring(2).toFloat();
    Serial.print("Updated ki to ");
    Serial.println(ki);
  }
  else if (input.startsWith("d="))
  {
    kd = input.substring(2).toFloat();
    Serial.print("Updated kd to ");
    Serial.println(kd);
  }
  else if (input.startsWith("s="))
  {
    set_point = input.substring(2).toInt();
    Serial.print("Updated set point to ");
    Serial.println(set_point);
  }
  else
  {
    Serial.println("Invalid input");
  }
}
