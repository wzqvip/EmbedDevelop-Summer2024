/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(12, OUTPUT);  //LED
  pinMode(7, OUTPUT);   //STBY
  pinMode(8, OUTPUT);   //negative 11-8
  pinMode(9, OUTPUT);   //positive 12-9
  pinMode(10, OUTPUT);  //PWM
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(7, HIGH);   //STBY = HIGH
  int Initial = analogRead(A1);
  for(int speed = 50; speed <= 100; speed += 10) {
    //正转
    digitalWrite(12, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);
    analogWrite(10, speed);
    delay(800);
    int voltage = analogRead(A1);
    Serial.println(speed);
    //反转
    digitalWrite(12, LOW);
    while(Initial - voltage > 0) {
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      analogWrite(10, speed);
      delay(10);
      voltage = analogRead(A1);
    }
    
    Serial.println(voltage);
  }

}
