#include "server.h"

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;
    do {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the string
            std::cout << "Received from client: " << buffer << std::endl;
            // Echo back to the client
            send(clientSocket, buffer, bytesReceived, 0);
        }
    } while (bytesReceived > 0);
    closesocket(clientSocket);
}