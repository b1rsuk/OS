#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int num_threads_waiting = 0;

void send_msg(int N) {
    pthread_mutex_lock(&mutex);
    num_threads_waiting = N;
    pthread_cond_broadcast(&cond);
    while (num_threads_waiting > 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void recv_msg() {
    pthread_mutex_lock(&mutex);
    num_threads_waiting--;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void* thread_function(void* arg) {
    printf("Thread %ld started\n", (long)arg);
    recv_msg();
    printf("Thread %ld received message\n", (long)arg);
    return NULL;
}

int main() {
    pthread_t threads[5];
    int i;

    for (i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*)(long)i);
    }

    send_msg(5);

    for (i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
