#include "main.h"
#include "lcd1602.h"
#include "gpio.h"
#include "pwm.h"
#include "sensor.h"

int fd;

int main() {
    // Iniciar o servidor
    if (system("/usr/bin/server &") == -1) {
        perror("Erro ao iniciar o servidor");
        return 1;
    }

    // Limpar o LCD duas vezes
    for (int i = 0; i < 2; i++) {
        lcd_send(fd, 0x01, 0);
        usleep(CMD_DELAY);
    }

    printf("Configurando sensores e servos...\n");
    configurePWM();
    setup_gpio(GPIO12);
    setup_gpio(GPIO13);
    setup_gpio(GPIO28);
    setup_gpio(GPIO17);

    // Abrir o dispositivo I2C
    fd = open(I2C_DEVICE_FILE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Erro ao abrir o dispositivo I2C");
        return -1;
    }

    // Configurar o endereço I2C do LCD
    if (ioctl(fd, I2C_SLAVE, LCD_ADDR) < 0) {
        perror("Erro ao configurar o endereço do escravo");
        close(fd);
        return -1;
    }

    // Inicializar o LCD
    if (lcd_init(fd) < 0) {
        fprintf(stderr, "Erro ao inicializar o LCD\n");
        close(fd);
        return -1;
    }

    // Limpar o LCD e exibir mensagem inicial
    lcd_send(fd, 0x01, 0);  // Limpar o display
    lcd_set_cursor(fd, 0, 0);
    lcd_print(fd, "  SmartParking");

    int last_vagas_ocupadas = -1;
    while (1) {
        // Ler sensores de vagas
        int vagas_ocupadas = read_sensor(GPIO12) + read_sensor(GPIO13);
        if (vagas_ocupadas != last_vagas_ocupadas) {
            last_vagas_ocupadas = vagas_ocupadas;

            // Atualizar o LCD com o número de vagas
            lcd_set_cursor(fd, 1, 0);
            lcd_print(fd, "Vagas:");
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%d", 2 - vagas_ocupadas);
            lcd_print(fd, buffer);
            lcd_print(fd, " | Ocup:");
            snprintf(buffer, sizeof(buffer), "%d", vagas_ocupadas);
            lcd_print(fd, buffer);
        }

        // Ler sensores de obstáculos e controlar servos
        int obstaculo1 = read_sensor(GPIO28);
        int obstaculo2 = read_sensor(GPIO17);
        if (obstaculo1) {
            setServoAngle(1, 0);
            sleep(2);
            setServoAngle(1, 90);
        }
        if (obstaculo2) {
            setServoAngle(2, 90);
            sleep(2);
            setServoAngle(2, 0);
        }

        sleep(1);
    }

    // Fechar o dispositivo I2C
    close(fd);
    return 0;
}