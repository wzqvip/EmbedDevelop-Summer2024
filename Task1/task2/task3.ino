/*

*/

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
  // int speed = 255;  
  int curr_pos = 0;
  int prev_pos = 0;
    
  digitalWrite(STBY, 1); // Enable
  
  for (int speed = 100; speed>0; speed -=10) {
    prev_pos = analogRead(Sensor);

    
    Serial.print("Prev: " + prev_pos);
    
    // Clockwise
    digitalWrite(N,1);
    digitalWrite(P,0);
    analogWrite(PWM, speed);
    delay(600); // 

    curr_pos = analogRead(Sensor);

    Serial.println("Curr: " + curr_pos);

    if (curr_pos - prev_pos > 50) {
      Serial.print(speed);
      Serial.println(" Works!");
      digitalWrite(LED,1);
    }
    else {
      digitalWrite(LED,0);
    }
    
    // Reverse to reset
    digitalWrite(N,0);
    digitalWrite(P,1);
    analogWrite(PWM, speed);
    delay(600); //

    // value -= 20; //Slow Down
  }
  
}
