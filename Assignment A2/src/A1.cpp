#include <A1.h>


float currentVolt = 0;

void A1setup() {
  generalSetup();
}

void A1loop() {
  // Just read the current value and set the led blinking rate correspondingly
  currentVolt = readTargetVolt();
  blink();
  Serial.print("Volt:");
  Serial.println(currentVolt);
}



void blink(){
  if (currentVolt<1.0f){
    digitalWrite(LED,HIGH);
    delay(500);
  }
  else if (currentVolt<3.3f){
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);
    delay(250);
  }
  else{
    digitalWrite(LED,HIGH);
    delay(50);
    digitalWrite(LED,LOW);
    delay(50);
  }
}