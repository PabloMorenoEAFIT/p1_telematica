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

    while (fscanf(file, "%s %s %s", dns_records[num_records].name, dns_records[num_records].type, dns_records[num_records].value) == 3) {
        num_records++;
    }

    fclose(file);
}

void handle_dns_request(SOCKET sockfd) {
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    char buffer[MAX_DOMAIN_NAME_LENGTH];
    int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
    if (recv_len == -1) {
        perror("Error al recibir datos");
        exit(EXIT_FAILURE);
    }

    // Procesar la solicitud DNS y buscar la respuesta en los registros cargados
    // Aquí debes implementar la lógica para buscar la respuesta en los registros cargados

    // Dummy response
    const char *response = "Respuesta DNS dummy";

    // Enviar la respuesta al cliente
    if (sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_len) == -1) {
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
    while (1) {
        fd_set tmp_fds;
        FD_ZERO(&tmp_fds);
        FD_SET(sockfd, &tmp_fds);
        int num_ready_fds = select(0, &tmp_fds, NULL, NULL, NULL);
        if (num_ready_fds == SOCKET_ERROR) {
            perror("Error en select");
            closesocket(sockfd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &tmp_fds)) {
            handle_dns_request(sockfd);
        }
    }

    // Cerrar el socket
    closesocket(sockfd);

    // Cerrar Winsock
    WSACleanup();

    return 0;
}
