#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_network_usage() {
    FILE *fp;
    char buffer[256];
    char interface[32];
    unsigned long rx_bytes, tx_bytes;
    unsigned long total_rx_bytes = 0, total_tx_bytes = 0;

    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    // Skip the first two lines
    fgets(buffer, sizeof(buffer), fp);
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "%s %lu %*u %*u %*u %*u %*u %*u %*u %lu", interface, &rx_bytes, &tx_bytes);

        // Remove trailing colon from interface name
        char *colon = strchr(interface, ':');
        if (colon) {
            *colon = '\0';
        }

        printf("Interface: %s\n", interface);
        printf("Received: %lu bytes\n", rx_bytes);
        printf("Transmitted: %lu bytes\n\n", tx_bytes);

        total_rx_bytes += rx_bytes;
        total_tx_bytes += tx_bytes;
    }
    fclose(fp);

    double total_rx_gb = total_rx_bytes / (1024.0 * 1024.0 * 1024.0);
    double total_tx_gb = total_tx_bytes / (1024.0 * 1024.0 * 1024.0);

    printf("Total Network Usage:\n");
    printf("Total Received: %.2f GB\n", total_rx_gb);
    printf("Total Transmitted: %.2f GB\n", total_tx_gb);
}

int main() {
    get_network_usage();
    return 0;
}
