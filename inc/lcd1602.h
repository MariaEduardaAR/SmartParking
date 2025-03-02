#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Definições de constantes
#define I2C_BUS "/dev/i2c-2"
#define LCD_ADDR 0x27

// Mascaras para controle do LCD
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_RS 0x01

// Delays configuráveis em microssegundos
#define PULSE_DELAY 500
#define CMD_DELAY 1000
#define INIT_DELAY 4500
#define TEXT_DELAY 1000000

// Protótipos das funções
int lcd_init(int fd);
int lcd_send(int fd, uint8_t data, uint8_t mode);
int lcd_print(int fd, const char *str);
void lcd_set_cursor(int fd, int row, int col);
void lcd_clear_line(int fd, int row);
void display_text_animation_with_repeats(int fd, const char **texts, int num_texts, int repeats);

#endif // LCD_H