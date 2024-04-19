#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_RECORDS 100

typedef struct {
    char domain[256];
    char ip[256];
} DNSRecord;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <ip_servidor> <puerto> <archivo_registros>\n", argv[0]);
        return 1;
    }

    char *ipServidor = argv[1];
    int puerto = atoi(argv[2]);
    char *archivoRegistros = argv[3];

    FILE *recordsFile = fopen(archivoRegistros, "r");

    if (recordsFile == NULL) {
        printf("Error al abrir el archivo de registros.\n");
        return 1;
    }

    // Leer registros DNS desde el archivo
    DNSRecord dnsRecords[MAX_RECORDS];
    int numRecords = 0;
    char line[MAX_BUFFER_SIZE];
    while (fgets(line, sizeof(line), recordsFile) != NULL && numRecords < MAX_RECORDS) {
        char *domain = strtok(line, ":");
        char *ip = strtok(NULL, "\n");
        if (domain != NULL && ip != NULL) {
            strcpy(dnsRecords[numRecords].domain, domain);
            strcpy(dnsRecords[numRecords].ip, ip);
            numRecords++;
        }
    }

    fclose(recordsFile);

    // Crear socket UDP
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        printf("Error al crear el socket.\n");
        return 1;
    }

    // Configurar dirección del servidor
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipServidor);
    serverAddr.sin_port = htons(puerto);

    // Enlazar socket al puerto
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Error al enlazar el socket al puerto.\n");
        close(serverSocket);
        return 1;
    }

    printf("Servidor DNS iniciado. Esperando consultas...\n");

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        int bytesReceived = recvfrom(serverSocket, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (bytesReceived < 0) {
            printf("Error al recibir datos del cliente.\n");
            break;
        }

        buffer[bytesReceived] = '\0';

        char *requestedItem = buffer;
        char *foundItem = NULL;

        // Buscar IP o nombre de dominio en los registros
        for (int i = 0; i < numRecords; i++) {
            if (strcmp(requestedItem, dnsRecords[i].domain) == 0 || strcmp(requestedItem, dnsRecords[i].ip) == 0) {
                foundItem = (strcmp(requestedItem, dnsRecords[i].domain) == 0) ? dnsRecords[i].ip : dnsRecords[i].domain;
                // Enviar la respuesta al cliente
                sendto(serverSocket, foundItem, strlen(foundItem), 0, (struct sockaddr *)&clientAddr, clientAddrSize);
                printf("Respondiendo a la consulta para %s: %s\n", requestedItem, foundItem);
                break;
            }
        }

        if (foundItem == NULL) {
            printf("No se encontró un registro para %s\n", requestedItem);
        }
    }

    // Cerrar socket
    close(serverSocket);

    return 0;
}
