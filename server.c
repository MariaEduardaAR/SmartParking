#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define GPIO12 "/sys/class/gpio/gpio12/value" // Vaga 1
#define GPIO13 "/sys/class/gpio/gpio13/value" // Vaga 2

int read_sensor(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;

    char buffer[2];
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    return atoi(buffer) == 0 ? 1 : 0;
}

void send_html(int client_sock) {
    char response[] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"
        "<!DOCTYPE html>\n"
        "<html lang=\"pt-br\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>SmartParking</title>\n"
        "    <style>\n"
        "        body {\n"
        "            font-family: Arial, sans-serif;\n"
        "            text-align: center;\n"
        "            background-color: #f4f4f4;\n"
        "        }\n"
        "        h1 {\n"
        "            margin-top: 20px;\n"
        "        }\n"
        "        .parking-lot {\n"
        "            display: flex;\n"
        "            flex-direction: column;\n"
        "            align-items: center;\n"
        "            margin-top: 20px;\n"
        "        }\n"
        "        .road {\n"
        "            width: 300px;\n"
        "            height: 200px;\n"
        "            background-color: gray;\n"
        "            position: relative;\n"
        "            border: 5px solid black;\n"
        "        }\n"
        "        .spot {\n"
        "            width: 80px;\n"
        "            height: 120px;\n"
        "            background-color: white;\n"
        "            border: 2px solid black;\n"
        "            display: inline-flex;\n"
        "            align-items: center;\n"
        "            justify-content: center;\n"
        "            margin: 5px;\n"
        "            position: absolute;\n"
        "        }\n"
        "        .spot.occupied {\n"
        "            background-color: red;\n"
        "        }\n"
        "        .spot.occupied img {\n"
        "            width: 60px;\n"
        "            height: auto;\n"
        "        }\n"
        "        #spot1 { top: 10px; left: 50px; }\n"
        "        #spot2 { top: 10px; right: 50px; }\n"
        "        .alert {\n"
        "            color: red;\n"
        "            font-weight: bold;\n"
        "            display: none;\n"
        "            margin-top: 10px;\n"
        "        }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>SmartParking</h1>\n"
        "    <p>Veja a quantidade de vagas:</p>\n"
        "    <div class=\"parking-lot\">\n"
        "        <div class=\"road\">\n"
        "            <div id=\"spot1\" class=\"spot\">Vaga 1</div>\n"
        "            <div id=\"spot2\" class=\"spot\">Vaga 2</div>\n"
        "        </div>\n"
        "    </div>\n"
        "    <p id=\"alert\" class=\"alert\">Estacionamento cheio!</p>\n"
        "    <script>\n"
        "        function atualizarVagas() {\n"
        "            fetch(\"http://10.4.1.2:8080/sensors\").then(response => response.json()).then(data => {\n"
        "                let spot1 = document.getElementById(\"spot1\");\n"
        "                let spot2 = document.getElementById(\"spot2\");\n"
        "                let alertMsg = document.getElementById(\"alert\");\n"
        "                \n"
        "                if (data.vaga1) {\n"
        "                    spot1.classList.add(\"occupied\");\n"
        "                    spot1.innerHTML = \"Ocupada\";\n"
        "                } else {\n"
        "                    spot1.classList.remove(\"occupied\");\n"
        "                    spot1.innerHTML = \"Vaga 1\";\n"
        "                }\n"
        "                \n"
        "                if (data.vaga2) {\n"
        "                    spot2.classList.add(\"occupied\");\n"
        "                    spot2.innerHTML = \"Ocupada\";\n"
        "                } else {\n"
        "                    spot2.classList.remove(\"occupied\");\n"
        "                    spot2.innerHTML = \"Vaga 2\";\n"
        "                }\n"
        "                \n"
        "                alertMsg.style.display = (data.vaga1 && data.vaga2) ? \"block\" : \"none\";\n"
        "            }).catch(error => console.error(\"Erro ao buscar dados:\", error));\n"
        "        }\n"
        "        setInterval(atualizarVagas, 2000);\n"
        "        atualizarVagas();\n"
        "    </script>\n"
        "</body>\n"
        "</html>";
    
    send(client_sock, response, sizeof(response) - 1, 0);
}

void send_json(int client_sock) {
    int vaga1_ocupada = read_sensor(GPIO12);
    int vaga2_ocupada = read_sensor(GPIO13);

    char response[256];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"
        "{\"vaga1\": %d, \"vaga2\": %d}",
        vaga1_ocupada, vaga2_ocupada
    );

    send(client_sock, response, strlen(response), 0);

    if (vaga1_ocupada && vaga2_ocupada) {
        printf("Todas as vagas estão ocupadas!\n");
    }
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erro ao criar socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao fazer bind");
        exit(1);
    }

    listen(server_fd, 5);
    printf("Servidor rodando na porta %d...\n", PORT);

    while (1) {
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("Erro ao aceitar conexão");
            continue;
        }

        // Verifica o caminho solicitado
        char buffer[1024];
        read(client_sock, buffer, sizeof(buffer) - 1);
        
        if (strstr(buffer, "/sensors") != NULL) {
            send_json(client_sock);
        } else {
            send_html(client_sock);
        }

        close(client_sock);
    }

    close(server_fd);
    return 0;
}