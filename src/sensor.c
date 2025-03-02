#include "sensor.h"
#include "main.h"

int read_sensor(const char *gpio) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", gpio);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Erro ao abrir sensor");
        return -1;
    }
    char buffer[2];  
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("Erro ao ler sensor");
        fclose(file);
        return -1;
    }
    fclose(file);
    return atoi(buffer) == 0 ? 1 : 0;
}