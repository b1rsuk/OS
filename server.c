#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024
#define PORT 3000

void write_file(int new_sock);

int main()
{
    char *ip = "127.0.0.1";
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror(" - Error in socket. Exiting ...");
        exit(1);
    }
    printf("+ Socket created.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(" - Error in bind. Exiting ...");
        exit(1);
    }
    printf("+ Binding success.\n");

    // Listen
    if(listen(sockfd, 10) == 0){
        printf("+ Listening ... \n");
    }else{
        printf(" - Error in listening. Exiting ...");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);

    printf("+ Client connected.\n"); // Added this line to print a message when a client connects.

    // Read file name
    recv(new_sock, buffer, SIZE, 0);
    if(strcmp(buffer, "exit") == 0){
        printf("Server exiting...\n");
        return 0;
    }

    // Write file
    write_file(new_sock);
    printf("+ File data received and written successfully.\n");

    return 0;
}

void write_file(int new_sock)
{
    int n;
    FILE *fp;
    char *filename = "recv.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if(fp == NULL){
        perror(" - Error in opening file. Exiting ...");
        exit(1);
    }
    while (1) {
        n = recv(new_sock, buffer, SIZE, 0);
        if (n <= 0){
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
}
