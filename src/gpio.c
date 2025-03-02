#include "gpio.h"
#include "main.h"

void writeToFile(const char *filename, const char *value) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Erro ao abrir arquivo");
        return;
    }
    fprintf(fp, "%s", value);
    fclose(fp);
}

void setup_gpio(const char *gpio) {
    char path[50];

     // Garante que o GPIO está limpo antes de exportar
     writeToFile("/sys/class/gpio/unexport", gpio); 
     usleep(100000); // Aguarda antes de exportar novamente
 
     // Exporta o GPIO
     writeToFile("/sys/class/gpio/export", gpio);
     usleep(100000); // Aguarda a exportação do GPIO
 
     // Define o GPIO como entrada
     snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", gpio);
     writeToFile(path, "in");
 
}



