#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define COUNTS_OF_CLIENT 5

int temperature = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* client(void* q){
    int file = open("/home/borsuk/Documents/lnu/os/aaaablyt/os/tmp.txt", O_RDONLY);
    if (file == -1) {
        perror("open");
        return NULL;
    }
    int* data = (int*)mmap(0, sizeof(int), PROT_READ, MAP_SHARED, file, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(file);
        return NULL;
    }
    close(file);
    while (1) {
        pthread_mutex_lock(&mutex);
        if (*data < 0) {
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        printf("temperature is equal to %d\n", *data);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* server(void* q){
    int file = open("/home/borsuk/Documents/lnu/os/aaaablyt/os/tmp.txt", O_RDWR | O_CREAT, 0644);
    if (file == -1) {
        perror("open");
        return NULL;
    }
    if (ftruncate(file, sizeof(int)) == -1) {
        perror("ftruncate");
        close(file);
        return NULL;
    }
    int* data = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(file);
        return NULL;
    }
    close(file);
    *data = 50;
    while (*data >= 0) {
        pthread_mutex_lock(&mutex);
        printf("Server sets temperature %d\n", *data);
        *data = -10 + rand() % 50;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    pthread_t client_threads[COUNTS_OF_CLIENT];
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server, NULL);
    for (int i = 0;  i < COUNTS_OF_CLIENT; i++) {
        pthread_create(&client_threads[i], NULL, client, NULL);
    }
    for (int i = 0;  i < COUNTS_OF_CLIENT; i++) {
        pthread_join(client_threads[i], NULL);
    }
    pthread_join(server_thread, NULL);
    return 0;
}
