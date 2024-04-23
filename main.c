#include <stdio.h>
#include <windows.h>

CRITICAL_SECTION mutex;
HANDLE cond_event;
int num_threads_waiting = 0;

void send_msg(int N) {
    EnterCriticalSection(&mutex);
    num_threads_waiting = N;
    SetEvent(cond_event);
    while (num_threads_waiting > 0) {
        LeaveCriticalSection(&mutex);
        WaitForSingleObject(cond_event, INFINITE);
        EnterCriticalSection(&mutex);
    }
    LeaveCriticalSection(&mutex);
}

void recv_msg() {
    EnterCriticalSection(&mutex);
    num_threads_waiting--;
    SetEvent(cond_event);
    LeaveCriticalSection(&mutex);
}

DWORD WINAPI thread_function(LPVOID arg) {
    printf("Thread %ld started\n", (long)arg);
    recv_msg();
    printf("Thread %ld received message\n", (long)arg);
    return 0;
}

int main() {
    InitializeCriticalSection(&mutex);
    cond_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE threads[5];
    int i;

    for (i = 0; i < 5; i++) {
        threads[i] = CreateThread(NULL, 0, thread_function, (LPVOID)(long)i, 0, NULL);
    }

    send_msg(5);

    WaitForMultipleObjects(5, threads, TRUE, INFINITE);

    for (i = 0; i < 5; i++) {
        CloseHandle(threads[i]);
    }

    DeleteCriticalSection(&mutex);
    CloseHandle(cond_event);

    return 0;
}
