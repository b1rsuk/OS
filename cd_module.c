#include <stdio.h>
#include <windows.h>

__declspec(dllexport) void cd_command() {
    char directory[MAX_PATH];
    printf("Enter directory path: ");
    scanf("%s", directory);
    if (!SetCurrentDirectoryA(directory)) {
        printf("Failed to change directory.\n");
    }
}

__declspec(dllexport) void pwd_command() {
    char cwd[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, cwd) != 0) {
        printf("Current directory: %s\n", cwd);
    } else {
        printf("Error getting current directory.\n");
    }
}