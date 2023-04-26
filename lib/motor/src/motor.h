#include <Servo.h>
#include <Arduino.h>

Servo ESC;

void motor_init() {
    ESC.attach(1);
    ESC.writeMicroseconds(1000);

    delay(5000);
}

void motor_speed(int speed) {
    ESC.writeMicroseconds(speed);
}