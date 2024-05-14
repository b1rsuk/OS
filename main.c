#include <stdio.h>
#include <stdlib.h>

void get_cpu_info() {
    FILE *fp;
    char buffer[1024];

    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/cpuinfo\n");
        return;
    }

    printf("CPU інформація:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    fclose(fp);
}

void get_memory_info() {
    FILE *fp;
    char buffer[1024];

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/meminfo\n");
        return;
    }

    printf("\nІнформація про пам'ять:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    fclose(fp);
}

int main() {
    get_cpu_info();
    get_memory_info();

    return 0;
}
