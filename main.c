#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <time.h>

#define COUNTS_OF_CLIENT 5

int temperature = 0;
HANDLE mutex;

void client(void* q) {
    HANDLE file = CreateFile(TEXT("C:\\tmp.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("Error opening file. Error code: %d\n", GetLastError());
        return;
    }
    int* data;
    DWORD fileSize = GetFileSize(file, NULL);
    HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (fileMapping == NULL) {
        printf("Error creating file mapping. Error code: %d\n", GetLastError());
        CloseHandle(file);
        return;
    }
    data = (int*)MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
    if (data == NULL) {
        printf("Error mapping view of file. Error code: %d\n", GetLastError());
        CloseHandle(fileMapping);
        CloseHandle(file);
        return;
    }
    CloseHandle(fileMapping);
    CloseHandle(file);
    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        if (*data < 0) {
            ReleaseMutex(mutex);
            return;
        }
        printf("Temperature is equal to %d\n", *data);
        ReleaseMutex(mutex);
        Sleep(1000);
    }
}

void server(void* q) {
    HANDLE file = CreateFile(TEXT("C:\\tmp.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("Error opening file. Error code: %d\n", GetLastError());
        return;
    }
    HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, sizeof(int), NULL);
    if (fileMapping == NULL) {
        printf("Error creating file mapping. Error code: %d\n", GetLastError());
        CloseHandle(file);
        return;
    }
    int* data = (int*)MapViewOfFile(fileMapping, FILE_MAP_WRITE, 0, 0, 0);
    if (data == NULL) {
        printf("Error mapping view of file. Error code: %d\n", GetLastError());
        CloseHandle(fileMapping);
        CloseHandle(file);
        return;
    }
    CloseHandle(fileMapping);
    CloseHandle(file);
    *data = 50;
    srand((unsigned int)time(NULL));
    while (*data >= 0) {
        WaitForSingleObject(mutex, INFINITE);
        printf("Server sets temperature %d\n", *data);
        *data = -10 + rand() % 50;
        ReleaseMutex(mutex);
        Sleep(1000);
    }
}

int main(int argc, const char* argv[]) {
    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        printf("Error creating mutex. Error code: %d\n", GetLastError());
        return 1;
    }
    HANDLE clientThreads[COUNTS_OF_CLIENT];
    HANDLE serverThread;
    unsigned int threadID;

    serverThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)server, NULL, 0, &threadID);
    if (serverThread == NULL) {
        printf("Error creating server thread. Error code: %d\n", GetLastError());
        return 1;
    }

    for (int i = 0; i < COUNTS_OF_CLIENT; i++) {
        clientThreads[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)client, NULL, 0, &threadID);
        if (clientThreads[i] == NULL) {
            printf("Error creating client thread %d. Error code: %d\n", i, GetLastError());
            return 1;
        }
    }

    WaitForMultipleObjects(COUNTS_OF_CLIENT, clientThreads, TRUE, INFINITE);
    WaitForSingleObject(serverThread, INFINITE);

    CloseHandle(mutex);
    for (int i = 0; i < COUNTS_OF_CLIENT; i++) {
        CloseHandle(clientThreads[i]);
    }
    CloseHandle(serverThread);

    return 0;
}
