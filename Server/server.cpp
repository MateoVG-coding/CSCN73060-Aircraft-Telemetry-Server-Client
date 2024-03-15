#include "server.h"

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    do {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the string

            // Deserialize received data into a Packet object
            Packet receivedPacket(buffer);

            // Display the contents of the received packet
            std::cout << "Received from client:" << std::endl;
            receivedPacket.Display(std::cout);

            // You can store the received packet in a vector or any other data structure as needed
        }
    } while (bytesReceived > 0);

    closesocket(clientSocket);
}