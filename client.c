#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024
#define PORT 3000

void send_file(FILE *fp, int sockfd);

int main()
{
    char *ip = "127.0.0.1";
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;
    char filename[SIZE];

    // Get the file name from the user
    printf("Enter the path to the file: ");
    scanf("%s", filename);

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

    // Connect
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror(" - Error in socket. Exiting ...");
        exit(1);
    }
    printf("+ Connected to server.\n");

    // Send the file name to the server
    send(sockfd, filename, sizeof(filename), 0);

    // Read file
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(" - Error in reading file. Exiting ...");
        exit(1);
    }

    // Send file
    send_file(fp, sockfd);
    printf("+ File data sent successfully.\n");

    return 0;
}

void send_file(FILE *fp, int sockfd)
{
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror(" - Error in sending file. Exiting ...");
            exit(1);
        }
        bzero(data, SIZE);
    }
}
