#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PROC_DIR "/proc"
#define OUTPUT_FILE "sysinfo.txt"

int main() {
    // Відкриття файлу для запису з прапором O_CREAT | O_WRONLY
    FILE *output = fopen(OUTPUT_FILE, "w");
    if (!output) {
        perror("fopen");
        return 1;
    }

    // Зчитування інформації про CPU
    FILE *cpuinfo = fopen(PROC_DIR "/cpuinfo", "r");
    if (cpuinfo) {
        char line[1024];
        char *key, *value;
        while (fgets(line, sizeof(line), cpuinfo)) {
            key = strtok(line, ":");
            if (!key) continue;
            value = strtok(NULL, "\n");
            fprintf(output, "%s: %s\n", key, value);
            printf("%s: %s\n", key, value);
        }
        fclose(cpuinfo);
    } else {
        perror("fopen");
    }

    // Зчитування інформації про пам'ять
    FILE *meminfo = fopen(PROC_DIR "/meminfo", "r");
    if (meminfo) {
        char line[1024];
        char *key, *value;
        while (fgets(line, sizeof(line), meminfo)) {
            key = strtok(line, ":");
            if (!key) continue;
            value = strtok(NULL, "\n");
            fprintf(output, "%s: %s\n", key, value);
            printf("%s: %s\n", key, value);
        }
        fclose(meminfo);
    } else {
        perror("fopen");
    }

    fclose(output);

    return 0;
}
