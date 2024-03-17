#include "server.h"

// Data structure that stores fuel consumption data
struct PlaneData {
    float totalFuelConsumption;
    int flights;

    PlaneData() : totalFuelConsumption(0.0f), flights(0) {}
};

// Define a map to store PlaneData objects using the plane ID
std::map<std::string, PlaneData> planeDataMap;

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

            // Get the plane ID from the received packet
            unsigned char planeID = receivedPacket.getPlaneID();

            // Convert plane ID to string (for storing)
            std::string planeIDString = std::to_string(planeID);

            // Calculate fuel consumption rate
            float elapsedTime = 0;
            char* timeField = const_cast<char*>(receivedPacket.getTimeField());
            char* fuelField = const_cast<char*>(receivedPacket.getFuelField());
            float fuelConsumptionRate = calculateFuelConsumption(timeField, fuelField, elapsedTime);
            
            // Check if planeID already exists in the map
            if (planeDataMap.find(planeIDString) == planeDataMap.end()) {
                // If not, initialize a new entry in the map
                planeDataMap[planeIDString] = PlaneData();
            }

            // Store the fuel consumption rate in the map
            planeDataMap[planeIDString].totalFuelConsumption += fuelConsumptionRate;
            planeDataMap[planeIDString].flights++;
        }
    } while (bytesReceived > 0);

    closesocket(clientSocket);
}

float calculateFuelConsumption(char* previousFuel, char* currentFuel, float elapsedTime)
{
    float previousFuelLevel = atof(previousFuel);
    float currentFuelLevel = atof(currentFuel);

    float fuelConsumptionRate = (previousFuelLevel - currentFuelLevel) / elapsedTime;

    return fuelConsumptionRate;
}