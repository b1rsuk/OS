#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_FILENAME_LENGTH 256
#define MAX_BUFFER_LENGTH 256
int state = 1;
char filename[MAX_FILENAME_LENGTH];
char filedata[MAX_BUFFER_LENGTH];
void outvector(char *outstring){
    printf("%s\n", outstring);
}
void* server(void* p){
    int fd_fifo;
    char buf[MAX_BUFFER_LENGTH];
    if((fd_fifo=open(filename, O_RDWR)) == - 1){
        strcat(filedata, "Direction isn't correct\n");
        return NULL;
    }
    if(read(fd_fifo, &buf, sizeof(buf)) == -1)
        fprintf(stderr, "Can’t be read from FIFO\n");
    else
        printf("Read from FIFO : %s\n",buf);
    return NULL;
}
void* client(void* q){
    printf("Enter exit if you wanna end program\n\n");
    while(state){
        printf("Please enter file name or exit: ");
        scanf("%s", filename);
        server(&filename);
        if(strcmp(filename, "exit") == 0)
            state = 0;
        else{
            pthread_t takedata;
            pthread_create(&takedata, NULL, server, NULL);
            pthread_join(takedata, NULL);
        }
    }
    return NULL;
}
int main(int argc, const char * argv[]) {
    pthread_t thread;
    pthread_create(&thread, NULL, client, NULL);
    pthread_join(thread, NULL);
    return 0;
}