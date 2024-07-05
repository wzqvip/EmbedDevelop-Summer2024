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
    // digitalWrite(PWM,HIGH);
    // analogWrite(PWM,255);
}

void A3loop(){
    
    for (int i = PWM_MAX; i > 0; i-=50)
    {
        int ticker = 0;
        analogWrite(PWM,i);
        int currentVal = analogRead(CURRENTVAL);
        int lastVal = currentVal;
        Serial.println(i);
        while (analogRead(CURRENTVAL) > 10)
        {

            digitalWrite(IN1,HIGH);
            digitalWrite(IN2,LOW);
            delay(20);
            lastVal = currentVal;
            currentVal = analogRead(CURRENTVAL);
            if (currentVal == lastVal)
            {
                ticker++;
            }
            else{
                ticker = 0;
            }
            if (ticker > 100)
            {
                Serial.print("PWM stopped at: ");
                Serial.println(i);
                ticker = 0;
                return;
            }
        }
        
        currentVal = analogRead(CURRENTVAL);
        lastVal = currentVal;
        while (analogRead(CURRENTVAL) < 1000)
        {
            digitalWrite(IN1,LOW);
            digitalWrite(IN2,HIGH);
            delay(20);
            lastVal = currentVal;
            currentVal = analogRead(CURRENTVAL);
            if (currentVal == lastVal)
            {
                ticker++;
            }
            else{
                ticker = 0;
            }
            if (ticker > 100)
            {
                Serial.print("PWM stopped at: ");
                Serial.println(i);
                ticker = 0;
                return;
            }
        }

    }
}




float voltToInt(float volt){
    return (min(volt,5.0f)*1024/5.0f);
}