#include "server.h"

// Define a map to store PlaneData objects using the plane ID
std::map<std::string, PlaneData> planeDataMap;
// Previous fuel level and timestamp for each plane, using planeID as key
std::map<std::string, std::pair<std::string, std::chrono::steady_clock::time_point>> previousData;

//Save & load data
void writePlaneData(const std::map<std::string, PlaneData>& planeDataMap) {
    std::ofstream outFile("planeData.txt");
    for (const auto& pair : planeDataMap) {
        outFile << pair.first << "," << pair.second.totalFuelConsumption << "," << pair.second.flights << std::endl;
    }
    outFile.close();
}

void loadPlaneData(std::map<std::string, PlaneData>& planeDataMap) {
    std::ifstream inFile("planeData.txt");
    std::string planeID;
    PlaneData data;

    while (inFile >> planeID >> data.totalFuelConsumption >> data.flights) {
        planeDataMap[planeID] = data;
    }
    inFile.close();
}

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

            // Convert plane ID to string (for storing)
            std::string planeIDString = std::to_string(receivedPacket.getPlaneID());
            std::string currentFuel = const_cast<char*>(receivedPacket.getCurrentFuel());
            
            auto now = std::chrono::steady_clock::now();

            // Check if this is the first packet for the plane
            if (previousData.find(planeIDString) != previousData.end()) {
                std::string previousFuel = previousData[planeIDString].first;
                auto previousTime = previousData[planeIDString].second;
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(now - previousTime).count() / 3600.0f; // Convert to hours

                // Calculate fuel consumption rate
                if (elapsedTime > 0) {
                    float fuelConsumptionRate = calculateFuelConsumption(const_cast<char*>(previousFuel.c_str()), const_cast<char*>(currentFuel.c_str()), elapsedTime);
                    // Check if planeID already exists in the map
                    if (planeDataMap.find(planeIDString) == planeDataMap.end()) {
                        // If not, initialize a new entry in the map
                        planeDataMap[planeIDString] = PlaneData();
                    }

                    // Store the fuel consumption rate in the map
                    planeDataMap[planeIDString].totalFuelConsumption += fuelConsumptionRate;
                    planeDataMap[planeIDString].flights++;
                    writePlaneData(planeDataMap);
                }
            }

            // Update previous data for the next calculation
            previousData[planeIDString] = std::make_pair(currentFuel, now);
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