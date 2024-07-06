#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/sysinfo.h>

void get_process_info() {
    DIR *dir;
    struct dirent *entry;
    FILE *fp;
    char path[256], buffer[256];
    unsigned long utime, stime;
    long rss;
    int pid, total_processes = 0;
    unsigned long total_memory = 0, total_cpu_time = 0;
    struct sysinfo info;

    // Get system uptime
    sysinfo(&info);
    unsigned long uptime = info.uptime;

    // Get total memory
    unsigned long total_mem = info.totalram * info.mem_unit;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    printf("%-10s %-10s %-10s %-10s\n", "PID", "CPU%", "MEM%", "CMD");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            pid = atoi(entry->d_name);
            snprintf(path, sizeof(path), "/proc/%d/stat", pid);

            fp = fopen(path, "r");
            if (fp != NULL) {
                // Read the required fields from /proc/[pid]/stat
                fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
                fclose(fp);

                snprintf(path, sizeof(path), "/proc/%d/status", pid);
                fp = fopen(path, "r");
                if (fp != NULL) {
                    while (fgets(buffer, sizeof(buffer), fp)) {
                        if (sscanf(buffer, "VmRSS: %ld kB", &rss) == 1) {
                            break;
                        }
                    }
                    fclose(fp);

                    // Calculate CPU and memory usage
                    unsigned long total_time = utime + stime;
                    double cpu_usage = 100.0 * (total_time / sysconf(_SC_CLK_TCK)) / uptime;
                    double mem_usage = 100.0 * rss * 1024 / total_mem;

                    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
                    fp = fopen(path, "r");
                    if (fp != NULL) {
                        if (fgets(buffer, sizeof(buffer), fp) && buffer[0] != '\0') {
                            printf("%-10d %-10.2f %-10.2f %-10s\n", pid, cpu_usage, mem_usage, buffer);
                        } else {
                            printf("%-10d %-10.2f %-10.2f [kernel thread]\n", pid, cpu_usage, mem_usage);
                        }
                        fclose(fp);
                    } else {
                        printf("%-10d %-10.2f %-10.2f [unknown]\n", pid, cpu_usage, mem_usage);
                    }

                    total_processes++;
                    total_memory += rss;
                    total_cpu_time += total_time;
                }
            }
        }
    }
    closedir(dir);

    printf("\nTotal processes: %d\n", total_processes);
    printf("Total memory usage: %lu kB\n", total_memory);
    printf("Total CPU time: %lu jiffies\n", total_cpu_time);
}

int main() {
    get_process_info();
    return 0;
}
