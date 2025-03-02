################################################################################
# Buildroot Makefile for the PROJETO application
################################################################################

PROJETO_VERSION = 1.0
PROJETO_SITE = package/projeto
PROJETO_LICENSE = MIT
PROJETO_SITE_METHOD = local

# Definindo diretórios
PROJETO_SRC_DIR = $(PROJETO_SITE)/src
PROJETO_INC_DIR = $(PROJETO_SITE)/inc
PROJETO_OBJ_DIR = $(PROJETO_SITE)/obj
PROJETO_BIN_DIR = $(PROJETO_SITE)/bin

# Lista de arquivos fonte
PROJETO_SRC_FILES = $(wildcard $(PROJETO_SRC_DIR)/*.c)
PROJETO_OBJ_FILES = $(patsubst $(PROJETO_SRC_DIR)/%.c, $(PROJETO_OBJ_DIR)/%.o, $(PROJETO_SRC_FILES))

# Comandos para compilar a aplicação
define PROJETO_BUILD_CMDS
    mkdir -p $(PROJETO_OBJ_DIR) $(PROJETO_BIN_DIR)
    $(TARGET_CC) $(TARGET_CFLAGS) -I$(PROJETO_INC_DIR) -c $(PROJETO_SRC_DIR)/main.c -o $(PROJETO_OBJ_DIR)/main.o
    $(TARGET_CC) $(TARGET_CFLAGS) -I$(PROJETO_INC_DIR) -c $(PROJETO_SRC_DIR)/lcd1602.c -o $(PROJETO_OBJ_DIR)/lcd1602.o
    $(TARGET_CC) $(TARGET_CFLAGS) -I$(PROJETO_INC_DIR) -c $(PROJETO_SRC_DIR)/gpio.c -o $(PROJETO_OBJ_DIR)/gpio.o
    $(TARGET_CC) $(TARGET_CFLAGS) -I$(PROJETO_INC_DIR) -c $(PROJETO_SRC_DIR)/pwm.c -o $(PROJETO_OBJ_DIR)/pwm.o
    $(TARGET_CC) $(TARGET_CFLAGS) -I$(PROJETO_INC_DIR) -c $(PROJETO_SRC_DIR)/sensor.c -o $(PROJETO_OBJ_DIR)/sensor.o
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(PROJETO_BIN_DIR)/projeto $(PROJETO_OBJ_FILES)
endef

# Comandos para instalar o binário no sistema de arquivos
define PROJETO_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(PROJETO_BIN_DIR)/projeto $(TARGET_DIR)/usr/bin/projeto
endef

# Inclui o pacote no Buildroot
$(eval $(generic-package))