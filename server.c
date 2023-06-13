#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUF_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", port);

    while (1) {
        int bytes_received = recvfrom(server_socket, buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &client_addr_len);
        if (bytes_received < 0) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        printf("Received message from %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Если клиент нашел клад - отправляем сообщение всем клиентам и завершаем работу сервера
        if (strcmp(buffer, "Treasure found")) {
            printf("Treasure found!\n");
            sprintf(buffer, "Treasure found at %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            int bytes_sent = sendto(server_socket, buffer, strlen(buffer), 0, (struct sockaddr *) &client_addr, client_addr_len);
            if (bytes_sent < 0) {
                perror("Error sending data");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    close(server_socket);
    return 0;
}