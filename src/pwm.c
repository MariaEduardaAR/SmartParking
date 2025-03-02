#include "pwm.h"
#include "main.h"
#include "gpio.h"

void configurePWM() {
    writeToFile("/sys/class/pwm/pwmchip0/export", "0");
    usleep(100000);
    writeToFile("/sys/class/pwm/pwmchip1/export", "0");
    usleep(100000);
    writeToFile(SERVO_PWM1 "enable", "0");
    writeToFile(SERVO_PWM1 "period", "20000000");
    writeToFile(SERVO_PWM1 "duty_cycle", "1900000");
    writeToFile(SERVO_PWM1 "enable", "1");
    writeToFile(SERVO_PWM2 "enable", "0");
    writeToFile(SERVO_PWM2 "period", "20000000");
    writeToFile(SERVO_PWM2 "duty_cycle", "1000000");
    writeToFile(SERVO_PWM2 "enable", "1");
}

void setServoAngle(int servo, int angle) {
    int duty_cycle = 1000000 + (angle * 10000);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", duty_cycle);
    if (servo == 1) {
        writeToFile(SERVO_PWM1 "duty_cycle", buffer);
    } else if (servo == 2) {
        writeToFile(SERVO_PWM2 "duty_cycle", buffer);
    }
}