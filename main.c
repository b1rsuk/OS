#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

#define COUNTS_OF_CLIENT 5

int temperature = 0;
HANDLE mutex;

void* client(void* q) {
    HANDLE file = CreateFileA("C:\\Users\\user\\Desktop\\OS\\tmp.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening file: %lu\n", GetLastError());
        return NULL;
    }

    HANDLE mapFile = CreateFileMapping(file, NULL, PAGE_READONLY, 0, sizeof(int), NULL);
    if (mapFile == NULL) {
        fprintf(stderr, "Error creating file mapping: %lu\n", GetLastError());
        CloseHandle(file);
        return NULL;
    }

    int* data = (int*)MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, sizeof(int));
    if (data == NULL) {
        fprintf(stderr, "Error mapping view of file: %lu\n", GetLastError());
        CloseHandle(mapFile);
        CloseHandle(file);
        return NULL;
    }

    CloseHandle(file);

    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        if (*data < 0) {
            ReleaseMutex(mutex);
            return NULL;
        }
        printf("temperature is equal to %d\n", *data);
        ReleaseMutex(mutex);
        Sleep(1000);
    }

    UnmapViewOfFile(data);
    CloseHandle(mapFile);

    return NULL;
}

void* server(void* q) {
    HANDLE file = CreateFileA("C:\\Users\\user\\Desktop\\OS\\tmp.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening file: %lu\n", GetLastError());
        return NULL;
    }

    HANDLE mapFile = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, sizeof(int), NULL);
    if (mapFile == NULL) {
        fprintf(stderr, "Error creating file mapping: %lu\n", GetLastError());
        CloseHandle(file);
        return NULL;
    }

    int* data = (int*)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
    if (data == NULL) {
        fprintf(stderr, "Error mapping view of file: %lu\n", GetLastError());
        CloseHandle(mapFile);
        CloseHandle(file);
        return NULL;
    }

    CloseHandle(file);

    *data = 50;
    while (*data >= 0) {
        WaitForSingleObject(mutex, INFINITE);
        printf("Server sets temperature %d\n", *data);
        *data = -10 + rand() % 50;
        ReleaseMutex(mutex);
        Sleep(1000);
    }

    UnmapViewOfFile(data);
    CloseHandle(mapFile);

    return NULL;
}

int main(int argc, const char* argv[]) {
    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        fprintf(stderr, "Error creating mutex: %lu\n", GetLastError());
        return 1;
    }

    HANDLE clientThreads[COUNTS_OF_CLIENT];
    HANDLE serverThread;

    unsigned int threadId;

    serverThread = (HANDLE)_beginthreadex(NULL, 0, server, NULL, 0, &threadId);
    if (serverThread == NULL) {
        fprintf(stderr, "Error creating server thread\n");
        CloseHandle(mutex);
        return 1;
    }

    for (int i = 0; i < COUNTS_OF_CLIENT; i++) {
        clientThreads[i] = (HANDLE)_beginthreadex(NULL, 0, client, NULL, 0, &threadId);
        if (clientThreads[i] == NULL) {
            fprintf(stderr, "Error creating client thread %d\n", i);
            CloseHandle(mutex);
            return 1;
        }
    }

    WaitForMultipleObjects(COUNTS_OF_CLIENT, clientThreads, TRUE, INFINITE);
    WaitForSingleObject(serverThread, INFINITE);

    CloseHandle(mutex);

    return 0;
}
