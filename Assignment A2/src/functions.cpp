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