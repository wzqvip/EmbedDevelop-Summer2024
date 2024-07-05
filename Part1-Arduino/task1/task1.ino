/*

Task1:

Read voltage from A1.
V<3.3: blink
1<V<3.3: speedup
V<1: on

*/

#define Input A1
#define LED 12 // LED on extension board

int status = 0;
int value = 0;
int v = 0;

void setup(){
  pinMode(Input, INPUT);
  pinMode(LED, OUTPUT);

  Serial.begin(9600);
}

void loop(){
  value = analogRead(Input);
  v = map(value, 0, 1023, 0, 5);

  Serial.print("Read: ");
  Serial.println(value);

  if (v < 1) {
    digitalWrite(LED, 1);
  }
  else {
    digitalWrite(LED, status);
    status = 1-status;
    delay(value);
  }

  delay(10);

}
