#include "lcd1602.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>

// Função para gerar pulso no pino Enable
static void lcd_pulse(int fd, uint8_t data) {
    uint8_t buf[1];
    
    buf[0] = data;
    if (write(fd, buf, 1) != 1) {
        perror("Erro no pulso Enable");
        return;
    }
    usleep(PULSE_DELAY);
    
    buf[0] = data | LCD_ENABLE;
    if (write(fd, buf, 1) != 1) {
        perror("Erro no pulso Enable");
        return;
    }
    usleep(PULSE_DELAY);
    
    buf[0] = data;
    if (write(fd, buf, 1) != 1) {
        perror("Erro no pulso Enable");
        return;
    }
    usleep(PULSE_DELAY);
}

// Função para enviar comandos/dados ao LCD
int lcd_send(int fd, uint8_t data, uint8_t mode) {
    uint8_t high_nibble = (data & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t low_nibble = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

    lcd_pulse(fd, high_nibble);
    lcd_pulse(fd, low_nibble);
    
    usleep(CMD_DELAY);
    return 0;
}

// Inicializar o LCD
int lcd_init(int fd) {
    usleep(INIT_DELAY);

    uint8_t init_sequence[] = {0x03, 0x03, 0x03, 0x02};
    for (int i = 0; i < 4; i++) {
        if (lcd_send(fd, init_sequence[i], 0) < 0) {
            return -1;
        }
        usleep(INIT_DELAY);
    }

    if (lcd_send(fd, 0x28, 0) < 0 ||  // modo 4 bits, 2 linhas
        lcd_send(fd, 0x08, 0) < 0 ||  // display off
        lcd_send(fd, 0x01, 0) < 0 ||  // clear display
        lcd_send(fd, 0x06, 0) < 0 ||  // entry mode set
        lcd_send(fd, 0x0C, 0) < 0) {  // display on
        return -1;
    }

    return 0;
}

// Envia uma string ao LCD
int lcd_print(int fd, const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (lcd_send(fd, (uint8_t)str[i], LCD_RS) < 0) {
            return -1;
        }
    }
    return 0;
}

// Posicionar o cursor
void lcd_set_cursor(int fd, int row, int col) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send(fd, 0x80 | (row_offsets[row] + col), 0);
}

// Limpar uma linha específica
void lcd_clear_line(int fd, int row) {
    lcd_set_cursor(fd, row, 0);
    for (int i = 0; i < 16; i++) {
        lcd_print(fd, " ");
    }
    lcd_set_cursor(fd, row, 0);
}

// Exibir texto com animação
void display_text_animation_with_repeats(int fd, const char **texts, int num_texts, int repeats) {
    for (int i = 0; i < repeats; i++) {
        for (int j = 0; j < num_texts; j++) {
            lcd_clear_line(fd, 0);
            lcd_clear_line(fd, 1);
            
            int text_len = strlen(texts[j]);
            int start_pos = (16 - text_len) / 2;
            
            lcd_set_cursor(fd, 0, start_pos);
            lcd_print(fd, texts[j]);
            
            usleep(TEXT_DELAY);
        }
    }
}