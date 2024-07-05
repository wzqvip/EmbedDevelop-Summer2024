#define LED 12
#define STBY 7
#define N 8
#define P 9
#define PWM 10
#define Tune A0
#define Sensor A1

void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(LED, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(N, OUTPUT);
  pinMode(P, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(Tune, INPUT);
  pinMode(Sensor, INPUT);
}

void loop() {
  Serial.println("=======Begin!=====");
  int curr_pos = 0;
  int prev_pos = 0;
  
  digitalWrite(STBY, 1); // 启用电机驱动
  
  for (int speed = 80; speed > 0; speed -= 10) {
    prev_pos = analogRead(Sensor); // 读取当前传感器值
    
    Serial.print("Prev: ");
    Serial.println(prev_pos);
    
    // 顺时针转动
    digitalWrite(N, 1);
    digitalWrite(P, 0);
    analogWrite(PWM, speed);
    delay(1000); // 延时以确保电机有足够时间移动
    
    curr_pos = analogRead(Sensor); // 读取新位置

    Serial.print("Curr: ");
    Serial.println(curr_pos);

    if (curr_pos - prev_pos > 5) {
      Serial.print("Speed: ");
      Serial.print(speed);
      Serial.println(" Works!");
      digitalWrite(LED, 1); // 位置改变超过50，点亮LED
      break; // 找到最小有效PWM值后退出循环
    } else {
      digitalWrite(LED, 0); // 否则熄灭LED
    }
    
    // 反向转动以复位
    digitalWrite(N, 0);
    digitalWrite(P, 1);
    analogWrite(PWM, speed);
    delay(1000); // 延时以确保电机有足够时间复位
  }
  
  // 停止电机
  digitalWrite(STBY, 0);
  Serial.println("=======End!=====");
  
  delay(5000); // 延时以便观察结果
}
