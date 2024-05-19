#include <stdio.h>
#include <unistd.h>

void cd_command() {
    char directory[100];
    printf("Enter directory path: ");
    scanf("%s", directory);
    if (chdir(directory) != 0) {
        printf("Failed to change directory.\n");
    }
}

void pwd_command() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}
