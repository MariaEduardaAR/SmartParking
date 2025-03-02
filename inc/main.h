#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c-dev.h>

#define GPIO12 "12"  // sensor de obstaculo para vaga - P8_12
#define GPIO13 "13"  // sensor de obstaculo para vaga - P8_11
#define GPIO28 "28"  // sensor de obstaculo para cancela entrada - P9_12
#define GPIO17 "17"  // sensor de obstaculo para cancela saida -P9_23
#define SERVO_PWM1 "/sys/class/pwm/pwmchip0/pwm0/" //servo motor entrada P9_42
#define SERVO_PWM2 "/sys/class/pwm/pwmchip1/pwm0/" //servo motor saida P9_14

#define I2C_DEVICE_FILE_PATH "/dev/i2c-2"

extern int fd;

#endif