#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void request_server(const char* request_type) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    int bytes_received;

    // Create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Connect to localhost
    server_addr.sin_port = htons(500);

    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Send request type to server
    send(client_fd, request_type, strlen(request_type), 0);

    // Receive data from server
    bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("%s\n", buffer);

    close(client_fd);
}

int main() {
    printf("Requesting disk usage...\n");
    request_server("disk");

    printf("Requesting memory usage...\n");
    request_server("memory");

    printf("Requesting network usage...\n");
    request_server("network");

    return 0;
}
