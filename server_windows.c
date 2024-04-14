#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_DOMAIN_NAME_LENGTH 256
#define MAX_IP_ADDRESS_LENGTH 16
#define MAX_RECORDS 100

struct dns_record {
    char name[MAX_DOMAIN_NAME_LENGTH];
    char type[5]; // A, NS, CNAME, SOA, MX, etc.
    char value[MAX_IP_ADDRESS_LENGTH];
};

struct dns_record dns_records[MAX_RECORDS];
int num_records = 0;

void load_dns_records(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir el archivo de configuracion");
        exit(EXIT_FAILURE);
    }

    char line[MAX_DOMAIN_NAME_LENGTH + MAX_IP_ADDRESS_LENGTH + 3]; // +3 for ':' and '\n'

    while (fgets(line, sizeof(line), file) != NULL && num_records < MAX_RECORDS) {
        // Elimina el salto de línea del final de la línea
        line[strcspn(line, "\n")] = '\0';

        // Divide la línea en nombre de dominio y dirección IP
        char *domain = strtok(line, ":");
        char *ip = strtok(NULL, ":");

        if (domain != NULL && ip != NULL) {
            // Guarda el registro DNS en la estructura dns_records
            strncpy(dns_records[num_records].name, domain, MAX_DOMAIN_NAME_LENGTH);
            strncpy(dns_records[num_records].value, ip, MAX_IP_ADDRESS_LENGTH);

            num_records++;
        }
    }

    fclose(file);
}

void handle_dns_request(SOCKET sockfd, const char *query, const struct sockaddr_in *client_addr, int client_len) {
    char buffer[MAX_DOMAIN_NAME_LENGTH];

    // Procesar la solicitud DNS y buscar la respuesta en los registros cargados
    const char *response = NULL;
    for (int i = 0; i < num_records; ++i) {
        if (strcmp(query, dns_records[i].name) == 0) {
            // Se encontró una coincidencia, asignar la respuesta
            response = dns_records[i].value;
            break;
        }
    }

    // Si no se encontró una respuesta, enviar una respuesta predeterminada
    if (!response) {
        response = "No se encontró la dirección IP para el nombre de dominio especificado";
    }

    // Enviar la respuesta al cliente
    if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)client_addr, client_len) == -1) {
        perror("Error al enviar respuesta");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <ip> <port> <config_file> <log_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Leer el archivo de configuracion
    load_dns_records(argv[3]);

    // Inicializar Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("Error al inicializar Winsock: %d\n", result);
        return 1;
    }

    // Crear el socket UDP
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        perror("Error al crear el socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Vincular el socket a la dirección del servidor
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error al vincular el socket");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Esperar y manejar las solicitudes DNS entrantes
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    char buffer[MAX_DOMAIN_NAME_LENGTH];

    while (1) {
        int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len == -1) {
            perror("Error al recibir datos");
            exit(EXIT_FAILURE);
        }
        buffer[recv_len] = '\0';
        handle_dns_request(sockfd, buffer, &client_addr, client_len);
    }

    // Cerrar el socket
    closesocket(sockfd);

    // Cerrar Winsock
    WSACleanup();

    return 0;
}
