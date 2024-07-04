#include "A3.h"


void A3setup(){
    Serial.begin(9600);   
  // put your setup code here, to run once:
    pinMode(LED, OUTPUT);
    pinMode(STBY, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(SETVAL, INPUT);
    pinMode(CURRENTVAL, INPUT);

    digitalWrite(STBY,HIGH);
    digitalWrite(PWM,HIGH);
}

void A3loop(){
    // digitalWrite(IN1,LOW);
    // digitalWrite(IN2,HIGH);
    // delay(50);
    // digitalWrite(IN1,HIGH);
    // digitalWrite(IN2,LOW);
    // delay(50);
    analogWrite(IN1,voltToInt(5));
    analogWrite(IN2,voltToInt(0));
    delay(200);
    
    analogWrite(IN1,voltToInt(0));
    analogWrite(IN2,voltToInt(5));
    delay(200);
}




float voltToInt(float volt){
    return (min(volt,5.0f)*1024/5.0f);
}