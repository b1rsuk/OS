#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 3000
#define SERVER_ADDR "127.0.0.1"

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char filename[256];
    char buffer[1024] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Enter the file path: ");
    scanf("%s", filename);

    send(client_socket, filename, strlen(filename), 0);

    while (1) {
        int valread = recv(client_socket, buffer, 1024, 0);
        if (valread <= 0) {
            break;
        }
        printf("%s", buffer);
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}