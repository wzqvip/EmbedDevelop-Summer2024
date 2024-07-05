#include <Arduino.h>
#include <stdio.h>
#include "config.h"

void generalSetup();
float readTargetVolt();
void rotate(int speed);
int updatePWM(int currentPWM, float targetVolt, float currentVolt);