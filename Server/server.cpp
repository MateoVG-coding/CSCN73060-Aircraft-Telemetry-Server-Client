#include "server.h"

// Define a map to store PlaneData objects using the plane ID
std::map<std::string, PlaneData> planeDataMap;
// Previous fuel level and timestamp for each plane, using planeID as key
std::map<std::string, std::pair<std::string, std::chrono::system_clock::time_point>> previousData;

std::mutex fileMutex;

void writePlaneData(const std::map<std::string, PlaneData>& planeDataMap) {
    std::ofstream outFile("planeData.txt"); // Open file in append mode

    // Lock the mutex to ensure exclusive access to the file
    std::lock_guard<std::mutex> lock(fileMutex);

    for (const auto& pair : planeDataMap) {
        outFile << pair.first << "," << pair.second.totalFuelConsumption << "," << pair.second.flights << std::endl;
    }
    outFile.close();
}

void loadPlaneData(std::map<std::string, PlaneData>& planeDataMap) {
    std::ifstream inFile("planeData.txt");
    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
        return;
    }

    std::string planeID;
    PlaneData data;

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string planeID, status;
        float fuelConsumption;
        int numberOfFlights;

        // Extracting variables from the line
        std::getline(iss, planeID, ',');
        iss >> fuelConsumption;
        iss.ignore(); // Skip the comma
        iss >> numberOfFlights;

        // Check if planeID exists in map, if not, create a new entry
        if (planeDataMap.find(planeID) == planeDataMap.end()) {
            PlaneData newData;
            newData.totalFuelConsumption = fuelConsumption;
            newData.flights = numberOfFlights;
            planeDataMap[planeID] = newData;
        }
        else {
            // Update existing entry
            planeDataMap[planeID].totalFuelConsumption += fuelConsumption;
            planeDataMap[planeID].flights += numberOfFlights;
        }
    }

    if (inFile.bad()) {
        std::cerr << "Error: An error occurred while reading from the file." << std::endl;
    }

    inFile.close();
}


std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp) {
    std::tm tm = {};

    // Parse date and time components separately
    std::istringstream ss(timestamp);
    std::string datePart, timePart;
    ss >> datePart >> timePart;

    // Parse the date part
    std::istringstream dateStream(datePart);
    dateStream >> std::get_time(&tm, "%d_%m_%Y");

    // Parse the time part
    std::istringstream timeStream(timePart);
    timeStream >> std::get_time(&tm, "%H:%M:%S");

    // Convert the time structure to a system_clock::time_point
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

void handleClient(SOCKET ConnectionSocket) 
{
    sockaddr_in CltAddr;					//Client Address for sending responses
    int len = sizeof(struct sockaddr_in);	//Length parameter for the recvfrom function call

    char RxBuffer[1024];
    int bytesReceived;

    do 
    {
        bytesReceived = recvfrom(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0, (struct sockaddr*)&CltAddr, &len);
        if (bytesReceived > 0)
        {
            RxBuffer[bytesReceived] = '\0'; // Null-terminate the string

            // Deserialize received data into a Packet object
            Packet receivedPacket(RxBuffer);

            // Display the contents of the received packet
            std::cout << "Received from client:" << std::endl;
            receivedPacket.Display(std::cout);

            // Convert plane ID to string (for storing)
            std::string planeIDString = receivedPacket.getPlaneID();
            std::string currentFuel = const_cast<char*>(receivedPacket.getCurrentFuel());

            // Get the timestamp from the received packet
            std::chrono::system_clock::time_point currentTime = parseTimestamp(receivedPacket.getCurrentTime());

            // Check if this is the first packet for the plane
            if (previousData.find(planeIDString) != previousData.end()) {
                std::string previousFuel = previousData[planeIDString].first;
                auto previousTime = previousData[planeIDString].second;
                auto elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - previousTime).count() / 3600.0f; // Convert to hours


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
                }
            }

            // Update previous data for the next calculation
            previousData[planeIDString] = std::make_pair(currentFuel, currentTime);

            // Send confirmation to the client
            send(ConnectionSocket, "OK", sizeof("OK"), 0);
        }
    } while (bytesReceived > 0);

    std::cout << "Connection Closed\n" << std::endl;

    closesocket(ConnectionSocket);
}

float calculateFuelConsumption(char* previousFuel, char* currentFuel, float elapsedTime)
{
    float previousFuelLevel = atof(previousFuel);
    float currentFuelLevel = atof(currentFuel);

    float fuelConsumptionRate = (previousFuelLevel - currentFuelLevel) / elapsedTime;

    return fuelConsumptionRate;
}