#ifndef GPIO_H
#define GPIO_H

void setup_gpio(const char *gpio);
void configureSensor(const char *gpio);
void writeToFile(const char *filename, const char *value);

#endif // GPIO_H