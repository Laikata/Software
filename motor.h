#pragma once
#include <Servo.h>

Servo ESC;

void motor_init() {
    ESC.attach(14);
    ESC.writeMicroseconds(1000);

    delay(5000);
}

void motor_speed(int speed) {
    ESC.writeMicroseconds(speed);
}