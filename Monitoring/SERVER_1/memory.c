#include <stdio.h>
#include <stdlib.h>

void get_memory_usage() {
    FILE *fp;
    char buffer[256];
    unsigned long mem_total = 0, mem_free = 0, mem_available = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "MemTotal: %lu kB", &mem_total);
        sscanf(buffer, "MemFree: %lu kB", &mem_free);
        sscanf(buffer, "MemAvailable: %lu kB", &mem_available);
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

        printf("Memory Usage:\n");
        printf("Total: %.2f GB\n", mem_total_gb);
        printf("Free: %.2f GB (%.2f%%)\n", mem_free_gb, free_percent);
        printf("Available: %.2f GB (%.2f%%)\n", mem_available_gb, available_percent);
        printf("Used: %.2f GB (%.2f%%)\n", mem_used_gb, used_percent);
    } else {
        printf("Failed to retrieve memory information.\n");
    }
}

int main() {
    get_memory_usage();
    return 0;
}
