#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void get_disk_usage(const char* path, char* buffer, size_t size) {
    struct statvfs stat;

    if (statvfs(path, &stat) != 0) {
        perror("statvfs");
        snprintf(buffer, size, "Error getting disk usage for %s\n", path);
        return;
    }

    unsigned long total = stat.f_blocks * stat.f_frsize;
    unsigned long available = stat.f_bavail * stat.f_frsize;
    unsigned long used = total - available;

    double total_gb = total / (1024.0 * 1024.0 * 1024.0);
    double available_gb = available / (1024.0 * 1024.0 * 1024.0);
    double used_gb = used / (1024.0 * 1024.0 * 1024.0);

    double used_percent = 100.0 * used / total;
    double available_percent = 100.0 * available / total;

    snprintf(buffer, size, 
             "Disk Usage for %s:\nTotal: %.2f GB\nUsed: %.2f GB (%.2f%%)\nAvailable: %.2f GB (%.2f%%)\n", 
             path, total_gb, used_gb, used_percent, available_gb, available_percent);
}

void get_memory_usage(char* buffer, size_t size) {
    FILE *fp;
    char mem_buffer[256];
    unsigned long mem_total = 0, mem_free = 0, mem_available = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen");
        snprintf(buffer, size, "Error getting memory usage\n");
        return;
    }

    while (fgets(mem_buffer, sizeof(mem_buffer), fp)) {
        sscanf(mem_buffer, "MemTotal: %lu kB", &mem_total);
        sscanf(mem_buffer, "MemFree: %lu kB", &mem_free);
        sscanf(mem_buffer, "MemAvailable: %lu kB", &mem_available);
    }
    fclose(fp);

    if (mem_total > 0) {
        double mem_total_gb = mem_total / 1048576.0;
        double mem_free_gb = mem_free / 1048576.0;
        double mem_available_gb = mem_available / 1048576.0;
        double mem_used_gb = (mem_total - mem_available) / 1048576.0;

        double free_percent = 100.0 * mem_free / mem_total;
        double available_percent = 100.0 * mem_available / mem_total;
        double used_percent = 100.0 * (mem_total - mem_available) / mem_total;

        snprintf(buffer, size, 
                 "Memory Usage:\nTotal: %.2f GB\nFree: %.2f GB (%.2f%%)\nAvailable: %.2f GB (%.2f%%)\nUsed: %.2f GB (%.2f%%)\n", 
                 mem_total_gb, mem_free_gb, free_percent, mem_available_gb, available_percent, mem_used_gb, used_percent);
    } else {
        snprintf(buffer, size, "Failed to retrieve memory information.\n");
    }
}

void get_network_usage(char* buffer, size_t size) {
    FILE *fp;
    char line[256];
    char interface[32];
    unsigned long rx_bytes, tx_bytes;
    unsigned long total_rx_bytes = 0, total_tx_bytes = 0;

    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL) {
        perror("fopen");
        snprintf(buffer, size, "Error getting network usage\n");
        return;
    }

    // Skip the first two lines
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%s %lu %*u %*u %*u %*u %*u %*u %*u %lu", interface, &rx_bytes, &tx_bytes);

        // Remove trailing colon from interface name
        char *colon = strchr(interface, ':');
        if (colon) {
            *colon = '\0';
        }

        total_rx_bytes += rx_bytes;
        total_tx_bytes += tx_bytes;
    }
    fclose(fp);

    double total_rx_gb = total_rx_bytes / (1024.0 * 1024.0 * 1024.0);
    double total_tx_gb = total_tx_bytes / (1024.0 * 1024.0 * 1024.0);

    snprintf(buffer, size, 
             "Total Network Usage:\nTotal Received: %.2f GB\nTotal Transmitted: %.2f GB\n", 
             total_rx_gb, total_tx_gb);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    const char* path = "/";  // Change this to the path of the disk you want to check

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 500
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Listen on localhost
    server_addr.sin_port = htons(500);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 500...\n");

    // Accept and handle client connections
    while ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) >= 0) {
        // Receive request type from client
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            close(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the received data

        if (strcmp(buffer, "disk") == 0) {
            get_disk_usage(path, buffer, sizeof(buffer));
        } else if (strcmp(buffer, "memory") == 0) {
            get_memory_usage(buffer, sizeof(buffer));
        } else if (strcmp(buffer, "network") == 0) {
            get_network_usage(buffer, sizeof(buffer));
        } else {
            snprintf(buffer, sizeof(buffer), "Invalid request\n");
        }

        send(client_fd, buffer, strlen(buffer), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
