#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_BUFFER_LENGTH 256

int state = 1;
char filename[MAX_FILENAME_LENGTH];
char filedata[MAX_BUFFER_LENGTH];

void outvector(char* outstring) {
    printf("%s\n", outstring);
}

void* server(void* p) {
    HANDLE hFifo;
    char buf[MAX_BUFFER_LENGTH];

    hFifo = CreateFile(TEXT(filename), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFifo == INVALID_HANDLE_VALUE) {
        strcat(filedata, "Direction isn't correct\n");
        return NULL;
    }

    DWORD bytesRead;
    if (!ReadFile(hFifo, buf, MAX_BUFFER_LENGTH, &bytesRead, NULL)) {
        fprintf(stderr, "Can’t be read from FIFO\n");
    } else {
        printf("Read from FIFO : %s\n", buf);
    }

    CloseHandle(hFifo);
    return NULL;
}

void* client(void* q) {
    printf("Enter 'exit' if you want to end the program.\n\n");

    while (state) {
        printf("Please enter file name or 'exit': ");
        scanf("%s", filename);

        server(&filename);

        if (strcmp(filename, "exit") == 0) {
            state = 0;
        } else {
            HANDLE takedata = (HANDLE)_beginthread(server, 0, NULL);
            WaitForSingleObject(takedata, INFINITE);
            CloseHandle(takedata);
        }
    }

    return NULL;
}

int main(int argc, const char* argv[]) {
    HANDLE thread = (HANDLE)_beginthread(client, 0, NULL);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    return 0;
}
