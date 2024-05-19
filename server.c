#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 3000

int main() {
    WSADATA wsa;
    SOCKET server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);
    char buffer[1024] = {0};
    FILE *file;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed with error code : %d", WSAGetLastError());
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen)) == INVALID_SOCKET) {
            printf("Accept failed with error code : %d", WSAGetLastError());
            return 1;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int valread = recv(new_socket, buffer, 1024, 0);
        if (valread <= 0) {
            closesocket(new_socket);
            continue;
        }

        buffer[valread] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            closesocket(new_socket);
            break;
        }

        file = fopen(buffer, "r");
        if (file == NULL) {
            send(new_socket, "File not found", strlen("File not found"), 0);
        } else {
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                send(new_socket, buffer, strlen(buffer), 0);
            }
            fclose(file);
        }

        closesocket(new_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}