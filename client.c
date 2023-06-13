#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: %s <server ip> <server port> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    char *message = argv[3];
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    char buffer[BUF_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Error converting IP address");
        exit(EXIT_FAILURE);
    }

    int bytes_sent = sendto(client_socket, message, strlen(message), 0, (struct sockaddr *) &server_addr, server_addr_len);
    if (bytes_sent < 0) {
        perror("Error sending data");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to %s:%d: %s\n", server_ip, server_port, message);

    int bytes_received = recvfrom(client_socket, buffer, BUF_SIZE, 0, (struct sockaddr *) &server_addr, &server_addr_len);
    if (bytes_received < 0) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }

    printf("Received message from %s:%d: %s\n", server_ip, server_port, buffer);

    close(client_socket);
    return 0;
}