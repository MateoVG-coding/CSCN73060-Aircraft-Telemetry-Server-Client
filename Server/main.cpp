#include "server.h"

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return -1;

    SOCKET ServerSocket;
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET) {
        WSACleanup();
        return -1;
    }

    sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(27000);
    if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }

    if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server listening on port 27000\n";

    std::cout << "Waiting for client connection...\n" << std::endl;

    std::vector<std::thread> clientThreads;

    while (true) {
        // Accept incoming connection
        SOCKET ConnectionSocket = accept(ServerSocket, NULL, NULL);
        if (ConnectionSocket == INVALID_SOCKET) {
            std::cerr << "accept() failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ServerSocket);
            WSACleanup();
            return -1;
        }

        std::cout << "Connection Established\n" << std::endl;

        // Create a thread to handle the client
        std::thread clientThread(handleClient, ConnectionSocket);
        clientThreads.push_back(std::move(clientThread));
    }

    // Join all client threads before exiting
    for (auto& thread : clientThreads) {
        thread.join();
    }

    // Close the listening socket
    closesocket(ServerSocket);	
    WSACleanup();

    return 0;
}
